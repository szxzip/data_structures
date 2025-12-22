#include "header.h"
#include <cstdlib>
#include <ctime>

// 构造函数
AlgorithmComparison::AlgorithmComparison(int size)
    : hungarian(size)
    , bruteForce(size)
    , n(size)
    , hungarianTime(0.0)
    , bruteForceTime(0.0)
    , isOptimal(true)
    , scoreDifference(0)
{
}

// 显示算法对比结果
void AlgorithmComparison::displayComparison() const
{
    cout << "\n========== 算法结果对比 ==========" << endl;
    cout << "问题规模: n = " << n << endl
         << endl;

    cout << "1. 性能对比:" << endl;
    cout << "   匈牙利算法: " << hungarianTime << " ms" << endl;
    cout << "   暴力搜索:   " << bruteForceTime << " ms" << endl;
    cout << "   速度比:     " << bruteForceTime / hungarianTime << " 倍" << endl;

    cout << "\n2. 分数对比:" << endl;
    cout << "   匈牙利算法总满意度: " << hungarian.totalScore << endl;
    cout << "   暴力搜索总满意度:   " << bruteForce.totalScore << endl;
    cout << "   分数差值:           " << scoreDifference << endl;

    if (isOptimal) {
        cout << "   ✓ 匈牙利算法找到了最优解" << endl;
    } else {
        cout << "   ✗ 匈牙利算法未找到最优解" << endl;
        cout << "      最优解比当前解高 " << scoreDifference << " 分" << endl;
    }

    cout << "\n3. 详细满意度对比:" << endl;
    cout << "   匈牙利算法 - 男方: " << hungarian.maleScore
         << "  女方: " << hungarian.femaleScore << endl;
    cout << "   暴力搜索   - 男方: " << bruteForce.maleScore
         << "  女方: " << bruteForce.femaleScore << endl;

    cout << "\n4. 匹配方案对比:" << endl;
    cout << "   男运动员 | 匈牙利算法匹配 | 暴力搜索匹配 | 是否相同" << endl;
    cout << "   --------|----------------|--------------|----------" << endl;

    for (int i = 0; i < n; i++) {
        string same = (hungarian.pairs[i] == bruteForce.pairs[i]) ? "✓" : "✗";
        cout << "     男" << setw(2) << i + 1 << "  ->  女" << setw(2) << hungarian.pairs[i] + 1
             << "        ->  女" << setw(2) << bruteForce.pairs[i] + 1
             << "        " << same << endl;
    }

    cout << "===================================" << endl;
}

/**
 * @brief SatisfactionMatrix构造函数
 */
SatisfactionMatrix::SatisfactionMatrix(int size)
    : n(size)
{
    resize(size);
}

/**
 * @brief 调整矩阵大小
 */
void SatisfactionMatrix::resize(int size)
{
    n = size;
    mf.resize(size);
    fm.resize(size);
    for (int i = 0; i < size; i++) {
        mf[i].resize(size, 0);
        fm[i].resize(size, 0);
    }
}

/**
 * @brief 随机生成满意度矩阵
 */
void SatisfactionMatrix::randomGenerate(int maxScore)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            mf[i][j] = rand() % maxScore + 1;
            fm[i][j] = rand() % maxScore + 1;
        }
    }
}

/**
 * @brief MatchingResult构造函数
 */
MatchingResult::MatchingResult(int size)
    : pairs(size, -1)
    , totalScore(0)
    , maleScore(0)
    , femaleScore(0)
{
}
