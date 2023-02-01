package log

import (
	"time"

	"go.uber.org/zap/zapcore"
)

// syslogTimeEncoder は QuickMPC のログ形式のRFCにおける `$time_local` の形に変換するためのエンコーダ
func syslogTimeEncoder(t time.Time, enc zapcore.PrimitiveArrayEncoder) {
	enc.AppendString(t.Format("2006-01-02 15:04:05 +09:00"))
}
