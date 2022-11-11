build() {
    docker buildx bake -f docker-compose.yml dev_mc1 dev_mc2 dev_mc3 dev_cc1 dev_cc2 dev_cc3 --load
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up dev_mc1 dev_mc_envoy1 dev_mc2 dev_mc_envoy2 dev_mc3 dev_mc_envoy3
}
