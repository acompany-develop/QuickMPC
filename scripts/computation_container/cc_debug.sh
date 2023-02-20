# packages/server/<path>/*_test.*の実行方法をTest/<path>/test.shに記述する
build() {
    docker buildx bake $COMPOSE_FILES_OPT dev_bts dev_cc1 dev_cc2 dev_cc3 --load
}

setup() {
    docker-compose $COMPOSE_FILES_OPT down -v
}

run() {
    docker-compose $COMPOSE_FILES_OPT up dev_cc1 dev_cc2 dev_cc3
}
