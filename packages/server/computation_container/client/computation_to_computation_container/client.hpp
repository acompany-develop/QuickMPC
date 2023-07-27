#pragma once

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <iostream>
#include <string>

#include "client/helper/helper.hpp"
#include "config_parse/config_parse.hpp"
#include "external/proto/computation_to_computation_container/computation_to_computation.grpc.pb.h"
#include "share/address_id.hpp"

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
        size_t size = 0;
        computationtocomputation::Shares s;
        auto a = s.mutable_address_id();
        a->set_share_id(share_ids[0].getShareId());
        a->set_job_id(share_ids[0].getJobId());
        a->set_party_id(party_id);
        // std::cout << "Client party , job, share" << party_id << " " << share_ids[0].getJobId()
        //           << " " << share_ids[0].getShareId() << std::endl;
        for (unsigned int i = 0; i < length; i++)
        {
            // string型のバイト数の取得
            size_t value_size = sizeof(values[i]);
            if (size + value_size > 1000000)
            {
                size = 0;
                share_vec.push_back(s);
                s = computationtocomputation::Shares{};
            }
            size = size + value_size;
            computationtocomputation::Shares_Share *multiple_shares = s.add_share_list();

            using rawT = std::decay_t<T>;

            if constexpr (std::is_same_v<rawT, bool>)
            {
                multiple_shares->set_flag(values[i]);
            }
            else if constexpr (std::is_integral_v<rawT> && sizeof(rawT) <= 8)
            {
                multiple_shares->set_num(values[i]);
            }
            else
            {
                computationtocomputation::BigIntByte *fp = multiple_shares->mutable_fp();
                auto [sgn, byte] = values[i].getSgnByte();
                fp->set_sgn(sgn);
                fp->set_byte(byte);
            }
        }
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
        std::vector<std::decay_t<T>> values;
        values.emplace_back(value);
        std::vector<qmpc::Share::AddressId> share_ids;
        share_ids.emplace_back(share_id);

        return exchangeShares(values, share_ids, 1, party_id);
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
        // リクエスト設定
        std::vector<computationtocomputation::Shares> shares;
        google::protobuf::Empty response;
        shares = makeShares(values, share_ids, length, party_id);
        grpc::Status status;
        // リトライポリシーに従ってリクエストを送る
        auto retry_manager = RetryManager("CC", "exchangeShares");
        grpc::ClientContext context;
        std::shared_ptr<grpc::ClientWriter<computationtocomputation::Shares>> stream(
            stub_->ExchangeShares(&context, &response)
        );
        do
        {
            for (size_t i = 0; i < shares.size(); i++)
            {
                if (!stream->Write(shares[i]))
                {
                    // Broken stream.
                    break;
                }
            }
            stream->WritesDone();
            status = stream->Finish();
        } while (retry_manager.retry(status));

        // 送信に成功
        return true;
    }
};
}  // namespace qmpc::ComputationToComputation
