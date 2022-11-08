#include "ProgressManager.hpp"

#include <chrono>
#include <optional>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "Logging/Logger.hpp"
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

    /**
     * add procedure progress to management target
     */
    void push(const std::shared_ptr<const Progress> elem)
    {
        const process_name_type key{elem->id(), elem->description()};
        {
            std::lock_guard<std::mutex> lock(mutex);
            progress[key] = elem;
        }
    }

    /**
     * get progress information with protobuf type
     */
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

    /**
     * called from `Progress`
     */
    void finishProgress(const Progress& elem)
    {
        // TODO: erase `elem` from progress which this class manages
        //       if that procedure maybe finished quickly.
        //       e.g., multiplication of vectors
        const process_name_type key{elem.id(), elem.description()};
        QMPC_LOG_DEBUG(
            "{}:{}:{} - [PROGRESS] {} {} : finished",
            __FILE__,
            __func__,
            __LINE__,
            key.first,
            key.second
        );
    }

    /**
     * generate unique id for procedure in specifig job UUID context
     */
    std::size_t generateId() { return counter++; }

    // setter, getter
    void updateJobStatus(const pb_common_types::JobStatus& status) { this->status = status; }
    pb_common_types::JobStatus getJobStatus() const { return this->status; }

private:
    std::mutex mutex;
    std::map<process_name_type, std::shared_ptr<const Progress>> progress;
    std::atomic<pb_common_types::JobStatus> status;
    std::atomic<process_name_type::first_type> counter{0};
};

Progress::Progress(const Builder& builder)
    : Progress(builder.id.value(), builder.description.value(), builder.observer.value())
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

template <ProgressOrder ORDER>
ProgressIters_<ORDER>::ProgressIters_(const Progress::Builder& builder, const std::size_t& size)
    : Progress(builder), size(size), index(0)
{
}
template <ProgressOrder ORDER>
void ProgressIters_<ORDER>::update(const std::size_t& index)
{
    this->index = index;
}
template <ProgressOrder ORDER>
float ProgressIters_<ORDER>::progress() const
{
    if constexpr (ORDER == ProgressOrder::ASCENDING)
    {
        return 100.f * index / size;
    }
    else
    {
        const auto rev = size - index;
        return 100.f * rev / size;
    }
}
template <ProgressOrder ORDER>
std::optional<std::string> ProgressIters_<ORDER>::details() const
{
    std::ostringstream os;
    os << index << "/" << size;
    return os.str();
}
template <ProgressOrder ORDER>
void ProgressIters_<ORDER>::before_finish()
{
    this->index = size;
}

void ProgressManager::log()
{
    const auto progresses = [&]()
    {
        std::lock_guard<std::recursive_mutex> lock(dict_mtx);
        return this->progresses;
    }();

    for (const auto& [job_uuid, observer] : progresses)
    {
        QMPC_LOG_INFO(
            "{}:{}:{} - [PROGRESS] Job UUID: {} -- start", __FILE__, __func__, __LINE__, job_uuid
        );
        for (const auto& proc : observer->info())
        {
            QMPC_LOG_INFO(
                "{}:{}:{} - [PROGRESS] {} {}: {:3.2f} % -- {}",
                __FILE__,
                __func__,
                __LINE__,
                proc.id(),
                proc.description(),
                proc.progress(),
                proc.has_details() ? proc.details() : "<no details>"
            );
        }
        QMPC_LOG_INFO(
            "{}:{}:{} - [PROGRESS] Job UUID: {} -- end", __FILE__, __func__, __LINE__, job_uuid
        );
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
    std::lock_guard<std::recursive_mutex> lock(dict_mtx);

    job_id_to_job_uuid[id] = uuid;
    job_uuid_to_job_id[uuid] = id;
    progresses[uuid] = std::make_shared<Observer>();
}

std::shared_ptr<Observer> ProgressManager::getObserver(const int& id)
{
    std::lock_guard<std::recursive_mutex> lock(dict_mtx);

    const std::size_t count = job_id_to_job_uuid.count(id);
    if (count == 0)
    {
        // This block should be reached when executing integration test
        const std::string uuid = generate_uuid();
        QMPC_LOG_INFO("ProgressManager: temporary uuid: {} was generated", uuid);
        // TODO: check whether uuid is temporary or not
        //       because generated objects were not going to be released.
        registerJob(id, uuid);
    }

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
    std::lock_guard<std::recursive_mutex> lock(dict_mtx);

    if (progresses.count(job_uuid) == 0)
    {
        QMPC_LOG_ERROR(
            "{}:{}:{} - observer with job_uuid: {} was not found",
            __FILE__,
            __func__,
            __LINE__,
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
        std::lock_guard<std::recursive_mutex> lock(dict_mtx);
        if (progresses.count(job_uuid) == 0)
        {
            return std::nullopt;
        }
        return progresses[job_uuid];
    }();

    if (!observer.has_value())
    {
        QMPC_LOG_WARN(
            "{}:{}:{} - observer with job_uuid: {} was not found",
            __FILE__,
            __func__,
            __LINE__,
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
            QMPC_LOG_ERROR(
                "{}:{}:{} - allocation of return value was failed -- job_uuid: {}",
                __FILE__,
                __func__,
                __LINE__,
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
    auto progress_manager = getInstance();
    QMPC_LOG_INFO("{}:{}:{} - Progress Manager Start" __FILE__, __func__, __LINE__);

    progress_manager->run();
    QMPC_LOG_INFO("{}:{}:{} - Progress Manager Exit" __FILE__, __func__, __LINE__);
}

}  // namespace qmpc::Job
