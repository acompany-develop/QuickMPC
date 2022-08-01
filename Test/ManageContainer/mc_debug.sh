build() {
    docker-compose -f docker-compose.yml build dev_mc1 dev_mc_envoy1 dev_mc2 dev_mc_envoy2 dev_mc3 dev_mc_envoy3 dev_cc1 dev_dbgate1 dev_bts dev_sharedb1 dev_secrets-server dev_cc2 dev_dbgate2 dev_sharedb2 dev_cc3 dev_dbgate3 dev_sharedb3
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up --build dev_mc1 dev_mc_envoy1 dev_mc2 dev_mc_envoy2 dev_mc3 dev_mc_envoy3
}
