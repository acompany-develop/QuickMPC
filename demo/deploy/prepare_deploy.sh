#!/bin/bash
# ./prepare_deploy.sh party_num party1_ip party2_ip bts_ip image_tag token
party_num=$1
((party_num++))
party1_ip=$2
party2_ip=$3
bts_ip=$4
image_tag=$5
token=$6

readonly CC_PATH="./config/computation_container"
readonly MC_PATH="./config/manage_container"
readonly BTS_PATH="./config/beaver_triple_service"

# create docker-compose.yml
template_file="./docker-compose.yml.template"
search="{tag}"
output_file="./docker-compose.yml"
sed "s/${search}/${image_tag}/g" "${template_file}" > "${output_file}"

if  [ "$party_num" != "3" ]; then
    ## CCのconfig作成
    printf 'PARTY_ID=%s
N_PARTIES=2
SP_ID=1
MC_TO_CC=http://0.0.0.0:50010
CC_TO_BTS=http://%s:54000
PORT_FOR_JOB=51020
PARTY_LIST1=http://%s:50020
PARTY_LIST2=http://%s:50020
BTS_TOKEN=%s
GETSHARE_TIME_LIMIT=50
' "${party_num}" "${bts_ip}" "${party1_ip}" "${party2_ip}" "${token}"  > $CC_PATH/.env

    ## MCのconfig作成
    allowedOrigin=""
    LF=$'\n'
    for i in $(seq 0 $(($length - 1))); do
    item=$(echo $jsonData | jq -r .allowedOrigin[${i}])
    allowedOrigin+="\"${item}\""
    if [ $i != $(($length - 1)) ]; then
        allowedOrigin+=",${LF}      "
    fi
    done

    #　MCのconfig作成
    PARTY_LIST1_PORT=$(( party_num ==1 ? 51011 : 50010  ))
    PARTY_LIST2_PORT=$(( party_num ==1 ? 50010 : 51011  ))
    printf 'PARTY_ID=%s
N_PARTIES=2
MANAGE=http://localhost:50011
COMPUTATION=http://computation_container:50010
PARTY_LIST1=http://%s:%s
PARTY_LIST2=http://%s:%s
ALLOWEDORIGIN=http://localhost:8080
' "${party_num}" "${party1_ip}" "${PARTY_LIST1_PORT}" "${party2_ip}" "${PARTY_LIST2_PORT}">$MC_PATH/.env

    printf 'IS_BTS=false' > ./.env
else
    # jwt作成
    for i in {0..1}
    do
        num=$((i + 1))
        printf '# registered claims is following
# "Expiration Time" represented by unix time
# `github.com/golang-jwt/jwt/v4` v4.4.2 package will parse this as `float64`,
# and `time.Now().Before(time.Unix(math.MaxInt64, 0))` returns `false`,
# so max value of `int64` cannot be used at the time.
exp: 9223371974719179007
# user defined claims is folloing
room_uuid: '' # this field is filled by generator
party_id: %s
party_info:
- id: 1
  address: %s
- id: 2
  address: %s
' "${num}" "${party1_ip}" "${party2_ip}">$BTS_PATH/sample.yaml

        make gen-jwt
        cp $BTS_PATH/client.sample.env $BTS_PATH/client$i.sample.env
    done
    printf 'IS_BTS=true' > ./.env
fi
