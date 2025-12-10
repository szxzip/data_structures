#include "header.h"

bool Validation::validateMatrixInput(const std::string& input, int& value)
{
    try {
        value = std::stoi(input);
        return value >= 0 && value <= 100; // 假设满意度范围0-100
    } catch (...) {
        return false;
    }
}

bool Validation::validateN(const std::string& input, int& n)
{
    try {
        n = std::stoi(input);
        return n > 0 && n <= 10; // 限制最大规模为10，暴力搜索可行
    } catch (...) {
        return false;
    }
}
