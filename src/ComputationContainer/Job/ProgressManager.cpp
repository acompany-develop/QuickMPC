#include "ProgressManager.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <optional>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "LogHeader/Logger.hpp"
#include "external/Proto/common_types/common_types.pb.h"

namespace qmpc::Job
{

static std::string generate_uuid()
{
    static thread_local boost::uuids::random_generator gen;
    return boost::lexical_cast<std::string>(gen());
}

class Observer
{
public:
    using process_name_type = std::pair<std::size_t, std::string>;

    void push(const std::shared_ptr<const Progress> elem)
    {
        const process_name_type key{elem->id(), elem->description()};
        {
            std::lock_guard<std::mutex> lock(mutex);
            progress[key] = elem;
        }
    }

    std::vector<pb_common_types::ProcedureProgress> info()
    {
        const auto progress = [&]()
        {
            std::lock_guard<std::mutex> lock(mutex);
            return this->progress;
        }();

        std::vector<pb_common_types::ProcedureProgress> ret;
        ret.reserve(progress.size());

        for (const auto& [_, elem] : progress)
        {
            static_cast<void>(_);  // unused
            pb_common_types::ProcedureProgress proc;
            proc.set_id(elem->id());
            proc.set_description(elem->description());
            proc.set_progress(elem->progress());
            proc.set_completed(elem->completed());
            const std::optional<std::string> details = elem->details();
            if (details.has_value())
            {
                proc.set_details(details.value());
            }
            ret.emplace_back(std::move(proc));
        }

        return ret;
    }

    void finishProgress(const Progress& elem)
    {
        // TODO: erase `elem` from progress which this class manages
        //       if that procedure maybe finished quickly.
        //       e.g., multiplication of vectors
        const process_name_type key{elem.id(), elem.description()};
        spdlog::debug("[PROGRESS] {} {} : finished", key.first, key.second);
    }

    std::size_t generateId() { return counter++; }

    void updateJobStatus(const pb_common_types::JobStatus& status) { this->status = status; }
    pb_common_types::JobStatus getJobStatus() const { return this->status; }

private:
    std::mutex mutex;
    std::map<process_name_type, std::shared_ptr<const Progress>> progress;
    std::atomic<pb_common_types::JobStatus> status;
    std::atomic<process_name_type::first_type> counter{0};
};

Progress::Progress(const Builder& builder)
    : Progress(builder.id, builder.description, builder.observer)
{
}
Progress::Progress(std::size_t id, std::string description, std::shared_ptr<Observer> observer)
    : id_(id), description_(description), observer_(observer), completed_(false)
{
}

std::optional<std::string> Progress::details() const { return std::nullopt; }
void Progress::before_finish() {}

void Progress::finish()
{
    before_finish();
    completed_ = true;
    observer_->finishProgress(*this);
}
const std::size_t& Progress::id() const { return id_; }
const std::string& Progress::description() const { return description_; }
const bool& Progress::completed() const { return completed_; }

ProgressIters::ProgressIters(const Progress::Builder& builder, const std::size_t& size)
    : Progress(builder), size(size), index(0)
{
}
void ProgressIters::update(const std::size_t& index) { this->index = index; }
float ProgressIters::progress() const { return 100.f * index / size; }
std::optional<std::string> ProgressIters::details() const
{
    std::ostringstream os;
    os << index << "/" << size;
    return os.str();
}
void ProgressIters::before_finish() { this->index = size; }

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
        for (const auto& proc : value->info())
        {
            spdlog::debug(
                "[PROGRESS] {} {}: {:3.2f} % -- {}",
                proc.id(),
                proc.description(),
                proc.progress(),
                proc.has_details() ? proc.details() : "<no details>"
            );
        }
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
    const std::size_t count = [&]()
    {
        std::lock_guard<std::mutex> lock(dict_mtx);
        return job_id_to_job_uuid.count(id);
    }();
    if (count == 0)
    {
        const std::string uuid = generate_uuid();
        spdlog::info("ProgressManager: temporary uuid: {} was generated", uuid);
        // TODO: check whether uuid is temporary or not.
        registerJob(id, uuid);
    }
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

    if (progresses.count(job_uuid) == 0)
    {
        spdlog::error(
            "{}:{}:{} observer with job_uuid: {} was not found",
            __FILE__,
            __LINE__,
            __func__,
            job_uuid
        );
        return;
    }

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

std::pair<std::optional<pb_common_types::JobProgress>, ProgressManager::StatusCode>
ProgressManager::getProgress(const std::string& job_uuid)
{
    const auto observer = [&]() -> std::optional<std::shared_ptr<Observer>>
    {
        std::lock_guard<std::mutex> lock(dict_mtx);
        if (progresses.count(job_uuid) == 0)
        {
            return std::nullopt;
        }
        return progresses[job_uuid];
    }();

    if (!observer.has_value())
    {
        spdlog::warn(
            "{}:{}:{} observer with job_uuid: {} was not found",
            __FILE__,
            __LINE__,
            __func__,
            job_uuid
        );
        return {std::nullopt, ProgressManager::StatusCode::NOT_FOUND};
    }

    const std::vector<pb_common_types::ProcedureProgress> procedures = observer.value()->info();

    pb_common_types::JobProgress progress;
    progress.set_job_uuid(job_uuid);
    progress.set_status(observer.value()->getJobStatus());

    for (const auto& proc : procedures)
    {
        auto ptr = progress.add_progresses();
        if (ptr == nullptr)
        {
            spdlog::error(
                "{}:{}:{} allocation of return value was failed -- job_uuid: {}",
                __FILE__,
                __LINE__,
                __func__,
                job_uuid
            );
            return {std::nullopt, ProgressManager::StatusCode::INTERNAL_ERROR};
        }
        *ptr = proc;
    }

    return {progress, ProgressManager::StatusCode::OK};
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
