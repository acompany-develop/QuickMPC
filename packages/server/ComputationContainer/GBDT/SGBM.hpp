#pragma once
#include <set>

#include "GBDT/SID3Regression.hpp"
#include "Math/Math.hpp"
#include "Share/Share.hpp"
namespace qmpc::GBDT
{

class SGBM
{
    const std::vector<Share> T;
    const std::vector<std::vector<std::vector<Share>>> S;
    const std::vector<Share> targets;
    const std::set<int> R;
    std::vector<std::shared_ptr<SID3Regression>> trees;
    Share init;
    FixedPoint lr = 0.1;

public:
    SGBM() = delete;
    SGBM(
        FixedPoint lr,
        const std::vector<Share> &T,
        const std::vector<std::vector<std::vector<Share>>> &S,
        const std::vector<Share> &targets,
        const std::set<int> &R
    )
        : T(T), S(S), targets(targets), R(R), lr(lr)
    {
    }
    /**
     * @brief 指定した決定技アルゴリズムでブースティングを実行
     * ブースティング時のハイパーパラメータ
     * 木の数、学習率lr,最大深度max_depth,葉の最大枚数max_leaves
     *
     *
     */
    auto boosting(size_t numIterations)
    {
        //予測結果
        std::vector<std::vector<Share>> y_dash;
        init = qmpc::Math::smean(targets);
        //現在の予測値
        std::vector<Share> currentY(T.size(), init);
        y_dash.emplace_back(currentY);
        for (size_t i = 0; i < numIterations; ++i)
        {
            //誤差
            std::vector<Share> diff(T.size());
            for (size_t i = 0; i < T.size(); ++i)
            {
                diff[i] = targets[i] - currentY[i];
            }
            auto tree = SID3Regression::createTree(T, S, diff, R);
            //勾配結果を取得
            auto grad = tree->grad();
            //予測値
            for (size_t i = 0; i < T.size(); ++i)
            {
                currentY[i] = currentY[i] + lr * grad[i];
            }
            y_dash.emplace_back(grad);
            trees.emplace_back(tree);
        }
        return currentY;
    }
    /**
     * @brief 推論関数（分類の場合は活性化関数を使用するように変更）
     *
     * @param data
     * @return auto
     */
    auto predict(const std::vector<std::vector<Share>> &data)
    {
        Share ret = init;
        for (const auto &tree : trees)
        {
            ret += lr * tree->predict(data);
        }
        return ret;
    }

    auto getJson() const
    {
        std::vector<nlohmann::json> jsons;
        for (const auto &tree : trees)
        {
            jsons.emplace_back(tree->createTreeJson());
        }
        return jsons;
    }
};
}  // namespace qmpc::GBDT
