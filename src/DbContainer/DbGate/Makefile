# bench計測結果の出力先
BENCH_PATH := /QuickMPC/Client/DbGateToDb
TEST_PATH := /QuickMPC/Client/DbGateToDb
BENCH_DIR := /QuickMPC/bench/log
DATE := $(shell date +%y%m%d%H%M%S%3N)

.PHONY: build
build:
	go mod vendor && go build

# testを実行する
.PHONY: test
test:
# 直列にtestする
	go test ${TEST_PATH} -parallel 1

# bench結果のログを見やすくパースする
.PHONY: benchstat
benchstat:
	benchstat ${BENCH_DIR}/test-${DATE}.out

# benchを実行して結果ログを見やすくパースして表示する
.PHONY: bench
bench:
# 直列に5回ずつ実行する
	go test ${BENCH_PATH} -bench . -parallel 1 -count 5 -benchmem -trace ${BENCH_DIR}/test-${DATE}.trace > ${BENCH_DIR}/test-${DATE}.out
	benchstat ${BENCH_DIR}/test-${DATE}.out

# bench結果を全て削除する
.PHONY: clean
clean:
	rm bench/log/*

# benchログのパースに必要なモジュールのinstall
.PHONY: setup
bench-setup:
	go install -u golang.org/x/perf/cmd/benchstat@latest
	go mod vendor