#!/bin/bash

build() {
    docker-compose build small_bts
}

setup() {
    docker-compose -f docker-compose.yml down -v
}

run() {
    docker-compose run small_bts /bin/sh -c 'cd /QuickMPC && go mod vendor && go test -cover -v $(go list github.com/acompany-develop/QuickMPC/packages/server/BeaverTripleService/TripleGenerator/...)'
}

teardown() {
    docker-compose -f docker-compose.yml down -v
}
