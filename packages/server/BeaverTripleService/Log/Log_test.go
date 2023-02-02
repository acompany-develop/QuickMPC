package log

import (
	"time"

	"go.uber.org/zap"
)

type constantClock time.Time

func (c constantClock) Now() time.Time {
	return time.Time(c)
}
func (c constantClock) NewTicker(duration time.Duration) *time.Ticker {
	return &time.Ticker{}
}

type S struct {
	n int
	s string
}

// テスト出力用各種変数
var strVal string = "str"
var intVal int = 1
var errVal error = nil
var arrayVal []int = []int{1, 2}
var structVal S = S{n: 1, s: "s"}

func init() {
	// テスト用のログ時間固定処理
	date := time.Date(2022, 5, 20, 17, 10, 30, 15, time.UTC)
	clock := constantClock(date)
	options["clock"] = zap.WithClock(clock)
}

//NOTE: fatalレベルのログを出すとexit(1)されてテストが落ちるのでfatalだけ除外
func ExampleDebug() {
	Debug(strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// 2022-05-20T17:10:30.000Z	DEBUG	Log/Log_test.go:39	str1 <nil> [1 2] {1 s}
}

func ExampleInfo() {
	Info(strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// 2022-05-20T17:10:30.000Z	INFO	Log/Log_test.go:45	str1 <nil> [1 2] {1 s}
}

func ExampleWarning() {
	Warning(strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// 2022-05-20T17:10:30.000Z	WARN	Log/Log_test.go:51	str1 <nil> [1 2] {1 s}
}

func ExampleError() {
	Error(strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// 2022-05-20T17:10:30.000Z	ERROR	Log/Log_test.go:57	str1 <nil> [1 2] {1 s}
}

func ExampleDebugf() {
	Debugf("%s %d %v %v %v", strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// 2022-05-20T17:10:30.000Z	DEBUG	Log/Log_test.go:63	str 1 <nil> [1 2] {1 s}
}

func ExampleInfof() {
	Infof("%s %d %v %v %v", strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// 2022-05-20T17:10:30.000Z	INFO	Log/Log_test.go:69	str 1 <nil> [1 2] {1 s}
}

func ExampleWarningf() {
	Warningf("%s %d %v %v %v", strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// 2022-05-20T17:10:30.000Z	WARN	Log/Log_test.go:75	str 1 <nil> [1 2] {1 s}
}

func ExampleErrorf() {
	Errorf("%s %d %v %v %v", strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// 2022-05-20T17:10:30.000Z	ERROR	Log/Log_test.go:81	str 1 <nil> [1 2] {1 s}
}
