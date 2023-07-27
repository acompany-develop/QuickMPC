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
namespace mp = boost::multiprecision;
using mp_int = boost::multiprecision::cpp_int;
using mp_float = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<50>>;
using SgnByte = std::pair<bool, std::string>;

class FixedPoint : private boost::operators<FixedPoint>
{
private:
    constexpr static long long shift = 100'000'000;
    mp_int value;

public:
    FixedPoint() : value(0) {}
    FixedPoint(const FixedPoint &v) : value(v.value) {}
    FixedPoint(FixedPoint &&v) : value(std::move(v.value)) {}
    template <
        typename T,
        std::enable_if_t<std::is_arithmetic_v<std::remove_reference_t<T>>, std::nullptr_t> =
            nullptr>
    FixedPoint(const T &v)
    {
        mp_float tmp = static_cast<mp_float>(v);
        tmp *= shift;
        value = static_cast<mp_int>(tmp);
    }
    FixedPoint(const std::string &str)
    {
        mp_float v_{str};
        value = static_cast<mp_int>(v_ * shift);
    }
    FixedPoint(const SgnByte &P)
    {
        const auto &[sgn, abs_byte] = P;
        import_bits(value, abs_byte.begin(), abs_byte.end(), 8);
        if (sgn)
        {
            value *= -1;
        }
    }
    FixedPoint &operator=(const FixedPoint &obj)
    {
        value = obj.value;
        return *this;
    }
    FixedPoint &operator=(FixedPoint &&obj)
    {
        value = std::move(obj.value);
        return *this;
    }
    template <typename T = mp_int>
    T getVal() const
    {
        // getVal() より getShiftedVal() の方が適切かもしれない
        return value.template convert_to<T>();
    }
    std::string getStrVal() const
    {
        mp_float ret = static_cast<mp_float>(value);
        ret /= shift;
        return ret.str(20, std::ios_base::fixed);
    }
    mp_int getRoundValue() const
    {
        mp_int ret = value / shift;
        if ((value % shift) * 2 >= shift)
        {
            ret++;
        }
        return ret;
    }
    SgnByte getSgnByte() const
    {
        bool sgn = value < 0;
        std::string bytes;
        export_bits(value, std::back_inserter(bytes), 8);
        return std::make_pair(sgn, bytes);
    }
    double getDoubleVal() const
    {
        mp_float ret = static_cast<mp_float>(value);
        return static_cast<double>(ret / shift);
    }
    constexpr static auto getShift() noexcept { return shift; }

    FixedPoint operator-() const
    {
        FixedPoint ret{};
        ret.value = -value;
        return ret;
    }

    FixedPoint &operator+=(const FixedPoint &obj)
    {
        value += obj.value;
        return *this;
    }
    FixedPoint &operator-=(const FixedPoint &obj)
    {
        value -= obj.value;
        return *this;
    }
    FixedPoint &operator*=(const FixedPoint &obj)
    {
        value *= obj.value;
        value /= shift;  // 整数で保持するため余分なshiftを除算する
        return *this;
    }
    FixedPoint &operator/=(const FixedPoint &obj)
    {
        value *= shift;
        value /= obj.value;
        return *this;
    }
    FixedPoint &operator%=(const FixedPoint &obj)
    {
        value %= obj.value;
        return *this;
    }
    FixedPoint &operator++()
    {
        value += shift;  // shift分ずれるので1*shift
        return *this;
    }
    FixedPoint &operator--()
    {
        value -= shift;  // shift分ずれるので1*shift
        return *this;
    }
    bool operator==(const FixedPoint &obj) const noexcept
    {
        return (value - obj.value >= -1 and value - obj.value <= 1) ? true : false;
    }
    bool operator<(const FixedPoint &obj) const noexcept { return value < obj.value; }
    /*
    標準入出力に入力するとShiftで割った結果が出力される。
    FixedPoint a{5};
    std::cout << a << std::endl;

    5が出力される

    */
    friend std::ostream &operator<<(std::ostream &os, const FixedPoint &fp)
    {
        os << std::fixed;
        os << std::setprecision(10);
        os << fp.value.template convert_to<mp_float>() / shift;
        os << std::resetiosflags(std::ios_base::floatfield);
        return os;
    }
};
inline auto abs(const FixedPoint &x)
{
    if (x < FixedPoint("0.0"))
    {
        return -x;
    }
    else
    {
        return x;
    }
}
template <typename FP>
std::string to_string(const FP &fp)
{
    static_assert(std::is_same_v<FP, FixedPoint>, "for FP");
    return fp.getStrVal();
}
}  // namespace qmpc::Utils
using FixedPoint = qmpc::Utils::FixedPoint;
