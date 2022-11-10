
build() {
    docker-compose -f docker-compose.yml build send_share_mc1 send_share_mc2 send_share_mc3 dev_mc_envoy1 dev_mc_envoy2 dev_mc_envoy3 
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up send_share_mc1 send_share_mc2 send_share_mc3
}
