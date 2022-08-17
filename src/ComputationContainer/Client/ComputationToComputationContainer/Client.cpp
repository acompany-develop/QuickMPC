#include "Client.hpp"

#include <unistd.h>

#include <chrono>
#include <string>
#include <thread>

#include "LogHeader/Logger.hpp"
#include "Share/AddressId.hpp"

namespace qmpc::ComputationToComputation
{

// 単一シェアを生成する場合
computationtocomputation::Share makeShare(
    std::string value, qmpc::Share::AddressId share_id, int party_id
)
{
    computationtocomputation::Share s;
    auto a = s.mutable_address_id();
    a->set_share_id(share_id.getShareId());
    a->set_job_id(share_id.getJobId());
    s.set_value(value);
    s.set_party_id(party_id);
    return s;
}

// 複数シェアを生成する場合
// 約1mbごとに分割して生成する
std::vector<computationtocomputation::Shares> makeShares(
    std::string values[], qmpc::Share::AddressId share_ids[], unsigned int length, int party_id
)
{
    std::vector<computationtocomputation::Shares> share_vec;
    share_vec.reserve(length);
    size_t addressId_size = sizeof(qmpc::Share::AddressId);
    size_t size = 0;
    computationtocomputation::Shares s;
    for (unsigned int i = 0; i < length; i++)
    {
        // string型のバイト数の取得
        size_t value_size = values[i].length();
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
        multiple_shares->set_value(values[i]);
    }
    s.set_party_id(party_id);
    share_vec.push_back(s);
    return share_vec;
}

Client::Client(const Url &endpoint) noexcept
{
    stub_ = createStub<computationtocomputation::ComputationToComputation>(endpoint);
    spdlog::info("{:<15} Client {:<30} is Active", "[Cc2Cc]", endpoint.url);
}

std::shared_ptr<Client> Client::getPtr(const Url &endpoint)
{
    /*
    shared_ptrでprivate constructorを管理するために作成したクラス
    外部からは作成できないため一時的にpublic constructorを作成できる
    スコープを抜けると消滅するため安全
    返却する型は元のクラスになるのでupCastされてオブジェクトはClientになる
    */
    struct impl : Client
    {
        impl(const Url &url) : Client(url) {}
    };
    auto ptr = std::make_shared<impl>(endpoint);
    return std::move(ptr);
}

// 単一シェアをexchangeする場合
bool Client::exchangeShare(std::string value, qmpc::Share::AddressId share_id, int party_id) const
{
    computationtocomputation::Share share;
    google::protobuf::Empty response;
    share = makeShare(value, share_id, party_id);
    while (true)
    {
        grpc::ClientContext context;
        grpc::Status status = stub_->ExchangeShare(&context, share, &response);
        // spdlog::info("exchange message : share_id is {} party_id is {}, value is
        // {}",share_id,party_id,value);
        if (!status.ok())
        {
            spdlog::error("{:<30} GetFeature rpc failed.", "[exchangeShare]");
            spdlog::error(
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
bool Client::exchangeShares(
    std::string values[], qmpc::Share::AddressId share_ids[], unsigned int length, int party_id
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
            spdlog::error("{:<30} GetFeature rpc failed.", "[exchangeShares]");
            spdlog::error(
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

}  // namespace qmpc::ComputationToComputation
