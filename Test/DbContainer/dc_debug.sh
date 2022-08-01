build() {
    docker-compose -f docker-compose.yml build dev_dbgate1 dev_sharedb1 dev_dbgate2 dev_sharedb2 dev_dbgate3 dev_sharedb3 dev_secrets-server
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose -f docker-compose.yml up --build dev_dbgate1 dev_dbgate2 dev_dbgate3
}
