#!make

CC = computation_container
MC = manage_container
BTS = beaver_triple_service

# 全てのコンテナイメージを構築
build:
	docker-compose -f $(CC)/docker-compose.yml build
	docker-compose -f $(MC)/docker-compose.yml build

# 全てのコンテナイメージを構築かつ、コンテナの実体化と立ち上げをバックグラウンドで実行
upd-build:
	docker-compose -f $(MC)/docker-compose.yml up -d --build
	docker-compose -f $(CC)/docker-compose.yml up -d --build

# 全てのコンテナイメージを構築せず、コンテナの実体化と立ち上げをバックグラウンドで実行
upd:
	docker-compose -f $(MC)/docker-compose.yml up -d
	docker-compose -f $(CC)/docker-compose.yml up -d

# 個々のコンテナ
upd-cc:
	docker-compose -f $(CC)/docker-compose.yml up -d
upd-mc:
	docker-compose -f $(MC)/docker-compose.yml up -d
upd-bts:
	docker-compose -f $(BTS)/docker-compose.yml up -d

# 個々のコンテナ
upd-cc-build:
	docker-compose -f $(CC)/docker-compose.yml up -d --build
upd-mc-build:
	docker-compose -f $(MC)/docker-compose.yml up -d --build
upd-bts-build:
	docker-compose -f $(BTS)/docker-compose.yml up -d --build

# 個々のコンテナ
up-cc-build:
	docker-compose -f $(CC)/docker-compose.yml up --build
up-mc-build:
	docker-compose -f $(MC)/docker-compose.yml up --build
up-bts-build:
	docker-compose -f $(BTS)/docker-compose.yml up --build

# 全てのコンテナイメージを構築せず、コンテナの実体化と立ち上げ
up-build:
	docker-compose -f $(CC)/docker-compose.yml up -d --build
	docker-compose -f $(MC)/docker-compose.yml up -d --build
	docker-compose -f $(CC)/docker-compose.yml logs -f&
	docker-compose -f $(MC)/docker-compose.yml logs -f

# 全てのコンテナイメージを構築せず、コンテナの実体化と立ち上げ
up:
	docker-compose -f $(CC)/docker-compose.yml up -d
	docker-compose -f $(MC)/docker-compose.yml up -d
	docker-compose -f $(CC)/docker-compose.yml logs -f&
	docker-compose -f $(MC)/docker-compose.yml logs -f

down:
	docker-compose -f $(MC)/docker-compose.yml down&
	docker-compose -f $(CC)/docker-compose.yml down
	- docker network rm $(NETWORK_NAME)

down-cc:
	docker-compose -f $(CC)/docker-compose.yml down
down-mc:
	docker-compose -f $(MC)/docker-compose.yml down
down-bts:
	docker-compose -f $(BTS)/docker-compose.yml down

rm:
	docker-compose -f $(MC)/docker-compose.yml rm -fs&
	docker-compose -f $(CC)/docker-compose.yml rm -fs

rm-all:
	docker-compose -f $(MC)/docker-compose.yml down --rmi all --volumes&
	docker-compose -f $(CC)/docker-compose.yml down --rmi all --volumes
	- $(MAKE) downnet

# Dbのデータを削除
rm-db:
	rm -r ../db/**/*

logs-cc:
	docker-compose -f $(CC)/docker-compose.yml logs -f
logs-mc:
	docker-compose -f $(MC)/docker-compose.yml logs -f
logs-bts:
	docker-compose -f $(BTS)/docker-compose.yml logs -f

login-cc:
	docker-compose -f $(CC)/docker-compose.yml exec cc /bin/bash
login-mc:
	docker-compose -f $(MC)/docker-compose.yml exec mc /bin/bash
login-bts:
	docker-compose -f $(BTS)/docker-compose.yml exec bts /bin/bash

ROOT_DIR := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
SOURCES := $(shell find $(ROOT_DIR) -type d -name '.cache' -prune -o -type f \( -name *.cpp -o -type f -name *.hpp \) -print)
CLANG_FORMAT = clang-format-14 --verbose --style=file:$(ROOT_DIR)/.clang-format

.PHONY: fmt
fmt:
	$(CLANG_FORMAT) -i $(SOURCES)

.PHONY: check
check:
	$(CLANG_FORMAT) --dry-run --Werror $(SOURCES)
