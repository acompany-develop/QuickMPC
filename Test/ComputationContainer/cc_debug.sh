# src/<path>/*_test.*の実行方法をTest/<path>/test.shに記述する
build() {
    docker-compose -f docker-compose.yml build dev_cc1 dev_cc2 dev_cc3 dev_bts
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up dev_cc1 dev_cc2 dev_cc3
}
