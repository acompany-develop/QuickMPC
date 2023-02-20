#!/bin/bash

build() {
    docker buildx bake $COMPOSE_FILES_OPT small_bts --load
}

setup() {
    docker-compose $COMPOSE_FILES_OPT down -v
}

run() {
    docker-compose $COMPOSE_FILES_OPT run small_bts /bin/sh -c 'cd /QuickMPC && go mod vendor && go test -cover -v $(go list github.com/acompany-develop/QuickMPC/packages/server/beaver_triple_service/triple_store/...)'
}

teardown() {
    docker-compose $COMPOSE_FILES_OPT down -v
}
