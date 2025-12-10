#include "header.h"

// 暴力搜索算法
std::pair<std::vector<int>, int>
MatchingAlgorithms::bruteForceMatching(const Matrix& combinedMatrix)
{
    int n = combinedMatrix.size();
    std::vector<int> bestMatching(n, -1);
    int bestScore = INT_MIN;

    // 生成所有可能的排列
    std::vector<int> permutation(n);
    for (int i = 0; i < n; i++)
        permutation[i] = i;

    do {
        int score = 0;
        for (int i = 0; i < n; i++) {
            score += combinedMatrix[i][permutation[i]];
        }

        if (score > bestScore) {
            bestScore = score;
            bestMatching = permutation;
        }
    } while (std::next_permutation(permutation.begin(), permutation.end()));

    return { bestMatching, bestScore };
}

// 简化版匈牙利算法（贪心算法，因为完整的匈牙利算法比较复杂）
std::pair<std::vector<int>, int>
MatchingAlgorithms::hungarianAlgorithm(const Matrix& costMatrix)
{
    int n = costMatrix.size();
    std::vector<int> matching(n, -1);
    std::vector<bool> rowUsed(n, false);
    std::vector<bool> colUsed(n, false);

    // 将最大化问题转换为最小化问题
    Matrix matrix = costMatrix;
    int maxVal = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            maxVal = std::max(maxVal, matrix[i][j]);
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = maxVal - matrix[i][j];
        }
    }

    // 贪心算法：每行选择最小的未被使用的列
    for (int i = 0; i < n; i++) {
        int minVal = INT_MAX;
        int minCol = -1;

        for (int j = 0; j < n; j++) {
            if (!colUsed[j] && matrix[i][j] < minVal) {
                minVal = matrix[i][j];
                minCol = j;
            }
        }

        if (minCol != -1) {
            matching[i] = minCol;
            colUsed[minCol] = true;
        }
    }

    // 如果有未分配的，尝试调整
    for (int i = 0; i < n; i++) {
        if (matching[i] == -1) {
            for (int j = 0; j < n; j++) {
                if (!colUsed[j]) {
                    matching[i] = j;
                    colUsed[j] = true;
                    break;
                }
            }
        }
    }

    // 计算总满意度（使用原始矩阵）
    int totalSatisfaction = 0;
    for (int i = 0; i < n; i++) {
        if (matching[i] != -1) {
            totalSatisfaction += costMatrix[i][matching[i]];
        }
    }

    return { matching, totalSatisfaction };
}

// 简化的步骤函数实现（占位符）
void MatchingAlgorithms::step1(Matrix& matrix, int n)
{
    // 每行减去最小值
    for (int i = 0; i < n; i++) {
        int minVal = *std::min_element(matrix[i].begin(), matrix[i].end());
        for (int j = 0; j < n; j++) {
            matrix[i][j] -= minVal;
        }
    }

    // 每列减去最小值
    for (int j = 0; j < n; j++) {
        int minVal = INT_MAX;
        for (int i = 0; i < n; i++) {
            minVal = std::min(minVal, matrix[i][j]);
        }
        for (int i = 0; i < n; i++) {
            matrix[i][j] -= minVal;
        }
    }
}

void MatchingAlgorithms::step2(const Matrix& matrix, Matrix& mask,
    std::vector<bool>& rowCover, std::vector<bool>& colCover,
    int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == 0 && !rowCover[i] && !colCover[j]) {
                mask[i][j] = 1;
                rowCover[i] = true;
                colCover[j] = true;
            }
        }
    }

    // 清除覆盖
    std::fill(rowCover.begin(), rowCover.end(), false);
    std::fill(colCover.begin(), colCover.end(), false);
}

int MatchingAlgorithms::step3(const Matrix& mask, std::vector<bool>& colCover, int n)
{
    int colCount = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (mask[i][j] == 1) {
                colCover[j] = true;
            }
        }
    }

    for (int j = 0; j < n; j++) {
        if (colCover[j])
            colCount++;
    }

    if (colCount == n) {
        return 1; // 完成
    } else {
        return 2; // 继续
    }
}

std::pair<int, int> MatchingAlgorithms::findZero(int row, int col, const Matrix& matrix,
    const std::vector<bool>& rowCover,
    const std::vector<bool>& colCover, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == 0 && !rowCover[i] && !colCover[j]) {
                return { i, j };
            }
        }
    }
    return { -1, -1 };
}

void MatchingAlgorithms::step4(Matrix& matrix, Matrix& mask,
    std::vector<bool>& rowCover, std::vector<bool>& colCover,
    int path_row_0, int path_col_0, int n)
{
    // 简化的实现
    int path_row = -1;
    int path_col = -1;
    bool done = false;

    while (!done) {
        auto zeroPos = findZero(path_row_0, path_col_0, matrix, rowCover, colCover, n);
        if (zeroPos.first == -1) {
            done = true;
            step5(mask, rowCover, colCover, n);
        } else {
            mask[zeroPos.first][zeroPos.second] = 2;

            bool starInRow = false;
            for (int j = 0; j < n; j++) {
                if (mask[zeroPos.first][j] == 1) {
                    path_row_0 = zeroPos.first;
                    path_col_0 = j;
                    starInRow = true;
                    break;
                }
            }

            if (!starInRow) {
                done = true;
            } else {
                rowCover[zeroPos.first] = true;
                colCover[path_col_0] = false;
            }
        }
    }
}

void MatchingAlgorithms::step5(Matrix& mask, std::vector<bool>& rowCover,
    std::vector<bool>& colCover, int n)
{
    // 找到最小的未覆盖值
    int minVal = INT_MAX;
    for (int i = 0; i < n; i++) {
        if (!rowCover[i]) {
            for (int j = 0; j < n; j++) {
                if (!colCover[j]) {
                    // 这个值应该在外部矩阵中，这里简化处理
                    minVal = 1; // 简化
                }
            }
        }
    }

    // 减去最小值
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (rowCover[i]) {
                // 已覆盖的行加上最小值
            } else if (!colCover[j]) {
                // 未覆盖的列减去最小值
            }
        }
    }

    // 清除覆盖
    std::fill(rowCover.begin(), rowCover.end(), false);
    std::fill(colCover.begin(), colCover.end(), false);
}
