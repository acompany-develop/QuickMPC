""" QuickMPCによるモデル値予測の動作デモ
本デモコードは次の操作を行っている．
パターン1:
    Step 1. QuickMPC動作のための準備
    Step 2. 予測するデータを送信
    Step 3. モデルの学習
        Step3-1 学習データをシェア化して送信
        Step3-2 学習リクエストを送りモデルパラメータのIDを取得
    Step 4. モデル値予測リクエストを送信
パターン2:
    Step 1. QuickMPC動作のための準備
    Step 2. 予測するデータを送信
    Step 2. 自前のモデルパラメータを送信
    Step 3. モデル値予測リクエストを送信
"""
import json
import logging
import time

from utils import make_statuses_detailed

from quickmpc import QMPC

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

""" Step 1. 諸準備 """
# QuickMPC Setting
qmpc: QMPC = QMPC(
    ["http://localhost:9001", "http://localhost:9002", "http://localhost:9003"]
)
# データ Setting
data_dir: str = "../data/"
secrets1, schema1 = qmpc.parse_csv_file(data_dir + "data1-1.csv")
secrets2, schema2 = qmpc.parse_csv_file(data_dir + "data1-2.csv")

""" Step2. 予測するデータを送信 """
logger.info("---- send_share start ----")
send_res1 = qmpc.send_share(secrets1, schema1)
if not send_res1["is_ok"]:
    raise
data_id1 = send_res1["data_id"]
logger.info(f"data_id1: {data_id1}")

send_res2 = qmpc.send_share(secrets2, schema2)
if not send_res2["is_ok"]:
    raise
data_id2 = send_res2["data_id"]
logger.info(f"data_id2: {data_id2}")
logger.info("---- send_share success ----\n")


def pattern1():
    """ Step 3-1 学習データをシェア化して送信 """
    # Demoでは学習データと予測したいデータを同じとするため省略

    """ Step 3-2. data_idに対してモデルの学習リクエスト送信 """
    data_ids = [data_id1, data_id2]
    # 学習モデルを追加する場合はここに追加する
    # 形式: [モデル学習Job，モデル関数，指定する列，真値]
    exe_set = [
        [qmpc.linear_regression, qmpc.linear_regression_predict,
         [[2, 3, 4], [5]], [s[0] for s in secrets2]],
        [qmpc.logistic_regression, qmpc.logistic_regression_predict,
         [[2, 3, 4], [6]], [s[1] for s in secrets2]]
    ]
    for exe_func, model_func, inp, true_val in exe_set:
        logger.info(f"---- {exe_func.__name__} fit start ----")
        exec_res = exe_func([data_ids, [0], [1, 1]], inp)

        model_param_job_uuid = exec_res["job_uuid"]
        logger.info(f"model_param_job_uuid: {model_param_job_uuid}")
        # DBに計算結果が書き込まれていないと思うので1秒待機
        time.sleep(1)

        param = []
        for _ in range(100):
            # 計算が終わるか100秒(1秒x100回)経つまで繰り返す
            get_res = qmpc.get_computation_result(model_param_job_uuid)
            if get_res["results"] is not None:
                param = get_res["results"]
                break

            statuses = make_statuses_detailed(get_res['statuses'])
            logger.info(f"statuses: {statuses}")

            time.sleep(1)

        if not param:
            logger.info(f"---- {exe_func.__name__} failed ----")
            continue

        logger.info(f"model parametes: {param}")
        logger.info(f"---- {exe_func.__name__} success ----")

        """ Step 4. モデル値予測リクエストを送信 """
        logger.info(f"---- {model_func.__name__} predict start ----")
        pred_res = model_func(model_param_job_uuid, [
                              [data_id1], [], [1]], inp[0])

        """ Step 5. 結果を取得 """
        job_uuid = pred_res["job_uuid"]
        logger.info(f"job_uuid: {job_uuid}")
        # DBに計算結果が書き込まれていないと思うので1秒待機
        time.sleep(1)

        for _ in range(100):
            # 計算が終わるか100秒(1秒x100回)経つまで繰り返す
            get_res = qmpc.get_computation_result(job_uuid)
            if get_res["results"] is not None:
                result_secrets = get_res["results"]
                logger.info(f"predict value: {result_secrets}")
                logger.info(f"   true value: {true_val}")

                logger.info(f"---- {model_func.__name__} predict success ----")
                break

            statuses = make_statuses_detailed(get_res['statuses'])
            logger.info(f"statuses: {statuses}")

            time.sleep(1)


def pattern2():
    """ Step 3. 学習済みのモデルパラメータを送信 """
    # モデルを追加する場合はここに追加する
    # 形式: [モデル関数，モデルパラメータ，真値]

    data2_1 = []
    with open(f"{data_dir}/data2-1.json") as f:
        data2_1 = json.load(f)

    exe_set = [
        [qmpc.linear_regression_predict,
         [-3.9106383, 12.81276596, 4.74468085, -3.30212766],
         [s[0] for s in secrets2]],
        [qmpc.logistic_regression_predict,
         [5.44821378, -0.67505836, -0.30945274, -0.72291515],
         [s[1] for s in secrets2]],
        [qmpc.decision_tree_predict,
         data2_1,
            [s[1] for s in secrets2]],
    ]
    for model_func, model_param, true_val in exe_set:
        logger.info("---- send model parametes start ----")
        send_res = qmpc.send_model_params(model_param)
        if not send_res["is_ok"]:
            raise
        model_param_job_uuid = send_res["job_uuid"]
        logger.info(f"model_param_job_uuid: {model_param_job_uuid}")
        logger.info("---- send model parametes success ----\n")

        """ Step 4. モデル値予測リクエストを送信 """
        logger.info(f"---- {model_func.__name__} predict start ----")
        pred_res = model_func(model_param_job_uuid, [
                              [data_id1], [], [1]], [2, 3, 4])

        """ Step 5. 結果を取得 """
        job_uuid = pred_res["job_uuid"]
        logger.info(f"job_uuid: {job_uuid}")
        # DBに計算結果が書き込まれていないと思うので1秒待機
        time.sleep(1)

        for _ in range(100):
            # 計算が終わるか100秒(1秒x100回)経つまで繰り返す
            get_res = qmpc.get_computation_result(job_uuid)
            if get_res["results"] is not None:
                result_secrets = get_res["results"]
                logger.info(f"predict value: {result_secrets}")
                logger.info(f"   true value: {true_val}")

                logger.info(f"---- {model_func.__name__} predict success ----")
                break

            statuses = make_statuses_detailed(get_res['statuses'])
            logger.info(f"statuses: {statuses}")

            time.sleep(1)


if __name__ == '__main__':
    pattern1()
    pattern2()
