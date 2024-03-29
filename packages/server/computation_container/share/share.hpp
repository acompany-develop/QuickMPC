#pragma once

#include <atomic>
#include <boost/operators.hpp>
#include <cmath>
#include <string>
#include <tuple>

#include "address_id.hpp"
#include "bts_handler/stock_bts.hpp"
#include "config_parse/config_parse.hpp"
#include "logging/logger.hpp"
#include "networking.hpp"
#include "random/random.hpp"

namespace qmpc::Share
{
template <typename SV>
class Share : boost::totally_ordered<Share<SV>>,
              boost::additive<Share<SV>>,
              boost::multipliable<Share<SV>>,
              boost::addable<Share<SV>, SV>,
              boost::subtractable<Share<SV>, SV>,
              boost::multipliable<Share<SV>, SV>,
              boost::dividable<Share<SV>, SV>,
              boost::equality_comparable<Share<SV>, SV>,
              boost::less_than_comparable<Share<SV>, SV>
{
private:
    SV value;
    AddressId address_id;

public:
    using value_type = SV;
    // TODO:Tripleが正式に実装されたら削除すること
    using Triple = std::tuple<SV, SV, SV>;
    Share() noexcept : value(), address_id() {}
    Share(const Share &s) noexcept : value(s.value), address_id() {}
    Share(Share &&s) noexcept : value(std::move(s.value)), address_id(std::move(s.address_id)) {}
    Share(const SV &value) noexcept : value(value), address_id() {}
    Share &operator=(const Share &obj)
    {
        value = obj.value;
        return *this;
    }
    Share &operator=(Share &&obj)
    {
        value = std::move(obj.value);
        return *this;
    }
    Share &operator=(const SV &obj)
    {
        value = obj;
        return *this;
    }
    SV getVal() const noexcept { return value; }
    double getDoubleVal() const { return value.getDoubleVal(); }
    auto getId() const noexcept { return address_id; }
    Share operator-() const
    {
        Share ret{*this};
        ret.value = -ret.value;
        return ret;
    }

    Share &operator+=(const Share &obj)
    {
        value += obj.value;
        return *this;
    }
    Share &operator-=(const Share &obj)
    {
        value -= obj.value;
        return *this;
    }
    Share &operator*=(const Share &obj)
    {
        // Beaver Triplet a, b, c のシェア [a], [b], [c] を得る
        auto t = qmpc::BtsHandler::StockTriple::getInstance()->get();
        Share a(std::get<0>(t[0])), b(std::get<1>(t[0])), c(std::get<2>(t[0]));

        // [d] = [x] - [a], [e] = [y] - [b] を計算する
        Share d;
        d = (*this) - a;
        Share e;
        e = obj - b;
        std::vector<Share> de = {(*this) - a, obj - b};

        // d, e を送受信し復元する
        std::vector<SV> de_rec = open_and_recons(de);

        // [xy] = [c] + d[b] + e[a] + de を計算する
        Share v;
        v = c + de_rec[0] * b + de_rec[1] * a + de_rec[0] * de_rec[1];
        *this = v;

        return *this;
    }

    Share &operator+=(const SV &obj)
    {
        Config *conf = Config::getInstance();
        if (conf->party_id == conf->sp_id)
        {
            value += obj;
        }
        return *this;
    }
    Share &operator-=(const SV &obj)
    {
        Config *conf = Config::getInstance();
        if (conf->party_id == conf->sp_id)
        {
            value -= obj;
        }
        return *this;
    }
    Share &operator*=(const SV &obj)
    {
        value *= obj;
        return *this;
    }
    friend std::vector<Share> operator+(const std::vector<Share> &left, const SV &right)
    {
        std::vector<Share> ret;
        ret.reserve(left.size());

        for (const auto &v : left)
        {
            ret.emplace_back(v + right);
        }

        return ret;
    }
    friend std::vector<Share> operator+(const SV &left, const std::vector<Share> &right)
    {
        return right + left;
    }
    friend std::vector<Share> operator-(const std::vector<Share> &left, const SV &right)
    {
        return left + (-right);
    }
    friend std::vector<Share> operator-(const SV &left, const std::vector<Share> &right)
    {
        return left + getAdditiveInvVec(right);
    }
    friend std::vector<Share> operator*(const std::vector<Share> &left, const SV &right)
    {
        std::vector<Share> ret;
        ret.reserve(left.size());

        for (const auto &v : left)
        {
            ret.emplace_back(v * right);
        }

        return ret;
    }
    friend std::vector<Share> operator*(const SV &left, const std::vector<Share> &right)
    {
        return right * left;
    }
    friend std::vector<Share> operator/(const std::vector<Share> &left, const SV &right)
    {
        std::vector<Share> ret;
        ret.reserve(left.size());

        for (const auto &v : left)
        {
            ret.emplace_back(v / right);
        }

        return ret;
    }

    Share &operator/=(const SV &obj)
    {
        value /= obj;
        return *this;
    }
    friend Share operator-(const SV &left, const Share &right)
    {
        Share ret{-right};
        ret += left;
        return ret;
    }
    friend bool operator<(const Share<FixedPoint> &left, const Share<FixedPoint> &right);
    friend bool operator==(const Share<FixedPoint> &left, const Share<FixedPoint> &right);
    friend bool operator<(const Share<FixedPoint> &left, const FixedPoint &right);
    friend bool operator==(const Share<FixedPoint> &left, const FixedPoint &right);

    friend std::vector<Share> operator+(
        const std::vector<Share> &obj1, const std::vector<Share> &obj2
    )
    {
        size_t n = obj1.size();
        if (n != obj2.size())
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("Not Equal Length operator vector<Share> !!")
            );
        }

        std::vector<Share> ret;
        ret.reserve(obj1.size());
        for (int i = 0; i < static_cast<int>(n); ++i)
        {
            ret.emplace_back(obj1[i] + obj2[i]);
        }
        return ret;
    }

    friend std::vector<Share> operator-(
        const std::vector<Share> &obj1, const std::vector<Share> &obj2
    )
    {
        size_t n = obj1.size();
        if (n != obj2.size())
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("Not Equal Length operator vector<Share> !!")
            );
        }

        std::vector<Share> ret;
        ret.reserve(obj1.size());
        for (int i = 0; i < static_cast<int>(n); ++i)
        {
            ret.emplace_back(obj1[i] - obj2[i]);
        }
        return ret;
    }

    friend std::vector<Share> operator*(
        const std::vector<Share> &obj1, const std::vector<Share> &obj2
    )
    {
        size_t n = obj1.size();
        if (n != obj2.size())
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("Not Equal Length operator vector<Share> !!")
            );
        }
        // Beaver Triplet a, b, c のシェア [a], [b], [c] を得る
        auto t = qmpc::BtsHandler::StockTriple::getInstance()->get(n);

        std::vector<Share> a, b, c;
        a.reserve(n);
        b.reserve(n);
        c.reserve(n);
        for (size_t i = 0; i < n; ++i)
        {
            a.emplace_back(Share(std::get<0>(t[i])));
            b.emplace_back(Share(std::get<1>(t[i])));
            c.emplace_back(Share(std::get<2>(t[i])));
        }
        // [d] = [x] - [a], [e] = [y] - [b] を計算する
        std::vector<Share> de(n * 2);
        for (size_t i = 0; i < n; ++i)
        {
            de[i] = obj1[i] - a[i];
            de[i + n] = obj2[i] - b[i];
        }

        std::vector<SV> de_rec = open_and_recons(de);

        // [xy] = [c] + d[b] + e[a] + de を計算する
        std::vector<Share> v(n);
        for (size_t i = 0; i < n; ++i)
        {
            v[i] = c[i] + de_rec[i] * b[i] + de_rec[n + i] * a[i] + de_rec[i] * de_rec[n + i];
        }

        return v;
    }
};

// vector の各要素の加法逆元を求める。
template <typename T>
std::vector<Share<T>> getAdditiveInvVec(const std::vector<Share<T>> &s)
{
    std::vector<Share<T>> ret;
    ret.reserve(s.size());
    for (const auto &v : s)
    {
        ret.emplace_back(-v);
    }
    return ret;
}

template <typename T>
Share<T> getConstantShare(const T &t)
{
    Share<T> s;
    s += t;
    return s;
}

template <typename T>
std::vector<Share<T>> getRandShares(long long min_val, long long max_val, int n)
{
    std::vector<Share<T>> ret;
    ret.reserve(n);

    auto r = RandGenerator::getInstance()->getRandVec<T>(min_val, max_val, n);
    for (const auto &v : r)
    {
        ret.emplace_back(Share(v));
    }
    return ret;
}

template <class T>
auto _isZero(const T &x)
{
    if constexpr (std::is_same_v<T, FixedPoint>)
    {
        return x.getDoubleVal() == 0.0;
    }
    else
    {
        return x == 0.0;
    }
}

template <typename SV>
Share<SV> getRandBitShare()
{
    auto bit = qmpc::BtsHandler::StockRandBit::getInstance()->get();
    return Share<SV>(bit[0]);
}

template <typename SV>
std::vector<Share<SV>> getRandBitShare(std::size_t amount)
{
    auto bit = qmpc::BtsHandler::StockRandBit::getInstance()->get(amount);

    std::vector<Share<SV>> ret(amount);
    for (size_t i = 0; i < amount; i++)
    {
        ret[i] = Share<SV>(bit[i]);
    }

    return ret;
}

template <typename T>
Share<T> getLSBShare(const Share<T> &y)
{
    Share<T> r0 = getRandBitShare<T>();
    Share<T> r_dash = Share(RandGenerator::getInstance()->getRand<T>(1, 1000));
    Share<T> t = y + r0 + T(2) * r_dash;
    T c = open_and_recons(t);
    c = T(c.getRoundValue()) % T(2);
    Share<T> b = c + r0 - T(2) * c * r0;
    return b;
}

template <typename T>
std::vector<Share<T>> getLSBShare(const std::vector<Share<T>> &y)
{
    std::vector<Share<T>> r0 = getRandBitShare<T>(static_cast<int>(y.size()));
    std::vector<Share<T>> r_dash = getRandShares<T>(1, 1000, static_cast<int>(y.size()));
    std::vector<Share<T>> t = y + r0 + T(2) * r_dash;
    std::vector<T> c = open_and_recons(t);
    std::vector<Share<T>> b;
    b.reserve(y.size());
    for (int j = 0; j < static_cast<int>(c.size()); ++j)
    {
        c[j] = T(c[j].getRoundValue()) % T(2);
        b.emplace_back(c[j] + r0[j] - T(2) * c[j] * r0[j]);
    }

    return b;
}

template <typename T>
Share<T> getFloor(const Share<T> &s)
{
    // m, k は誤差が小さくなる値を設定している。
    // これ以上値を大きくすると、誤差が大きくなる可能性があるので注意。
    int m = 16;
    int k = 32;

    Share<T> x = s * T(std::pow(2, m));
    Share<T> y = T(std::pow(2, k)) + x;
    for (int i = 0; i < m; ++i)
    {
        Share<T> b = getLSBShare(y);
        y = (y - b) * T(0.5);
    }
    return y - T(std::pow(2, k - m));
}

template <typename T>
std::vector<Share<T>> getFloor(const std::vector<Share<T>> &s)
{
    // m, k は誤差が小さくなる値を設定している。
    // これ以上値を大きくすると、誤差が大きくなる可能性があるので注意。
    int m = 16;
    int k = 32;
    std::vector<Share<T>> x = s * T(std::pow(2, m));
    std::vector<Share<T>> y = T(std::pow(2, k)) + x;
    std::vector<Share<T>> ret;
    ret.reserve(s.size());
    for (int i = 0; i < m; ++i)
    {
        std::vector<Share<T>> b = getLSBShare(y);
        y = (y - b) * T(0.5);
    }
    for (int j = 0; j < static_cast<int>(y.size()); ++j)
    {
        ret.emplace_back(y[j] - T(std::pow(2, k - m)));
    }

    return ret;
}
}  // namespace qmpc::Share

using Share = qmpc::Share::Share<FixedPoint>;
