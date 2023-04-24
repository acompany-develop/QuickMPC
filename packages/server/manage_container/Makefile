t     = mc

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

rm:
	docker-compose rm -fs

rm-all:
	docker-compose down --rmi all --volumes

# Dbのデータを削除
rm-db:
	rm -r ../db/**/*

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
