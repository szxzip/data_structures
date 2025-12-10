#include "header.h"

bool FileIO::readMatrixFromFile(const std::string& filename, Matrix& matrix, int n)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return false;
    }

    matrix = Matrix(n, std::vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (!(file >> matrix[i][j])) {
                std::cerr << "读取矩阵数据错误" << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool FileIO::saveResultToFile(const std::string& filename,
    const std::vector<int>& matching,
    int totalSatisfaction)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建文件: " << filename << std::endl;
        return false;
    }

    file << "最佳配对结果:\n";
    for (int i = 0; i < matching.size(); i++) {
        if (matching[i] != -1) {
            file << "男运动员 " << i + 1 << " ↔ 女运动员 " << matching[i] + 1 << "\n";
        }
    }
    file << "总满意度: " << totalSatisfaction << "\n";
    file.close();
    return true;
}
