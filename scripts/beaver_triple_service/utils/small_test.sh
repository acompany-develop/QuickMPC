#!/bin/bash

build() {
    docker buildx bake -f docker-compose.yml small_bts --load
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose run small_bts /bin/sh -c 'cd /QuickMPC && go mod vendor && go test -cover -v $(go list github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/Utils/...)'
}

teardown() {
    docker-compose -f docker-compose.yml down -v
}
