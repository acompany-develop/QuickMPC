package log

import (
	"go.uber.org/zap"
	"go.uber.org/zap/zapcore"
)

var appLogOptions map[string]zap.Option
var appLogCfg zap.Config

// AppLogger は、ApplicationLoggerのグローバルなシングルトンインスタンス
var AppLogger ApplicationLogger

func init() {
	level := zap.NewAtomicLevel()
	level.SetLevel(zapcore.DebugLevel)

	appLogCfg = zap.NewProductionConfig()
	appLogCfg.Level = zap.NewAtomicLevelAt(zap.DebugLevel)
	appLogCfg.EncoderConfig.TimeKey = "time_local"
	appLogCfg.EncoderConfig.LevelKey = "log_levels"
	appLogCfg.EncoderConfig.CallerKey = "processing_location"
	appLogCfg.EncoderConfig.MessageKey = "custom_message"
	appLogCfg.EncoderConfig.EncodeTime = syslogTimeEncoder
	appLogCfg.OutputPaths = []string{"stdout"}
	appLogCfg.ErrorOutputPaths = []string{"stdout"}

	appLogOptions = map[string]zap.Option{}
}

type ApplicationLogger struct{}

// $time_local | $log_levels | $processing_location - $custom_message
// # $time_local          ： 時刻(YYYY-MM-DD HH:MM:SS (+|-)NNNN)
// # $log_levels          ： ログレベル(左寄せ5桁 空白埋め)
// # $processing_location ： 処理場所(ファイル名:関数名:行数)
// # $custom_message      ： カスタムメッセージ
func getApplicationLogger() *zap.SugaredLogger {
	logger, _ := appLogCfg.Build(zap.AddCallerSkip(1))
	for _, option := range appLogOptions {
		logger = logger.WithOptions(option)
	}
	defer logger.Sync()
	return logger.Sugar()
}

func (app ApplicationLogger) SetStacktrace() {
	appLogOptions["stacktrace"] = zap.AddStacktrace(zap.DebugLevel)
}

func (app ApplicationLogger) UnsetStacktrace() {
	delete(appLogOptions, "stacktrace")
}

func (app ApplicationLogger) Debug(args ...interface{}) {
	getApplicationLogger().Debug(args...)
}

func (app ApplicationLogger) Info(args ...interface{}) {
	getApplicationLogger().Info(args...)
}

func (app ApplicationLogger) Warning(args ...interface{}) {
	getApplicationLogger().Warn(args...)
}

func (app ApplicationLogger) Error(args ...interface{}) {
	getApplicationLogger().Error(args...)
}

// 使用すると強制終了するので注意
func (app ApplicationLogger) Fatal(args ...interface{}) {
	getApplicationLogger().Fatal(args...)
}

func (app ApplicationLogger) Debugf(template string, args ...interface{}) {
	getApplicationLogger().Debugf(template, args...)
}

func (app ApplicationLogger) Infof(template string, args ...interface{}) {
	getApplicationLogger().Infof(template, args...)
}

func (app ApplicationLogger) Warningf(template string, args ...interface{}) {
	getApplicationLogger().Warnf(template, args...)
}

func (app ApplicationLogger) Errorf(template string, args ...interface{}) {
	getApplicationLogger().Errorf(template, args...)
}

// 使用すると強制終了するので注意
func (app ApplicationLogger) Fatalf(template string, args ...interface{}) {
	getApplicationLogger().Fatalf(template, args...)
}
