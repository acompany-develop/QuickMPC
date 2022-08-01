#!/bin/bash
# ----- NOTE: 以下は変更不要 ----- #

# BuildKitを使用して高速にBuild
export DOCKER_BUILDKIT=1
export COMPOSE_DOCKER_CLI_BUILD=1

# 引数が正しくない時に発火する関数
usage_exit() {
    echo "Usage: $1 'build' or 'run' or ''" 1>&2
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


if [ $# -eq 1 ]; then
    source $1 && setup && build && run
else
    source $1
    case $2 in
    build)
        build
        ;;
    run)
        setup && run
        ;;
    *)
        usage_exit
        ;;
    esac
fi

exit 0
