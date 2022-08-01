# ghcr.io（github container registry）をあるコンテナをpushまたはpullするためにやること

## 手順1 : 個人アクセストークンを作成してdocker loginする

- [このページ](https://docs.github.com/ja/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token) を参考に個人アクセストークンを作成する
    - Setting→Developer Setting→Personal access token→[Generate new token]
    - トークンに付与する権限を設定
        - pushの場合、write:packages にチェックを入れる
        - pullの場合、read:packeges にチェックを入れる
- docker login

```bash
$ export GITHUB_TOKEN=<作成したトークン>
$ echo $GITHUB_TOKEN | docker login ghcr.io -u acompany-develop --password-stdin
```

## 手順2 : イメージをpushまたはpullする

- イメージをpushする
    - tagの付け方は `ghcr.io/acompany-develop/<image name>:<tag name>`

```bash
$ docker tag computation_container ghcr.io/acompany-develop/computation_container:v0.0.6
$ docker push ghcr.io/acompany-develop/computation_container:base-220128
```

- イメージをpullする

```bash
$ docker pull ghcr.io/acompany-develop/computation_container:base-220128
```
