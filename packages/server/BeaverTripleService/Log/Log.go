package log

import (
	"go.uber.org/zap"
	"go.uber.org/zap/zapcore"
)

var options map[string]zap.Option
var cfg *zap.Config

func init() {
	options = map[string]zap.Option{}

	level := zap.NewAtomicLevel()
	level.SetLevel(zapcore.DebugLevel)
	cfg = &zap.Config{
		Level:    level,
		Encoding: "console",
		EncoderConfig: zapcore.EncoderConfig{
			TimeKey:        "Time",
			LevelKey:       "Level",
			NameKey:        "Name",
			CallerKey:      "Caller",
			MessageKey:     "Msg",
			StacktraceKey:  "St",
			EncodeLevel:    zapcore.CapitalLevelEncoder,
			EncodeTime:     zapcore.ISO8601TimeEncoder,
			EncodeDuration: zapcore.StringDurationEncoder,
			EncodeCaller:   zapcore.ShortCallerEncoder,
		},
		DisableStacktrace: true,
		// fileにも出力する場合はファイルパスを追加する
		OutputPaths:      []string{"stdout"},
		ErrorOutputPaths: []string{"stderr"},
	}
}

// NOTE: ビルドしたものをグローバルに置いた場合テストで想定しない挙動が起きる
// その回避策として毎回loggerのビルドを行う
func getLogger() *zap.SugaredLogger {
	logger, _ := cfg.Build(zap.AddCallerSkip(1))
	for _, option := range options {
		logger = logger.WithOptions(option)
	}
	return logger.Sugar()
}

func SetStacktrace() {
	options["stacktrace"] = zap.AddStacktrace(zap.DebugLevel)
}
func UnsetStacktrace() {
	delete(options, "stacktrace")
}

func Debug(args ...interface{}) {
	getLogger().Debug(args...)
}

func Info(args ...interface{}) {
	getLogger().Info(args...)
}

func Warning(args ...interface{}) {
	getLogger().Warn(args...)
}

func Error(args ...interface{}) {
	getLogger().Error(args...)
}

// 使用すると強制終了するので注意
func Fatal(args ...interface{}) {
	getLogger().Fatal(args...)
}

func Debugf(template string, args ...interface{}) {
	getLogger().Debugf(template, args...)
}

func Infof(template string, args ...interface{}) {
	getLogger().Infof(template, args...)
}

func Warningf(template string, args ...interface{}) {
	getLogger().Warnf(template, args...)
}

func Errorf(template string, args ...interface{}) {
	getLogger().Errorf(template, args...)
}

// 使用すると強制終了するので注意
func Fatalf(template string, args ...interface{}) {
	getLogger().Fatalf(template, args...)
}
