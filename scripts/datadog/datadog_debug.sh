build() {
    docker buildx bake $COMPOSE_FILES_OPT dev_bts dev_cc1 dev_cc2 dev_cc3 dev_mc1 dev_mc2 dev_mc3 --load &&\
    docker-compose $COMPOSE_FILES_OPT build datadog
}

setup() {
    docker-compose $COMPOSE_FILES_OPT down -v
}

run() {
    docker-compose $COMPOSE_FILES_OPT up dev_bts dev_cc1 dev_cc2 dev_cc3 dev_mc1 dev_mc2 dev_mc3 datadog
}
