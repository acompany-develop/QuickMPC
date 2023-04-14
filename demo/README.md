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
以下ではVMを3台立ち上げた場合を例に挙げて説明します。
1. `./terraform`ディレクトリ内の[README.md](terraform/README.md)を参照の上、GCP環境を立ち上げる
2. 作成したGCP環境に本リポジトリをcloneする
    ```
    git clone git@github.com:acompany-develop/QuickMPC.git
    ```
3. `QuickMPC/demo/deploy`ディレクトリに移動する
    ```
    cd QuickMPC/demo/deploy
    ```
4. QuickMPCを立ち上げる用の環境を構築する
    以下のコマンドを実行し、Docker,docker-compose,GNU Makeをinstallします。
    ```
    sh setup.sh
    ```
5. dockerイメージを設定する <br>
    [docker-compose.yml](./deploy/docker-compose.yml)の`image`項目を修正する <br>
    この[ページ](https://github.com/acompany-develop/QuickMPC/tags)の最新のtagを設定します。
    ※ 全てのサーバのコンテナイメージは同じtagに設定する必要があります。
    ```
    version: '3.3'
    services:
    bts:
        container_name: beaver_triple_service
        image: ghcr.io/acompany-develop/quickmpc-bts:{tag}
        command:
    ...
    ```

6. JSON Web Token(以下JWT)を生成する(サーバ3のみ)
    1. サーバ3のマシンにてconfig/beaver_triple_service/sample.yamlの{}内を適切に修正する。
        ```
        # registered claims is following
        # "Expiration Time" represented by unix time
        # `github.com/golang-jwt/jwt/v4` v4.4.2 package will parse this as `float64`,
        # and `time.Now().Before(time.Unix(math.MaxInt64, 0))` returns `false`,
        # so max value of `int64` cannot be used at the time.
        exp: 9223371974719179007
        # user defined claims is folloing
        room_uuid: '' # this field is filled by generator
        party_id: 1
        party_info:
        - id: 1
            address: {サーバ1のip}
        - id: 2
            address: {サーバ2のip}
        ```

    2. 以下のコマンドを実行する
        ```
        make gen-jwt
        ```

    3. client.sample.envが更新されるため、更新されたファイルの中身をサーバ1に送る
    4. party_idを2に更新の上、上記の処理を行い、生成されたファイルの中身をサーバ2に送る
7. 環境変数を設定する
    それぞれの{}内を適切に設定の上書き換えます。 <br>
    1. サーバ1,サーバ2の場合
        * config/computation_container/.env
            ```
            PARTY_ID={サーバの番号　ex) サーバ1なら1}
            N_PARTIES=2
            SP_ID=1
            MC_TO_CC=http://0.0.0.0:50010
            CC_TO_BTS=https://{サーバ3のドメイン or IPアドレス}:54000
            PORT_FOR_JOB=51020
            PARTY_LIST1=https://{サーバ1のドメイン or IPアドレス}:50020
            PARTY_LIST2=https://{サーバ2のドメイン or IPアドレス}:50020
            BTS_TOKEN={ステップ6でサーバ3から受け取った値を設定する}
            GETSHARE_TIME_LIMIT=50
            ```
        * config/manage_container/.env
            ```
            PARTY_ID={サーバの番号　ex) サーバ1なら1}
            N_PARTIES=2
            MANAGE=http://localhost:50011
            COMPUTATION=http://computation_container:50010
            PARTY_LIST1=https://{サーバ1のドメイン}:{サーバ1なら51011,サーバ2なら50010}
            PARTY_LIST2=https://{サーバ2のドメイン}:{サーバ1なら50010,サーバ2なら51011}
            ALLOWEDORIGIN=http://localhost:8080
            ```
    2. サーバ3の場合
        サーバ3は以下のファイルのみ修正を行います。
        * ./.env
            ```
            IS_BTS=true
            ```
8. SSL証明書の配置 <br>
    certificateディレクトリに秘密鍵と証明書を配置します。
    ```
    certificate
    ├── Server.crt # 証明書をリネーム
    └── Server.key # 秘密鍵をリネーム
    ```
9. コンテナを起動する <br>
    以下のコマンドでコンテナを起動します。
    ```
    make upnet
    make upd
    ```
10. GCPのネットワーク設定で{variable.tfで設定したinstance_name}-vpc-{0,1}ネットワークのファイアウォール設定を変更する. <br>
    clientからリクエストを送れるように50000番ポートを解放します。

## 計算
`./client_demo`の[README.md](./client_demo/README-ja.md)を参考の上、計算デモを実行します。
