import json
import pathlib
import sys
import os
import time
from typing import Dict, List, Optional

import jinja2
import invoke
from invoke import task, Context

SLEEP_TIME = 10
COMPLETE_STATUS_SET = {"CREATE_COMPLETE", "UPDATE_COMPLETE", "UPDATE_ROLLBACK_COMPLETE"}


class CustomError(Exception):
    """想定可能な例外をハンドリングする際に使うExceptionクラス

    Attributes:
        message -- エラーの説明
    """

    def __init__(self, message=""):
        self.message = message
        super().__init__(self.message)

    def __str__(self):
        return f'想定されたエラー({self.message})'


class CloudformationStack:
    __stack_name: str
    __stack_json: List[Optional[Dict]]
    __context: Context
    __SLEEP_TIME: int = 10

    def __init__(self, context: Context, stack_name: str, party_num: int):
        self.__stack_name = stack_name
        self.__context = context
        self.__stack_json = [dict()] * party_num
        for id in range(party_num):
            self.__load_json(id, reload=True)

    def __load_json(self, id: int, reload=True) -> None:
        if reload:
            try:
                r: invoke.Result = self.__context.run(f"aws cloudformation describe-stacks --stack-name {self.__stack_name+str(id)} --output json")
                self.__stack_json[id] = json.loads(r.stdout)
            except BaseException as e:
                if "Encountered a bad command exit code!" in str(e):
                    self.__stack_json[id] = None
                    return
                raise e

    def create_or_update_stack(self, id: int, domain_prefix: str = "staging", wait=True) -> bool:
        sub_command = "update-stack" if self.is_created(id) else "create-stack"
        try:
            if self.__stack_name == "compose-infrastructure":
                with self.__context.cd("../Infrastructure"):
                    self.__context.run(f"\
                        aws cloudformation {sub_command} \
                            --stack-name {self.__stack_name+str(id)} \
                            --template-body file://cloudformation.yaml \
                            --capabilities CAPABILITY_IAM")

            elif self.__stack_name == "compose-dns":
                args: Dict[str]
                with open(f'./outputs/get_infra_ids{id}.json', 'r') as f:
                    args = json.load(f)

                with self.__context.cd("../DNS"):
                    self.__context.run(f"\
                        aws cloudformation {sub_command} \
                            --stack-name {self.__stack_name+str(id)} \
                            --template-body file://cloudformation.yaml \
                            --capabilities CAPABILITY_IAM \
                            --parameters \
                            ParameterKey=ExistingAwsVpc,ParameterValue={args['VpcId']} \
                            ParameterKey=ExistingEIP,ParameterValue={args['PublicEIP']} \
                            ParameterKey=ExistingLoadbalancer,ParameterValue={args['LoadbalancerId']} \
                            ParameterKey=ExistingLoadbalancerDNSName,ParameterValue={args['LoadbalancerDNSName']} \
                            ParameterKey=ExistingLoadbalancerZoneId,ParameterValue={args['LoadbalancerZoneId']} \
                            ParameterKey=RecordName,ParameterValue={domain_prefix+str(id)}")

            elif self.__stack_name == "compose-pipeline":
                args: Dict[str]
                cert_id: str
                with open(f'./outputs/get_infra_ids{id}.json', 'r') as f:
                    args = json.load(f)
                with open(f'./outputs/alb_cert{id}.json', 'r') as f:
                    cert_id = json.load(f)['CertId']

                with self.__context.cd("../Pipeline"):
                    self.__context.run(f"\
                        aws cloudformation {sub_command} \
                            --stack-name {self.__stack_name+str(id)} \
                            --template-body file://cloudformation.yaml \
                            --capabilities CAPABILITY_IAM \
                            --parameters \
                            ParameterKey=PartyId,ParameterValue={str(id)} \
                            ParameterKey=ExistingAwsVpc,ParameterValue={args['VpcId']} \
                            ParameterKey=ExistingPrivateSubnet,ParameterValue={args['PrivateSubnet']} \
                            ParameterKey=ExistingEcsCluster,ParameterValue={args['ClusterName']} \
                            ParameterKey=ExistingCert,ParameterValue={cert_id} \
                            ParameterKey=ApplicationStackName,ParameterValue=compose-application{str(id)}")

            else:
                raise CustomError("Unexpected stack_name")
        except BaseException as e:
            if "Encountered a bad command exit code!" in str(e):
                return False
            raise e
        if wait:
            if sub_command == "create-stack":
                self.__wait_until_complete(id, 'create')
            else:
                self.__wait_until_complete(id, 'update')
        return True

    def delete_stack(self, id: int, wait=True, reload=False) -> None:
        if reload:
            self.__load_json(id)
        self.__context.run(f"aws cloudformation delete-stack --stack-name {self.__stack_name+str(id)}")
        if wait:
            self.__wait_until_complete(id, 'delete')

    def __wait_until_complete(self, id: int, command_name: str):
        command_name = command_name.upper()
        while True:
            status: str = self.get_stack_status(id, reload=True)
            if status == f"{command_name}_IN_PROGRESS":
                print(f"INFO: StackStatus is {status}, sleep {self.__SLEEP_TIME}s...")
                time.sleep(self.__SLEEP_TIME)
            elif status == f"{command_name}_COMPLETE_CLEANUP_IN_PROGRESS":
                print(f"INFO: StackStatus is {status}, sleep {self.__SLEEP_TIME}s...")
                time.sleep(self.__SLEEP_TIME)
            elif status == f"{command_name}_FAILED":
                raise CustomError(f"create {self.__stack_name} stack failed")
            elif status == f"{command_name}_COMPLETE":
                return
            elif status == "NONE":
                return
            else:
                raise CustomError(f"{command_name} {self.__stack_name} stack failed with unexpected status: {status}")

    def get_stack_status(self, id: int, reload=False) -> Optional[str]:
        self.__load_json(id, reload)
        if self.__stack_json[id] is None:
            return "NONE"
        status = self.__stack_json[id].get("Stacks", [{}])[0].get("StackStatus", "None")
        return status

    def get_stack_outputs(self, output_keys: List[str], id: int, reload=False) -> Dict[str, str]:
        self.__load_json(id, reload)
        outputs: Optional[List] = self.__stack_json[id].get("Stacks", [{}])[0].get("Outputs", None)
        if outputs is None:
            raise CustomError("StackOutputs is None")

        results: Dict[str, Optional[str]] = {output_key: None for output_key in output_keys}
        for output in outputs:
            if output["OutputKey"] in output_keys:
                results[output["OutputKey"]] = output["OutputValue"]
                if output["OutputValue"] is None:
                    raise CustomError(f"StackOutputs {output['OutputValue']} is None")
        return results

    def is_created(self, id) -> bool:
        if self.__stack_json[id] is None:
            return False
        status: str = self.get_stack_status(id, reload=False)
        if status in COMPLETE_STATUS_SET:
            return True
        else:
            raise CustomError(f"create {self.__stack_name+str(id)} stack failed with unexpected status: {status}")

    def update_security_group(self, party_num: int):
        ip_list: List[str] = []
        sg_list: List[str] = []
        for id in range(party_num):
            with open(f'./outputs/get_infra_ids{id}.json', 'r') as f:
                args = json.load(f)
                ip_list.append(args['PublicEIP'])
                sg_list.append(args['SecurityGroup'])

        # Security Groupのルールに全パーティのIPを追加
        for sg in sg_list:
            for ip in ip_list:
                self.__context.run(f"\
                        aws ec2 authorize-security-group-ingress \
                            --group-id {sg} \
                            --protocol tcp \
                            --port 0-65535 \
                            --cidr {ip}/32")


@task
def create_infra(c, party_num=3, wait=True):
    # type: (Context, int, bool) -> None
    """デプロイに必要なAWSインフラリソースの作成タスク

    Attributes:
        c -- invoke.Context 自動で渡される
        party_num -- MPC の Party 数
        wait -- 実行完了までブロッキングする稼働か否かのフラグ

    Informations:
        - [Docker ComposeとAmazonECSのチュートリアル実行]
          (Docs/docker-compose-and-ecs-tutorial-run.md)
    """
    infra_stack = CloudformationStack(c, "compose-infrastructure", party_num)
    for id in range(party_num):
        infra_stack.create_or_update_stack(id, wait)


@task
def get_infra_ids(c, party_num=3):
    # type: (Context, int) -> None
    """create_infraで作成したAWSインフラリソースのIDを取得するタスク

    Attributes:
        c -- invoke.Context 自動で渡される
        party_num -- MPC の Party 数

    Informations:
        - [Docker ComposeとAmazonECSのチュートリアル実行]
          (Docs/docker-compose-and-ecs-tutorial-run.md)
    """
    try:
        infra_stack = CloudformationStack(c, "compose-infrastructure", party_num)

        for id in range(party_num):
            print(f"INFO: StackStatus is {infra_stack.get_stack_status(id, reload=False)}")
            results = infra_stack.get_stack_outputs(["VpcId", "PublicEIP", "PrivateSubnet", "SecurityGroup", "ClusterName",
                                                    "LoadbalancerId", "LoadbalancerEndpoint", "LoadbalancerDNSName", "LoadbalancerZoneId"], id, reload=False)

            os.makedirs('./outputs', exist_ok=True)

            if results is not None:
                with open(f'./outputs/get_infra_ids{id}.json', 'w') as f:
                    json.dump(results, f, indent=4)

            print(f"INFO: {sys._getframe().f_code.co_name} Done")

    except CustomError as e:
        raise e

    except BaseException as e:
        raise e


@task
def update_security_group(c, party_num=3):
    # type: (Context, bool) -> None
    """create_infraで作成したSecurity Groupのルールを更新するタスク

    Attributes:
        c -- invoke.Context 自動で渡される

    Informations:
        - [Docker ComposeとAmazonECSのチュートリアル実行]
          (Docs/docker-compose-and-ecs-tutorial-run.md)
    """
    try:
        infra_stack = CloudformationStack(c, "compose-infrastructure", party_num)
        infra_stack.update_security_group(party_num)
    except CustomError as e:
        raise e

    except BaseException as e:
        raise e


@task
def create_certs(c, party_num=3, domain_prefix="staging", wait=True):
    """create_infraで作成したALBの証明書を発行するタスク

    Attributes:
        c -- invoke.Context 自動で渡される

    Informations:
        - [Docker ComposeとAmazonECSのチュートリアル実行]
          (Docs/docker-compose-and-ecs-tutorial-run.md)
    """
    try:
        dns_stack = CloudformationStack(c, "compose-dns", party_num)
        for id in range(party_num):
            dns_stack.create_or_update_stack(id, domain_prefix, wait)
            results = dns_stack.get_stack_outputs(["CertId"], id, reload=False)

            os.makedirs('./outputs', exist_ok=True)

            if results is not None:
                with open(f'./outputs/alb_cert{id}.json', 'w') as f:
                    json.dump(results, f, indent=4)

    except CustomError as e:
        raise e

    except BaseException as e:
        raise e


@task
def create_pipeline(c, party_num=3, wait=True):
    # type: (Context, int, bool) -> None
    """applicationをデプロイするために必要なAWS CodePipelineの作成タスク

    Attributes:
        c -- invoke.Context 自動で渡される
        party_num -- MPC の Party 数
        wait -- 実行完了までブロッキングするか否かのフラグ

    Informations:
        - [Docker ComposeとAmazonECSのチュートリアル実行]
          (Docs/docker-compose-and-ecs-tutorial-run.md)
    """
    try:
        pipeline_stack = CloudformationStack(c, "compose-pipeline", party_num)
        for id in range(party_num):
            pipeline_stack.create_or_update_stack(id, wait)
            results = pipeline_stack.get_stack_outputs(["S3BucketName"], id, reload=False)

            os.makedirs('./outputs', exist_ok=True)

            if results is not None:
                with open(f'./outputs/s3_bucket{id}.json', 'w') as f:
                    json.dump(results, f, indent=4)

        print(f"INFO: {sys._getframe().f_code.co_name} Done")

    except BaseException as e:
        raise e


@task
def generate_configs(c, party_num=3, domain_prefix='staging', bts_url=None):
    # type: (Context, int, str, Optional[str]) -> None
    """各パーティの設定ファイルを生成する

    Attributes:
        c -- invoke.Context 自動で渡される
        party_num -- MPC の Party 数
        domain_prefix -- {domain_prefix}[0-9]+.qmpc.run に変換される
        bts_url -- BeaverTripleService への URL
    """

    with c.cd('../Application/Config'):
        templates_root_dir = pathlib.Path('../Application/Config/Templates')
        loader = jinja2.FileSystemLoader(str(templates_root_dir))
        for item in templates_root_dir.glob('**/*'):
            if item.is_dir():
                continue
            item = item.relative_to(templates_root_dir)
            print(f"INFO: Template file is {item.resolve()}")
            template = jinja2.Environment(loader=loader, keep_trailing_newline=True).get_template(name=str(item))
            for i in range(party_num):
                # TODO: BTS 接続先を渡せるようにする
                args = {
                    'party_num': party_num,
                    'domain_prefix': domain_prefix,
                    'party_id': i + 1,
                    'bts_url': bts_url,
                }
                result = template.render(args)
                dst = templates_root_dir.parent / f"Party{i}" / item
                if dst.suffix == '.jinja':
                    dst = dst.with_suffix('')
                dst = dst.resolve()
                print(f"INFO: -> {dst} Done")
                c.run(f"mkdir -p {dst.parent}")
                with open(dst, 'w') as f:
                    f.write(result)


@task
def deploy_to_s3(c, party_num=3):
    # type: (Context, int) -> None
    """applicationをデプロイするために S3 に upload する

    Attributes:
        c -- invoke.Context 自動で渡される
        party_num -- MPC の Party 数

    Informations:
        - [Docker ComposeとAmazonECSのチュートリアル実行]
          (Docs/docker-compose-and-ecs-tutorial-run.md)
    """

    with c.cd("../../../"):
        # git 管理下のファイルのみをアーカイブ
        c.run("echo \"$(cd QuickMPC && git ls-files)\" | awk '{print \"QuickMPC/\"$0}' | zip -@ ./QuickMPC/Deploy/Application/compose-bundle.zip")
        # Config ファイルを追加
        c.run("find QuickMPC/Deploy/Application/Config -type f | zip -@ --update ./QuickMPC/Deploy/Application/compose-bundle.zip")

    s3_bucket: str

    c.cd("./QuickMPC/Deploy/Scripts")
    for id in range(party_num):
        with open(f'./outputs/s3_bucket{id}.json', 'r') as f:
            s3_bucket = json.load(f)['S3BucketName']

        with c.cd("../Application"):
            c.run(f"aws s3 cp compose-bundle.zip s3://{s3_bucket}/compose-bundle.zip")

    with c.cd("../Application"):
        c.run("rm compose-bundle.zip")  # 後片付け


@task
def delete_all(c, party_num=3):
    # type: (Context, int) -> None
    """demo application のため作った環境を全て削除する
    Attributes:
        c -- invoke.Context 自動で渡される
        party_num -- MPC の Party 数
    Informations:
        - [Docker ComposeとAmazonECSのチュートリアル実行]
          (Docs/docker-compose-and-ecs-tutorial-run.md)
    """

    try:
        application_stack = CloudformationStack(c, "compose-application", party_num)
        infra_stack = CloudformationStack(c, "compose-infrastructure", party_num)
        pipeline_stack = CloudformationStack(c, "compose-pipeline", party_num)

        for id in range(party_num):
            application_stack.delete_stack(id, True)
            pipeline_outputs = pipeline_stack.get_stack_outputs(
                ['S3BucketName', 'CcEcrName', 'McEcrName', 'McEnvoyEcrName', 'DbgEcrName', 'ShareDbEcrName', 'SsEcrName'],
                id, reload=False)
            bucket_name = pipeline_outputs['S3BucketName']
            ecr_repos = []
            ecr_repo_names = ['CcEcrName', 'McEcrName', 'McEnvoyEcrName', 'DbgEcrName', 'ShareDbEcrName', 'SsEcrName']
            for name in ecr_repo_names:
                ecr_repos.append(pipeline_outputs[name])
            print(pipeline_outputs)

            c.run(f"aws s3api delete-objects \
                    --bucket {bucket_name} --delete \
                    \"$(aws s3api list-object-versions \
                    --bucket \"{bucket_name}\" \
                    --output=json \
                    --query='{{Objects: Versions[].{{Key:Key,VersionId:VersionId}}}}')\"")

            c.run(f"aws s3 rb s3://{bucket_name}")
            for ecr_repo in ecr_repos:
                c.run(f"aws ecr delete-repository --repository-name {ecr_repo} --force")
            pipeline_stack.delete_stack(id, True)
            infra_stack.delete_stack(id, True)

    except BaseException as e:
        raise e
