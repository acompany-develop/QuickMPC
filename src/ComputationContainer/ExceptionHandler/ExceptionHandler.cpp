#include <dlfcn.h>

#include <iostream>
#include <sstream>

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
    std::cerr << CALL_WITH_DBG_INFO(generateLogHeader) << "exception handler is called.  stacktrace is following." << std::endl;

    const boost::stacktrace::stacktrace bt = boost::stacktrace::stacktrace();
    const std::size_t frames = bt.size();
    for (std::size_t i = 0; i < frames; ++i) {
        std::cerr << CALL_WITH_DBG_INFO(generateLogHeader) << i << "# " << bt[i] << std::endl;
    }

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
