
#include "Compare.hpp"

#include "ConfigParse/ConfigParse.hpp"
#include "LogHeader/Logger.hpp"

namespace qmpc::Share
{

// アルゴリズムの詳細はこちら:
// Docs/faster-comparison-operators.md
bool operator<(const Share<FixedPoint> &left, const Share<FixedPoint> &right)
{
    Share<FixedPoint> s = left - right;
    Share s_ltz = LTZ(s);
    open(s_ltz);
    auto ret = recons(s_ltz);

    if (ret.getDoubleVal() > 0.95)
    {
        return true;
    }
    else if (ret.getDoubleVal() >= 0.5)
    {
        spdlog::error(
            "This operation (Share == Share) determined to be false, but it could be true."
        );
        spdlog::error(
            "If you want to ignore the error and continue the calculation, replace 'exit' with "
            "'return false;'. "
        );
        std::exit(EXIT_FAILURE);
        // return true;
    }
    else if (ret.getDoubleVal() >= 0.05)
    {
        spdlog::error(
            "This operation (Share < FixedPoint) determined to be false, but it could be true."
        );
        spdlog::error(
            "If you want to ignore the error and continue the calculation, replace 'exit' with "
            "'return false;'. "
        );
        std::exit(EXIT_FAILURE);
        // return false;
    }
    else
    {
        return false;
    }
}

// [left == right] <=> [not (left < right)] and [not (right < left)]
// アルゴリズムの詳細はこちら:
// Docs/faster-comparison-operators.md
bool operator==(const Share<FixedPoint> &left, const Share<FixedPoint> &right)
{
    auto x_ret = (left < right);
    auto y_ret = (right < left);
    auto ret = (FixedPoint(1) - x_ret) * (FixedPoint(1) - y_ret);

    if (ret.getDoubleVal() > 0.95)
    {
        return true;
    }
    else if (ret.getDoubleVal() >= 0.5)
    {
        spdlog::error(
            "This operation (Share == Share) determined to be true, "
            "but it could be false."
        );
        spdlog::error(
            "If you want to ignore the error and continue the "
            "calculation, replace 'exit' with 'return true;'. "
        );
        std::exit(EXIT_FAILURE);
        // return true;
    }
    else if (ret.getDoubleVal() >= 0.05)
    {
        spdlog::error(
            "This operation (Share == Share) determined to be true, "
            "but it could be false."
        );
        spdlog::error(
            "If you want to ignore the error and continue the "
            "calculation, replace 'exit' with 'return true;'. "
        );
        std::exit(EXIT_FAILURE);
        // return false;
    }
    else
    {
        return false;
    }
}

bool operator<(const Share<FixedPoint> &left, const FixedPoint &right)
{
    Share<FixedPoint> s = left - right;
    Share s_ltz = LTZ(s);
    open(s_ltz);
    auto ret = recons(s_ltz);

    if (ret.getDoubleVal() > 0.95)
    {
        return true;
    }
    else if (ret.getDoubleVal() >= 0.5)
    {
        spdlog::error(
            "This operation (Share == Share) determined to be true, "
            "but it could be false."
        );
        spdlog::error(
            "If you want to ignore the error and continue the "
            "calculation, replace 'exit' with 'return true;'. "
        );
        std::exit(EXIT_FAILURE);
        // return true;
    }
    else if (ret.getDoubleVal() >= 0.05)
    {
        spdlog::error(
            "This operation (Share == Share) determined to be true, "
            "but it could be false."
        );
        spdlog::error(
            "If you want to ignore the error and continue the "
            "calculation, replace 'exit' with 'return true;'. "
        );
        std::exit(EXIT_FAILURE);
        // return false;
    }
    else
    {
        return false;
    }
}
bool operator==(const Share<FixedPoint> &left, const FixedPoint &right)
{
    auto x_ret = (left < right);
    auto y_ret = (right < left);
    auto ret = (FixedPoint(1) - x_ret) * (FixedPoint(1) - y_ret);

    if (ret.getDoubleVal() > 0.95)
    {
        return true;
    }
    else if (ret.getDoubleVal() >= 0.5)
    {
        spdlog::error(
            "This operation (Share == Share) determined to be true, "
            "but it could be false."
        );
        spdlog::error(
            "If you want to ignore the error and continue the "
            "calculation, replace 'exit' with 'return true;'. "
        );
        std::exit(EXIT_FAILURE);
        // return true;
    }
    else if (ret.getDoubleVal() >= 0.05)
    {
        spdlog::error(
            "This operation (Share == Share) determined to be true, "
            "but it could be false."
        );
        spdlog::error(
            "If you want to ignore the error and continue the "
            "calculation, replace 'exit' with 'return true;'. "
        );
        std::exit(EXIT_FAILURE);
        // return false;
    }
    else
    {
        return false;
    }
}

// Less Than Zero ([s < 0])
// アルゴリズムの詳細はこちら:
// Docs/faster-comparison-operators.md
Share<FixedPoint> LTZ(const Share<FixedPoint> &s)
{
    int m = 16;
    int k = 48;

    // s に 2^m をかけて整数化を試みる
    Share<FixedPoint> x = s * FixedPoint(std::to_string(1LL << m));
    Share<FixedPoint> y = FixedPoint(std::to_string(1LL << k)) + x;
    Share<FixedPoint> z = getLSBShare(y);
    y = (y - z) * FixedPoint(0.5);
    for (int i = 1; i < k; ++i)
    {
        Share<FixedPoint> b = getLSBShare(y);
        z += (b * FixedPoint(std::to_string(1LL << i)));
        y = (y - b) * FixedPoint(0.5);
    }
    return (z - x) / FixedPoint(std::to_string(1LL << k));
}

// 以下、サブプロトコル

// 乱数rのシェア[r]を生成する。
// 0 から p-1 のランダム値を返す。
// Random Number Sharing
Share<PrimeField> subRNS()
{
    return Share<PrimeField>(RandGenerator::getInstance()->getRand<PrimeField>());
}

// 乱数ビットrのシェア[r]_Bを生成する。
// Random Bit Sharing
Share<PrimeField> subRBS()
{
    while (true)
    {
        Share<PrimeField> r = subRNS();
        Share<PrimeField> square_r = r * r;
        open(square_r);
        PrimeField square_r_rec = recons(square_r);
        if (square_r_rec.getVal() != 0)
        {
            PrimeField r_dash = modsqrt(square_r_rec);
            PrimeField inv_r_dash = r_dash.getInv();
            PrimeField inv_two(PrimeField::inverse_of_two);
            return inv_two * (inv_r_dash * r + PrimeField(1));
        }
    }
}

// 乱数ビットシェア [r_0]_B, ..., [r_l]_B を生成する。
// Random Number Bitwise-Sharing
std::vector<Share<PrimeField>> subRBVS()
{
    std::vector<Share<PrimeField>> r_shares(PrimeField::l);
    for (int i = 0; i < PrimeField::l; i++)
    {
        r_shares[i] = subRBS();
    }
    // 本来であれば，r < pであるかチェックをしなければいけないが，
    // (1) チェックに時間を要する
    // (2) r < pである確率はほぼ1 (正確には0.9999999999999999968016...)
    // より，一旦チェックは省略．
    return r_shares;
}

// subUnboundedFanInOrの内部で使用する(l+1)次多項式
// 係数は f_65_or_coefficient に相当
Share<PrimeField> f65Or(const std::vector<Share<PrimeField>> &A_shares)
{
    Share<PrimeField> ans(PrimeField(0));
    for (int i = 1; i < PrimeField::l + 1; i++)
    {
        PrimeField coef(f_65_or_coefficient[i]);
        ans += (coef * A_shares[i - 1]);
    }
    PrimeField coef0(f_65_or_coefficient[0]);
    ans = ans + coef0;
    return ans;
}

// subUnboundedFanInAndの内部で使用する(l+1)次多項式
// 係数は f_65_and_coefficient に相当
Share<PrimeField> f65And(const std::vector<Share<PrimeField>> &A_shares)
{
    Share<PrimeField> ans(PrimeField(0));
    for (int i = 1; i < PrimeField::l + 1; i++)
    {
        PrimeField coef(f_65_and_coefficient[i]);
        ans += (coef * A_shares[i - 1]);
    }
    PrimeField coef0(f_65_and_coefficient[0]);
    ans = ans + coef0;
    return ans;
}

// ビットシェア[a_{l-1}]_p, ..., [a_0]_p が与えられたときに、
// 多入力論理和 b_i = V_{i=0}^{l-1}a_i のシェア [b_{l-1}]_p, ..., [b_0]_p
// を生成するサブプロトコル。
Share<PrimeField> subUnboundedFanInOr(const std::vector<Share<PrimeField>> &a_shares)
{
    // step 1
    Share<PrimeField> A_share(PrimeField(0));
    for (int i = 0; i < PrimeField::l; i++)
    {
        A_share += a_shares[i];
    }
    A_share = A_share + PrimeField(1);

    // step 2
    // f_65_or_coefficientの作成
    // このステップはf65Or() 関数で利用する

    // step3, 4, 5
    std::vector<Share<PrimeField>> b_shares(PrimeField::l);          // [b_i]
    std::vector<Share<PrimeField>> b_dash_shares(PrimeField::l);     // [b'_i]
    std::vector<Share<PrimeField>> b_inverse_shares(PrimeField::l);  // [b^{-1}_i]
    for (int i = 0; i < PrimeField::l; i++)
    {
        b_shares[i] = subRNS();
        b_dash_shares[i] = subRNS();
    }
    std::vector<Share<PrimeField>> B_shares = b_shares * b_dash_shares;
    open(B_shares);
    std::vector<PrimeField> B_recons = recons(B_shares);
    for (int i = 0; i < PrimeField::l; i++)
    {
        b_inverse_shares[i] = B_recons[i].getInv() * b_dash_shares[i];
    }

    // step6
    // Share<PrimeField> c_shares[PrimeField::l];
    // for (int i = 0; i < PrimeField::l; i++)
    // {
    //     if (i == 0)
    //     {
    //         c_shares[i] = A_share * b_inverse_shares[i];
    //     }
    //     else
    //     {
    //         c_shares[i] = A_share * b_shares[i - 1] * b_inverse_shares[i];
    //     }
    // }

    // 上記コメントアウト部分の実装を、一括乗算を用いて書き直している。
    // step6でどんな処理をしているのか分かりやすくするため、あえて上記のコメントアウトを残している。
    //
    // A_share*b_inverse_shares[i]の部分は、一括乗算を用いてまとめて乗算できるが、*b_shares[i-1]の部分は、工夫しないと一括乗算を用いることはできない。
    // 理由：iの値によって、b_shares[i-1]を乗算するかしないか変化するため(i=0のときは乗算しない、つまり「1のシェア」を乗算していることと同じ)。さらに、b_shares
    // の末尾の要素は使わないため。 【どうやって解決したか】
    // 1. まずA_shareをPrimeField::l個持つベクトルtmp_A_sharesを作成。
    // 2. tmp_A_sharesとb_inverse_sharesを一括乗算する（結果をc_sharesに格納）。
    // 3. c_sharesと以下のvectorを一括乗算する。
    //    vector(「1のシェア」、b_shares[0], b_shares[1], ...,
    //    b_shares[PrimeField::l-2])
    // 以上のようなvectorを作成するため、b_sharesの末尾の要素を取り出し、「1のシェア」を先頭に追加したtmp_b_sharesを作成している。
    // 今回は、「1のシェア」として、パーティ数nの逆元を使用している(nの逆元がnつ揃うと、1になるため)。
    std::vector<Share<PrimeField>> tmp_A_shares(PrimeField::l, A_share);
    std::vector<Share<PrimeField>> c_shares = tmp_A_shares * b_inverse_shares;
    std::vector<Share<PrimeField>> tmp_b_shares = b_shares;
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    tmp_b_shares.pop_back();
    tmp_b_shares.insert(
        tmp_b_shares.begin(), Share<PrimeField>(PrimeField(n_parties).getInv())
    );  // パーティ数nの逆元を値にもつShare<PrimeField>を先頭にインサート
    c_shares = c_shares * tmp_b_shares;
    open(c_shares);
    std::vector<PrimeField> c_recons = recons(c_shares);

    // step7
    std::vector<Share<PrimeField>> A_shares(PrimeField::l);
    PrimeField tmp{1};
    for (int i = 0; i < PrimeField::l; i++)
    {
        tmp *= c_recons[i];
        A_shares[i] = tmp * b_shares[i];
    }

    // step8
    Share<PrimeField> ans = f65Or(A_shares);
    return ans;
}

// ビットシェア[a_{l-1}]_p, ..., [a_0]_p が与えられたときに、
// 多入力論理積 のシェア [b_0]_p, ..., [b_{l-1}]_p を生成するサブプロトコル。
// equality_testで使用
Share<PrimeField> subUnboundedFanInAnd(const std::vector<Share<PrimeField>> &a_shares)
{
    // step 1
    Share<PrimeField> A_share(PrimeField(0));
    for (int i = 0; i < PrimeField::l; i++)
    {
        A_share += a_shares[i];
    }
    A_share = A_share + PrimeField(1);

    // step 2
    // f_65_and_coefficientの作成
    // このステップはf65And() 関数で利用する

    // step3, 4, 5
    std::vector<Share<PrimeField>> b_shares(PrimeField::l);          // [b_i]
    std::vector<Share<PrimeField>> b_dash_shares(PrimeField::l);     // [b'_i]
    std::vector<Share<PrimeField>> b_inverse_shares(PrimeField::l);  // [b^{-1}_i]
    for (int i = 0; i < PrimeField::l; i++)
    {
        b_shares[i] = subRNS();
        b_dash_shares[i] = subRNS();
    }
    std::vector<Share<PrimeField>> B_shares = b_shares * b_dash_shares;
    open(B_shares);
    std::vector<PrimeField> B_recons = recons(B_shares);
    for (int i = 0; i < PrimeField::l; i++)
    {
        b_inverse_shares[i] = B_recons[i].getInv() * b_dash_shares[i];
    }

    // step6
    // Share<PrimeField> c_shares[PrimeField::l];
    // for (int i = 0; i < PrimeField::l; i++)
    // {
    //     if (i == 0)
    //     {
    //         c_shares[i] = A_share * b_inverse_shares[i];
    //     }
    //     else
    //     {
    //         c_shares[i] = A_share * b_shares[i - 1] * b_inverse_shares[i];
    //     }
    // }

    // 上記コメントアウト部分の実装を、一括乗算を用いて書き直している。
    // step6でどんな処理をしているのか分かりやすくするため、あえて上記のコメントアウトを残している。
    //
    // A_share*b_inverse_shares[i]の部分は、一括乗算を用いてまとめて乗算できるが、*b_shares[i-1]の部分は、工夫しないと一括乗算を用いることはできない。
    // 理由：iの値によって、b_shares[i-1]を乗算するかしないか変化するため(i=0のときは乗算しない、つまり「1のシェア」を乗算していることと同じ)。さらに、b_shares
    // の末尾の要素は使わないため。 【どうやって解決したか】
    // 1. まずA_shareをPrimeField::l個持つベクトルtmp_A_sharesを作成。
    // 2. tmp_A_sharesとb_inverse_sharesを一括乗算する（結果をc_sharesに格納）。
    // 3. c_sharesと以下のvectorを一括乗算する。
    //    vector(「1のシェア」、b_shares[0], b_shares[1], ...,
    //    b_shares[PrimeField::l-2])
    // 以上のようなvectorを作成するため、b_sharesの末尾の要素を取り出し、「1のシェア」を先頭に追加したtmp_b_sharesを作成している。
    // 今回は、「1のシェア」として、パーティ数nの逆元を使用している(nの逆元がnつ揃うと、1になるため)。
    std::vector<Share<PrimeField>> tmp_A_shares(PrimeField::l, A_share);
    std::vector<Share<PrimeField>> c_shares = tmp_A_shares * b_inverse_shares;
    std::vector<Share<PrimeField>> tmp_b_shares = b_shares;
    Config *conf = Config::getInstance();
    int n_parties = conf->n_parties;
    tmp_b_shares.pop_back();
    tmp_b_shares.insert(
        tmp_b_shares.begin(), Share<PrimeField>(PrimeField(n_parties).getInv())
    );  // パーティ数nの逆元を値にもつShare<PrimeField>を先頭にインサート
    c_shares = c_shares * tmp_b_shares;
    open(c_shares);
    std::vector<PrimeField> c_recons = recons(c_shares);

    // step7
    std::vector<Share<PrimeField>> A_shares(PrimeField::l);
    PrimeField tmp{1};
    for (int i = 0; i < PrimeField::l; i++)
    {
        tmp *= c_recons[i];
        A_shares[i] = tmp * b_shares[i];
    }

    // step8
    Share<PrimeField> ans = f65And(A_shares);
    return ans;
}

// start_index から PrimeField::l-1 まで0をパディングする
// start_index未満の場合はそのままa_sharesの値を反映させる
// 例: subPadding0([2, 3, 4, 1, 5], 2, ret[])  ---> ret = [2, 3, 0, 0, 0]
// ※注意: subPrefixOr以外で使わないこと！！！
// (メモリ範囲外へのアクセスを防ぐため)
std::vector<Share<PrimeField>> subPadding0(
    const std::vector<Share<PrimeField>> &a_shares, int start_index
)
{
    std::vector<Share<PrimeField>> ret(PrimeField::l);
    for (int i = 0; i < std::min(start_index, PrimeField::l); i++)
    {
        ret[i] = a_shares[i];
    }
    return ret;
}

// ビットシェア[a_1]_p, ..., [a_l]_p が与えられたときに、b_i=\lor_{j=1}^ia_j
// を満たすPrefix-Or [b_1]_p, ..., [b_l]_p を生成するサブプロトコル。
std::vector<Share<PrimeField>> subPrefixOr(const std::vector<Share<PrimeField>> &a_shares)
{
    std::vector<Share<PrimeField>> ret(PrimeField::l);
    // step1
    std::vector<Share<PrimeField>> x_shares(PrimeField::lambda);
    for (int i = 0; i < PrimeField::lambda; i++)
    {
        auto beg = std::begin(a_shares);
        std::advance(beg, i * PrimeField::lambda);
        auto end = beg;
        std::advance(end, PrimeField::lambda);
        std::vector<Share<PrimeField>> tmp(beg, end);
        tmp.resize(PrimeField::l);
        x_shares[i] = subUnboundedFanInOr(tmp);
    }

    // step2
    std::vector<Share<PrimeField>> y_shares(PrimeField::lambda);
    for (int i = 0; i < PrimeField::lambda; i++)
    {
        auto tmp = subPadding0(x_shares, i + 1);
        y_shares[i] = subUnboundedFanInOr(tmp);
    }

    // step3
    std::vector<Share<PrimeField>> f_shares(PrimeField::lambda);
    f_shares[0] = x_shares[0];
    for (int i = 1; i < PrimeField::lambda; i++)
    {
        f_shares[i] = y_shares[i] - y_shares[i - 1];
    }

    // step4
    std::vector<Share<PrimeField>> ai0_shares(PrimeField::lambda);
    for (int j = 0; j < PrimeField::lambda; j++)
    {
        ai0_shares[j] = Share<PrimeField>(PrimeField(0));
        for (int i = 0; i < PrimeField::lambda; i++)
        {
            int index = PrimeField::lambda * i + j;
            ai0_shares[j] += (f_shares[i] * a_shares[index]);
        }
    }

    // step5
    std::vector<Share<PrimeField>> bi0_shares(PrimeField::lambda);
    for (int j = 0; j < PrimeField::lambda; j++)
    {
        auto tmp = subPadding0(ai0_shares, j + 1);
        bi0_shares[j] = subUnboundedFanInOr(tmp);
    }

    // step6
    std::vector<Share<PrimeField>> s_shares(PrimeField::lambda);
    for (int i = 0; i < PrimeField::lambda; i++)
    {
        s_shares[i] = y_shares[i] - f_shares[i];
    }

    // step7
    for (int i = 0; i < PrimeField::lambda; i++)
    {
        for (int j = 0; j < PrimeField::lambda; j++)
        {
            int index = PrimeField::lambda * i + j;
            ret[index] = f_shares[i] * bi0_shares[j] + s_shares[i];
        }
    }
    return ret;
}

// [a]_B, [b]_Bが与えられたときに，[a < b]_pを生成するサブプロトコル
Share<PrimeField> subBitwiseLessThan(
    const std::vector<Share<PrimeField>> &a_shares, const std::vector<Share<PrimeField>> &b_shares
)
{
    // step1
    std::vector<Share<PrimeField>> c_shares(PrimeField::l);
    auto ab_shares = a_shares * b_shares;
    for (int i = 0; i < PrimeField::l; i++)
    {
        c_shares[i] = a_shares[i] + b_shares[i] - (PrimeField(2) * ab_shares[i]);
    }

    // step2
    std::vector<Share<PrimeField>> d_shares = subPrefixOr(c_shares);

    // step3
    std::vector<Share<PrimeField>> e_shares(PrimeField::l);
    e_shares[0] = d_shares[0];
    for (int i = 1; i < PrimeField::l; i++)
    {
        e_shares[i] = d_shares[i] - d_shares[i - 1];
    }

    // step4
    auto eb = e_shares * b_shares;
    Share<PrimeField> ret(PrimeField(0));
    for (int i = 0; i < PrimeField::l; i++)
    {
        ret += eb[i];
    }

    return ret;
}

// Bitshare to Share<PrimeField>
// [a]_B を [a]_p に変換する
// 主に subHalfLessThan で使用 (テストでも使用する場合あり)
Share<PrimeField> BStoSC(const std::vector<Share<PrimeField>> &a_shares)
{
    Share<PrimeField> ret(PrimeField(0));
    PrimeField coef(1);
    for (int i = PrimeField::l - 1; i >= 0; i--)
    {
        ret += (coef * a_shares[i]);
        coef *= PrimeField(2);
    }
    return ret;
}

// PrimeField to BitShare
// PrimeField x を [x]_B に変換し，bit_sharesに格納する
// 主に subHalfLessThan で使用 (テストでも使用する場合あり)
std::vector<Share<PrimeField>> PFtoBS(const PrimeField &x)
{
    std::vector<Share<PrimeField>> bit_shares(PrimeField::l);
    Config *conf = Config::getInstance();
    auto y = x.getVal();
    for (int i = PrimeField::l - 1; i >= 0; i--)
    {
        PrimeField r = RandGenerator::getInstance()->getRand<PrimeField>();  // r = r1 + r2 + r3
        auto s = sharize(r);
        if (conf->party_id == conf->sp_id)
        {
            bit_shares[i] =
                Share<PrimeField>(PrimeField(y % 2) - r + s.getVal());  // (bit) - r + r1
        }
        else
        {
            bit_shares[i] = Share<PrimeField>(s);
            // Share<PrimeField>::incrementId(1); // IDのズレを補正
        }
        y = y / 2;
    }
    return bit_shares;
}

// [a]が与えられたときに，[a < p/2]_pを生成するサブプロトコル．
// 本プロトコルで使用する．
Share<PrimeField> subHalfLessThan(Share<PrimeField> a_share)
{
    // step1
    std::vector<Share<PrimeField>> r_shares = subRBVS();
    Share<PrimeField> r_share = BStoSC(r_shares);

    // step2
    Share<PrimeField> c_share = (PrimeField(2) * a_share) + r_share;
    open(c_share);
    PrimeField c = recons(c_share);

    // cをビットシェア化 (step3の際に必要)
    std::vector<Share<PrimeField>> c_shares = PFtoBS(c);

    // step3
    Share<PrimeField> cr, ret;
    cr = subBitwiseLessThan(c_shares, r_shares);
    if (c.getVal() % 2 == 0)
    {
        ret =
            (cr * (PrimeField(1) - r_shares[PrimeField::l - 1])
             + (PrimeField(1) - cr) * r_shares[PrimeField::l - 1]);
    }
    else
    {
        ret =
            (cr * r_shares[PrimeField::l - 1]
             + (PrimeField(1) - cr) * (PrimeField(1) - r_shares[PrimeField::l - 1]));
    }

    // step4
    return PrimeField(1) - ret;
}

// 以下、メインプロトコル

// [a], c1, c2が与えられたときに，[c1 < a < c2]を生成するプロトコル
Share<PrimeField> intervalTest(PrimeField c1, Share<PrimeField> a_share, PrimeField c2)
{
    // step1
    std::vector<Share<PrimeField>> r_shares = subRBVS();
    Share<PrimeField> r_share = BStoSC(r_shares);

    // step2
    Share<PrimeField> c_share = a_share + r_share;
    open(c_share);
    PrimeField c = recons(c_share);

    // step3, 4
    PrimeField r_low, r_high;
    std::vector<Share<PrimeField>> r_lows(PrimeField::l, ShareComp{}),
        r_highs(PrimeField::l, ShareComp{});
    Share<PrimeField> ret;
    if (c1 < c && c < c2)
    {
        r_low = c - c1 - PrimeField(1);
        r_high = c + PrimeField(PrimeField::p) - c2 + PrimeField(1);
        r_lows = PFtoBS(r_low);
        r_highs = PFtoBS(r_high);
        ret = PrimeField(1)
              - (subBitwiseLessThan(r_lows, r_shares) * subBitwiseLessThan(r_shares, r_highs));
    }
    else
    {
        if (c2 <= c)
        {
            r_low = c - c2;
            r_high = c - c1;
        }
        else if (c <= c1)
        {
            r_low = c + PrimeField(PrimeField::p) - c2;
            r_high = c + PrimeField(PrimeField::p) - c1;
        }
        r_lows = PFtoBS(r_low);
        r_highs = PFtoBS(r_high);
        ret = subBitwiseLessThan(r_lows, r_shares) * subBitwiseLessThan(r_shares, r_highs);
    }

    return ret;
}

// [a], [b]が与えられたときに，[a < b]を生成するプロトコル
Share<PrimeField> comparisonTest(Share<PrimeField> a_share, Share<PrimeField> b_share)
{
    // step1
    Share<PrimeField> w, x, y, z;
    w = subHalfLessThan(a_share);
    x = subHalfLessThan(b_share);
    y = subHalfLessThan(a_share - b_share);

    auto xy = x * y;
    // step2
    z = w * (x + y - PrimeField(2) * xy) + PrimeField(1) - y - x + xy;
    return z;
}

// [a], [b]が与えられたときに、[a = b]を生成するプロトコル
Share<PrimeField> equalityTest(Share<PrimeField> a_share, Share<PrimeField> b_share)
{
    // step1
    std::vector<Share<PrimeField>> r_shares = subRBVS();
    Share<PrimeField> r_share = BStoSC(r_shares);

    // step2
    Share<PrimeField> c_share = a_share - b_share + r_share;
    open(c_share);
    PrimeField c = recons(c_share);
    auto x = c.getVal();

    // step3
    std::vector<Share<PrimeField>> c_dash_shares(PrimeField::l);
    for (int i = PrimeField::l - 1; i >= 0; i--)
    {
        if (x % 2 == 1)
        {
            c_dash_shares[i] = r_shares[i];
        }
        else
        {
            c_dash_shares[i] = PrimeField(1) - r_shares[i];
        }
        x >>= 1;
    }

    // step4
    Share<PrimeField> ret = subUnboundedFanInAnd(c_dash_shares);
    return ret;
}
}  // namespace qmpc::Share
