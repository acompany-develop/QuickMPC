#!make
NETWORK_NAME = shared-network
NETWORK_GATEWAY = 10.0.0.1
NETWORK_SUBNET = 10.0.0.0/16

CC = ComputationContainer
DC = DbContainer
MC = ManageContainer
BTS = BeaverTripleService

# docker-composeを束ねるネットワークの作成
upnet:
	@if [ -z "`docker network ls | grep $(NETWORK_NAME)`" ]; then docker network create --gateway $(NETWORK_GATEWAY) --subnet $(NETWORK_SUBNET) $(NETWORK_NAME); fi

# docker-composeを束ねるネットワークの削除
downnet:
	- docker network rm $(NETWORK_NAME)

# 全てのコンテナイメージを構築
build:
	docker-compose -f $(DC)/docker-compose.yml build
	docker-compose -f $(CC)/docker-compose.yml build
	docker-compose -f $(MC)/docker-compose.yml build

# 全てのコンテナイメージを構築かつ、コンテナの実体化と立ち上げをバックグラウンドで実行
upd-build: upnet
	docker-compose -f $(DC)/docker-compose.yml up -d --build
	docker-compose -f $(MC)/docker-compose.yml up -d --build
	docker-compose -f $(CC)/docker-compose.yml up -d --build

# 全てのコンテナイメージを構築せず、コンテナの実体化と立ち上げをバックグラウンドで実行
upd: upnet
	docker-compose -f $(DC)/docker-compose.yml up -d
	docker-compose -f $(MC)/docker-compose.yml up -d
	docker-compose -f $(CC)/docker-compose.yml up -d

# 個々のコンテナ
upd-cc:
	docker-compose -f $(CC)/docker-compose.yml up -d
upd-dc:
	docker-compose -f $(DC)/docker-compose.yml up -d
upd-mc:
	docker-compose -f $(MC)/docker-compose.yml up -d
upd-bts:
	docker-compose -f $(BTS)/docker-compose.yml up -d

# 個々のコンテナ
upd-cc-build:
	docker-compose -f $(CC)/docker-compose.yml up -d --build
upd-dc-build:
	docker-compose -f $(DC)/docker-compose.yml up -d --build
upd-mc-build:
	docker-compose -f $(MC)/docker-compose.yml up -d --build
upd-bts-build:
	docker-compose -f $(BTS)/docker-compose.yml up -d --build

# 個々のコンテナ
up-cc-build:
	docker-compose -f $(CC)/docker-compose.yml up --build
up-dc-build:
	docker-compose -f $(DC)/docker-compose.yml up --build
up-mc-build:
	docker-compose -f $(MC)/docker-compose.yml up --build
up-bts-build:
	docker-compose -f $(BTS)/docker-compose.yml up --build

# 全てのコンテナイメージを構築せず、コンテナの実体化と立ち上げ
up-build: upnet
	docker-compose -f $(DC)/docker-compose.yml up -d --build
	docker-compose -f $(CC)/docker-compose.yml up -d --build
	docker-compose -f $(MC)/docker-compose.yml up -d --build
	docker-compose -f $(DC)/docker-compose.yml logs -f&
	docker-compose -f $(CC)/docker-compose.yml logs -f&
	docker-compose -f $(MC)/docker-compose.yml logs -f

# 全てのコンテナイメージを構築せず、コンテナの実体化と立ち上げ
up: upnet
	docker-compose -f $(DC)/docker-compose.yml up -d
	docker-compose -f $(CC)/docker-compose.yml up -d
	docker-compose -f $(MC)/docker-compose.yml up -d
	docker-compose -f $(DC)/docker-compose.yml logs -f&
	docker-compose -f $(CC)/docker-compose.yml logs -f&
	docker-compose -f $(MC)/docker-compose.yml logs -f

down:
	docker-compose -f $(MC)/docker-compose.yml down&
	docker-compose -f $(CC)/docker-compose.yml down&
	docker-compose -f $(DC)/docker-compose.yml down
	- docker network rm $(NETWORK_NAME)

down-cc:
	docker-compose -f $(CC)/docker-compose.yml down
down-dc:
	docker-compose -f $(DC)/docker-compose.yml down
down-mc:
	docker-compose -f $(MC)/docker-compose.yml down
down-bts:
	docker-compose -f $(BTS)/docker-compose.yml down

rm:
	docker-compose -f $(MC)/docker-compose.yml rm -fs&
	docker-compose -f $(CC)/docker-compose.yml rm -fs&
	docker-compose -f $(DC)/docker-compose.yml rm -fs

rm-all:
	docker-compose -f $(MC)/docker-compose.yml down --rmi all --volumes&
	docker-compose -f $(CC)/docker-compose.yml down --rmi all --volumes&
	docker-compose -f $(DC)/docker-compose.yml down --rmi all --volumes
	- $(MAKE) downnet

# Dbのデータをと秘密情報を削除
rm-db:
	- rm -r $(DC)/Db/*/data/lib
	- rm $(DC)/Db/*/nodestatus/initialized
	- yes | docker system prune -a --filter label=secrets-server
	- docker-compose -f $(DC)/docker-compose.yml rm -fvs secrets-server

ps:
	docker ps

images:
	docker images

logs-cc:
	docker-compose -f $(CC)/docker-compose.yml logs -f
logs-dc:
	docker-compose -f $(DC)/docker-compose.yml logs -f
logs-mc:
	docker-compose -f $(MC)/docker-compose.yml logs -f
logs-bts:
	docker-compose -f $(BTS)/docker-compose.yml logs -f

login-cc:
	docker-compose -f $(CC)/docker-compose.yml exec cc /bin/bash
login-dbg:
	docker-compose -f $(DC)/docker-compose.yml exec dbg /bin/bash
login-sharedb:
	docker-compose -f $(DC)/docker-compose.yml exec sharedb /bin/bash
login-mc:
	docker-compose -f $(MC)/docker-compose.yml exec mc /bin/bash
login-bts:
	docker-compose -f $(BTS)/docker-compose.yml exec mc /bin/bash

ROOT_DIR := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
SOURCES := $(shell git ls-tree -r develop --name-only | grep -E '.*\.(cpp|hpp)')
CLANG_FORMAT = clang-format-14 --verbose --style=file:$(ROOT_DIR)/.clang-format

.PHONY: fmt
fmt:
	$(CLANG_FORMAT) -i $(SOURCES)

.PHONY: check
check:
	$(CLANG_FORMAT) --dry-run --Werror $(SOURCES)
