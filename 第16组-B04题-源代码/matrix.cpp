#include "header.h"

// 构造函数
SatisfactionMatrix::SatisfactionMatrix(int size)
    : n(size) // 初始化矩阵维度
{
    resize(size); // 调用resize函数分配内存
}

// 调整矩阵大小
void SatisfactionMatrix::resize(int size)
{
    n = size; // 更新矩阵维度
    mf.resize(size);
    fm.resize(size);

    // 为每个子向量分配空间并初始化为0
    for (int i = 0; i < size; i++) {
        mf[i].resize(size, 0);
        fm[i].resize(size, 0);
    }
}

// 随机生成满意度矩阵
void SatisfactionMatrix::randomGenerate()
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // 生成男性i对女性j的满意度分数（1到maxScore）
            mf[i][j] = rand() % 101;
            // 生成女性j对男性i的满意度分数（1到maxScore）
            fm[i][j] = rand() % 101;
        }
    }
}

// 构造函数
MatchingResult::MatchingResult(int size)
    : pairs(size, -1) // 初始化配对数组，所有元素设为-1（表示未配对）
    , totalScore(0)
    , maleScore(0)
    , femaleScore(0)
{
}
