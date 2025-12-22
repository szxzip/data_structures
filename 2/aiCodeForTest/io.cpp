#include "header.h"

// 从文件加载矩阵数据
bool loadMatrixFromFile(const string& filename, SatisfactionMatrix& matrix)
{
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return false;
    }

    int n;
    if (!(file >> n)) {
        cerr << "错误：文件格式不正确" << endl;
        file.close();
        return false;
    }

    if (n <= 0 || n > 20) {
        cerr << "错误：运动员数量必须在1-20之间" << endl;
        file.close();
        return false;
    }

    matrix.resize(n);

    // 读取男对女矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (!(file >> matrix.mf[i][j])) {
                cerr << "错误：读取男对女矩阵数据失败" << endl;
                file.close();
                return false;
            }
        }
    }

    // 读取女对男矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (!(file >> matrix.fm[i][j])) {
                cerr << "错误：读取女对男矩阵数据失败" << endl;
                file.close();
                return false;
            }
        }
    }

    file.close();
    return true;
}

// 保存矩阵数据到文件
void saveMatrixToFile(const string& filename, const SatisfactionMatrix& matrix)
{
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法创建文件 " << filename << endl;
        return;
    }

    // 保存运动员数量
    file << matrix.n << endl;

    // 保存男对女矩阵
    for (int i = 0; i < matrix.n; i++) {
        for (int j = 0; j < matrix.n; j++) {
            file << matrix.mf[i][j] << " ";
        }
        file << endl;
    }

    // 保存女对男矩阵
    for (int i = 0; i < matrix.n; i++) {
        for (int j = 0; j < matrix.n; j++) {
            file << matrix.fm[i][j] << " ";
        }
        file << endl;
    }

    file.close();
}
