#pragma once
#include <memory>
#include <optional>
#include <utility>

#include <boost/exception/all.hpp>
#include <boost/format.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>

#include "LogHeader/Logger.hpp"

#define QMPC_LOG_LOCATION() \
    spdlog::source_loc { __FILE__, __LINE__, SPDLOG_FUNCTION }
#define QMPC_LOGGER_CALL(func, ...) func(QMPC_LOG_LOCATION(), __VA_ARGS__)
#define QMPC_LOG_INFO(...) QMPC_LOGGER_CALL(qmpc::Log::Info, __VA_ARGS__)
#define QMPC_LOG_DEBUG(...) QMPC_LOGGER_CALL(qmpc::Log::Debug, __VA_ARGS__)
#define QMPC_LOG_ERROR(...) QMPC_LOGGER_CALL(qmpc::Log::Error, __VA_ARGS__)
#define QMPC_LOG_WARN(...) QMPC_LOGGER_CALL(qmpc::Log::Warn, __VA_ARGS__)

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

    static void loadLogLevel(const std::optional<std::string> &log_level_name)
    {
        getInstance().logger->set_level(spdlog::level::info);
        const spdlog::level::level_enum level = [&log_level_name]()
        {
            if (log_level_name.has_value())
            {
                return spdlog::level::from_str(log_level_name.value());
            }
            QMPC_LOG_INFO("log level was not set, QMPC uses default value");
            return spdlog::level::info;
        }();
        QMPC_LOG_INFO("log level: {}", spdlog::level::to_string_view(level));
        getInstance().logger->set_level(level);
    }

    template <typename... Args>
    static void Info(
        const spdlog::source_loc loc, fmt::format_string<Args...> fmt, Args &&...message
    )
    {
        getInstance().logger->log(loc, spdlog::level::info, fmt, std::forward<Args>(message)...);
    }
    template <typename T>
    static void Info(const spdlog::source_loc loc, const T &value)
    {
        getInstance().logger->log(loc, spdlog::level::info, value);
    }

    template <typename... Args>
    static void Debug(
        const spdlog::source_loc loc, fmt::format_string<Args...> fmt, Args &&...message
    )
    {
        getInstance().logger->log(loc, spdlog::level::debug, fmt, std::forward<Args>(message)...);
    }
    template <typename T>
    static void Debug(const spdlog::source_loc loc, const T &value)
    {
        getInstance().logger->log(loc, spdlog::level::debug, value);
    }

    template <typename... Args>
    static void Error(
        const spdlog::source_loc loc, fmt::format_string<Args...> fmt, Args &&...message
    )
    {
        getInstance().logger->log(loc, spdlog::level::err, fmt, std::forward<Args>(message)...);
    }
    template <typename T>
    static void Error(const spdlog::source_loc loc, const T &value)
    {
        getInstance().logger->log(loc, spdlog::level::err, value);
    }

    template <typename... Args>
    static void Warn(
        const spdlog::source_loc loc, fmt::format_string<Args...> fmt, Args &&...message
    )
    {
        getInstance().logger->log(loc, spdlog::level::warn, fmt, std::forward<Args>(message)...);
    }
    template <typename T>
    static void Warn(const spdlog::source_loc loc, const T &value)
    {
        getInstance().logger->log(loc, spdlog::level::warn, value);
    }

};  // end Log
}  // namespace qmpc
