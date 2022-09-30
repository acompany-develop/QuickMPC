#include "SID3Regression.hpp"

#include <set>

namespace qmpc::GBDT
{
/**
 * @brief この決定木での勾配を出力する
 * 最小二乗の場合は(t_i - y_i)が勾配になる
 *
 * @return std::vector<Share>
 */
std::vector<Share> SID3Regression::grad() const
{
    if (isTerminal())
    {
        return grads;
    }
    std::vector<Share> ret(T.size());
    for (const auto &child : children)
    {
        auto child_grad = child->grad();
        for (int i = 0; i < size; ++i)
        {
            ret[i] += child_grad[i];
        }
    }
    return ret;
}

Share SID3Regression::dot(const std::vector<Share> &x, const std::vector<Share> &y) const
{
    auto ret = x * y;
    return qmpc::Math::sum(ret);
}
/**
 * @brief 情報重要度を出力する。（これを使って条件分割を最適化する）
 *
 * @return Share
 */
Share SID3Regression::infomationGain(const std::vector<Share> &category) const
{
    auto mask = category * T;
    auto mask_size_share = qmpc::Math::sum(mask);
    open(mask_size_share);
    auto mask_size = recons(mask_size_share);
    if (mask_size == 0)
    {
        // TODO:個数が0の場合はとりあえず最大値
        return Share(FixedPoint(1000000000));
    }
    auto average = dot(mask, targets) / mask_size;
    std::vector<Share> averageVec(T.size(), average);
    auto average_mask = averageVec * mask;
    auto target_mask = targets * mask;
    Share ret;
    for (size_t i = 0; i < T.size(); ++i)
    {
        Share cur = target_mask[i] - average_mask[i];
        cur *= cur;
        ret += cur;
    }
    ret /= FixedPoint(2);
    return ret;
}
/**
 * @brief 最適な分割の説明変数を選択して返却する
 *
 * @return std::pair<double, int> 最小二乗誤差、インデックス
 */
std::pair<double, int> SID3Regression::IG() const
{
    std::vector<std::pair<double, int>> categoryErrors;
    for (const auto &r : R)
    {
        Share sumError;
        auto categories = S[r];
        for (auto &&category : categories)
        {
            sumError += infomationGain(category);
        }
        open(sumError);
        auto categoryError = recons(sumError);
        categoryErrors.emplace_back(categoryError.getDoubleVal(), r);
    }
    if (categoryErrors.empty()) return {-1, -1};
    return *min_element(categoryErrors.begin(), categoryErrors.end());
}
// /**
//  * @brief 指定した属性の個数を返す
//  *
//  * @param classes
//  * @return int
//  */
// int SID3Regression::countClassAttr(const std::vector<Share> &classes)
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
nlohmann::json SID3Regression::createTreeJson() const
{
    nlohmann::json ret;
    ret["size"] = size;
    if (not isTerminal()) ret["att_class"] = att_class;
    ret["error_value"] = error_value;
    auto arr = nlohmann::json::array();
    for (const auto &child : children)
    {
        auto tree = child->createTreeJson();
        arr.emplace_back(tree);
    }
    if (isTerminal()) ret["y_dash"] = y.getVal().getDoubleVal();
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
Share SID3Regression::predict(const std::vector<std::vector<Share>> &data) const
{
    if (isTerminal())
    {
        return y;
    }
    int index = 0;
    auto attribute_value = data[att_class];
    Share ret;
    // spdlog::info("att class is {}",att_class);
    // spdlog::info("sttribute size {}",attribute_value.size());
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
 * @return std::shared_ptr<SID3Regression>
 */
std::shared_ptr<SID3Regression> SID3Regression::createTree(
    const std::vector<Share> &T,
    const std::vector<std::vector<std::vector<Share>>> &S,
    const std::vector<Share> &targets,
    const std::set<int> &R
)
{
    auto current = std::make_shared<SID3Regression>(T, S, targets, R);
    std::tie(current->error_value, current->att_class) = current->IG();

    // TODO:ここではデータ数に対してうまく調整したほうが良いかも
    if (R.empty() or (T.size() >= 3u * current->size) or (0u == current->size)) return current;
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
}  // namespace qmpc::GBDT
