
build() {
    docker-compose -f docker-compose.yml build send_share_mc1 send_share_mc2 send_share_mc3 dev_mc_envoy1 dev_dbgate1 dev_sharedb1 dev_secrets-server dev_mc_envoy2 dev_dbgate2 dev_sharedb2 dev_mc_envoy3 dev_dbgate3 dev_sharedb3
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up --build send_share_mc1 send_share_mc2 send_share_mc3
}
