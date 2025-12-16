#include "header.h"
#include <cstdlib>
#include <ctime>

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
