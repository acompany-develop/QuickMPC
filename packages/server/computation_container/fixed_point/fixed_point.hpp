#pragma once

#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/operators.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <type_traits>

namespace qmpc::Utils
{
template <typename T, typename U>
inline static constexpr T mypow(T a_, U n_) noexcept
{
    T a = a_;
    U n = n_;
    T ret = 1;
    while (n > 0)
    {
        if (n & 1) ret *= a;
        a *= a;
        n >>= 1;
    }
    return ret;
}
namespace mp = boost::multiprecision;
using cpp_dec_float = mp::number<mp::cpp_dec_float<50>>;
/*
T:保持する整数型
D:変換する浮動小数点型
length:10^length が最大値
resolution:10^resolutionを1とする
*/
template <
    typename T = boost::multiprecision::cpp_int,
    typename D = cpp_dec_float,
    int length = 18,
    int resolution = 8>
class FixedPointImpl : private boost::operators<FixedPointImpl<>>
{
private:
    constexpr static long long shift = mypow(10ll, resolution);
    constexpr static long long maxInt =
        mypow(10ll, length - resolution);  // FixedPointImplがとりうる整数の最大値
    T value;

public:
    FixedPointImpl() : value(0) {}
    FixedPointImpl(const FixedPointImpl &v) : value(v.value) {}
    FixedPointImpl(FixedPointImpl &&v) : value(std::move(v.value)) {}
    template <
        typename U,
        std::enable_if_t<
            std::is_arithmetic_v<
                std::remove_reference_t<U>> or std::is_convertible_v<T, std::remove_reference_t<U>>,
            std::nullptr_t> = nullptr>
    FixedPointImpl(const U &v)
    {
        if constexpr (std::is_floating_point_v<U>)
        {
            if (boost::math::isinf(v))
            {
                value = static_cast<T>(maxInt);
            }
            else
            {
                // cast to `D` first to avoid going infinity
                const D value_float = static_cast<D>(v);
                value = static_cast<T>(value_float * shift);
            }
        }
        else
            value = static_cast<T>(v) * shift;
    }
    FixedPointImpl(const std::string &str)
    {
        D v_{str};
        if (boost::math::isinf(v_))
        {
            value = static_cast<T>(maxInt);
        }
        else
            value = static_cast<T>(v_ * shift);
    }
    FixedPointImpl &operator=(const FixedPointImpl &obj)
    {
        value = obj.value;
        return *this;
    }
    FixedPointImpl &operator=(FixedPointImpl &&obj)
    {
        value = std::move(obj.value);
        return *this;
    }
    template <typename U = T>
    U getVal() const
    {
        return value.template convert_to<U>();
    }
    std::string getStrVal() const
    {
        D ret{this->getVal<D>()};
        if (boost::math::isinf(ret))
        {
            ret = static_cast<D>(maxInt);
        }
        ret /= shift;
        return ret.str(20, std::ios_base::fixed);
    }
    T getRoundValue() const
    {
        D ret{this->getVal<D>()};
        if (boost::math::isinf(ret))
        {
            ret = static_cast<D>(maxInt);
        }
        ret /= shift;
        ret = mp::round(ret);
        return static_cast<T>(ret);
    }

    D getSqrtValue() const
    {
        D ret{this->getVal<D>()};
        if (boost::math::isinf(ret))
        {
            ret = static_cast<D>(maxInt);
        }
        ret /= shift;
        ret = mp::sqrt(ret);
        return ret;
    }
    double getDoubleVal() const
    {
        auto ret{this->getVal<double>()};
        if (boost::math::isinf(ret))
        {
            ret = static_cast<double>(maxInt);
        }
        return ret / shift;
    }
    constexpr static auto getShift() noexcept { return shift; }
    constexpr static auto getMaxInt() noexcept { return maxInt; }
    FixedPointImpl getInv() const
    {
        D inv{this->getVal<D>()};
        if (boost::math::isinf(inv))
        {
            inv = static_cast<D>(maxInt);
        }
        return FixedPointImpl(D{shift} / inv);
    }
    FixedPointImpl operator-() const
    {
        FixedPointImpl ret{};
        ret.value = -value;
        return ret;
    }

    FixedPointImpl &operator+=(const FixedPointImpl &obj)
    {
        value += obj.value;
        return *this;
    }
    FixedPointImpl &operator-=(const FixedPointImpl &obj)
    {
        value -= obj.value;
        return *this;
    }
    /*
    TODO:
    オーバーフローの可能性が10^shift^2 = 10^12の掛け算でかなり高くなってしまうので
    一時的に浮動小数点に戻してから演算を行うことも考慮しておく
    */
    FixedPointImpl &operator*=(const FixedPointImpl &obj)
    {
        //直接変換でオーバーフローする場合は以下のように文字列に変換する
        // D tmp{D(value.template str()) * D(obj.value.template str())};
        // std::string str = tmp.template str();
        // value = T(str);
        // value /= shift;
        value *= obj.value;
        value /= shift;  //整数で保持するため余分なshiftを除算する
        return *this;
    }
    FixedPointImpl &operator/=(const FixedPointImpl &obj)
    {
        D inv{obj.getVal<D>()};
        if (boost::math::isinf(inv))
        {
            inv = static_cast<D>(maxInt);
        }
        D v = (this->getVal<D>() / inv) * shift;
        value = v.template convert_to<T>();
        return *this;
    }
    FixedPointImpl &operator%=(const FixedPointImpl &obj)
    {
        value %= obj.value;
        return *this;
    }
    FixedPointImpl &operator++()
    {
        value += shift;  // shift分ずれるので1*shift
        return *this;
    }
    FixedPointImpl &operator--()
    {
        value -= shift;  // shift分ずれるので1*shift
        return *this;
    }
    bool operator==(const FixedPointImpl &obj) const noexcept
    {
        return (value - obj.value >= -1 and value - obj.value <= 1) ? true : false;
    }
    bool operator<(const FixedPointImpl &obj) const noexcept { return value < obj.value; }
    /*
    標準入出力に入力するとShiftで割った結果が出力される。
    FixedPoint a{5};
    std::cout << a << std::endl;

    5が出力される

    */
    friend std::ostream &operator<<(std::ostream &os, const FixedPointImpl &fp)
    {
        os << std::fixed;
        os << std::setprecision(10);
        os << fp.value.template convert_to<D>() / shift;
        os << std::resetiosflags(std::ios_base::floatfield);
        return os;
    }
};
inline auto abs(const FixedPointImpl<> &x)
{
    if (x < FixedPointImpl<>("0.0"))
    {
        return -x;
    }
    else
    {
        return x;
    }
}
template <typename T>
std::string to_string(const FixedPointImpl<T> &fp)
{
    return fp.getStrVal();
}
}  // namespace qmpc::Utils
using FixedPoint = qmpc::Utils::FixedPointImpl<>;
