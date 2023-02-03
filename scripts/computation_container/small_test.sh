# !!!!!!!!!! NOTE: 関数の内部処理はexit_statusのANDを取るために必ずワンライナーで記述する !!!!!!!!!!


# build処理を記述する関数
build() {
    docker buildx bake -f docker-compose.yml small_cc --load
}

# runの前に実行されるsetup処理を記述する関数
# INFO: runを冪等に実行できるように初期化を行う
setup() {
    docker-compose -f docker-compose.yml down -v
}

# run処理を記述する関数
# NOTE: この関数は例外的にワンライナーで書かなくて良い
run() {
    docker-compose -f docker-compose.yml run -T small_cc /bin/sh -c "cd /QuickMPC && bazel test //test/unit_test:all --config=debug --test_env=IS_TEST=true --test_output=all"
}

# runの後に実行されるteardown処理を記述する関数
# INFO: runの後に副作用を残さないように初期化を行う
teardown() {
    docker-compose -f docker-compose.yml down -v
}
