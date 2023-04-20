#pragma once

#include <list>

#include "client/computation_to_db/client.hpp"
#include "job/job_base.hpp"
#include "share/share.hpp"

namespace qmpc::Job
{

// メッシュコード変換関数
class MeshCodeFunction
{
    using Share = ::Share;
    const std::vector<Share> latitude;
    const std::vector<Share> longitude;

public:
    MeshCodeFunction(const std::vector<Share> &latitude, const std::vector<Share> &longitude)
        : latitude(latitude), longitude(longitude)
    {
    }

    // アルゴリズムの詳細はこちら:
    // docs/meshcode-conversion-protocol.md
    std::vector<std::vector<Share>> meshcode_transform()
    {
        std::vector<Share> p = qmpc::Share::getFloor(FixedPoint("1.5") * latitude);
        std::vector<Share> a = FixedPoint("60") * latitude - FixedPoint("40") * p;
        std::vector<Share> q = qmpc::Share::getFloor(a / FixedPoint("5"));
        std::vector<Share> b = a - FixedPoint("5") * q;
        std::vector<Share> r = qmpc::Share::getFloor(FixedPoint("2") * b);
        std::vector<Share> c = FixedPoint("60") * b - FixedPoint("30") * r;
        std::vector<Share> s = qmpc::Share::getFloor(c / FixedPoint("15"));
        std::vector<Share> d = c - FixedPoint("15") * s;
        std::vector<Share> t = qmpc::Share::getFloor(d / FixedPoint("7.5"));
        std::vector<Share> e = d - FixedPoint("7.5") * t;

        std::vector<Share> u = qmpc::Share::getFloor(longitude - FixedPoint("100"));
        std::vector<Share> f = longitude - FixedPoint("100") - u;
        std::vector<Share> v = qmpc::Share::getFloor(FixedPoint("8") * f);
        std::vector<Share> g = FixedPoint("60") * f - FixedPoint("7.5") * v;
        std::vector<Share> w = qmpc::Share::getFloor(FixedPoint("60") * g / FixedPoint("45"));
        std::vector<Share> h = FixedPoint("60") * g - FixedPoint("45") * w;
        std::vector<Share> x = qmpc::Share::getFloor(h / FixedPoint("22.5"));
        std::vector<Share> i = h - FixedPoint("22.5") * x;
        std::vector<Share> y = qmpc::Share::getFloor(i / FixedPoint("11.25"));
        std::vector<Share> j = i - FixedPoint("11.25") * y;

        std::vector<Share> m = (s * FixedPoint("2")) + (x + FixedPoint("1"));
        std::vector<Share> n = (t * FixedPoint("2")) + (y + FixedPoint("1"));

        std::vector<std::vector<Share>> ret;
        ret.reserve(latitude.size());

        for (int i = 0; i < static_cast<int>(latitude.size()); ++i)
        {
            std::vector<Share> tmp{p[i], u[i], q[i], v[i], r[i], w[i], m[i], n[i]};
            ret.emplace_back(tmp);
        }

        return ret;
    }
};

class MeshCodeJob : public JobBase<MeshCodeJob>
{
    static auto toString(const std::vector<std::vector<Share>> &values)
    {
        std::vector<std::vector<std::string>> results;
        results.reserve(values.size());
        for (const auto &value : values)
        {
            std::vector<std::string> row;
            row.reserve(values.size());
            for (const auto &x : value)
            {
                row.emplace_back(x.getVal().getStrVal());
            }
            results.emplace_back(row);
        }
        return results;
    }

public:
    using JobBase<MeshCodeJob>::JobBase;
    auto compute(
        const std::string job_uuid,
        const qmpc::ComputationToDb::ValueTable &table,
        const std::vector<std::list<int>> &arg
    )
    {
        std::vector<Share> latitudes;
        std::vector<Share> longitudes;

        auto table_str = table.getTable();
        for (const auto &index : arg[0])
        {
            latitudes.emplace_back(table_str[0][index - 1]);
            longitudes.emplace_back(table_str[1][index - 1]);
        }
        auto f = MeshCodeFunction(latitudes, longitudes);
        std::vector<std::vector<Share>> meshcode_list = f.meshcode_transform();

        auto results = toString(meshcode_list);
        auto column_number = (results.empty() ? -1 : results[0].size());
        auto computationResultWriter =
            qmpc::ComputationToDb::ComputationResultWriter(job_uuid, 1, column_number);
        computationResultWriter.emplace(results);
        computationResultWriter.write();
    }
};
}  // namespace qmpc::Job
