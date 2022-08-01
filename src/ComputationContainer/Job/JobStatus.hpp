#pragma once

#include <stdexcept>
#include <optional>

#include "Client/ComputationToDbGate/Client.hpp"
#include "external/Proto/common_types/common_types.pb.h"
#include <boost/format.hpp>
#include "Logging/Logger.hpp"
namespace qmpc::Job
{
// Protocol Buffers の定義を引っ張ってくる
using Status = pb_common_types::JobStatus;

class StatusManager
{
    std::shared_ptr<qmpc::ComputationToDbGate::Client> db_client =
        qmpc::ComputationToDbGate::Client::getInstance();
    std::optional<std::string> job_uuid;
    Status status;

    void registerJob() const { db_client->registerJob(job_uuid.value(), static_cast<int>(status)); }

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
    }

    Status getStatus() const { return status; }
};
}  // namespace qmpc::Job
