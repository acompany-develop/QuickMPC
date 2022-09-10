build() {
    docker buildx bake -f docker-compose.yml dev_dbgate1 dev_sharedb1 dev_dbgate2 dev_sharedb2 dev_dbgate3 dev_sharedb3 dev_secrets-server --load
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up dev_dbgate1 dev_dbgate2 dev_dbgate3
}
