.. _quickstart:

.. _quickmpc-libclient-py: https://github.com/acompany-develop/QuickMPC/tree/main/packages/client/libclient-py
.. _pandas: https://pandas.pydata.org/

Quickstart
==========
本ページでは `quickmpc-libclient-py`_ を用いたMPC(Multi Party Computation)の始め方を説明します．
まだquickmpc-libclient-pyをinstallしていない方は :ref:`Installation <installation>` を参考にinstallをしてください．

csvデータの操作
-------------
quickmpc-libclient-pyは `pandas`_ likeなinterfaceを提供します．
例えばcsvファイルの読み取りはpandasと同じように :ref:`read_csv関数 <quickmpc.pandas>` により実現できます．
また， 返り値はpandas.DataFrameになっており，pandasと同じように加工することができます．

.. code-block:: python3

  import quickmpc.pandas as qpd

  df = qpd.read_csv("data.csv", index_col="ID")
  df = df.applymap(lambda x: x%2)


MPCの開始
---------
:ref:`qmpcクラス <quickmpc>` を使用して，あらかじめ用意した2つ以上のQuickMPCサーバに接続します．

.. code-block:: python3

  import quickmpc

  qmpc = quickmpc.QMPC([
    "http://~~~:~~~",
    "http://~~~:~~~",
    "http://~~~:~~~",
  ])

読み込んだcsvデータを次のようにしてサーバに送信します．

.. code-block:: python3

  sdf = qmpc.send_to(df)

自分以外のユーザが送信したデータは次のようにして取得します．

.. code-block:: python3

  data_id = "~~~~" # 他者が sdf.get_id() により出力したID
  sdf = qmpc.load_from(data_id)

使用するデータをすべてサーバに送信したら各種MPCを実行します．

.. code-block:: python3

  sdf_res = sdf.sum()
  sdf_res = sdf.join(sdf_other)

計算結果の取得
--------------
MPCの結果はファイルに出力するか，pandas.DataFrameに出力して引き続き加工することができます．

.. code-block:: python3

  df = sdf_res.to_csv("filename.csv") # ファイルに出力
  df = sdf_res.to_data_frame() # DataFrameに出力

一部の実行時間の長い計算は `progress` オプションを設定して計算ステータスをログに出力できます．

.. code-block:: python3

  df = sdf_res.to_data_frame(progress=True)
