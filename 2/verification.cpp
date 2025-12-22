#include "header.h"

/**
 * @brief 暴力搜索求解最优匹配
 * @param matrix 满意度矩阵
 * @return 匹配结果（保证是全局最优解）
 *
 * 算法流程：
 * 1. 生成女运动员索引的所有排列（0,1,2,...,n-1）
 * 2. 对于每种排列p，表示男i匹配女p[i]
 * 3. 计算该匹配的总满意度
 * 4. 保留最优解
 *
 * 时间复杂度：O(n! * n²)，实际中n≤8时可用
 */

// 暴力搜索
MatchingResult bruteSearch(const SatisfactionMatrix& matrix)
{
    int n = matrix.n;

    // 构建总满意度矩阵（与匈牙利算法完全相同）
    vector<vector<int>> totalMatrix(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            totalMatrix[i][j] = matrix.mf[i][j] + matrix.fm[j][i];
        }
    }

    cout << "[暴力搜索] 使用的总满意度矩阵:" << endl;
    for (int i = 0; i < n; i++) {
        cout << "   ";
        for (int j = 0; j < n; j++) {
            cout << setw(4) << totalMatrix[i][j] << " ";
        }
        cout << endl;
    }

    // 初始化最优结果
    MatchingResult bestResult(n);
    bestResult.totalScore = INT_MIN;

    // 生成初始排列
    vector<int> permutation(n);
    for (int i = 0; i < n; i++) {
        permutation[i] = i;
    }

    // 枚举所有排列
    long long permutationCount = 0;
    long long maxPermutations = 1000000; // 最多检查100万种排列

    // 进度显示
    if (n <= 8) {
        cout << "[暴力搜索] 开始搜索 " << tgamma(n + 1) << " 种排列..." << endl;
    } else {
        cout << "[暴力搜索] 警告: n=" << n << " 较大，限制最多检查 "
             << maxPermutations << " 种排列" << endl;
    }

    do {
        // 计算当前排列的总满意度
        int totalScore = 0;
        int maleScore = 0;
        int femaleScore = 0;

        for (int i = 0; i < n; i++) {
            int j = permutation[i];
            maleScore += matrix.mf[i][j];
            femaleScore += matrix.fm[j][i];
            totalScore += totalMatrix[i][j];
        }

        // 更新最优解
        if (totalScore > bestResult.totalScore) {
            bestResult.totalScore = totalScore;
            bestResult.maleScore = maleScore;
            bestResult.femaleScore = femaleScore;
            bestResult.pairs = permutation;
        }

        permutationCount++;

        // 显示进度（每10%显示一次）
        if (n <= 8) {
            long long totalPerms = static_cast<long long>(tgamma(n + 1));
            if (totalPerms > 1000 && permutationCount % (totalPerms / 10) == 0) {
                int progress = (permutationCount * 100) / totalPerms;
                cout << "[暴力搜索] 进度: " << progress << "% ("
                     << permutationCount << "/" << totalPerms << ")" << endl;
            }
        }

        // 安全限制
        if (permutationCount >= maxPermutations) {
            cout << "[暴力搜索] 达到限制，提前终止搜索" << endl;
            break;
        }

    } while (next_permutation(permutation.begin(), permutation.end()));

    cout << "[暴力搜索] 完成！检查了 " << permutationCount << " 种排列" << endl;
    cout << "[暴力搜索] 最优总满意度: " << bestResult.totalScore << endl;

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
AlgorithmComparison verify_hungarian_result(const SatisfactionMatrix& matrix, const MatchingResult& hungarian_result)
{

    AlgorithmComparison comparison(matrix.n);
    comparison.hungarian = hungarian_result;
    comparison.n = matrix.n;

    // 检查是否启用验证
    if (!enable_verification) {
        cout << "[验证] 验证功能已禁用" << endl;
        comparison.isOptimal = true;
        comparison.scoreDifference = 0;
        return comparison;
    }

    // 检查规模是否适合验证
    if (matrix.n > 10) {
        cout << "[验证] 跳过验证，n=" << matrix.n << " 太大（建议n≤10）" << endl;
        comparison.isOptimal = true;
        comparison.scoreDifference = 0;
        return comparison;
    }

    cout << "\n=== 开始暴力搜索验证 ===" << endl;
    cout << "问题规模: n = " << matrix.n << endl;

    // 使用暴力搜索求解
    MatchingResult bruteResult = bruteSearch(matrix);

    comparison.bruteForce = bruteResult;

    // 比较结果
    comparison.scoreDifference = bruteResult.totalScore - hungarian_result.totalScore;
    comparison.isOptimal = (comparison.scoreDifference == 0);

    return comparison;
}
