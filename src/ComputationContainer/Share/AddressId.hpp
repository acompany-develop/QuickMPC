#pragma once
#include <atomic>
#include <iostream>
#include <mutex>
namespace qmpc::Share
{
class AddressId
{
    thread_local static inline std::atomic<int> share_id_generator = 0;
    thread_local static inline unsigned int
        job_id_assigned_in_thread;  // threadに割り当てられたjobidで同一job内ではこの値をみんな参照

private:
    int share_id;
    unsigned int
        job_id;  // 実質入る値はthreadに割り当てられたjob_idと同じだが，将来の拡張性を考えて残しておく。
    int thread_id;

public:
    AddressId() noexcept
        : share_id(share_id_generator++), job_id(job_id_assigned_in_thread), thread_id()
    {
    }
    AddressId(AddressId &&from) { *this = from; }
    AddressId(const AddressId &from) { *this = from; }
    AddressId &operator=(const AddressId &from)
    {
        share_id = from.share_id;
        job_id = from.job_id;
        thread_id = from.thread_id;
        return *this;
    }
    AddressId &operator=(AddressId &&from)
    {
        share_id = std::move(from.share_id);
        job_id = std::move(from.job_id);
        thread_id = std::move(from.thread_id);
        return *this;
    }
    static void setJobId(const unsigned int &job_id) { job_id_assigned_in_thread = job_id; }
    static auto getThreadJobId() noexcept { return job_id_assigned_in_thread; }
    auto getJobId() const noexcept { return job_id; }
    auto getShareId() const noexcept { return share_id; }
    auto getThreadId() const noexcept { return thread_id; }
    friend std::ostream &operator<<(std::ostream &out, const qmpc::Share::AddressId &address)
    {
        out << "Address: share_id is " << address.getShareId();
        out << " job_id is " << address.getJobId();
        out << " thread_id is " << address.getThreadId();
        out << std::endl;
        return out;
    }
};
}  // namespace qmpc::Share