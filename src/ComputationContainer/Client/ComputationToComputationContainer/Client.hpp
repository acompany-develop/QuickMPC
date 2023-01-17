#pragma once

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <iostream>
#include <string>

#include "Client/Helper/Helper.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "Share/AddressId.hpp"
#include "external/Proto/ComputationToComputationContainer/computation_to_computation.grpc.pb.h"

namespace qmpc::ComputationToComputation
{
class Client
{
    friend class Server;

private:
    std::unique_ptr<computationtocomputation::ComputationToComputation::Stub> stub_;
    Client() noexcept = default;
    ~Client() noexcept = default;
    Client(const Url &endpoint) noexcept;
    Client &operator=(Client &&) noexcept = default;
    static std::shared_ptr<Client> getPtr(const Url &);
    // 単一シェアを生成する場合
    template <typename T>
    computationtocomputation::Share makeShare(
        T &&value, qmpc::Share::AddressId share_id, int party_id
    ) const
    {
        computationtocomputation::Share s;
        auto a = s.mutable_address_id();
        a->set_share_id(share_id.getShareId());
        a->set_job_id(share_id.getJobId());

        if constexpr (std::is_same_v<std::decay_t<T>, bool>)
        {
            s.set_flag(value);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, int>)
        {
            s.set_num(value);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, long>)
        {
            s.set_num64(value);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, float>)
        {
            s.set_f(value);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, double>)
        {
            s.set_d(value);
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
        {
            s.set_byte(value);
        }
        else
        {
            s.set_byte(to_string(value));
        }
        s.set_party_id(party_id);
        return s;
    }

    // 複数シェアを生成する場合
    // 約1mbごとに分割して生成する
    template <typename T>
    std::vector<computationtocomputation::Shares> makeShares(
        const std::vector<T> &values,
        const std::vector<qmpc::Share::AddressId> &share_ids,
        unsigned int length,
        int party_id
    ) const
    {
        std::vector<computationtocomputation::Shares> share_vec;
        share_vec.reserve(length);
        size_t addressId_size = sizeof(qmpc::Share::AddressId);
        size_t size = 0;
        computationtocomputation::Shares s;
        for (unsigned int i = 0; i < length; i++)
        {
            // string型のバイト数の取得
            size_t value_size = sizeof(values[i]);
            // ShareId,JobId,ThreadId,PartyIdの16byte
            if (size + value_size + addressId_size + sizeof(s.party_id()) > 1000000)
            {
                size = 0;
                s.set_party_id(party_id);
                share_vec.push_back(s);
                s = computationtocomputation::Shares{};
            }
            // 一つのsharesにつきPartyIdは一つだけなので分割しない際はShareId,JobId,ThreadIdの12byte
            size = size + value_size + addressId_size;
            computationtocomputation::Shares_Share *multiple_shares = s.add_share_list();
            auto a = multiple_shares->mutable_address_id();
            a->set_share_id(share_ids[i].getShareId());
            a->set_job_id(share_ids[i].getJobId());
            if constexpr (std::is_same_v<std::decay_t<T>, bool>)
            {
                multiple_shares->set_flag(values[i]);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, int>)
            {
                multiple_shares->set_num(values[i]);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, long>)
            {
                multiple_shares->set_num64(values[i]);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, float>)
            {
                multiple_shares->set_f(values[i]);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, double>)
            {
                multiple_shares->set_d(values[i]);
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
            {
                multiple_shares->set_byte(values[i]);
            }
            else
            {
                multiple_shares->set_byte(to_string(values[i]));
            }
        }
        s.set_party_id(party_id);
        share_vec.push_back(s);
        return share_vec;
    }

public:
    Client(Client &&) noexcept = delete;
    Client(const Client &) noexcept = delete;
    Client &operator=(const Client &) noexcept = delete;
    template <typename T>
    bool exchangeShare(T &&value, qmpc::Share::AddressId share_id, int party_id) const
    {
        computationtocomputation::Share share;
        google::protobuf::Empty response;
        share = makeShare(value, share_id, party_id);
        while (true)
        {
            grpc::ClientContext context;
            grpc::Status status = stub_->ExchangeShare(&context, share, &response);
            // QMPC_LOG_INFO("exchange message : share_id is {} party_id is {}, value is
            // {}",share_id,party_id,value);
            if (!status.ok())
            {
                QMPC_LOG_ERROR("{:<30} GetFeature rpc failed.", "[exchangeShare]");
                QMPC_LOG_ERROR(
                    "ERROR({0}): {1}\n{2}",
                    status.error_code(),
                    status.error_message(),
                    status.error_details()
                );
            }
            else
                return true;
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    // 複数シェアを一括exchangeする場合
    template <typename T>
    bool exchangeShares(
        const std::vector<T> &values,
        const std::vector<qmpc::Share::AddressId> &share_ids,
        unsigned int length,
        int party_id
    ) const
    {
        std::vector<computationtocomputation::Shares> shares;
        google::protobuf::Empty response;
        shares = makeShares(values, share_ids, length, party_id);
        while (true)
        {
            grpc::ClientContext context;
            std::shared_ptr<grpc::ClientWriter<computationtocomputation::Shares>> stream(
                stub_->ExchangeShares(&context, &response)
            );
            for (size_t i = 0; i < shares.size(); i++)
            {
                if (!stream->Write(shares[i]))
                {
                    // Broken stream.
                    break;
                }
            }
            stream->WritesDone();
            grpc::Status status = stream->Finish();
            if (!status.ok())
            {
                QMPC_LOG_ERROR("{:<30} GetFeature rpc failed.", "[exchangeShares]");
                QMPC_LOG_ERROR(
                    "ERROR({}): {}\n{}",
                    status.error_code(),
                    status.error_message(),
                    status.error_details()
                );
            }
            else
                return true;
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
};
}  // namespace qmpc::ComputationToComputation
