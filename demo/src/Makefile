#!make
SHELL:=/bin/bash


-include .env
export $(-shell sed 's/=.*//' .env)

gen-jwt:
	docker-compose $(COMPOSE_FILES_OPT) down -v
	docker-compose $(COMPOSE_FILES_OPT) run --rm bts /bin/sh -c './beaver_triple_service generateJwt --file /QuickMPC/config/sample.yaml --output /QuickMPC/config'

# 環境変数に合わせてコンテナを起動する
upd:
	if "${IS_BTS}"; then docker-compose up -d bts; else docker-compose up -d mc; fi;

down:
	docker-compose down

rm-db:
	docker-compose down -v
