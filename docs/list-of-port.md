# QuickMPCで使用するポート

## ポート番号一覧

|  送信元      |  送信先      |  宛先ポート      |  プロトコル      |  プロトコルバージョン  |  備考      |
|  :--------  |  :--------  |  :-----------  |  :------------  |  :---------------  |  :------- |
|  [libclient](https://github.com/acompany-develop/QuickMPC/tree/main/packages/client/libclient-py#readme)  |  [manage_envoy_container](https://github.com/acompany-develop/QuickMPC/blob/main/config/manage_container/manage1/envoy.yaml)   |  50000          |  https or http  |  gRPC              |           |
|  [manage_container](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/manage_container#readme)     |  [manage_envoy_container](https://github.com/acompany-develop/QuickMPC/blob/main/config/manage_container/manage1/envoy.yaml)   |  50010         |  https or http  |  gRPC              |           |
|  [manage_envoy_container](https://github.com/acompany-develop/QuickMPC/blob/main/config/manage_container/manage1/envoy.yaml)   |  [manage_container](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/manage_container#readme)     |  50011, 51011  |  http           |  gRPC              |  内部通信  |
|  [manage_container](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/manage_container#readme)     |  [computation_container](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/computation_container#readme)    |  50010         |  http           |  gRPC              |  内部通信  |
|  [computation_container](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/computation_container#readme)    |  [computation_envoy_container](https://github.com/acompany-develop/QuickMPC/blob/main/config/computation_container/compute1/envoy.yaml)   |  50020, 51020  |  https or http  |  gRPC              |           |
|  [computation_envoy_container](https://github.com/acompany-develop/QuickMPC/blob/main/config/computation_container/compute1/envoy.yaml)   |  [computation_container](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/computation_container#readme)    |  50020, 51020  |  http           |  gRPC              |  内部通信  |
|  [computation_container](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/computation_container#readme)    |  [bts_envoy](https://github.com/acompany-develop/QuickMPC/blob/main/config/beaver_triple_service/envoy.yaml)  |  50020         |  https or http  |  gRPC              |           |
|  [bts_envoy](https://github.com/acompany-develop/QuickMPC/blob/main/config/beaver_triple_service/envoy.yaml)  |  [beaver_triple_service](https://github.com/acompany-develop/QuickMPC/tree/main/packages/server/beaver_triple_service#readme)        |  54000         |  http           |  gRPC              |  内部通信  |

## 補足
プロトコルがhttps or htttpの通信に関しては、ssl証明書の管理方法によりプロトコルが異なる。
* ssl証明書を用いない場合→http
* LBでssl終端を持つ場合→http
* envoyコンテナでssl終端を持つ場合→https
