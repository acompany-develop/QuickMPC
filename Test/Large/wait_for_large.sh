# Largeテストの結果をgithub actionsで確認するためのシェルスクリプト
#!/bin/bash

# スクリプト内でエラーがあればexitする
set -e

ci_auth_token="$1"
large_endpoint="$2"
pr_number="$3"
pr_title="$4"
pr_url="$5"
run_id="$6"

generate_post_data()
{
    cat <<EOF
{
    "pr_number": "$pr_number",
    "pr_title": "$pr_title",
    "pr_url": "$pr_url",
    "run_id": "$run_id"
}
EOF
}

# ci実行リクエスト送信
job_id=$(curl -k -X POST -H "Content-Type: application/json" -H "Authorization: Bearer $ci_auth_token" --data "$(generate_post_data)" "$large_endpoint/ci")

for i in `seq 1 120` # 60秒ごとに結果確認を120回繰り返す
do
    sleep 60
    response=$(curl -k -X GET -H "Content-Type: text/plain" -H "Authorization: Bearer $ci_auth_token" "$large_endpoint/job?job_id=$job_id")
    echo $response
    if [ $response == "passed" ]; then # passedなら成功
        exit 0
    elif [ $response == "failed" ]; then # failedなら失敗
        exit 1
    else # waiting, doingなら継続
        continue
    fi
done

exit 1 # 120分を超えると失敗
