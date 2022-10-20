#include "ProgressManager.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <sstream>

#include "LogHeader/Logger.hpp"
#include "external/Proto/common_types/common_types.pb.h"

namespace qmpc::Job
{
class Observer
{
public:
    using process_name_type = std::pair<std::size_t, std::string>;

    void push(const std::shared_ptr<Progress> elem)
    {
        const process_name_type key{elem->id(), elem->description()};
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
            spdlog::debug("[PROGRESS] {} {}: {}", key.first, key.second, value->progress());
        }
    }

    void finishProgress(const Progress& elem)
    {
        // TODO
        const process_name_type key{elem.id(), elem.description()};
        spdlog::debug("[PROGRESS] {} {} : finished", key.first, key.second);
    }

    std::size_t generateId() { return counter++; }

    void updateJobStatus(const pb_common_types::JobStatus& status) { this->status = status; }

private:
    std::mutex mutex;
    std::map<process_name_type, std::shared_ptr<Progress>> progress;
    std::atomic<pb_common_types::JobStatus> status;
    std::atomic<process_name_type::first_type> counter{0};
};

Progress::Progress(const Builder& builder)
    : Progress(builder.id, builder.description, builder.observer)
{
}
Progress::Progress(std::size_t id, std::string description, std::shared_ptr<Observer> observer)
    : id_(id), description_(description), observer_(observer)
{
}

void Progress::finish() { observer_->finishProgress(*this); }
const std::size_t& Progress::id() const { return id_; }
const std::string& Progress::description() const { return description_; }

ProgressIters::ProgressIters(const Progress::Builder& builder, const std::size_t& size)
    : Progress(builder), size(size), index(0)
{
}
void ProgressIters::update(const std::size_t& index) { this->index = index; }
std::string ProgressIters::progress() const
{
    std::ostringstream os;
    os << index << "/" << size;
    return os.str();
}

void ProgressManager::log()
{
    const auto progresses = [&]()
    {
        std::lock_guard<std::mutex> lock(dict_mtx);
        return this->progresses;
    }();

    for (const auto& [key, value] : progresses)
    {
        spdlog::debug("[PROGRESS] {} -- start", key);
        value->show();
        spdlog::debug("[PROGRESS] {} -- end", key);
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

    job_id_to_job_uuid[id] = uuid;
    job_uuid_to_job_id[uuid] = id;
    progresses[uuid] = std::make_shared<Observer>();
}

std::shared_ptr<Observer> ProgressManager::getObserver(const int& id)
{
    std::lock_guard<std::mutex> lock(dict_mtx);
    return progresses[job_id_to_job_uuid[id]];
}

std::size_t ProgressManager::getProgressId(std::shared_ptr<Observer> observer)
{
    return observer->generateId();
}

void ProgressManager::push(std::shared_ptr<Observer> observer, std::shared_ptr<Progress> elem)
{
    observer->push(elem);
}

void ProgressManager::updateJobStatus(
    const std::string& job_uuid, const pb_common_types::JobStatus& status
)
{
    std::lock_guard<std::mutex> lock(dict_mtx);
    auto observer = progresses[job_uuid];
    observer->updateJobStatus(status);

    if (status == pb_common_types::JobStatus::COMPLETED)
    {
        progresses.erase(job_uuid);
        auto job_id = job_uuid_to_job_id[job_uuid];
        job_id_to_job_uuid.erase(job_id);
        job_uuid_to_job_id.erase(job_uuid);
    }
}

std::shared_ptr<ProgressManager> ProgressManager::getInstance()
{
    static auto instance = std::make_shared<ProgressManager>();
    return instance;
}
void ProgressManager::runProgressManager()
{
    spdlog::set_level(spdlog::level::debug);
    auto progress_manager = getInstance();
    spdlog::info("Progress Manager Start");

    progress_manager->run();
    spdlog::info("Progress Manager Exit");
}

}  // namespace qmpc::Job
