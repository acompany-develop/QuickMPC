
build() {
    docker buildx bake -f docker-compose.yml send_share_mc1 send_share_mc2 send_share_mc3 dev_dbgate1 dev_sharedb1 dev_secrets-server dev_dbgate2 dev_sharedb2 dev_dbgate3 dev_sharedb3 --load
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up --build send_share_mc1 send_share_mc2 send_share_mc3
}
