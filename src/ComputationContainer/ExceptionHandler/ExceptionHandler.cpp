#include <dlfcn.h>

#include <iostream>
#include <sstream>
#include <regex>

#include <boost/stacktrace.hpp>

// qmpc::Log, spdlog cannot be used here,
// so, this is re-implementing log format.

static std::string getTime()
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

static std::string generateLogHeader(const std::string file_name, const std::string func_name, const int line)
{
    std::stringstream ss;
    ss << getTime() << " | ERROR | " << file_name << ":" << func_name << ":" << line << " - ";
    return ss.str();
}

#define CALL_WITH_DBG_INFO(func) func(__FILE__, __func__, __LINE__)

extern "C"
{

// to override `throw`, re-define __cxa_throw
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
void __cxa_throw(void* ptr, std::type_info* tinfo, void (*dest)(void*))
{
    // logging
    const std::string log_format_head = CALL_WITH_DBG_INFO(generateLogHeader);

    std::cerr << log_format_head << "exception handler is called.  stacktrace is following." << std::endl;

    std::stringstream ss;
    ss << boost::stacktrace::stacktrace();
    const std::string stacktrace = ss.str();

    // replace newline to follow log format
    // this way is used because iteration over stacktrace cause performance issue
    const std::string fmt = "\n" + log_format_head;
    std::regex regex(R"(\n)");
    std::regex_replace(std::ostream_iterator<char>(std::cerr), std::begin(stacktrace), std::end(stacktrace), regex, fmt);
    std::cerr << "stacktrace is dumped." << std::endl;

    // find next definition of __cxa_throw which is in standard library.
    auto* handle = reinterpret_cast<decltype(__cxa_throw)*>(dlsym(RTLD_NEXT, __func__));
    if (!handle)
    {
        return;
    }

    // call original __cxa_throw
    handle(ptr, tinfo, dest);
}
#pragma GCC diagnostic warning "-Wbuiltin-declaration-mismatch"

}
