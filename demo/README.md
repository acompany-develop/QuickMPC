# QuickMPCデモ実行手順

## QuickMPCを構築する

### local環境にQuickMPCを立ち上げる場合
1. QuickMPC/scriptsディレクトリに移動する
    ```
    (カレントディレクトリが本ディレクトリの時)
    cd ../scripts
    ```
2. scriptsディレクトリ内で以下のコマンドを実行する
    ```
    make debug
    ```
### GCP環境にQuickMPCを立ち上げる場合
1. `./terraform`ディレクトリ内の[README.md](terraform/README.md)を参照の上、GCP環境を立ち上げる
2. dockerイメージを設定する <br>
    この[ページ](https://github.com/acompany-develop/QuickMPC/tags)の最新のtagを[variables.tf](./terraform/application/vaiables.tf)の`docker_image_tag`に設定します。
    ```
    ...
    variable "docker_image_tag" {
       default = "{tag}" # {tag}を書き換える
    }
    ```
3. `./terraform`ディレクトリ内にて以下のコマンドを実行する
    ```
    make deploy-quickmpc
    ```
※ 本来FireWallの設定などで50000番ポートを開ける必要がありますが、コマンドを実行した端末のIPからのリクエストはterraformにて設定していますのでそのまま計算を実行できます
### その他のインフラ環境でQuickMPCを立ち上げる場合
以下ではインフラ環境を3台用意した場合を例に挙げて説明します。
※ 事前にそれぞれの環境にssh接続しておきます
1. それぞれのインフラ環境にに本リポジトリをcloneする
    ```
    git clone https://github.com/acompany-develop/QuickMPC
    ```
2. `QuickMPC/demo/deploy`ディレクトリに移動する
    ```
    cd QuickMPC/demo/deploy
    ```
3. QuickMPCを立ち上げる用の環境を構築する
    以下のコマンドを実行し、Docker,docker-compose,GNU Makeをinstallします。
    ```
    sh setup.sh
    ```
    ※ sudoを使わずdockerを使えるようにするためにOSの再起動、もしくは一度ログアウトする必要があります。
4. 環境変数を設定します <br>
    以下のコマンドを実行します
    ```
    sh prepare_deploy.sh {party_num} {ip_list} {container image tag}
    ```
    |  名前  |  説明(例)  |
    | ----  | ---- |
    |  party_num  |  0-indexでサーバの番号(サーバ1上でコマンドを実行するなら0)  |
    |  ip_list  |  各ipをカンマ区切りで文字列にする(127.0.0.1,127.0.0.2,127.0.0.3)  |
    |  container image tag  |  containerイメージのtagを設定する(0.3.8)  |
    conrainerイメージについては、この[ページ](https://github.com/acompany-develop/QuickMPC/tags)の最新のtagを設定します。
5. JSON Web Token(以下JWT)を他パーティに送る(サーバ3のみ) <br>
    ステップ4を実行することで`./config/beaver_triple_service`ディレクトリに`client{0,1}.sample.env`が生成される

    `client0.sample.env`をサーバ1に、`client1.sample.env`をサーバ2に送る

6. JWTを配置する(サーバ1,2のみ) <br>
    サーバ3から送られてきたJWTファイルを`client.sample.env`にリネームし、`./config/beaver_triple_service`に配置する
7. コンテナを起動する <br>
    以下のコマンドでコンテナを起動します。
    ```
    make upd
    ```
8. Fire Wallの設定などて50000番ポートでアクセスできるように設定する

## 計算
`./client_demo`の[README.md](./client_demo/README-ja.md)を参考の上、計算デモを実行します。
