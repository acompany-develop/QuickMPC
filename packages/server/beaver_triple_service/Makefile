#!make
t     = bts

build:
	docker-compose build

upd:
	docker-compose up -d

upd-build:
	docker-compose up -d --build

up:
	docker-compose up

up-build:
	docker-compose up --build

down:
	docker-compose down
	- $(MAKE) downnet

rm:
	docker-compose rm -fs

rm-all:
	docker-compose down --rmi all --volumes

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

run:
	docker-compose up -d --build
	docker-compose exec $(t) /bin/bash -c "go mod vendor && go build && ./beaver_triple_service"
