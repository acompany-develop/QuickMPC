#pragma once

#include "FixedPoint/FixedPoint.hpp"
#include "PrimeField/PrimeField.hpp"
#include "ConfigParse/ConfigParse.hpp"
#include "Networking.hpp"
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