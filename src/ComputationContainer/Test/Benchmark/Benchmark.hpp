#pragma once
#include <chrono>
#include <iostream>
#include <string>

#include "LogHeader/Logger.hpp"

/**
 * 演算 operation_func の実行時間を計測する．
 * 計測を n_iterations 回行い，各回の実行時間を表示し，
 * 最後に平均実行時間を表示する．
 */
template <typename LAMBDA>
static void measureExecTime(
    const std::string test_name, const int n_iterations, LAMBDA &&operation_func
)
{
    double sum_time_ms = 0.0;
    for (int i = 1; i <= n_iterations; ++i)
    {
        const auto clock_start = std::chrono::system_clock::now();
        operation_func();
        const auto clock_end = std::chrono::system_clock::now();
        const auto elapsed_time_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start).count();
        QMPC_LOG_INFO(
            "[{0} {1}/{2}] Elapsed time = {3} ms", test_name, i, n_iterations, elapsed_time_ms
        );
        sum_time_ms += elapsed_time_ms;
    }
    const auto mean_time_ms = sum_time_ms / n_iterations;
    QMPC_LOG_INFO("[{0}] Mean elapsed time = {1} ms", test_name, mean_time_ms);
}

/**
 * テストケース名にsizeを追記して返す．
 * 一括演算のテスト時にvectorのsizeを表示させるために使用する．
 */
template <typename T>
static std::string getTestNameWithSize(const std::string test_name, const T size)
{
    std::string s = test_name;
    s += " with size " + std::to_string(size);
    return s;
}
