#include <iostream>
#include <sstream>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "logging/logger.hpp"
#include "spdlog/sinks/ostream_sink.h"

#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"

template <typename Func>
std::vector<std::string> consoleDebug(Func &&f)
{
    std::ostringstream oss;
    auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);

    qmpc::Log::addSink(oss_sink);

    f();

    qmpc::Log::removeSink(oss_sink);

    std::string output(oss.str());
    std::vector<std::string> testVec;
    boost::algorithm::split(testVec, output, boost::is_any_of("-|"));

    for (auto &msg : testVec)
    {
        boost::algorithm::trim(msg);
    }

    return testVec;
}
TEST(LogTest, InfoTest)
{
    int a = 5;
    double b = 3.4;
    std::string message = "info log";

    auto testVec = consoleDebug([=]() { QMPC_LOG_INFO("{}|{}|{}", message, a, b); });
    QMPC_LOG_INFO("size: {}", testVec.size());
    for (const auto &v : testVec)
    {
        QMPC_LOG_INFO("v={}", v);
    }
    EXPECT_EQ("info", testVec[3]);
    EXPECT_EQ("info log", testVec[5]);
    EXPECT_EQ("5", testVec[6]);
    EXPECT_EQ("3.4", testVec[7]);
}

TEST(LogTest, DebugTest)
{
    qmpc::Log::loadLogLevel("debug");

    std::string message = "debug log";
    auto testVec = consoleDebug([=]() { QMPC_LOG_DEBUG(message); });

    qmpc::Log::loadLogLevel(std::nullopt);
    for (const auto &v : testVec)
    {
        QMPC_LOG_INFO("v={}", v);
    }

    EXPECT_EQ("debug", testVec[3]);
    EXPECT_EQ("debug log", testVec[5]);
}

TEST(LogTest, ErrorTest)
{
    std::string message = "error log";
    auto testVec = consoleDebug([message]() { QMPC_LOG_ERROR(message); });
    EXPECT_EQ("error", testVec[3]);
    EXPECT_EQ("error log", testVec[5]);
}
void f() { qmpc::Log::throw_with_trace(std::runtime_error("runtime error")); }
TEST(LogTest, exceptionTest)
{
    try
    {
        f();
    }
    catch (const std::exception &e)
    {
        auto testVec = consoleDebug(
            [&e]()
            { QMPC_LOG_ERROR("{} | {}", *boost::get_error_info<qmpc::Log::traced>(e), e.what()); }
        );
        for (const auto &v : testVec)
        {
            QMPC_LOG_INFO("v={}", v);
        }

        EXPECT_EQ("error", testVec[3]);
        EXPECT_EQ("runtime error", testVec[testVec.size() - 1]);
    }
}
