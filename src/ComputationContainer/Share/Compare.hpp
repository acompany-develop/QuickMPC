#pragma once

#include <future>

#include "ConfigParse/ConfigParse.hpp"
#include "FixedPoint/FixedPoint.hpp"
#include "Networking.hpp"
#include "PrimeField/PrimeField.hpp"
#include "Share.hpp"
namespace qmpc::Share
{
using PrimeField = qmpc::PrimeField<>;
// TODO:int64までしか対応していないためint128以上にするには修正が必要
inline const boost::multiprecision::uint128_t f_65_or_coefficient[65] = {
    18446744073709551493ull, 2575104960981055254ull,  4007652756006865783ull,
    4100635666949508952ull,  13520771370602560384ull, 10644542489970931539ull,
    13748537863183121296ull, 11359914866391222416ull, 11247219807349180267ull,
    10960075435459066463ull, 16764588636202349672ull, 17506652363315270202ull,
    16166875542546532309ull, 3964617125041583173ull,  11719255904316824165ull,
    10424936898019828672ull, 4205127743406738449ull,  14520034406506226582ull,
    13839012095455519695ull, 10500139574289659412ull, 2962336181382223452ull,
    6767675871271430215ull,  6933421926280334339ull,  15958841553693851655ull,
    15160740279984758829ull, 7422142630459751295ull,  8493480566397125402ull,
    5197722549818600602ull,  14735780618044279429ull, 1253747699690153233ull,
    16331314050017923346ull, 8939522708637072316ull,  14886963398089785966ull,
    9073751420990236602ull,  12505410719513991034ull, 2125245660187079351ull,
    15846075763828250700ull, 3886182126918060030ull,  8108428213219044342ull,
    7230779232974847667ull,  16872135629383927467ull, 6378144199063646497ull,
    17663587804867607672ull, 16148746862917335565ull, 15537620600452524460ull,
    5258026759045415754ull,  13903054235566673560ull, 13960238641761739087ull,
    17636644537631434137ull, 14478303024279957496ull, 2874963986830092253ull,
    3027861925099033411ull,  3827102190137661457ull,  12528878074288803348ull,
    5215393447045447612ull,  9586533887964671917ull,  3356091034157294274ull,
    2902984712153508625ull,  6333090465997358509ull,  9996528576620247823ull,
    11806089667835996467ull, 15792408873328964335ull, 14951514151164516184ull,
    2230240327554514938ull,  17774600287293087221ull};

// TODO:int64までしか対応していないためint128以上にするには修正が必要
//  subUnboundedFanInAndで使用する(l+1)次多項式の係数
inline const boost::multiprecision::uint128_t f_65_and_coefficient[65] = {
    1ull,
    1095567251290110014ull,
    15603599130927532848ull,
    3927973679664754768ull,
    13253920145236317331ull,
    5072689204291235733ull,
    3435840349470671764ull,
    16549778763406408630ull,
    12459576446030141042ull,
    10696557716079594807ull,
    3197018912709867365ull,
    9970630734852788482ull,
    10106868463622397398ull,
    6870526778099185113ull,
    16162773849234898198ull,
    6512084038109877353ull,
    17791231165580233294ull,
    4939553442392971102ull,
    6613779837673282520ull,
    6112835044719072700ull,
    11561852329015516476ull,
    3241087653075867336ull,
    11115359391888299599ull,
    1734929295488186025ull,
    11958404333307668373ull,
    1438215050425705879ull,
    11073690707525551423ull,
    17532628264929026480ull,
    6082710703763137390ull,
    5125529121156609847ull,
    3536240549677738615ull,
    168123567554747533ull,
    17222647251913610444ull,
    6314115150414057577ull,
    5720266728093558989ull,
    11315745394801430483ull,
    13585209705064903598ull,
    10609659846084906928ull,
    6341781282790677798ull,
    394864220565298862ull,
    5249871464994426133ull,
    13296832158245447365ull,
    12801767259528789327ull,
    8166545265620715462ull,
    12622067858020089155ull,
    10852787575795086238ull,
    15358952924301378654ull,
    14141418894305759068ull,
    13215619082908852856ull,
    9900973216959770499ull,
    11398882017370494561ull,
    1875791515713213273ull,
    18179510337970552075ull,
    12349041994205078879ull,
    4559441349647564738ull,
    4543633939816633313ull,
    9247229033112764012ull,
    9230709934058803069ull,
    12860062884571458050ull,
    14898881793951465380ull,
    9282848170176021024ull,
    16257860633325038337ull,
    546853837372292893ull,
    3893473855680099452ull,
    672143786416464336ull};

bool operator<(const Share<FixedPoint> &left, const Share<FixedPoint> &right);
bool operator==(const Share<FixedPoint> &left, const Share<FixedPoint> &right);
bool operator<(const Share<FixedPoint> &left, const FixedPoint &right);
bool operator==(const Share<FixedPoint> &left, const FixedPoint &right);

Share<FixedPoint> LTZ(const Share<FixedPoint> &s);
/// @brief l=32 split array
inline constexpr std::array<int, 7> delta = {5, 5, 5, 5, 5, 5, 2};
template <typename T, std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
Share<T> operator==(const Share<T> &left, const Share<T> &right)
{
    return equality(left, right);
}
/// @brief generate one-hot-vector used by unitv
/// @tparam N generate share-value mod N
/// @return random share [r] and r-th value is 1 ,otherwise is 0 in n length array
template <size_t N>
std::pair<Share<int>, std::vector<Share<int>>> unitvPrep()
{
    std::vector<Share<int>> e(N * N);
    std::vector<AddressId> addressIds(N * N);
    std::vector<Share<int>> ret(N * N);
    Config *conf = Config::getInstance();
    auto server = ComputationToComputation::Server::getServer();
    auto random_s = RandGenerator::getInstance()->getRandVec<long long>(1, 1 << 20, N);
    auto r = RandGenerator::getInstance()->getRand<long long>(0, N - 1);
    int n_parties = conf->n_parties;
    int pt_id = conf->party_id - 1;
    for (int i = 0; i < N * N; ++i)
    {
        addressIds[i] = e[i].getId();
    }
    if (pt_id == 0)
    {
        for (int i = 0; i < N * N; ++i)
        {
            if (i % N == i / N) e[i] = 1;
            e[i] += random_s[i % N];
        }

        std::rotate(e.begin(), e.begin() + r * N, e.end());
        send(e, (pt_id + 1) % n_parties + 1);
        auto s = receive<int>((pt_id + n_parties - 1) % n_parties + 1, addressIds);

        for (int i = 0; i < N * N; ++i)
        {
            ret[i] = s[i] - random_s[i % N];
        }
    }
    else
    {
        auto s = receive<int>((pt_id + n_parties - 1) % n_parties + 1, addressIds);
        for (int i = 0; i < N * N; ++i)
        {
            e[i] = s[i] + random_s[i % N];
        }
        std::rotate(e.begin(), e.begin() + r * N, e.end());
        send(e, (pt_id + 1) % n_parties + 1);
        for (int i = 0; i < N * N; ++i)
        {
            ret[i] = -random_s[i % N];
        }
    }
    return {r, ret};
}

template <typename T, int N = 32, int N_dash = 32>
std::vector<Share<T>> unitv(const Share<T> &n)
{
    auto [r, v] = unitvPrep<32>();
    auto diff = n - r;
    open(diff);
    auto rec = recons(diff);
    int m = (rec % N + N) % N;
    std::vector<Share<T>> ret(N);
    for (int i = 0; i < N; ++i)
    {
        ret[i] = v[N * m + i];
    }
    return ret;
}
template <typename Arr>
std::vector<int> expand(int x, const Arr &delta)
{
    unsigned int x_u = static_cast<unsigned int>(x);
    std::vector<int> delta_sum;
    std::vector<int> ret;
    delta_sum.emplace_back(0);
    int sum = 0;
    for (auto &&a : delta)
    {
        sum += a;
        delta_sum.emplace_back(sum);
    }

    reverse(delta_sum.begin(), delta_sum.end());
    for (auto &&delta_num : delta_sum)
    {
        ret.emplace_back(static_cast<int>(1ll * x_u / (1ll << delta_num)));
        x_u = static_cast<unsigned int>(1ll * x_u % (1ll << delta_num));
    }
    return ret;
}
template <typename T>
Share<T> equality1(const Share<T> &x, const Share<T> &y)
{
    auto g = unitv<T>(x - y);
    return g[0];
}
template <typename T>
Share<T> equality(const Share<T> &x, const Share<T> &y)
{
    Config *conf = Config::getInstance();
    int d{};
    if (conf->party_id == 1)
    {
        d = x.getVal() - y.getVal();
    }
    else
    {
        d = y.getVal() - x.getVal();
    }
    auto d_expand = expand(d, delta);
    int m = d_expand.size();
    std::vector<Share<int>> p(m), q(m);
    Share<T> f{};
    for (int i = 0; i < m; ++i)
    {
        if (conf->party_id == 1)
        {
            p[i] = d_expand[i];
        }
        else
        {
            q[i] = d_expand[i];
        }
        f += equality1(p[i], q[i]);
    }
    return unitv(f)[m];
}
// 以下、サブプロトコル
Share<PrimeField> subRNS();                // Random Number Sharing
Share<PrimeField> subRBS();                // Random Bit Sharing
std::vector<Share<PrimeField>> subRBVS();  // Random Number Bitwise-Sharing
Share<PrimeField> f65Or(const std::vector<Share<PrimeField>> &A_shares);
Share<PrimeField> f65And(const std::vector<Share<PrimeField>> &A_shares);
Share<PrimeField> subUnboundedFanInOr(const std::vector<Share<PrimeField>> &a_shares);
Share<PrimeField> subUnboundedFanInAnd(const std::vector<Share<PrimeField>> &a_shares);
std::vector<Share<PrimeField>> subPadding0(
    const std::vector<Share<PrimeField>> &a_shares, int start_index
);
std::vector<Share<PrimeField>> subPrefixOr(const std::vector<Share<PrimeField>> &a_shares);
Share<PrimeField> subBitwiseLessThan(
    const std::vector<Share<PrimeField>> &a_shares, const std::vector<Share<PrimeField>> &b_shares
);
Share<PrimeField> BStoSC(const std::vector<Share<PrimeField>> &a_shares);
std::vector<Share<PrimeField>> PFtoBS(const PrimeField &x);
Share<PrimeField> subHalfLessThan(Share<PrimeField> a_share);

// 以下、メインプロトコル
Share<PrimeField> intervalTest(PrimeField c1, Share<PrimeField> a_share, PrimeField c2);
Share<PrimeField> comparisonTest(Share<PrimeField> a_share, Share<PrimeField> b_share);
Share<PrimeField> equalityTest(Share<PrimeField> a_share, Share<PrimeField> b_share);

/*FixedPointの型とPrimeFieldの型が互換性があるため上位の方にキャストされてそのご下位の方にキャストしなおしている
そのため内部の方が変更される場合は修正が必要
*/
inline PrimeField Fp2Pf(const FixedPoint &fp)
{
    auto p_half{PrimeField::p / 2};
    return PrimeField(static_cast<decltype(PrimeField::p)>(fp.getVal() + p_half));
}

}  // namespace qmpc::Share