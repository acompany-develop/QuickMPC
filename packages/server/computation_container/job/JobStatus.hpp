#pragma once

#include <optional>
#include <stdexcept>

#include <boost/format.hpp>
#include <boost/stacktrace/stacktrace.hpp>

#include "Client/ComputationToDb/Client.hpp"
#include "Logging/Logger.hpp"
#include "ProgressManager.hpp"
#include "external/proto/common_types/common_types.pb.h"
namespace qmpc::Job
{
// Protocol Buffers の定義を引っ張ってくる
using Status = pb_common_types::JobStatus;

class StatusManager
{
    std::shared_ptr<qmpc::ComputationToDb::Client> db_client =
        qmpc::ComputationToDb::Client::getInstance();
    std::optional<std::string> job_uuid;
    Status status;

    void registerJob() const
    {
        db_client->registerJob(job_uuid.value(), static_cast<int>(status));
        ProgressManager::getInstance()->updateJobStatus(job_uuid.value(), status);
    }

public:
    // TODO: Model が Job に統合され、不要になったら削除する
    StatusManager() : job_uuid(std::nullopt), status(Status::UNKNOWN) {}

    StatusManager(const std::string &job_uuid) : job_uuid(job_uuid), status(Status::PRE_JOB)
    {
        registerJob();
    }

    void initJobID(const std::string &job_uuid_)
    {
        if (job_uuid.has_value())
        {
            qmpc::Log::throw_with_trace(
                std::logic_error("JobStatusError: JobUUID is already initialized")
            );
        }
        job_uuid = job_uuid_;
        status = Status::PRE_JOB;
        registerJob();
    }

    void nextStatus()
    {
        // Descriptor を通して範囲内かチェックする
        const google::protobuf::EnumDescriptor *descriptor =
            google::protobuf::GetEnumDescriptor<Status>();
        const google::protobuf::EnumValueDescriptor *value =
            descriptor->FindValueByNumber(static_cast<int>(status));
        const int next_index = value->index() + 1;
        if (next_index >= descriptor->value_count())
        {
            qmpc::Log::throw_with_trace(
                std::out_of_range((boost::format("next status value:(%1%) is going to out of "
                                                 "range:([0, %2%))")
                                   % next_index % descriptor->value_count())
                                      .str())
            );
        }
        const google::protobuf::EnumValueDescriptor *next_value = descriptor->value(next_index);
        status = static_cast<Status>(next_value->number());
        db_client->updateJobStatus(job_uuid.value(), static_cast<int>(status));
        ProgressManager::getInstance()->updateJobStatus(job_uuid.value(), status);
    }

    /**
     * when an error occured, StatusManager holder should call this method.
     * this method save error information via db client.
     */
    void error(
        const std::exception &e, const std::optional<boost::stacktrace::stacktrace> &stacktrace
    )
    {
        pb_common_types::JobErrorInfo info;
        info.set_what(e.what());
        if (stacktrace.has_value())
        {
            pb_common_types::Stacktrace stacktrace_pb;
            for (const auto &frame : stacktrace.value())
            {
                // if frame has missing info, it will skip
                if (frame.source_line() == 0)
                {
                    continue;
                }
                pb_common_types::Stacktrace::Frame *frame_pb = stacktrace_pb.add_frames();
                frame_pb->set_source_location(frame.source_file());
                frame_pb->set_source_line(frame.source_line());
                frame_pb->set_function_name(frame.name());
            }
            *info.mutable_stacktrace() = stacktrace_pb;
        }
        status = Status::ERROR;
        db_client->saveErrorInfo(job_uuid.value(), info);
        ProgressManager::getInstance()->updateJobStatus(job_uuid.value(), status);
    }

    Status getStatus() const { return status; }
};
}  // namespace qmpc::Job
