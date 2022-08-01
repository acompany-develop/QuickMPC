# ログイン時に読み込まれる.bashrc
# bashに入ると自動的にsecrets-serverから秘密情報を取得する

if [ -n "${lowercase_DB_NAME}" ]; then
    # src/DbContainer/Db/DockerfileでARGに`lowercase_DB_NAME`が渡された時
    export $(curl -sS http://secrets-server:8000/.${lowercase_DB_NAME}.env | xargs)
else
    # src/DbContainer/DbGate/DockerfileでARGに`lowercase_DB_NAMES`が渡された時
    # `,`区切りで`lowercase_DB_NAME`のlistにパース
    list=(${lowercase_DB_NAMES//,/ })
    for lowercase_DB_NAME in "${list[@]}"; do
        # 複数の`lowercase_DB_NAME`が同じ名前の環境変数を持っているので先頭に`lowercase_DB_NAME_`を足してからexport
        export $(curl -sS http://secrets-server:8000/.${lowercase_DB_NAME}.env | sed -e "s/^/${lowercase_DB_NAME}_/" | xargs)
    done
fi
