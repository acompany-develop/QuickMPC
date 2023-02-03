#include "matrix.hpp"

#include <Eigen/LU>

#include "logging/logger.hpp"
#include "networking.hpp"
namespace qmpc::Share
{
/************** utils **************/
template <class T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> setFromVector(
    const std::vector<std::vector<T>> &mat_v
)
{
    int h = mat_v.size();
    int w = mat_v[0].size();
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> ret(h, w);
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            ret(i, j) = mat_v[i][j];
        }
    }
    return ret;
}

/************** Share Matrix **************/
ShareMatrix::ShareMatrix(const std::vector<std::vector<Share>> &mat_v)
    : h(mat_v.size()), w(mat_v[0].size()), mat(setFromVector<Share>(mat_v))
{
}
ShareMatrix::ShareMatrix(const MatrixType &mat_s) : h(mat_s.rows()), w(mat_s.cols()), mat(mat_s) {}

// Eigen methods
const ::Share &ShareMatrix::operator()(int i, int j) const { return mat(i, j); }
ShareMatrix ShareMatrix::operator+(const ShareMatrix &obj) const
{
    return ShareMatrix(mat + obj.mat);
}
ShareMatrix ShareMatrix::operator-(const ShareMatrix &obj) const
{
    return ShareMatrix(mat - obj.mat);
}
ShareMatrix ShareMatrix::transpose() const { return ShareMatrix(mat.transpose()); }

// original methods
std::vector<std::vector<::Share::value_type>> ShareMatrix::open_and_recons() const
{
    auto ret = std::vector<std::vector<Share::value_type>>();
    ret.reserve(h);
    // NOTE: begin, endチェック
    for (int i = 0; i < h; ++i)
    {
        std::vector<Share> tmp;
        tmp.reserve(w);
        for (int j = 0; j < w; ++j)
        {
            tmp.emplace_back(mat(i, j));
        }
        open(tmp);
        ret.emplace_back(recons(tmp));
    }
    return ret;
}

ShareMatrix ShareMatrix::operator*(const ShareMatrix &obj) const
{
    if (w != obj.h)
    {
        qmpc::Log::throw_with_trace(std::runtime_error("Matrix Mul Error!"));
    }

    // 積だけまとめて計算してしまう
    int all_size = h * obj.w * w;
    std::vector<Share> a;
    std::vector<Share> b;
    a.reserve(all_size);
    b.reserve(all_size);
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < obj.w; ++j)
        {
            for (int k = 0; k < w; ++k)
            {
                a.emplace_back(mat(i, k));
                b.emplace_back(obj.mat(k, j));
            }
        }
    }
    auto mul = a * b;

    // まとめて計算した積を1つずつ取り出す
    int it = 0;
    auto ret = std::vector<std::vector<Share>>(h, std::vector<Share>(obj.w));
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < obj.w; ++j)
        {
            auto s = Share();
            for (int k = 0; k < w; ++k)
            {
                // mat[i][k] * obj.mat[k][j]
                s += mul[it];
                ++it;
            }
            ret[i][j] = s;
        }
    }
    return ShareMatrix(ret);
}

ShareMatrix operator*(double d, const ShareMatrix &smat)
{
    auto m = smat.mat;
    for (int i = 0; i < m.rows(); ++i)
    {
        for (int j = 0; j < m.cols(); ++j)
        {
            m(i, j) *= d;
        }
    }
    return ShareMatrix(m);
}

ShareMatrix ShareMatrix::identity(int h, int w)
{
    auto identity = std::vector<std::vector<Share>>(h, std::vector<Share>(w));
    for (int i = 0; i < h; ++i)
    {
        identity[i][i] += FixedPoint("1");
    }
    return identity;
}

ShareMatrix ShareMatrix::inverse() const
{
    auto R_ = std::vector<std::vector<Share>>(h, std::vector<Share>(w));
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            // 乱数の範囲はいくつかの実験で定めたもの，最適であるとは限らない
            FixedPoint rnd = RandGenerator::getInstance()->getRand<FixedPoint>(0, 100) / 50.0;
            R_[i][j] = rnd;
        }
    }
    auto R = ShareMatrix(R_);
    auto U_s = R * mat;
    auto U = Matrix(U_s.open_and_recons());
    auto U_inv = U.inverse();
    auto U_inv_s = U_inv.sharize_mat();
    auto mat_inv = U_inv_s * R;
    return mat_inv;
}

std::vector<std::vector<::Share>> ShareMatrix::get_row() const
{
    std::vector<std::vector<Share>> ret;
    ret.reserve(h);
    for (int i = 0; i < h; ++i)
    {
        std::vector<Share> tmp;
        tmp.reserve(w);
        for (int j = 0; j < w; ++j)
        {
            tmp.emplace_back(mat(i, j));
        }
        ret.emplace_back(tmp);
    }
    return ret;
}

/************** Matrix **************/
Matrix::Matrix(const std::vector<std::vector<Share::value_type>> &mat_v)
    : mat(setFromVector<Share::value_type>(mat_v))
{
}
Matrix::Matrix(const MatrixType &mat) : mat(mat) {}

// Eigen methods
const ::Share::value_type &Matrix::operator()(int i, int j) const { return mat(i, j); }
Matrix Matrix::inverse() const { return Matrix(mat.inverse()); }

ShareMatrix Matrix::sharize_mat() const
{
    int h = mat.rows();
    int w = mat.cols();
    std::vector<std::vector<Share>> ret(h, std::vector<Share>(w));
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            ret[i][j] = qmpc::Share::sharize(mat(i, j));
        }
    }
    return ret;
}
}  // namespace qmpc::Share
