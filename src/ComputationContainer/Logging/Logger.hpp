#pragma once
#include <boost/exception/all.hpp>
#include <boost/format.hpp>
#include <boost/stacktrace.hpp>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <variant>

namespace qmpc
{
// TODO: std::coutをspdlogに差し替え
// TODO: クラスにする必要がないが念のためクラス構造にしておく
class Log
{
    enum class LogLevel
    {
        Info,
        Debug,
        Error
    };
    Log::LogLevel level;
    static inline std::unordered_map<qmpc::Log::LogLevel, std::string> logLevelStr = {
        {Log::LogLevel::Info, "INFO"},
        {Log::LogLevel::Debug, "DEBUG"},
        {Log::LogLevel::Error, "ERROR"}};

private:
    std::string getTime() const
    {
        char tt[100];
        auto t = time(nullptr);
        tm local;
        [[maybe_unused]] auto ret =
            localtime_r(&t, &local);  // ローカル時間(タイムゾーンに合わせた時間)を取得
        // TODO:osの環境変数にタイムゾーンの設定がない場合は時間がずれる
        strftime(tt, 256, "%Y-%m-%d %H:%M:%S%z", &local);
        return std::string(tt);
    }
    template <typename First, typename... Args>
    void write(std::ostream &os, First &&first, Args &&...args) const
    {
        std::stringstream ss;
        const char *delim = "|";
        ss << first;
        ((ss << delim << args), ...);
        os << ss.str() << std::endl;
    }

public:
    using traced = boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;
    Log() = delete;
    Log &operator=(Log &&) = delete;
    Log &operator=(const Log &) = delete;
    constexpr Log(Log::LogLevel level) : level(level) {}
    //例外生成用関数
    template <class E>
    static void throw_with_trace(const E &e)
    {
        throw boost::enable_error_info(e) << traced(boost::stacktrace::stacktrace());
    }

    template <typename... Args>
    static void writeLog(Log::LogLevel level, std::ostream &os, Args &&...args)
    {
        Log log(level);
        log.write(os, log.getTime(), Log::logLevelStr[level], std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void Info(Args &&...message)
    {
        writeLog(Log::LogLevel::Info, std::cout, std::forward<Args>(message)...);
    }

    template <typename... Args>
    static void Debug(Args &&...message)
    {
        writeLog(
            Log::LogLevel::Debug,
            std::cout,
            boost::stacktrace::stacktrace()[0],
            std::forward<Args>(message)...
        );
    }
    template <typename... Args>
    static void Error(Args &&...message)
    {
        writeLog(Log::LogLevel::Error, std::cerr, std::forward<Args>(message)...);
    }

};  // end Log
}  // namespace qmpc
