#!/bin/bash
set -e
trap finally EXIT

gcp_service_account_key=$1
gcp_project_id=$2
runner_id=$3
channel_id=$4
slack_token=$5
SCRIPT_DIR=$(cd $(dirname $0); pwd)

git fetch --tags
latest_tag=$(git tag --list | grep -E '[0-9]+\.[0-9]+\.[0-9]+' | tail -1)

function finally() {
    # logをslackに送信
    curl -F initial_comment="QuickMPC version: $latest_tag \`\`\`$( sed -n '/test_print_result/,$p' /home/ubuntu/log/result-$runner_id.log | sed 's/\[[0-9][0-9]*m//g')\`\`\`" -F file=@/home/ubuntu/log/result-$runner_id.log -F channels=$channel_id -H "Authorization: Bearer $slack_token" https://slack.com/api/files.upload
    # post processing
    rm ~/.ssh/known_hosts
    cd $SCRIPT_DIR/../demo/terraform/gcp
    terraform destroy --auto-approve
}

./setup.sh $latest_tag $gcp_service_account_key $gcp_project_id

# Create GCP instance
cd ../demo/terraform/gcp
terraform init && terraform plan && terraform apply --auto-approve && terraform output -json > ../application/output.json

# Deploy quickmpc
cd ../application
terraform init && terraform plan && terraform apply --auto-approve && terraform output -json > ../../../benchmark_src/terraform/output.json

#Run benchmark test
cd ../../../benchmark_src/terraform
terraform init && terraform plan && terraform apply --auto-approve > /home/ubuntu/log/result-$runner_id.log
