#include "header.h"

/**
 * @brief 验证匈牙利算法的结果
 */
AlgorithmComparison verify_hungarian_result(
    const SatisfactionMatrix& matrix,
    const MatchingResult& hungarian_result,
    double hungarian_time)
{

    AlgorithmComparison comparison(matrix.n);
    comparison.hungarian = hungarian_result;
    comparison.hungarianTime = hungarian_time;
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
    BruteForceSolver bruteSolver;
    auto brute_start = chrono::high_resolution_clock::now();
    MatchingResult bruteResult = bruteSolver.solve(matrix);
    auto brute_end = chrono::high_resolution_clock::now();

    comparison.bruteForceTime = chrono::duration_cast<chrono::milliseconds>(
        brute_end - brute_start)
                                    .count();
    comparison.bruteForce = bruteResult;

    // 比较结果
    comparison.scoreDifference = bruteResult.totalScore - hungarian_result.totalScore;
    comparison.isOptimal = (comparison.scoreDifference == 0);

    // 显示对比结果
    comparison.displayComparison();

    return comparison;
}

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
