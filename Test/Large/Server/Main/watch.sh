#!/bin/bash
set -e

function urldecode() { : "${*//+/ }"; echo -e "${_//%/\\x}"; }

pr_number=$(urldecode "$1")
pr_title=$(urldecode "$2")
pr_url=$(urldecode "$3")
job_id="$4"
auth_token="$5"

all_test_result="passed"

# CCの停止を検知し、exit statusからテスト結果を取得
(docker events --filter type=container --filter event=die --format '{{.Actor.Attributes.name}},{{.Actor.Attributes.exitCode}}' &) |
awk -F ',' '/'computation_container'/ {
    if($2==0) {
        cc_test_result = "passed"
    } else {
        all_test_result = "failed"
        cc_test_result = "failed"
    }
    print cc_test_result > "cc_test_result.txt"
    exit 0
}' &&
cc_test_result="$(cat cc_test_result.txt)"
if [ $cc_test_result == "passed" ]; then
    cc_test_slack="Passed :white_check_mark:"
elif [ $cc_test_result == "failed" ]; then
    cc_test_slack="Failed :x:"
    all_test_result="failed"
fi
# slackにCCのテストログを送信
bash -c "curl -sS -F file=@cc_test_log_$job_id -F $'initial_comment=CC Integration Test: $cc_test_slack\n#$pr_number $pr_title\n$pr_url' -F 'channels=#dev_13_azure_notice' -H 'Authorization: Bearer $SLACK_TOKEN' https://slack.com/api/files.upload"

# docker eventsのプロセスをkill
pkill -f "docker events" &&

# # MCの停止を検知し、exit statusからテスト結果を取得
# (docker events --filter type=container --filter event=die --format '{{.Actor.Attributes.name}},{{.Actor.Attributes.exitCode}}' &) |
# awk -F ',' '/'manage_container'/ {
#     if($2==0) {
#         mc_test_result = "passed"
#     } else {
#         all_test_result = "failed"
#         mc_test_result = "failed"
#     }
#     print mc_test_result > "mc_test_result.txt"
#     exit 0
# }' &&
# mc_test_result="$(cat mc_test_result.txt)"
# if [ $mc_test_result == "passed" ]; then
#     mc_test_slack="Passed :white_check_mark:"
# elif [ $mc_test_result == "failed" ]; then
#     mc_test_slack="Failed :x:"
#     all_test_result="failed"
# fi
# # slackにMCのテストログを送信
# bash -c "curl -sS -F file=@mc_test_log_$job_id -F $'initial_comment=MC Integration Test: $mc_test_slack\n#$pr_number $pr_title\n$pr_url' -F 'channels=#213_azure_notice' -H 'Authorization: Bearer $SLACK_TOKEN' https://slack.com/api/files.upload"

# # docker eventsのプロセスをkill
# pkill -f "docker events" &&

# # DCの停止を検知し、exit statusからテスト結果を取得
# (docker events --filter type=container --filter event=die --format '{{.Actor.Attributes.name}},{{.Actor.Attributes.exitCode}}' &) |
# awk -F ',' '/'dbgate_test_container'/ {
#     if($2==0) {
#         dc_test_result = "passed"
#     } else {
#         all_test_result = "failed"
#         dc_test_result = "failed"
#     }
#     print dc_test_result > "dc_test_result.txt"
#     exit 0
# }' &&
# dc_test_result="$(cat dc_test_result.txt)"
# if [ $dc_test_result == "passed" ]; then
#     dc_test_slack="Passed :white_check_mark:"
# elif [ $dc_test_result == "failed" ]; then
#     dc_test_slack="Failed :x:"
#     all_test_result="failed"
# fi
# # slackにDCのテストログを送信
# bash -c "curl -sS -F file=@dc_test_log_$job_id -F $'initial_comment=DC Integration Test: $dc_test_slack\n#$pr_number $pr_title\n$pr_url' -F 'channels=#213_azure_notice' -H 'Authorization: Bearer $SLACK_TOKEN' https://slack.com/api/files.upload"

# # docker eventsのプロセスをkill
# pkill -f "docker events" &&

# レジストリのcc_testイメージの特定のバージョンを削除
curl -i \
  -X DELETE \
  -H "Authorization: Bearer $GITHUB_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/orgs/acompany-develop/packages/container/cc_test/versions/$( \
      curl -H "Authorization: Bearer $GITHUB_TOKEN" \
        -H "Accept: application/vnd.github.v3+json" \
        https://api.github.com/orgs/acompany-develop/packages/container/cc_test/versions \
        | jq --arg a $job_id '.[] | select(.metadata.container.tags[] | contains($a)) | .id')

# # レジストリのmc_testイメージの特定のバージョンを削除
# curl -i \
#   -X DELETE \
#   -H "Authorization: Bearer $GITHUB_TOKEN" \
#   -H "Accept: application/vnd.github.v3+json" \
#   https://api.github.com/orgs/acompany-develop/packages/container/mc_test/versions/$( \
#       curl -H "Authorization: Bearer $GITHUB_TOKEN" \
#         -H "Accept: application/vnd.github.v3+json" \
#         https://api.github.com/orgs/acompany-develop/packages/container/mc_test/versions \
#         | jq --arg a $job_id '.[] | select(.metadata.container.tags[] | contains($a)) | .id')

# # レジストリのdbg_test, sharedb_testイメージの特定のバージョンを削除
# curl -i \
#   -X DELETE \
#   -H "Authorization: Bearer $GITHUB_TOKEN" \
#   -H "Accept: application/vnd.github.v3+json" \
#   https://api.github.com/orgs/acompany-develop/packages/container/dbg_test/versions/$( \
#       curl -H "Authorization: Bearer $GITHUB_TOKEN" \
#         -H "Accept: application/vnd.github.v3+json" \
#         https://api.github.com/orgs/acompany-develop/packages/container/dbg_test/versions \
#         | jq --arg a $job_id '.[] | select(.metadata.container.tags[] | contains($a)) | .id')

# curl -i \
#   -X DELETE \
#   -H "Authorization: Bearer $GITHUB_TOKEN" \
#   -H "Accept: application/vnd.github.v3+json" \
#   https://api.github.com/orgs/acompany-develop/packages/container/sharedb_test/versions/$( \
#       curl -H "Authorization: Bearer $GITHUB_TOKEN" \
#         -H "Accept: application/vnd.github.v3+json" \
#         https://api.github.com/orgs/acompany-develop/packages/container/sharedb_test/versions \
#         | jq --arg a $job_id '.[] | select(.metadata.container.tags[] | contains($a)) | .id')

# ログファイル削除
rm cc_test_log_*
# rm mc_test_log_*
# rm dc_test_log_*

# 一時的に使用したファイル削除
rm "cc_test_result.txt"
# rm "mc_test_result.txt"
# rm "dc_test_result.txt"

# jobのstatusを更新
echo $all_test_result
curl -k -X PUT -H "Content-Type: application/json" -H "Authorization: Bearer $auth_token" -d @- "https://localhost:3001/job" << EOS
{"job_id": "$job_id", "status": "$all_test_result"}
EOS