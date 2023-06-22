#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <type_traits>

#include "common_types/common_types.pb.h"

namespace qmpc::Job
{
class Observer;

struct Progress
{
    struct Builder
    {
        Builder() : id(std::nullopt), description(std::nullopt), observer(std::nullopt) {}
        Builder& setId(const std::size_t& id)
        {
            this->id = id;
            return *this;
        }
        Builder& setDesc(const std::string& desc)
        {
            this->description = desc;
            return *this;
        }
        Builder& setObserver(const std::shared_ptr<Observer>& observer)
        {
            this->observer = observer;
            return *this;
        }

        friend Progress;

    private:
        std::optional<std::size_t> id;
        std::optional<std::string> description;
        std::optional<std::shared_ptr<Observer>> observer;
    };

    Progress() = delete;
    /**
     * instantiate Progress from Builder.
     * bacause all parameters is needed,
     * this constructor throw `std::bad_optional_access`
     * if even one parameter is not set.
     */
    Progress(const Builder& builder);

    /**
     * represent progress by [0.0, 100.0].
     */
    virtual float progress() const = 0;
    /**
     * optional method to describe progress in details by string
     */
    virtual std::optional<std::string> details() const;

    /**
     * this function applies followings:
     * - call `Progress::before_finish()`
     * - notify observer that this progress was finished
     * - set completed flag as true
     */
    virtual void finish() final;

    // getter functions
    virtual const std::size_t& id() const final;
    virtual const std::string& description() const final;
    virtual const bool& completed() const final;

private:
    Progress(std::size_t id, std::string description, std::shared_ptr<Observer> observer);

    /**
     * optional overridable function.
     * override this function if inherited progress class has to do something before finish.
     * e.g. set progress as completed in internal representation.
     * this function is called from `Progress::finish()`.
     */
    virtual void before_finish();

    std::size_t id_;
    std::string description_;
    std::shared_ptr<Observer> observer_;
    bool completed_;
};

enum class ProgressOrder
{
    ASCENDING,
    DESCENDING
};
template <ProgressOrder ORDER>
struct ProgressIters_ : public Progress
{
    /**
     * use ProgressManager::createProgress to instantiate
     * @param builder used for constructing Progress
     * @param size number of iterations
     * @see ProgressManager::createProgress
     */
    ProgressIters_(const Progress::Builder& builder, const std::size_t& size);

    /**
     * update progress with atomic store
     * @param index desired value
     */
    void update(const std::size_t& index);

    /**
     * calculate progress according as `ORDER`
     */
    float progress() const override;
    std::optional<std::string> details() const override;

private:
    /**
     * set `index` = `size`
     */
    void before_finish() override;

    std::size_t size;
    std::atomic<std::size_t> index;
};
using ProgressIters = ProgressIters_<ProgressOrder::ASCENDING>;
template struct ProgressIters_<ProgressOrder::ASCENDING>;
template struct ProgressIters_<ProgressOrder::DESCENDING>;

/**
 * utility class to manage Progress.
 * call Progress::finish when this object becomes end of life by going out block.
 */
template <typename T>
class ScopedProgress
{
    // std::derived_from
    static_assert(std::is_base_of_v<qmpc::Job::Progress, T>);
    static_assert(std::is_convertible_v<const volatile T*, const volatile Progress*>);

public:
    ScopedProgress(std::shared_ptr<T> progress) : progress(progress) {}
    ScopedProgress(const ScopedProgress<T>&) = delete;
    ScopedProgress(ScopedProgress<T>&& rhs) : progress(rhs.progress) { rhs.progress = nullptr; }
    ~ScopedProgress()
    {
        if (progress)
        {
            progress->finish();
        }
    }

    ScopedProgress& operator=(const ScopedProgress<T>&) = delete;
    ScopedProgress& operator=(const ScopedProgress<T>&&) = delete;

    std::shared_ptr<T> operator->() { return progress; }
    std::shared_ptr<const T> operator->() const { return progress; }

private:
    std::shared_ptr<T> progress;
};

class ProgressManager
{
    std::mutex shutdown_mtx;
    std::condition_variable shutdown_cv;
    bool shutdown_flag;

    std::recursive_mutex dict_mtx;  //!< mutex which is used for touching shared dictionaries
    std::map<int, std::string> job_id_to_job_uuid;                //!< conversion table
    std::map<std::string, int> job_uuid_to_job_id;                //!< conversion table
    std::map<std::string, std::shared_ptr<Observer>> progresses;  //!< managing Observer by job UUID

    /**
     * wait shutdown, and call log at 5 second intervals
     */
    void run();
    /**
     * log progress information each job UUID
     */
    void log();
    /**
     * get Observer by job id
     */
    std::shared_ptr<Observer> getObserver(const int& id);
    /**
     * request Observer to generate unique id
     */
    std::size_t getProgressId(std::shared_ptr<Observer> observer);
    /**
     * request Observer to manage Progress
     */
    void push(std::shared_ptr<Observer> observer, std::shared_ptr<Progress> elem);

public:
    /**
     * @return singleton ProgressManager instance
     */
    static std::shared_ptr<ProgressManager> getInstance();
    /**
     * call run(), this statis method is used for lauching thread
     * @see run()
     */
    static void runProgressManager();

    /**
     * send shutdown request
     */
    void shutdown();
    /**
     * this function applies followings:
     * - add mapping to dictionaries
     * - instantiate Observer which has empty to manage
     */
    void registerJob(const int& id, const std::string& uuid);

    /**
     * this function applies followings:
     * - remove mapping from dictionaries
     * - remove Observer from manage
     */
    void unregisterJob(const std::string& uuid);

    /**
     * Progress generator. this function applies followings:
     * - create Builder
     * - instantiate inherited progress class with args
     * - request Observer to manager progress
     * - return Progress wrapped by ScopedProgress
     * @param job_id job id associated with Share
     * @param desc description of procedure
     * @param args constructor arguments except Builder
     */
    template <class progress_type, class... Args>
    ScopedProgress<progress_type> createProgress(
        const int& job_id, const std::string& desc, Args&&... args
    )
    {
        auto observer = getObserver(job_id);
        auto progress_id = getProgressId(observer);
        auto builder = Progress::Builder{}.setId(progress_id).setDesc(desc).setObserver(observer);

        const auto progress = std::make_shared<progress_type>(builder, args...);

        push(observer, progress);

        return ScopedProgress<progress_type>(progress);
    }

    void updateJobStatus(const std::string& job_uuid, const pb_common_types::JobStatus& status);

    /**
     * status code for getProgress
     */
    enum class StatusCode
    {
        OK,
        NOT_FOUND,
        INTERNAL_ERROR,
    };
    /**
     * @param job_uuid job UUID generated by MC
     * @return progress matching job UUID with status code
     */
    std::pair<std::optional<pb_common_types::JobProgress>, StatusCode> getProgress(
        const std::string& job_uuid
    );
};
}  // namespace qmpc::Job
