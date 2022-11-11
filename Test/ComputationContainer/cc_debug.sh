# src/<path>/*_test.*の実行方法をTest/<path>/test.shに記述する
build() {
    docker buildx bake -f docker-compose.yml dev_cc1 dev_cc2 dev_cc3 --load
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up dev_cc1 dev_cc2 dev_cc3
}
