# !!!!!!!!!! NOTE: 関数の内部処理はexit_statusのANDを取るために必ずワンライナーで記述する !!!!!!!!!!


# build処理を記述する関数
build() {
    docker-compose -f docker-compose.yml build --build-arg BUILD_OPT="--config=debug" medium_cc1 medium_cc2 medium_cc3 dev_bts
}

# runの前に実行されるsetup処理を記述する関数
# INFO: runを冪等に実行できるように初期化を行う
setup() {
    docker-compose -f docker-compose.yml down -v
}

# run処理を記述する関数
# NOTE: この関数は例外的にワンライナーで書かなくて良い
run() {
    build
    docker-compose -f docker-compose.yml up medium_cc1 medium_cc2 medium_cc3
}

# runの後に実行されるteardown処理を記述する関数
# INFO: runの後に副作用を残さないように初期化を行う
teardown() {
    docker-compose -f docker-compose.yml down -v
}
