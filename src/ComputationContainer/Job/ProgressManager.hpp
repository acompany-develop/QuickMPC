#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>

namespace qmpc::Job
{
struct Progress
{
    virtual std::string progress() const;
};

struct ProgressIters : public Progress
{
    ProgressIters(const std::size_t& size);

    void update(const std::size_t& index);

    std::string progress() const override;

private:
    std::size_t size;
    std::atomic<std::size_t> index;
};

class ProgressManager
{
    std::mutex shutdown_mtx;
    std::condition_variable shutdown_cv;
    bool shutdown_flag;

    std::mutex dict_mtx;
    std::map<int, std::string> dict;

    class Impl;
    std::map<std::string, std::shared_ptr<Impl>> progresses;

    void run();
    void log();
    void push(const int& id, const std::string& desc, std::shared_ptr<Progress> elem);

public:
    static std::shared_ptr<ProgressManager> getInstance();
    static void runProgressManager();
    void shutdown();
    void registerJob(const int& id, const std::string& uuid);

    template <typename progress_type, class... Args>
    std::shared_ptr<progress_type> createProgress(
        const int& id, const std::string& desc, Args&&... args
    )
    {
        const auto progress = std::make_shared<progress_type>(args...);
        push(id, desc, progress);
    }
};
}  // namespace qmpc::Job
