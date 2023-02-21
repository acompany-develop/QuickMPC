build() {
    docker buildx bake $COMPOSE_FILES_OPT dev_mc1 dev_mc2 dev_mc3 dev_cc1 dev_cc2 dev_cc3 --load
}

setup() {
    docker-compose $COMPOSE_FILES_OPT down -v
}

run() {
    docker-compose $COMPOSE_FILES_OPT up dev_mc1 dev_mc_envoy1 dev_mc2 dev_mc_envoy2 dev_mc3 dev_mc_envoy3
}
