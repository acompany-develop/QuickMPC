#pragma once
#include <boost/exception/all.hpp>
#include <boost/format.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <variant>

#include "LogHeader/Logger.hpp"

namespace qmpc
{
class Log
{
private:
    std::shared_ptr<spdlog::logger> logger;
    Log() : logger(spdlog::default_logger())
    {
        logger->set_pattern("%Y-%m-%d %T %z | %^%-5l%$ | %g:%!:%# - %v");
    }
    Log &operator=(Log &&) = delete;
    Log &operator=(const Log &) = delete;

    static Log &getInstance()
    {
        static Log instance{};
        return instance;
    }

public:
    using traced = boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;
    // 例外生成用関数
    template <class E>
    [[noreturn]] static void throw_with_trace(const E &e)
    {
        throw boost::enable_error_info(e) << traced(boost::stacktrace::stacktrace());
    }

    template <typename... Args>
    static void Info(
        const spdlog::source_loc loc, fmt::format_string<Args...> fmt, Args &&...message
    )
    {
        getInstance().logger->log(loc, spdlog::level::info, fmt, std::forward<Args>(message)...);
    }

    template <typename... Args>
    static void Debug(
        const spdlog::source_loc loc, fmt::format_string<Args...> fmt, Args &&...message
    )
    {
        getInstance().logger->log(loc, spdlog::level::debug, fmt, std::forward<Args>(message)...);
    }
    template <typename... Args>
    static void Error(
        const spdlog::source_loc loc, fmt::format_string<Args...> fmt, Args &&...message
    )
    {
        getInstance().logger->log(loc, spdlog::level::err, fmt, std::forward<Args>(message)...);
    }
    template <typename... Args>
    static void Warn(
        const spdlog::source_loc loc, fmt::format_string<Args...> fmt, Args &&...message
    )
    {
        getInstance().logger->log(loc, spdlog::level::warn, fmt, std::forward<Args>(message)...);
    }

};  // end Log
}  // namespace qmpc

#define QMPC_LOG_LOCATION() \
    spdlog::source_loc { __FILE__, __LINE__, SPDLOG_FUNCTION }
#define QMPC_LOGGER_CALL(func, ...) func(QMPC_LOG_LOCATION(), __VA_ARGS__)
#define QMPC_LOG_INFO(...) QMPC_LOGGER_CALL(qmpc::Log::Info, __VA_ARGS__)
#define QMPC_LOG_DEBUG(...) QMPC_LOGGER_CALL(qmpc::Log::Debug, __VA_ARGS__)
#define QMPC_LOG_ERROR(...) QMPC_LOGGER_CALL(qmpc::Log::Error, __VA_ARGS__)
#define QMPC_LOG_WARN(...) QMPC_LOGGER_CALL(qmpc::Log::Warn, __VA_ARGS__)
