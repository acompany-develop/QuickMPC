import io
import os
import pathlib
from typing import Callable, Iterable, Dict, Union, Optional
import json
from invoke import task, Context
import fabric
import fabric.transfer

from ruamel.yaml import YAML
import jinja2


def project_root_relative(context: Context) -> pathlib.Path:
    with io.StringIO() as stdout:
        context.run(
            'git rev-parse --show-superproject-working-tree --show-toplevel | head -1',
            out_stream=stdout)
        root = pathlib.Path(stdout.getvalue().strip())
        cwd = pathlib.Path(context.cwd).resolve()
        return pathlib.Path(os.path.relpath(str(root), str(cwd)))


def update_dict(destination: Dict, path: Iterable, value: str) -> Dict:
    if len(path) == 0:
        return {
            '__path__': value,
        }
    else:
        item = update_dict(
            destination
            .get(path[0], {}),
            path[1:], value)
        destination[path[0]] = item
        destination['__path__'] = str(pathlib.Path(item['__path__']).parent)
        return destination


def update_dict_values(iter: Union[Dict, str], func: Callable[[str], str]):
    if isinstance(iter, str):
        return func(iter)
    else:
        for key in iter.keys():
            iter[key] = update_dict_values(iter[key], func)
        return iter


@task
def generate_docker_compose(c, party_num=3, docker_image_tag=''):
    # type: (Context, int, str) -> None
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
        - 例) `':s20220608'`
    '''

    templates_root_dir = pathlib.Path(c.cwd) / pathlib.Path('./templates')
    print(templates_root_dir.resolve())
    loader = jinja2.FileSystemLoader(str(templates_root_dir))

    env_locations = None
    with open('./.output/env-locations.json') as f:
        env_locations = json.load(f)

    for item in templates_root_dir.glob('**/docker-compose.yml.jinja'):
        if item.is_dir():
            continue
        item = item.relative_to(templates_root_dir)
        print(f"INFO: Template file is {item.resolve()}")

        destination_path = templates_root_dir.parent / 'docker-compose' / item
        if destination_path.suffix == '.jinja':
            destination_path = destination_path.with_suffix('')
        destination_path = destination_path.resolve()
        print(f"INFO: -> {destination_path}")
        c.run(f"mkdir -p {destination_path.parent}")

        docker_context: pathlib.Path = None
        with c.cd(destination_path.parent):
            docker_context = project_root_relative(c)

        template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(name=str(item))
        args = {
            'party_num': party_num,
            'docker_image_tag': docker_image_tag,
            'docker_context': str(docker_context),
            'envs': update_dict_values(env_locations, lambda p: os.path.relpath(p, destination_path.parent)),
        }
        result = template.render(args)
        with open(destination_path, 'w') as f:
            f.write(result)


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
def generate_config(c, party_num=3):
    # type: (Context, int) -> None
    '''各パーティの設定ファイルを生成

    `./templates/config` 以下のファイルを元に
    `./config/party*` を生成する。
    各ファイル、ディレクトリへの絶対パスを記載した `.output/env-locations.json` を書き出す。

    Parameters
    ----------
    c
        Context 自動で渡される
    party_num
        MPC の Party 数
    '''

    templates_root_dir = pathlib.Path(c.cwd) / pathlib.Path('./templates')
    print(templates_root_dir.resolve())
    loader = jinja2.FileSystemLoader(str(templates_root_dir))

    output = dict()

    for item in templates_root_dir.glob('config/**/*'):
        if item.is_dir():
            continue
        item = item.relative_to(templates_root_dir)
        print(f"INFO: Template file is {item.resolve()}")

        for i in range(party_num):
            dst_sub_path = item
            if dst_sub_path.suffix == '.jinja':
                dst_sub_path = dst_sub_path.with_suffix('')
            destination_path = templates_root_dir.parent / f"config/party{i}" / dst_sub_path
            destination_path = destination_path.resolve()
            print(f"INFO: -> {destination_path}")
            c.run(f"mkdir -p {destination_path.parent}")

            update_dict(output, (i,) + dst_sub_path.parts, str(destination_path))

            template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(name=str(item))
            args = {
                'party_num': party_num,
                'party_id': i + 1,
            }
            result = template.render(args)
            with open(destination_path, 'w') as f:
                f.write(result)

    c.run('mkdir -p ./.output')
    with open('./.output/env-locations.json', mode='w') as f:
        json.dump(output, f, indent=2)


@task
def filter_for_kompose(c, compose_path='./docker-compose/docker-compose.yml'):
    # type: (Context, str) -> None
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

    yaml = YAML()
    yaml.preserve_quotes = True
    target = None
    with open(compose_path) as f:
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

    c.run('mkdir -p ./kompose')
    with open('./kompose/docker-compose.yml', mode='w') as f:
        yaml.dump(target, f)


@task
def generate_k8s_manifests(c, compose_path='./kompose/docker-compose.yml'):
    # type: (Context, str) -> None
    '''kompose を用いて k8s マニフェストを生成する

    Parameters
    ----------
    c
        Context 自動で渡される
    compose_path
        入力となる `docker-compose.yml` ファイルへのパス
    '''

    c.run('mkdir -p ./manifests')
    c.run(f"kompose convert -v -f {compose_path}"
          + ' --volumes hostPath'
          + ' -o ./manifests')


@task
def replace_k8s_host_path(c, manifests_dir='./manifests'):
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
            lambda src: src.replace(config_info['__path__'], '/opt/QuickMPC/Config')
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
