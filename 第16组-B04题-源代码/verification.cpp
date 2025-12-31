#include "header.h"

// 暴力搜索（n <= 10 可用）
MatchingResult* bruteSearch(const SatisfactionMatrix& matrix)
{
    int n = matrix.n;

    if (n > 10) {
        cerr << "n 过大，暴力搜索已禁用！" << endl;
        return nullptr;
    }

    // 构建总满意度矩阵
    vector<vector<int>> totalMatrix(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            totalMatrix[i][j] = matrix.mf[i][j] + matrix.fm[j][i];
        }
    }

    // 初始化最优结果
    MatchingResult* bestResult = new MatchingResult();
    bestResult->totalScore = 0;

    // 生成初始排列
    vector<int> permutation(n);
    for (int i = 0; i < n; i++) {
        permutation[i] = i;
    }

    // 枚举所有排列
    do {
        int totalScore = 0;
        int maleScore = 0;
        int femaleScore = 0;

        // 计算当前排列的总满意度
        for (int i = 0; i < n; i++) {
            int j = permutation[i]; // 男 i 配对女 j
            maleScore += matrix.mf[i][j];
            femaleScore += matrix.fm[j][i];
            totalScore += totalMatrix[i][j];
        }

        // 更新最优解
        if (totalScore > bestResult->totalScore) {
            bestResult->totalScore = totalScore;
            bestResult->maleScore = maleScore;
            bestResult->femaleScore = femaleScore;
            bestResult->pairs = permutation;
        }

    } while (next_permutation(permutation.begin(), permutation.end())); // 遍历所有排列并写入数组，完全升序 -> 完全降序

    return bestResult;
}

AlgorithmComparison::AlgorithmComparison(int size)
    : hungarian(size)
    , bruteForce(size)
    , n(size)
    , isOptimal(true)
    , scoreDifference(0)
{
}

// 验证匈牙利算法的结果
AlgorithmComparison* verify_hungarian_result(const SatisfactionMatrix& matrix, const MatchingResult& hungarian_result)
{

    AlgorithmComparison* comparison = new AlgorithmComparison(matrix.n);
    comparison->hungarian = hungarian_result;
    comparison->n = matrix.n;

    // 检查是否启用验证
    if (!enable_verification) {
        cerr << "验证功能已禁用。" << endl;
        return nullptr;
    }

    // 检查 n 是否过大
    if (comparison->n > 10) {
        cerr << "n > 10，验证功能已禁用。" << endl;
        return nullptr;
    }

    // 使用暴力搜索求解
    MatchingResult* bruteResult = bruteSearch(matrix);

    if (bruteResult != nullptr) {
        comparison->bruteForce = *bruteResult;
        comparison->scoreDifference = bruteResult->totalScore - hungarian_result.totalScore;
        comparison->isOptimal = (comparison->scoreDifference == 0);
        delete bruteResult;
    } else {
        cerr << "验证失败" << endl;
        return nullptr;
    }

    return comparison;
}
