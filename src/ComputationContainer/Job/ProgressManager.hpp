#pragma once

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
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
        Builder() {}
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
        Builder& setObserver(const std::shared_ptr<Observer> observer)
        {
            this->observer = observer;
            return *this;
        }

        friend Progress;

    private:
        std::size_t id;
        std::string description;
        std::shared_ptr<Observer> observer;
    };

    Progress() = delete;
    Progress(const Builder& builder);
    virtual float progress() const = 0;
    virtual std::optional<std::string> details() const;

    virtual void finish() final;
    virtual const std::size_t& id() const final;
    virtual const std::string& description() const final;

private:
    Progress(std::size_t id, std::string description, std::shared_ptr<Observer> observer);

    virtual void before_finish();

    std::size_t id_;
    std::string description_;
    std::shared_ptr<Observer> observer_;
};

struct ProgressIters : public Progress
{
    ProgressIters(const Progress::Builder& builder, const std::size_t& size);

    void update(const std::size_t& index);

    float progress() const override;
    std::optional<std::string> details() const override;

private:
    void before_finish() override;

    std::size_t size;
    std::atomic<std::size_t> index;
};

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

    std::mutex dict_mtx;
    std::map<int, std::string> job_id_to_job_uuid;
    std::map<std::string, int> job_uuid_to_job_id;
    std::map<std::string, std::shared_ptr<Observer>> progresses;

    void run();
    void log();
    std::shared_ptr<Observer> getObserver(const int& id);
    std::size_t getProgressId(std::shared_ptr<Observer> observer);
    void push(std::shared_ptr<Observer> observer, std::shared_ptr<Progress> elem);

public:
    static std::shared_ptr<ProgressManager> getInstance();
    static void runProgressManager();
    void shutdown();
    void registerJob(const int& id, const std::string& uuid);

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

    enum class StatusCode
    {
        OK,
        NOT_FOUND,
        INTERNAL_ERROR,
    };
    std::pair<std::optional<pb_common_types::JobProgress>, StatusCode> getProgress(
        const std::string& job_uuid
    );
};
}  // namespace qmpc::Job
