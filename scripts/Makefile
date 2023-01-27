#!make
NETWORK_NAME = shared-network
NETWORK_GATEWAY = 10.0.0.1
NETWORK_SUBNET = 10.0.0.0/16
# testのターゲットディレクトリ
# ex. make test t=./Hoge -> ./Hoge以下から*test.shを探す
t =.
# testのbuild, runの切り替え
# ex. make test m=build -> build()に定義した処理だけ走る
m =
# testの実行するshellscriptのprefixを指定する
# ex. make test p=unit -> unit*test.shが実行される
p =

# docker-composeを束ねるネットワークの作成
upnet:
	@if [ -z "`docker network ls | grep $(NETWORK_NAME)`" ]; then docker network create --gateway $(NETWORK_GATEWAY) --subnet $(NETWORK_SUBNET) $(NETWORK_NAME); fi

# docker-composeを束ねるネットワークの削除
downnet:
	- docker network rm $(NETWORK_NAME)

test: upnet
	# tで指定されたパス以下のtest.shを全て探してきて実行する
	echo -n "" > result
	find $(t) -type f -name "$(p)*test.sh" | xargs -t -I{} bash tester.sh {} $m &&\
	test ! -s result && exit 1;\
	EXIT_CODE=$$(awk '{s+=$$1} END {print s}' result) &&\
	if [ $${EXIT_CODE} -ne 0 ]; then echo >&2 "command exited with $${EXIT_CODE}" && exit $${EXIT_CODE}; fi;\

debug: upnet
	# tで指定されたパス以下のdebug.shを全て探してきて実行する
	echo -n "" > result
	if [ "$(t)" = "." ] && [ "$(p)" = "" ]; then p=full; fi; \
	find $(t) -type f -name "$${p}*debug.sh" | xargs -t -I{} bash debugger.sh {} $m

rm:
	docker-compose rm -fs

# Dbのデータを削除
rm-db:
	docker-compose down -v
