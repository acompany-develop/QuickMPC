#!/bin/bash

build() {
    docker buildx bake -f small_bts --load
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose run small_bts /bin/sh -c 'cd /QuickMPC && go mod vendor && go test -count 1 -cover -v $(go list github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService/Server/EngineToBts/...)'
}

teardown() {
    docker-compose -f docker-compose.yml down -v
}
