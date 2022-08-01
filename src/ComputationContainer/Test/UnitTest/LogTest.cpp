#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"
#include "Logging/Logger.hpp"
#include <vector>
template <typename Stream, typename Func>
std::vector<std::string> consoleDebug(Stream &&s, Func &&f)
{
    std::stringstream ss;
    std::streambuf *buffer = s.rdbuf();
    s.rdbuf(ss.rdbuf());
    f();
    s.rdbuf(buffer);

    std::string str;
    std::vector<std::string> testVec;
    char delim = '|';
    while (std::getline(ss, str, delim))
    {
        testVec.push_back(str);
    }
    return testVec;
}
TEST(LogTest, InfoTest)
{

    int a = 5;
    double b = 3.4;
    std::string message = "info log";

    auto testVec = consoleDebug(std::cout, [=]()
                                { qmpc::Log::Info(message, a, b); });
    EXPECT_EQ("INFO", testVec[1]);
    EXPECT_EQ("info log", testVec[2]);
    EXPECT_EQ("5", testVec[3]);
    EXPECT_EQ("3.4\n", testVec[4]);
}

TEST(LogTest, DebugTest)
{
    std::string message = "debug log";
    auto testVec = consoleDebug(std::cout, [=]()
                                { qmpc::Log::Debug(message); });

    EXPECT_EQ("DEBUG", testVec[1]);
    EXPECT_EQ("debug log\n", testVec[3]);
}

TEST(LogTest, ErrorTest)
{
    std::string message = "error log";
    auto testVec = consoleDebug(std::cerr, [message]()
                                { qmpc::Log::Error(message); });
    EXPECT_EQ("ERROR", testVec[1]);
    EXPECT_EQ("error log\n", testVec[2]);
}
void f()
{
    qmpc::Log::throw_with_trace(std::runtime_error("runtime error"));
}
TEST(LogTest, exceptionTest)
{
    try
    {
        f();
    }
    catch (const std::exception &e)
    {
        auto testVec = consoleDebug(std::cerr, [&e]()
                                    { qmpc::Log::Error(*boost::get_error_info<qmpc::Log::traced>(e), e.what()); });

        EXPECT_EQ("ERROR", testVec[1]);
        EXPECT_EQ("runtime error\n", testVec[3]);
    }
}
