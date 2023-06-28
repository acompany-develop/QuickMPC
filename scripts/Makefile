#!make
SHELL:=/bin/bash
# scripts/.envがあれば読み込んで環境変数を設定する
-include .env
export $(-shell sed 's/=.*//' .env)
# testのターゲットディレクトリ
# ex. make test t=./Hoge -> ./Hoge以下から*test.shを探す
t =.
# testのbuild, runの切り替え
# ex. make test m=build -> build()に定義した処理だけ走る
m =
# testの実行するshellscriptのprefixを指定する
# ex. make test p=unit -> unit*test.shが実行される
p =
# datadog-agentを有効化する
# ex. make test dd=1 -> datadogコンテナが起動する
dd = 0
# tester.sh, debugger.shで利用できるようにexportする
export IS_ENABLE_DATADOG=$(dd)
# docker-composeの読み込み先を指定
## ファイル名のパターンを指定
pattern="docker-compose*.yml"
## ファイル名の一覧を取得し、-fオプションを付けて連結する
export COMPOSE_FILES_OPT=$(shell find . -name $(pattern) -exec printf -- '-f %s ' {} +)

test:
	# tで指定されたパス以下のtest.shを全て探してきて実行する
	echo -n "" > result
	find $(t) -type f -name "$(p)*test.sh"
	find $(t) -type f -name "$(p)*test.sh" | xargs -t -I{} bash tester.sh {} $m &&\
	test ! -s result && exit 1;\
	EXIT_CODE=$$(awk '{s+=$$1} END {print s}' result) &&\
	if [ $${EXIT_CODE} -ne 0 ]; then echo >&2 "command exited with $${EXIT_CODE}" && exit $${EXIT_CODE}; fi;

debug:
	# tで指定されたパス以下のdebug.shを全て探してきて実行する
	echo -n "" > result
	find $(t) -type f -name "${p}*debug.sh"
	if [ "$(t)" = "." ] && [ "$(p)" = "" ]; then p=full; fi; \
	find $(t) -type f -name "$${p}*debug.sh" | xargs -t -I{} bash debugger.sh {} $m

rm:
	docker-compose $(COMPOSE_FILES_OPT) rm -fs

# Dbのデータを削除
rm-db:
	docker-compose $(COMPOSE_FILES_OPT) down -v

gen-jwt:
	cp $t ../config/beaver_triple_service/sample.yaml
	docker-compose $(COMPOSE_FILES_OPT) down -v
	docker buildx bake $(COMPOSE_FILES_OPT) small_bts --load
	docker-compose $(COMPOSE_FILES_OPT) run --rm small_bts /bin/sh -c './beaver_triple_service generateJwt --file /QuickMPC/config/sample.yaml --output /QuickMPC/config'

create-gke-cluster:
	cd kubernetes/terraform && \
	terraform init && \
	terraform plan && \
	terraform apply

delete-gke-cluster:
	cd kubernetes/terraform && \
	terraform destroy
