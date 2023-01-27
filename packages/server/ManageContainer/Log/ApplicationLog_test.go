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

var appLogger ApplicationLogger

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
	appLogOptions["clock"] = zap.WithClock(clock)
}

//NOTE: fatalレベルのログを出すとexit(1)されてテストが落ちるのでfatalだけ除外
func ExampleApplicationLogger_Debug() {
	appLogger.Debug(strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// {"log_levels":"debug","time_local":"2022-05-20 17:10:30 +09:00","processing_location":"Log/ApplicationLog_test.go:41","custom_message":"str1 <nil> [1 2] {1 s}"}
}

func ExampleApplicationLogger_Info() {
	appLogger.Info(strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// {"log_levels":"info","time_local":"2022-05-20 17:10:30 +09:00","processing_location":"Log/ApplicationLog_test.go:47","custom_message":"str1 <nil> [1 2] {1 s}"}
}

func ExampleApplicationLogger_Warning() {
	appLogger.Warning(strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// {"log_levels":"warn","time_local":"2022-05-20 17:10:30 +09:00","processing_location":"Log/ApplicationLog_test.go:53","custom_message":"str1 <nil> [1 2] {1 s}"}
}

func ExampleApplicationLogger_Error() {
	appLogger.Error(strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// {"log_levels":"error","time_local":"2022-05-20 17:10:30 +09:00","processing_location":"Log/ApplicationLog_test.go:59","custom_message":"str1 <nil> [1 2] {1 s}","stacktrace":"github.com/acompany-develop/QuickMPC/src/ManageContainer/Log.ExampleApplicationLogger_Error\n\t/QuickMPC/Log/ApplicationLog_test.go:59\ntesting.runExample\n\t/usr/local/go/src/testing/run_example.go:63\ntesting.runExamples\n\t/usr/local/go/src/testing/example.go:44\ntesting.(*M).Run\n\t/usr/local/go/src/testing/testing.go:1721\nmain.main\n\t_testmain.go:111\nruntime.main\n\t/usr/local/go/src/runtime/proc.go:250"}
}

func ExampleApplicationLogger_Debugf() {
	appLogger.Debugf("%s %d %v %v %v", strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// {"log_levels":"debug","time_local":"2022-05-20 17:10:30 +09:00","processing_location":"Log/ApplicationLog_test.go:65","custom_message":"str 1 <nil> [1 2] {1 s}"}
}

func ExampleApplicationLogger_Infof() {
	appLogger.Infof("%s %d %v %v %v", strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// {"log_levels":"info","time_local":"2022-05-20 17:10:30 +09:00","processing_location":"Log/ApplicationLog_test.go:71","custom_message":"str 1 <nil> [1 2] {1 s}"}
}

func ExampleApplicationLogger_Warningf() {
	appLogger.Warningf("%s %d %v %v %v", strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// {"log_levels":"warn","time_local":"2022-05-20 17:10:30 +09:00","processing_location":"Log/ApplicationLog_test.go:77","custom_message":"str 1 <nil> [1 2] {1 s}"}
}

func ExampleApplicationLogger_Errorf() {
	appLogger.Errorf("%s %d %v %v %v", strVal, intVal, errVal, arrayVal, structVal)
	// Output:
	// {"log_levels":"error","time_local":"2022-05-20 17:10:30 +09:00","processing_location":"Log/ApplicationLog_test.go:83","custom_message":"str 1 <nil> [1 2] {1 s}","stacktrace":"github.com/acompany-develop/QuickMPC/src/ManageContainer/Log.ExampleApplicationLogger_Errorf\n\t/QuickMPC/Log/ApplicationLog_test.go:83\ntesting.runExample\n\t/usr/local/go/src/testing/run_example.go:63\ntesting.runExamples\n\t/usr/local/go/src/testing/example.go:44\ntesting.(*M).Run\n\t/usr/local/go/src/testing/testing.go:1721\nmain.main\n\t_testmain.go:111\nruntime.main\n\t/usr/local/go/src/runtime/proc.go:250"}
}
