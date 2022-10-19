#include "ProgressManager.hpp"

#include <atomic>
#include <chrono>

#include "LogHeader/Logger.hpp"

namespace qmpc::Job
{
class ProgressManager::Impl
{
public:
    using process_name_type = std::pair<int, std::string>;

    void push(const std::string& desc, const std::shared_ptr<Progress> elem)
    {
        const process_name_type::first_type id = counter++;
        const process_name_type key{id, desc};
        {
            std::lock_guard<std::mutex> lock(mutex);
            progress[key] = elem;
        }
    }

    void show()
    {
        const auto progress = [&]()
        {
            std::lock_guard<std::mutex> lock(mutex);
            return this->progress;
        }();
        for (const auto& [key, value] : progress)
        {
            spdlog::info("{} {}: {}", key.first, key.second, value->progress());
        }
    }

private:
    std::mutex mutex;
    std::map<process_name_type, std::shared_ptr<Progress>> progress;
    std::atomic<process_name_type::first_type> counter{0};
};

void ProgressManager::log()
{
    const auto progresses = [&]()
    {
        std::lock_guard<std::mutex> lock(dict_mtx);
        return this->progresses;
    }();

    for (const auto& [key, value] : progresses)
    {
        spdlog::info("{}", key);
        value->show();
    }
}

void ProgressManager::run()
{
    std::unique_lock<std::mutex> lock(shutdown_mtx);
    while (!shutdown_cv.wait_for(lock, std::chrono::seconds(5), [this] { return shutdown_flag; }))
    {
        this->log();
    }
}

void ProgressManager::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(shutdown_mtx);
        shutdown_flag = true;
    }
    shutdown_cv.notify_all();
}

void ProgressManager::registerJob(const int& id, const std::string& uuid)
{
    std::lock_guard<std::mutex> lock(dict_mtx);

    dict[id] = uuid;
    progresses[uuid] = std::make_shared<Impl>();
}

void ProgressManager::push(const int& id, const std::string& desc, std::shared_ptr<Progress> elem)
{
    const auto impl = [&]()
    {
        std::lock_guard<std::mutex> lock(dict_mtx);
        return progresses[dict[id]];
    }();
    impl->push(desc, elem);
}

std::shared_ptr<ProgressManager> ProgressManager::getInstance()
{
    static auto instance = std::make_shared<ProgressManager>();
    return instance;
}
void ProgressManager::runProgressManager()
{
    auto progress_manager = getInstance();
    spdlog::info("Progress Manager Start");

    progress_manager->run();
    spdlog::info("Progress Manager Exit");
}

}  // namespace qmpc::Job
