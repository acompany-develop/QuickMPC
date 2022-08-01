#include "SID3.hpp"
#include <set>

namespace qmpc::GBDT
{

    /**
     * @brief 分割したデータの中で最も多い属性を返す
     *
     * @param U 目的変数で分割したデータ
     * @return Share
     */
    Share SID3::majorityClass(const std::vector<std::vector<Share>> &U) const
    {
        std::vector<std::pair<int, int>> base_count;
        int index = 0;
        for (const auto &u : U)
        {
            auto count_share = qmpc::Math::sum(u);
            open(count_share);
            auto count = recons(count_share);
            base_count.emplace_back(static_cast<int>(count.getDoubleVal()), index);
            index++;
        }
        auto [max_num, max_num_index] = *max_element(base_count.begin(), base_count.end());
        // TODO:正確にShare化する必要があるが分類の場合はこのままで良い
        Share ret = qmpc::Share::getConstantShare(FixedPoint(max_num_index));
        return ret;
    }
    /**
     * @brief 目的変数の値に従ってデータを分割する
     *
     * @return std::vector<std::vector<Share>>
     */
    std::vector<std::vector<Share>> SID3::splitTransaction() const
    {
        //分割データ管理
        // U[目標変数の取れる値][dataindex] = {1 or 0}
        std::vector<std::vector<Share>> U;
        for (const auto &target : targets)
        {
            auto si = target * T;
            U.emplace_back(si);
        }
        return U;
    }

    Share SID3::dot(const std::vector<Share> &x, const std::vector<Share> &y) const
    {
        auto ret = x * y;
        return qmpc::Math::sum(ret);
    }
    /**
     * @brief 最大Gini係数の計数値と属性列インデックスを返す
     *
     * @param U
     * @return auto
     */
    auto SID3::getMaxGini(const std::vector<std::vector<Share>> &U) const
    {
        auto gini_indexies = calcGini(U, R);
        for (const auto &[gini_, index] : gini_indexies)
        {
            // spdlog::info("gini_ is {}",gini_);
            // spdlog::info("index is {}",index);
        }
        if (gini_indexies.empty())
            return std::make_pair<double, int>(-1, -1);
        auto ret = *std::max_element(gini_indexies.begin(), gini_indexies.end());
        return ret;
    }

    /**
     * @brief Gini不純度を計算する
     * セキュアなGini係数計算らしいがいらない気がする
     * 普通にGini係数計算にしたほうがいいかもしれない
     * @return double
     */
    std::vector<std::pair<double, int>> SID3::calcGini(
        const std::vector<std::vector<Share>> &U, const std::set<int> &R) const
    {
        //これが一番gini係数に近くなるらしい
        constexpr int a = 8;
        int attribute_size = R.size();
        int target_value_size = std::size(targets);

        std::vector<Share> Gini;
        std::vector<int> gini_index;
        Gini.reserve(attribute_size);
        gini_index.reserve(attribute_size);
        for (const auto &r : R)
        {
            int attribute_value_size = std::size(S[r]);
            for (int j = 0; j < attribute_value_size; ++j)
            {
                Share y;
                std::vector<Share> x(target_value_size);
                for (int i = 0; i < target_value_size; ++i)
                {
                    x[i] = dot(U[i], S[r][j]);
                    y += x[i];
                }
                auto xx_ = x * x;
                auto xx = qmpc::Math::sum(xx_);
                y *= FixedPoint(a);
                y += 1;
                Gini.emplace_back(xx / y);
                gini_index.emplace_back(r);
            }
        }
        open(Gini);
        auto recGini = recons(Gini);

        std::vector<std::pair<double, int>> ret;
        int index = 0;
        for (auto &&gini : recGini)
        {
            ret.emplace_back(gini.getDoubleVal(), gini_index[index]);
            index++;
        }
        return ret;
    }
    /**
     * @brief 情報重要度を出力する。（これを使って条件分割を最適化する）
     *
     * @return double
     */
    // double SID3::infomationGain(const std::vector<Share> &category)
    // {
    //     auto infoGain = I();
    //     for (const auto &child : children)
    //     {
    //         infoGain -= child->I();
    //     }
    //     spdlog::info("information gain is {}",infoGain);
    //     return infoGain;
    // }
    // /**
    //  * @brief 指定した属性の個数を返す
    //  *
    //  * @param classes
    //  * @return int
    //  */
    // int SID3::countClassAttr(const std::vector<Share> &classes)
    // {
    //     Share ret;
    //     size_t n = std::size(T);
    //     size_t classSize = std::size(classes);
    //     for (int i = 0; i < n; ++i)
    //     {
    //         for (int class_index = 0; class_index < classSize; ++class_index)
    //         {
    //             Share zeroOrone = qmpc::Math::sum(S[i][class_index] * classes);
    //             ret += zeroOrone;
    //         }
    //     }
    //     open(ret);
    //     auto count = recons(ret);
    //     return static_cast<int>(count.getDoubleVal());
    // }
    /**
     * @brief 決定木のJsonを返す（この関数を呼び出した元オブジェクトをルートとした）
     *
     * @return nlohmann::json
     */
    nlohmann::json SID3::createTreeJson() const
    {
        nlohmann::json ret;
        ret["gini"] = gini_index;
        ret["size"] = size;
        ret["class"] = y.getDoubleVal();
        auto arr = nlohmann::json::array();
        for (const auto &child : children)
        {
            auto tree = child->createTreeJson();
            arr.emplace_back(tree);
        }
        if (not isTerminal())
            ret["att_class"] = att_class;
        ret["children"] = arr;
        return ret;
    }

    /**
     * @brief　このオブジェクトをルートとした決定木Fの予測値y^を返す。
     *
     * @param data 予測したいデータbitの配列にしたもの
     * 注意：学習で使ったデータ配列ではなく[属性][属性値]の配列を扱う
     * 例）1列目1,2,3 2列目1,2 3列目1,2,3,4 {{0,0,1}, { 0,1,} ,{0,0,1,0}}
     * 　　この場合は1列目3 2列目2 3列目３　のデータという意味
     * 学習で作成した分岐条件配列との内積が1の時に子ノードに進む
     * 分岐がない場合のclassが予測値y^
     * @return Share
     */
    Share SID3::predict(const std::vector<std::vector<Share>> &data) const
    {
        if (isTerminal())
        {
            return y;
        }
        int index = 0;
        auto attribute_value = data[att_class];
        Share ret;
        // spdlog::info("att class is {}",att_class);
        // spdlog::info("attribute size {}",attribute_value.size());
        // spdlog::info("children size is {}",children.size());
        for (const auto &child : children)
        {
            ret += attribute_value[index] * child->predict(data);
            index++;
        }
        return ret;
    }
    /**
     * @brief 決定木を作成する
     *
     * @return std::shared_ptr<SID3>
     */
    std::shared_ptr<SID3> SID3::createTree(
        const std::vector<Share> &T,
        const std::vector<std::vector<std::vector<Share>>> &S,
        const std::vector<std::vector<Share>> &targets,
        const std::set<int> &R)
    {
        auto current = std::make_shared<SID3>(T, S, targets, R);
        auto U = current->splitTransaction();
        current->y = current->majorityClass(U);
        // spdlog::info("T is {}",current->size);
        // spdlog::info("R size is {}",R.size());

        std::tie(current->gini_index, current->att_class) = current->getMaxGini(U);

        // spdlog::info("max gini is {}",current->gini_index);
        // spdlog::info("gini index is {}",current->att_class);
        // TODO:ここではデータ数に対してうまく調整したほうが良いかも
        if (R.empty() or T.size() / 2u >= (unsigned int)(current->size) or 0 == current->size or current->gini_index <= eps)
            return current;
        auto R_ = R;
        R_.erase(current->att_class);
        for (size_t j = 0; j < S[current->att_class].size(); ++j)
        {
            auto T_ = T * S[current->att_class][j];
            auto child = createTree(T_, S, targets, R_);
            current->children.emplace_back(child);
        }
        return current;
    }
} // namespace qmpc::GBDT