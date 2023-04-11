import io
import os
import pathlib
import copy
from typing import Callable, Iterable, Dict, Union, Optional, Any
import json
import shutil
import re
import secrets
import base64
from invoke import task, Context, Result
import fabric
import fabric.transfer

from ruamel.yaml import YAML
import jinja2

from pprint import pprint


def project_root_relative(context: Context) -> pathlib.Path:
    with io.StringIO() as stdout:
        context.run(
            'git rev-parse --show-superproject-working-tree --show-toplevel | head -1',
            out_stream=stdout)
        root = pathlib.Path(stdout.getvalue().strip())
        cwd = pathlib.Path(context.cwd).resolve()
        return pathlib.Path(os.path.relpath(str(root), str(cwd)))


def update_dict_values(iter: Union[Dict, str], func: Callable[[str], str]):
    if isinstance(iter, str):
        print(iter, func(iter))
        return func(iter)
    else:
        for key in iter.keys():
            iter[key] = update_dict_values(iter[key], func)
        return iter


def replace(src, path: Iterable[str], func: Callable[[str], str]):
    if len(path) == 0 and isinstance(src, str):
        return func(src)

    if isinstance(src, dict):
        key = path[0]
        if key in src:
            src[key] = replace(src[key], path[1:], func)

        # DFS
        for iter in src.values():
            iter = replace(iter, path, func)
    if isinstance(src, list):
        for index, iter in enumerate(src):
            src[index] = replace(iter, path, func)

    return src


@task
def generate_qmpc_setting(c: Context, party_list_raw: str, bts_host: str, bts_tag: str):
    '''
    '''
    party_list = party_list_raw.split(',')
    templates_root_dir = pathlib.Path(c.cwd) / pathlib.Path('../templates')
    loader = jinja2.FileSystemLoader(templates_root_dir)

    qmpc_config_path = pathlib.Path('qmpc_setting/config.yaml.jinja')

    # secret となる 256 bit の乱数を生成する
    SECRET_BITS = 256
    SECRET_BYTES = SECRET_BITS // 8

    secret_bytes = secrets.token_bytes(SECRET_BYTES)
    secret_base64 = base64.standard_b64encode(secret_bytes).decode('ascii')

    template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(
        name=str(qmpc_config_path)
    )

    args = {
        'party_list': party_list,
        'bts_host': bts_host,
        'bts_tag': bts_tag,
        'bts_jwt_secret': secret_base64,
    }

    result = template.render(args)

    destination_path = pathlib.Path('./.output') / qmpc_config_path.with_suffix('')
    c.run(f"mkdir -p {destination_path.parent}")

    with open(destination_path, mode='w') as f:
        f.write(result)


def load_qmpc_setting(path: str | pathlib.Path) -> dict[str, Any]:
    yaml = YAML()
    yaml.preserve_quotes = True
    with open(path) as f:
        return yaml.load(f)


@task
def generate_jwt_tokens(context: Context, exp=9223371974719179007, qmpc_config_path='./.output/qmpc_setting/config.yaml'):
    '''
    '''
    qmpc_setting = load_qmpc_setting(qmpc_config_path)

    # template から jwt token 生成用の設定ファイルを作成
    templates_root_dir = pathlib.Path(context.cwd) / pathlib.Path('../templates')
    loader = jinja2.FileSystemLoader(templates_root_dir)

    jwt_config_tamplate_path = pathlib.Path('./qmpc_setting') / 'jwt.yaml.jinja'

    template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(
        name=str(jwt_config_tamplate_path)
    )

    def generate(args: dict[str, Any], jwt_config_dst_path: pathlib.Path, jwt_envs_dir: pathlib.Path):
        result = template.render(args)

        context.run(f"mkdir -p {jwt_config_dst_path.parent}")

        with open(jwt_config_dst_path, mode='w') as f:
            f.write(result)

        # jwt token を生成する
        context.run(f"mkdir -p {jwt_envs_dir}")

        uid_cmd: Result = context.run('id -u')
        uid = int(uid_cmd.stdout.strip())
        gid_cmd: Result = context.run('id -g')
        gid = int(gid_cmd.stdout.strip())

        context.run(rf"""docker run \
                        -u {uid}:{gid} \
                        -v {jwt_config_dst_path.resolve()}:/input/{jwt_config_dst_path.name} \
                        -v {jwt_envs_dir.resolve()}:/output \
                        --env JWT_BASE64_SECRET_KEY={qmpc_setting['bts']['secret']} \
                        ghcr.io/acompany-develop/quickmpc-bts:{qmpc_setting['bts']['tag']} \
                        ./beaver_triple_service generateJwt \
                        --file /input/{jwt_config_dst_path.name} \
                        --output /output
                    """)

    for party in qmpc_setting['party_list']:
        print(party)

        args = {
            'setting': qmpc_setting,
            'exp': exp,
            'party_id': party['party_id']
        }

        jwt_config_dst_path = pathlib.Path('./.output') \
            / jwt_config_tamplate_path.parent / 'parties' / f"{party['party_id']}" / jwt_config_tamplate_path.name
        jwt_config_dst_path = jwt_config_dst_path.with_suffix('')

        jwt_envs_dir = pathlib.Path('./.output/config/') / 'parties' / f"{party['party_id']}" / 'jwt-envs'

        generate(args, jwt_config_dst_path, jwt_envs_dir)

    # generate jwt token for healthcheck
    args = {
        'exp': exp,
        # required for authentication
        'setting': {'party_list': {'id': 1, 'address': ''}},  # dummy data
        'party_id': 1,
    }

    jwt_config_dst_path = pathlib.Path('./.output') \
        / jwt_config_tamplate_path.parent / 'others' / jwt_config_tamplate_path.name
    jwt_config_dst_path = jwt_config_dst_path.with_suffix('')

    jwt_envs_dir = pathlib.Path('./.output/config/') / 'others' / 'jwt-envs'

    generate(args, jwt_config_dst_path, jwt_envs_dir)


def update_path_info(destination: Dict, path: Iterable, value: str) -> Dict:
    if len(path) == 0:
        return {
            '__path__': value,
        }
    else:
        item = update_path_info(
            destination
            .get(path[0], {}),
            path[1:], value)
        destination[path[0]] = item
        destination['__path__'] = str(pathlib.Path(item['__path__']).parent)
        return destination


@task
def generate_config(context: Context, qmpc_config_path='./.output/qmpc_setting/config.yaml'):
    '''各パーティの設定ファイルを生成

    `./templates/config` 以下のファイルを元に
    `./.output/config/party*` を生成する。
    各ファイル、ディレクトリへの絶対パスを記載した `.output/env-locations.json` を書き出す。

    Parameters
    ----------
    c
        Context 自動で渡される
    party_num
        MPC の Party 数
    '''

    qmpc_setting = load_qmpc_setting(qmpc_config_path)

    # template rendering
    templates_root_dir = pathlib.Path(context.cwd) / pathlib.Path('../templates/config')
    print(templates_root_dir.resolve())

    output = dict()

    # generate configuration files for each party
    templates_party_dir = templates_root_dir / 'party'
    loader = jinja2.FileSystemLoader(str(templates_party_dir))
    for item in templates_party_dir.glob('./**/*'):
        if item.is_dir():
            continue
        item = item.relative_to(templates_party_dir)
        print(f"INFO: Template file is {(templates_party_dir / item).resolve()}")

        for i in range(len(qmpc_setting['party_list'])):
            dst_sub_path = item
            if dst_sub_path.suffix == '.jinja':
                dst_sub_path = dst_sub_path.with_suffix('')
            destination_path = pathlib.Path('./.output') / f"config/parties/{i+1}" / dst_sub_path
            destination_path = destination_path.resolve()
            print(f"INFO: -> {destination_path}")
            context.run(f"mkdir -p {destination_path.parent}")

            update_path_info(output, ('parties', str(i + 1)) + dst_sub_path.parts, str(destination_path))

            template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(name=str(item))
            args = {
                'setting': qmpc_setting,
                'party_id': i + 1,
            }
            result = template.render(args)
            with open(destination_path, 'w') as f:
                f.write(result)

    # generate configuration files for services which is outside party
    templates_others_dir = templates_root_dir / 'others'
    loader = jinja2.FileSystemLoader(str(templates_others_dir))
    for item in templates_others_dir.glob('./**/*'):
        if item.is_dir():
            continue
        item = item.relative_to(templates_others_dir)
        print(f"INFO: Template file is {(templates_others_dir / item).resolve()}")

        dst_sub_path = item
        if dst_sub_path.suffix == '.jinja':
            dst_sub_path = dst_sub_path.with_suffix('')
        destination_path = pathlib.Path('./.output') / 'config' / 'others' / dst_sub_path
        destination_path = destination_path.resolve()
        print(f"INFO: -> {destination_path}")
        context.run(f"mkdir -p {destination_path.parent}")

        update_path_info(output, ('others',) + dst_sub_path.parts, str(destination_path))

        template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(name=str(item))
        args = {
            'setting': qmpc_setting,
        }
        result = template.render(args)
        with open(destination_path, 'w') as f:
            f.write(result)

    # add jwt envs
    jwt_envs_dir = pathlib.Path('./.output/config')
    for item in jwt_envs_dir.glob("**/*.jwt.env"):
        relpath = item.relative_to(jwt_envs_dir)
        print(f"INFO: env file is {(jwt_envs_dir / relpath).resolve()}")
        update_path_info(output, relpath.parts, str(item.resolve()))

    context.run('mkdir -p ./.output')
    with open('./.output/env-locations.json', mode='w') as f:
        json.dump(output, f, indent=2)


@task
def generate_docker_compose(context: Context,
                            qmpc_config_path='./.output/qmpc_setting/config.yaml',
                            docker_image_tag=''):
    '''docker-compose.yml ファイルを生成

    `./templates/docker-compose.yml.jinja` ファイルを元に
    `./docker-compose/docker-compose.yml` を生成する。
    テンプレートファイルには以下を引数に与える。

    - コマンドライン引数
    - ``generate-config`` の結果 `./.output/env-locations.json`
    - 生成先からプロジェクトルートへの相対パス

    Parameters
    ----------
    c
        Context 自動で渡される
    party_num
        MPC の Party 数
    docker_image_tag
        - 共通の docker image tag
        - 例) `'s20220608'`
    '''

    qmpc_setting = load_qmpc_setting(qmpc_config_path)

    templates_root_dir = pathlib.Path(context.cwd) / pathlib.Path('../templates/docker-compose')
    print(templates_root_dir.resolve())
    loader = jinja2.FileSystemLoader(templates_root_dir)

    env_locations = None
    with open('./.output/env-locations.json') as f:
        env_locations = json.load(f)

    for item in templates_root_dir.glob('**/docker-compose.party.yaml.jinja'):
        if item.is_dir():
            continue
        item = item.relative_to(templates_root_dir)
        print(f"INFO: Template file is {item.resolve()}")

        # generate configuration files for each party
        for cfg_party in qmpc_setting['party_list']:
            destination_path = pathlib.Path('./.output') / f"docker-compose/parties/{cfg_party['party_id']}" / 'docker-compose.yaml'
            destination_path = destination_path.resolve()
            print(f"INFO: -> {destination_path}")
            context.run(f"mkdir -p {destination_path.parent}")

            docker_context: Optional[pathlib.Path] = None
            with context.cd(destination_path.parent):
                docker_context = project_root_relative(context)

            template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(name=str(item))
            args = {
                'party_id': cfg_party['party_id'],
                'docker_image_tag': docker_image_tag,
                'docker_context': str(docker_context),
                'envs': update_dict_values(copy.deepcopy(env_locations), lambda p: os.path.relpath(p, destination_path.parent)),
            }
            result = template.render(args)
            with open(destination_path, 'w') as f:
                f.write(result)

    # generate docker-compose.yml for services which is outside party
    for item in templates_root_dir.glob('**/docker-compose.bts.yaml.jinja'):
        item = item.relative_to(templates_root_dir)

        destination_path = pathlib.Path('./.output') / 'docker-compose/others/beaver-triple-service' / 'docker-compose.yaml'
        destination_path = destination_path.resolve()
        print(f"INFO: -> {destination_path}")
        context.run(f"mkdir -p {destination_path.parent}")

        docker_context: Optional[pathlib.Path] = None
        with context.cd(destination_path.parent):
            docker_context = project_root_relative(context)

        template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(name=str(item))
        args = {
            'docker_context': str(docker_context),
            'envs': update_dict_values(copy.deepcopy(env_locations), lambda p: os.path.relpath(p, destination_path.parent)),
            'qmpc_setting': qmpc_setting,
        }
        result = template.render(args)
        with open(destination_path, 'w') as f:
            f.write(result)


@task
def filter_for_kompose(c, _compose_root_dir='./.output/docker-compose'):
    '''kompose が入力として受け付ける docker-compose.yml ファイルを生成する

    `docker-compose.yml` ファイルへのパス ``compose_path`` を元に
    `kompose` が入力として受け付ける docker-compose.yml ファイルを
    `./kompose/docker-compose.yml` に書き出す。
    `kompose` では対応していないプロパティを削除する。

    Parameters
    ----------
    c
        Context 自動で渡される
    compose_path
        入力となる `docker-compose.yml` ファイルへのパス
    '''

    compose_root_dir = pathlib.Path(_compose_root_dir)
    output_root_dir = pathlib.Path('./.output/kompose')

    for item in compose_root_dir.glob('**/docker-compose.yaml'):
        if item.is_dir():
            continue

        yaml = YAML()
        yaml.preserve_quotes = True
        target = None
        with open(item) as f:
            target = yaml.load(f)

        services = target['services']
        for svc in services.values():
            def del_key_if_exists(src: Dict, path: Iterable[str]):
                if len(path) == 0:
                    return

                if path[0] not in src:
                    return
                if len(path) == 1:
                    del src[path[0]]
                    return

                del_key_if_exists(src[path[0]], path[1:])

            del_key_if_exists(svc, ['build'])
            del_key_if_exists(svc, ['depends_on'])
            del_key_if_exists(svc, ['healthcheck', 'start_period'])

            # 異なるネットワークからのトラフィックを弾くような
            # NetworkPolicy が生成され、Client -> envoy まで届かなくなるため
            # `networks` key を削除する
            del_key_if_exists(svc, ['networks'])

        # 同様に NetworkPolicy が生成されないように削除
        del_key_if_exists(target, ['networks'])

        output_path = output_root_dir / item.relative_to(compose_root_dir)

        c.run(f"mkdir -p {output_path.parent}")
        with open(output_path, mode='w') as f:
            yaml.dump(target, f)


@task
def generate_k8s_manifests(c, _compose_root_dir='./.output/kompose'):
    # type: (Context, str) -> None
    '''kompose を用いて k8s マニフェストを生成する

    Parameters
    ----------
    c
        Context 自動で渡される
    compose_path
        入力となる `docker-compose.yml` ファイルへのパス
    '''

    compose_root_dir = pathlib.Path(_compose_root_dir)
    output_root_dir = pathlib.Path('./.output/komposed')

    for item in compose_root_dir.glob('**/docker-compose.yaml'):
        if item.is_dir():
            continue

        output_path = output_root_dir / item.relative_to(compose_root_dir)

        c.run(f"mkdir -p {output_path.parent}")

        c.run(rf"""kompose convert -v -f {item.resolve()} \
                    --volumes hostPath \
                    --with-kompose-annotation=false \
                    -o {output_path.parent}
                """)


@task
def replace_k8s_host_path(c, manifests_dir='./.output/komposed'):
    # type: (Context, str) -> None
    '''k8s マニフェストの hostPath を変更する

    kompose で変換した k8s マニフェストに記載される設定ファイルへのパスは
    スクリプトを実行したマシンでの絶対パスに設定されるため
    `/opt/QuickMPC` 以下になるように置き換える

    Parameters
    ----------
    c
        Context 自動で渡される
    manifests_dir
        k8s マニフェストが置かれているディレクトリ
    '''

    # Config ファイルへの path を取得
    config_info: Optional[Dict] = None
    with open('./.output/env-locations.json') as f:
        config_info = json.load(f)

    candidates: list[str] = [config_info['__path__']]
    for party_id in config_info['parties']:
        if party_id == '__path__':
            continue
        candidates += [config_info['parties'][party_id]['__path__']]
    candidates += [config_info['others']['__path__']]

    def find_deepest_path(src: str):
        ret: Optional[str] = None
        for cand in candidates:
            if src.find(cand) != 0:
                continue
            if not ret or len(ret) < len(cand):
                ret = cand
        return ret

    yaml = YAML()
    yaml.preserve_quotes = True

    _manifests_dir = pathlib.Path(manifests_dir)
    for manifest_path in _manifests_dir.glob('**/*.yaml'):
        manifest: Optional[YAML] = None
        with open(manifest_path) as f:
            manifest = yaml.load(f)

        manifest = replace(
            manifest,
            ('hostPath', 'path'),
            lambda src: src.replace(find_deepest_path(src), '/opt/QuickMPC/config')
        )

        with open(manifest_path, mode='w') as f:
            yaml.dump(manifest, f)


@task
def remove_network_policy(c, manifests_dir='./.output/komposed'):
    # type: (Context, str) -> None
    '''
    '''

    yaml = YAML()
    yaml.preserve_quotes = True

    _manifests_dir = pathlib.Path(manifests_dir)
    for manifest_path in _manifests_dir.glob('**/*.yaml'):
        manifest: Optional[YAML] = None
        with open(manifest_path) as f:
            manifest = yaml.load(f)

        if manifest['kind'] == 'NetworkPolicy':
            manifest_path.unlink()


@task
def tidy_up_manifests(c, manifests_dir='./.output/komposed'):
    # type: (Context, str) -> None
    '''
    '''
    manifests_root_dir = pathlib.Path(manifests_dir)
    output_root_dir = pathlib.Path('./.output/manifests')
    c.run(f"mkdir -p {output_root_dir}")

    # copy "others" manifests as-is
    shutil.copytree(manifests_root_dir / 'others', output_root_dir / 'others', dirs_exist_ok=True)

    # copy common manifests of "party"
    output_parties_dir = output_root_dir / 'parties'
    output_parties_common = output_parties_dir / 'common'
    c.run(f"mkdir -p {output_parties_common}")

    yaml = YAML()
    yaml.preserve_quotes = True

    for manifest_path in manifests_root_dir.glob('parties/1/*.yaml'):
        manifest: Optional[YAML] = None
        with open(manifest_path) as f:
            manifest = yaml.load(f)

        if manifest['kind'] == 'ConfigMap':
            continue

        shutil.copy2(manifest_path, output_parties_common)

    # copy ConfigMap resources
    manifests_parties_dir = manifests_root_dir / 'parties'
    for manifest_path in manifests_parties_dir.glob('**/*.yaml'):
        manifest: Optional[YAML] = None
        with open(manifest_path) as f:
            manifest = yaml.load(f)

        if manifest['kind'] != 'ConfigMap':
            continue

        output_path = output_parties_dir / manifest_path.relative_to(manifests_parties_dir)

        c.run(f"mkdir -p {output_path.parent}")

        shutil.copy2(manifest_path, output_path)

    # rename ConfigMap key name
    def rename(src: str):
        pattern = re.compile(r'parties-\d+?-')
        return re.sub(pattern, '', src)

    for manifest_path in output_root_dir.glob('**/*.yaml'):
        manifest: Optional[YAML] = None
        with open(manifest_path) as f:
            manifest = yaml.load(f)

        manifest = replace(
            manifest,
            ('name',),
            rename
        )

        with open(manifest_path, mode='w') as f:
            yaml.dump(manifest, f)


@task
def transfer_files(c, destination, manifests_dir='./manifests', config_dir='./config'):
    # type: (Context, str, str, str) -> None
    '''k8s マニフェストと設定ファイルをデプロイ先にコピーする

    ``manifests_dir`` と ``config_dir`` を圧縮し、
    ``destination`` に送信、解凍する。
    k8s マニフェストは指定したユーザーのホームディレクトリに、
    設定ファイルは `/opt/QuickMPC/Config` ディレクトリに展開される。

    Note
    ----
    `/opt/QuickMPC` ディレクトリは作成済みで、
    ログインするユーザーによる書込の権限がある状態であること。

    Parameters
    ----------
    c
        Context 自動で渡される
    destination
        - デプロイ先
        - 例) `'ubuntu@ec2-13-215-46-105.ap-southeast-1.compute.amazonaws.com'`
    manifests_dir
        k8s マニフェストが置かれているディレクトリ
    config_dir
        設定ファイルが置かれているディレクトリ
    '''

    output_dir = pathlib.Path('./.output')
    manifests_archive_filename = pathlib.Path('manifests.tar.gz')
    config_archive_filename = pathlib.Path('config.tar.gz')
    manifests_archive_path = pathlib.Path(output_dir / manifests_archive_filename)
    config_archive_path = pathlib.Path(output_dir / config_archive_filename)

    # archive files
    c.run(f"tar zcvf {str(manifests_archive_path)} {manifests_dir}")
    c.run(f"tar zcvf {str(config_archive_path)} -C {config_dir} .")

    # upload archived files
    conn = fabric.Connection(destination)
    transfer = fabric.transfer.Transfer(conn)
    transfer.put(f"{str(manifests_archive_path)}", str(manifests_archive_filename))
    transfer.put(f"{str(config_archive_path)}", '/opt/QuickMPC')

    # extract archived files
    conn.run(f"tar zxvf {manifests_archive_filename}")

    with conn.cd('/opt/QuickMPC'):
        conn.run(f"tar zxvf {config_archive_filename} -C Config")
