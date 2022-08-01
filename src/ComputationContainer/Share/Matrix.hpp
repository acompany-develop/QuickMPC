#pragma once
#include <vector>
#include "Share.hpp"
#include <Eigen/Core>

// 自作クラスでEigenの行列を扱うための各種宣言
// 参考1: https://eigen.tuxfamily.org/dox/TopicCustomizing_CustomScalar.html
// 参考2: https://eigen.tuxfamily.org/dox/structEigen_1_1NumTraits.html
namespace Eigen
{
template <>
struct NumTraits<Share::value_type> : NumTraits<double>
{
    using Real = Share::value_type;
    using NonInteger = Share::value_type;
    using Nested = Share::value_type;
    using Literal = Share::value_type;
    enum
    {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
    };
};
template <>
struct NumTraits<Share> : NumTraits<double>
{
    using Real = Share;
    using NonInteger = Share;
    using Nested = Share;
    using Literal = Share;
    enum
    {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
    };
};
}  // namespace Eigen

namespace qmpc::Share
{
class ShareMatrix
{
    using Share = ::Share;
    using MatrixType = Eigen::Matrix<Share, Eigen::Dynamic, Eigen::Dynamic>;

    const int h;
    const int w;
    const MatrixType mat;

public:
    ShareMatrix(const std::vector<std::vector<Share>> &);
    ShareMatrix(const MatrixType &);

    std::vector<std::vector<Share::value_type>> open_and_recons() const;

    const Share &operator()(int i, int j) const;
    ShareMatrix operator+(const ShareMatrix &) const;
    ShareMatrix operator-(const ShareMatrix &) const;
    ShareMatrix operator*(const ShareMatrix &) const;

    ShareMatrix transpose() const;
    ShareMatrix inverse() const;

    std::vector<std::vector<Share>> get_row() const;
};

class Matrix
{
    using Share = ::Share;
    using MatrixType = Eigen::Matrix<Share::value_type, Eigen::Dynamic, Eigen::Dynamic>;

    const MatrixType mat;

public:
    Matrix(const std::vector<std::vector<Share::value_type>> &);
    Matrix(const MatrixType &);

    const Share::value_type &operator()(int i, int j) const;

    Matrix inverse() const;
    ShareMatrix sharize_mat() const;
};
}  // namespace qmpc::Share