#!/bin/bash
# ----- NOTE: 以下は変更不要 ----- #

# BuildKitを使用して高速にBuild
export DOCKER_BUILDKIT=1
export COMPOSE_DOCKER_CLI_BUILD=1

## Makefileでexportした環境変数を表示する
echo "COMPOSE_FILES_OPT = $COMPOSE_FILES_OPT"
echo "IS_ENABLE_DATADOG = $IS_ENABLE_DATADOG"


# 引数が正しくない時に発火する関数
usage_exit() {
    echo "Usage: $1 'build' or 'run' or 'all' or ''" 1>&2
    exit 1
}
# build()の定義を強要するために未定義関数として宣言
build() {
    echo "build()が定義されていません"
    exit 1
}

# run()の定義を強要するために未定義関数として宣言
run() {
    echo "run()が定義されていません"
    exit 1
}

# setup()の定義を強要するために未定義関数として宣言
setup() {
    echo "setup()が定義されていません"
    exit 1
}

main() {
    source $1
    local tmp=$?
    if [ $tmp -gt 0 ]; then
        echo "$1の読み込みに失敗"
        return $tmp
    fi
    case $2 in
    all)
        local ret
        ret=$tmp
        main $1 build
        ret=$?
        if [ $ret -gt 0 ]; then
            return $ret
        fi
        main $1 run
        ret=$(($ret + $?))
        if [ $ret -gt 0 ]; then
            return $ret
        fi
        return $ret
        ;;
    build)
        build
        tmp=$?
        if [ $tmp -gt 0 ]; then
            echo "buildで失敗"
            return $tmp
        fi
        ;;
    run)
        setup
        tmp=$?
        if [ $tmp -gt 0 ]; then
            echo "setupで失敗"
            return $tmp
        fi
        if [ "$IS_ENABLE_DATADOG" = "1" ]; then
            docker-compose $COMPOSE_FILES_OPT up -d datadog
            tmp=$?
            if [ $tmp -gt 0 ]; then
                echo "datadogで失敗"
                return $tmp
            fi
        fi
        run
        tmp=$?
        if [ $tmp -gt 0 ]; then
            echo "runで失敗"
            return $tmp
        fi
        ;;
    *)
        usage_exit
        ;;
    esac
}

mode=$2
if [ $# -eq 1 ]; then
    mode='all'
fi
main $1 $mode

exit 0
