#pragma once
#include "Math/Math.hpp"
#include "Share/Share.hpp"

namespace qmpc::GBDT
{
using Share = qmpc::Share::Share<FixedPoint>;
/**
 * @brief SID3に使用するデータクラス
 * T：全データのビット　例）4つのデータなら{1,1,1,1}　-> 1番目を使うと {0,1,1,1}
 * S：[属性列][属性値][index]
 * target : [属性値][index]
 *
 */
class bitParam
{
    //全体のデータ管理
    std::vector<Share> T;
    //ビットシェアデータ
    // [属性列][属性値][dataindex] = {1 or 0}
    // TODO：分類での使用の場合はデータ容量削減のため専用のクラスを使用を要検討
    std::vector<std::vector<std::vector<Share>>> S;
    //目的値分類 S0i
    // target[目標変数の取れる値][dataindex] = {0 or 1}で保持する
    std::vector<std::vector<Share>> targets;

public:
    bitParam() = delete;
    bitParam(
        const std::vector<Share> &T,
        const std::vector<std::vector<std::vector<Share>>> &S,
        const std::vector<std::vector<Share>> &targets
    )
        : T(T), S(S), targets(targets)
    {
    }
};

/**
 * @brief ブースティング決定木　基本アルゴリズムはID3
 * 入力：属性の分類配列R、データxの属性のbitベクトル、
 * 基本的にはID3になるので分類は離散的になる
 * 例）1.5,4.2,10.3,50 入力
 * 　　1~5:0,6~10:1,10以上:2　分類項目
 * 　　0,0,1,2,1 〜という値で扱う
 * 　　データ数3とすると
 * 　　attribute[0] ={1,0,1}
 * 　　attribute[1] ={0,1,0}
 * 　　attribute[2] ={0,0,0}
 * 　　のようにする
 */

class SID3
{
    //全体のデータ管理
    std::vector<Share> T;
    //ビットシェアデータ
    // [属性列][属性値][dataindex] = {1 or 0}
    // TODO：分類での使用の場合はデータ容量削減のため専用のクラスを使用を要検討
    std::vector<std::vector<std::vector<Share>>> S;
    //目的値分類 S0i
    // target[目標変数の取れる値][dataindex] = {0 or 1}で保持する
    std::vector<std::vector<Share>> targets;
    /*
    目的変数の値を分類するための変数
    二値分類の場合は1,0
    多値分類の場合は離散値のどれか
    TODO：回帰の場合は不動小数等で保存する必要がある
    */
    int att_class;
    //ジニ係数
    double gini_index;
    //現在の目的変数の予測属性値
    int currentAttribute;
    //トランザクションサイズ
    int size;
    //属性値管理
    std::set<int> R;
    //子ノード
    // CART等の場合は２個で確定、ID3等では複数もあり
    std::vector<std::shared_ptr<SID3>> children;
    //予測値（回帰と分類で表示内容が異なる
    Share y;
    inline static const double eps = 0.000001;

private:
    Share dot(const std::vector<Share> &, const std::vector<Share> &) const;
    int countClassAttr(const std::vector<Share> &classes);
    double infomationGain(const std::vector<Share> &category) const;
    std::vector<std::pair<double, int>> calcGini(
        const std::vector<std::vector<Share>> &U, const std::set<int> &R
    ) const;
    int dataSize() const
    {
        auto T_size = qmpc::Math::sum(T);
        open(T_size);
        auto size = recons(T_size);
        return static_cast<int>(size.getDoubleVal() + 0.1);
    }
    bool isTerminal() const { return children.empty(); }
    std::vector<std::vector<Share>> splitTransaction() const;
    Share majorityClass(const std::vector<std::vector<Share>> &U) const;
    auto getMaxGini(const std::vector<std::vector<Share>> &U) const;

public:
    SID3() = delete;
    //入力にはビットシェア化したデータとそれを分類するためのクラスビットシェアが必要
    //分類される項目は事前に区分を決めておく

    SID3(
        const std::vector<Share> &T,
        const std::vector<std::vector<std::vector<Share>>> &bit_data,
        const std::vector<std::vector<Share>> &targets,
        const std::set<int> &R
    )
        : T(T), S(bit_data), targets(targets), R(R), att_class(-1), y(FixedPoint(-1))
    {
        size = dataSize();
    }
    SID3(const nlohmann::json &json)
        : gini_index(json["gini"])
        , size(json["size"])
        , y(Share(FixedPoint(json["class"].get<double>())))
    {
        if (json.find("att_class") != json.end())
        {
            att_class = json["att_class"];
        }
        children.reserve(json["children"].size());
        for (const auto &child_json : json["children"])
        {
            children.emplace_back(std::make_unique<SID3>(child_json));
        }
    }
    ~SID3()
    {
        // spdlog::info("destructor {}",size);
    }
    nlohmann::json createTreeJson() const;
    Share predict(const std::vector<std::vector<Share>> &data) const;
    static std::shared_ptr<SID3> createTree(
        const std::vector<Share> &T,
        const std::vector<std::vector<std::vector<Share>>> &S,
        const std::vector<std::vector<Share>> &targets,
        const std::set<int> &R
    );
};
}  // namespace qmpc::GBDT