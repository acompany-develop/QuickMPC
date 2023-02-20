
build() {
    docker buildx bake $COMPOSE_FILES_OPT send_share_mc1 send_share_mc2 send_share_mc3 --load
}

setup() {
    docker-compose $COMPOSE_FILES_OPT down -v
}

run() {
    docker-compose $COMPOSE_FILES_OPT up send_share_mc1 send_share_mc2 send_share_mc3
}
