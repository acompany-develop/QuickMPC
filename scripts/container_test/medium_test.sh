# !!!!!!!!!! NOTE: 関数の内部処理はexit_statusのANDを取るために必ずワンライナーで記述する !!!!!!!!!!


# build処理を記述する関数
build() {
    docker buildx bake $COMPOSE_FILES_OPT medium_container dev_bts dev_cc1 dev_cc2 dev_cc3 dev_mc1 dev_mc2 dev_mc3 --load
}

# runの前に実行されるsetup処理を記述する関数
# INFO: runを冪等に実行できるように初期化を行う
setup() {
    rm /tmp/QuickMPC
    docker-compose $COMPOSE_FILES_OPT down -v
}

# run処理を記述する関数
# NOTE: この関数は例外的にワンライナーで書かなくて良い
run() {
    # DooDでmountしてコンテナを立てるようとするとpathがデーモンのあるhostから参照されてしまう
    # コンテナ内からhostのpathが見えず指定できないため，明示的な場所にQuickMPCを作ってここを指定するようにする
    ln -s $(pwd)/../ /tmp/QuickMPC
    docker-compose $COMPOSE_FILES_OPT up medium_container
}

# runの後に実行されるteardown処理を記述する関数
# INFO: runの後に副作用を残さないように初期化を行う
teardown() {
    rm /tmp/QuickMPC
    docker-compose $COMPOSE_FILES_OPT down -v
}
