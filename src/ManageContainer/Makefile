t     = mc

NETWORK_NAME = shared-network
NETWORK_GATEWAY = 10.0.0.1
NETWORK_SUBNET = 10.0.0.0/16

# docker-composeを束ねるネットワークの作成
upnet:
	@if [ -z "`docker network ls | grep $(NETWORK_NAME)`" ]; then docker network create --gateway $(NETWORK_GATEWAY) --subnet $(NETWORK_SUBNET) $(NETWORK_NAME); fi

# docker-composeを束ねるネットワークの削除
downnet:
	docker network rm $(NETWORK_NAME)

build: upnet
	docker-compose build

upd: upnet
	docker-compose up -d

upd-build: upnet
	docker-compose up -d --build

up: upnet
	docker-compose up

up-build: upnet
	docker-compose up --build

down:
	docker-compose down

rm:
	docker-compose rm -fs

rm-all:
	docker-compose down --rmi all --volumes

# Dbのデータを削除
rm-db:
	rm -r ../Db/**/*

ps:
	docker-compose ps

images:
	docker-compose images

top:
	docker-compose top

logs:
	docker-compose logs -f

login:
	docker-compose exec $(t) /bin/bash
