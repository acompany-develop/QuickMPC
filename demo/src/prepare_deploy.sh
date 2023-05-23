#!/bin/bash
# ./prepare_deploy.sh party_id party_str image_tag
party_id=$1           # パーティの番号を引数から取得
party_str=$2          # パーティのipリストを引数から取得
image_tag=$3          # containerイメージのタグを引数から取得

# 0-indexを1-indexに変換(terraformのcount.indexが0-indexのため引数は0-index)
((party_id++))
# パーティのリストをカンマで区切って配列に格納
party_list=(${party_str//,/ })

# party_listの最後の要素をbts_ipとして取得
bts_ip=${party_list[(${#party_list[@]} - 1)]}

# btsを除いたパーティの数を取得
n_parties=$((${#party_list[@]} - 1))

# 各configファイルの出力先を定義
readonly CC_PATH="./config/computation_container"
readonly MC_PATH="./config/manage_container"
readonly BTS_PATH="./config/beaver_triple_service"

# テンプレートファイル内の"{tag}"をimage_tagで置換し、出力ファイルを作成
template_file="./docker-compose.yml.template"
search="{tag}"
output_file="./docker-compose.yml"
sed "s/${search}/${image_tag}/g" "${template_file}" > "${output_file}"

if [ $party_id -le $n_parties ]; then
    # CCのconfig作成
    printf 'PARTY_ID=%s
N_PARTIES=%s
SP_ID=1
MC_TO_CC=http://0.0.0.0:50010
CC_TO_BTS=http://%s:54000
PORT_FOR_JOB=51020
GETSHARE_TIME_LIMIT=50\n' "${party_id}" "${n_parties}" "${bts_ip}" > $CC_PATH/.env

    # 各パーティの情報をCCのconfigに追記
    for ((i=1; i<=n_parties; i++)); do
        printf 'PARTY_LIST%s=http://%s:50020\n' "${i}" "${party_list[$((i-1))]}" >> $CC_PATH/.env
    done

    # MCのconfig作成
    printf 'PARTY_ID=%s
N_PARTIES=%s
MANAGE=http://localhost:50011
COMPUTATION=http://computation_container:50010
ALLOWEDORIGIN=http://localhost:8080
' "${party_id}" "${n_parties}" >$MC_PATH/.env

    # 各パーティの情報をMCのconfigに追記
    for ((i=1; i<=n_parties; i++)); do
        PORT=$(( ${i} == ${party_id} ? 51011 : 50010  ))
        printf 'PARTY_LIST%s=http://%s:%s\n' "${i}" "${party_list[$((i-1))]}" "${PORT}" >> $MC_PATH/.env
    done
    printf 'IS_BTS=false' > ./.env
else
    # jwt作成
    for ((i=1; i<=n_parties; i++));
    do
        printf '# registered claims is following
# "Expiration Time" represented by unix time
# `github.com/golang-jwt/jwt/v4` v4.4.2 package will parse this as `float64`,
# and `time.Now().Before(time.Unix(math.MaxInt64, 0))` returns `false`,
# so max value of `int64` cannot be used at the time.
exp: 9223371974719179007
# user defined claims is folloing
room_uuid: '' # this field is filled by generator
party_id: %s
party_info:' "${i}" > $BTS_PATH/sample.yaml

        # 各パーティの情報をsample.yamlに追記
        for ((j=1; j<=n_parties; j++)); do
            printf '
  - id: %s
    address: %s' "${j}" "${party_list[$((j-1))]}" >> $BTS_PATH/sample.yaml
        done
        make gen-jwt
        cp $BTS_PATH/client.sample.env $BTS_PATH/client$((i-1)).sample.env
    done
    printf 'IS_BTS=true' > ./.env
fi
