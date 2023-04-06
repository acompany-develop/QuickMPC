
build() {
    docker buildx bake $COMPOSE_FILES_OPT dev_bts dev_mc1 dev_mc2 dev_mc3 --load
}

setup() {
    docker-compose $COMPOSE_FILES_OPT down -v
}

run() {
    docker-compose $COMPOSE_FILES_OPT up dev_bts dev_mc1 dev_mc2 dev_mc3
}
