開発環境をDockerfileで定義しているため、hostマシンではVSCodeの拡張機能(linterやformatter, Intellisense等)が使えません。

そのため、起動中のコンテナに入って中でVSCodeを開いて開発を行う必要があります。

以下にその手順を記します。

# Remote Containerの拡張機能を追加する

1. Remote - Containersをインストールする

![Remote - Containers](./Images/vscode-remote-ext.png)

1. [開発用のコンテナを起動する](https://github.com/acompany-develop/QuickMPC#%E5%90%84%E3%82%B3%E3%83%B3%E3%83%86%E3%83%8A%E3%81%AE%E9%96%8B%E7%99%BA%E6%96%B9%E6%B3%95)
2. `Attach Visual Studio Code`からコンテナに入る

    ![Attach Visual Studio Code](./Images/vscode-attach.png)

    もしくは

    ![Untitled](./Images/vscode-attach-alt.png)


# おまけ：VSCodeの拡張機能を使う方法

## 例. formatter

1. 入ったコンテナで適当なC++のファイルを開き、`Shift + alt + F`(windows)、`Shift + Option + F` (mac)でformatterが有効か試す
2. 以下のポップアップが表示されると思うので、`フォーマッタをインストール`をクリック

    ![Install the C++ Formatter](./Images/vscode-cpp-formatter-is-not-installed.png)

3. C/C++の拡張機能をインストールする

    ![Install the C++ extension](./Images/vscode-cpp-ext.png)

4. QuickMPC/.vscode/settings.jsonでformatを強制されているので、コードを保存したり張り付けたりすると自動で整形されるようになる


### 例. Intellisense

1. 入ったコンテナの `/QuickMPC` ディレクトリで以下のコマンドを実行する

    ```
    root@container_id:/QuickMPC# bazel build //:compdb
    ```

    - `compile_commands.json` を生成する
        - 生成場所: `/QuickMPC/bazel-bin/compile_commands.json`
    - `bazel build //:all` でも生成できる
2. 必要に応じて Visual Studio Code で「ウィンドウの再読込」を行う
    - Visual Studio Code が `compile_commands.json` の場所を検索してくれる
