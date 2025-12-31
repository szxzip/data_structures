#include "header.h"

// 从文件加载矩阵数据
bool loadMatrixFromFile(const string& filename, SatisfactionMatrix& matrix)
{
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl;
        return false;
    }

    int n = 0; // 运动员数量
    file >> n; // 若开头不合规，则之后的判定会拒绝通过

    // 运动员数量 1-20
    if (n <= 0 || n > 20) {
        cerr << "错误：无效的运动员数量！" << endl;
        file.close();
        return false;
    }

    matrix.resize(n);

    int lineNumber = 1; // 当前行号
    string line;
    getline(file, line); // 清除第一行换行符（和多余字符，如果有的话）

    // 读取男对女矩阵
    for (int i = 0; i < n; i++) {
        lineNumber++;

        if (!getline(file, line)) {
            cerr << "错误：无法读取第 " << lineNumber << " 行！" << endl;
            file.close();
            return false;
        }

        // 将行转化为字符串流
        istringstream iss(line);

        for (int j = 0; j < n; j++) {
            if (!(iss >> matrix.mf[i][j])) { // 读入数组
                cerr << "错误：第 " << lineNumber << " 行格式不正确！" << endl;
                file.close();
                return false;
            }
            if (matrix.mf[i][j] < 0 || matrix.mf[i][j] > 100) {
                cerr << "错误：第 " << lineNumber << " 行格式不正确！" << endl;
                file.close();
                return false;
            }
        }
    }

    // 读取女对男矩阵
    for (int i = 0; i < n; i++) {
        lineNumber++;

        if (!getline(file, line)) {
            cerr << "错误：无法读取第 " << lineNumber << " 行！" << endl;
            file.close();
            return false;
        }

        // 将行转化为字符串流
        istringstream iss(line);

        for (int j = 0; j < n; j++) {
            if (!(iss >> matrix.fm[i][j])) { // 读入数组
                cerr << "错误：第 " << lineNumber << " 行格式不正确！" << endl;
                file.close();
                return false;
            }
            if (matrix.fm[i][j] < 0 || matrix.fm[i][j] > 100) {
                cerr << "错误：第 " << lineNumber << " 行格式不正确！" << endl;
                file.close();
                return false;
            }
        }
    }

    file.close();
    return true;
}

// 保存矩阵数据到文件
bool saveMatrixToFile(const string& filename, const SatisfactionMatrix& matrix)
{
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法创建文件 " << filename << endl;
        return false;
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
    return true;
}
