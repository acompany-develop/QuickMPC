
build() {
    docker buildx bake -f docker-compose.yml send_share_mc1 send_share_mc2 send_share_mc3 --load
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up send_share_mc1 send_share_mc2 send_share_mc3
}
