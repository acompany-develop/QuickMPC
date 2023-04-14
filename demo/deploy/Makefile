#!make
SHELL:=/bin/bash


-include .env
export $(-shell sed 's/=.*//' .env)
NETWORK_NAME = shared-network
NETWORK_GATEWAY = 10.0.0.1
NETWORK_SUBNET = 10.0.0.0/16

# docker-composeを束ねるネットワークの作成
upnet:
	@if [ -z "`docker network ls | grep $(NETWORK_NAME)`" ]; then docker network create --gateway $(NETWORK_GATEWAY) --subnet $(NETWORK_SUBNET) $(NETWORK_NAME); fi

# docker-composeを束ねるネットワークの削除
downnet:
	- docker network rm $(NETWORK_NAME)

gen-jwt: upnet
	docker-compose $(COMPOSE_FILES_OPT) down -v
	docker-compose $(COMPOSE_FILES_OPT) run --rm bts /bin/sh -c './beaver_triple_service generateJwt --file /QuickMPC/config/sample.yaml --output /QuickMPC/config'

# 環境変数に合わせてコンテナを起動する
upd:
	if "${IS_BTS}"; then docker-compose up -d bts; else docker-compose up -d mc; fi;

down:
	docker-compose down

rm-db:
	docker-compose down -v
