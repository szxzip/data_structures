#include "header.h"

Matrix MatrixUtils::createMatrix(int n, int defaultValue)
{
    return Matrix(n, std::vector<int>(n, defaultValue));
}

Matrix MatrixUtils::addMatrices(const Matrix& m1, const Matrix& m2)
{
    int n = m1.size();
    Matrix result = createMatrix(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result[i][j] = m1[i][j] + m2[j][i]; // 注意：fm[j][i] 因为索引含义不同
        }
    }
    return result;
}

int MatrixUtils::calculateTotalSatisfaction(const Matrix& combined,
    const std::vector<int>& matching)
{
    int total = 0;
    for (int i = 0; i < matching.size(); i++) {
        if (matching[i] != -1) {
            total += combined[i][matching[i]];
        }
    }
    return total;
}

void MatrixUtils::printMatrix(const Matrix& matrix)
{
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << std::setw(4) << val << " ";
        }
        std::cout << std::endl;
    }
}

std::string MatrixUtils::matrixToString(const Matrix& matrix)
{
    std::stringstream ss;
    for (const auto& row : matrix) {
        for (int val : row) {
            ss << std::setw(4) << val << " ";
        }
        ss << "\n";
    }
    return ss.str();
}

bool MatrixUtils::isValidMatrix(const Matrix& matrix, int n)
{
    if (matrix.size() != n)
        return false;
    for (const auto& row : matrix) {
        if (row.size() != n)
            return false;
    }
    return true;
}
