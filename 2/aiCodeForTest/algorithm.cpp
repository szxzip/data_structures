#include "header.h"

HungarianAlgorithm::HungarianAlgorithm()
    : N(0)
{
    // 构造函数初始化
}

bool HungarianAlgorithm::dfs(int u)
{

    visitedX[u] = true; // 表示该男选手已访问

    for (int w = 0; w < N; w++) {
        if (visitedY[w]) { // 跳过已经访问过的女选手，防止重复递归
            continue;
        }

        int gap = ex[u] + ey[w] - total[u][w]; // 计算男女选手期望只和与总期望矩阵对应项的差

        if (gap == 0) {
            visitedY[w] = true; // 记录该女选手已经被访问

            if (matchY[w] == -1 || dfs(matchY[w])) { // 如果该女选手还没有匹配搭档或者她现在的搭档可以再另寻一个女选手组队
                matchX[u] = w;
                matchY[w] = u;
                return true;
            }
        } else {
            slack[w] = min(slack[w], gap); // 更新这位女选手的松弛量，可以理解为还差多少期望可以找到一个搭档
        }
    }

    return false;
}

MatchingResult HungarianAlgorithm::solve(const SatisfactionMatrix& matrix)
{

    // 处理n=0边界情况
    if (matrix.n == 0) {
        MatchingResult result(0);
        return result;
    }

    N = matrix.n; // 问题规模为n人

    total.resize(N, vector<int>(N, 0)); // 构建满意度矩阵
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            // 总满意度 = 男对女的满意度 + 女对男的满意度
            total[i][j] = matrix.mf[i][j] + matrix.fm[j][i];
        }
    }

    ex.resize(N, 0); // 初始化期望值数组
    ey.resize(N, 0);
    matchX.resize(N, -1); // 初始化匹配数组
    matchY.resize(N, -1);
    visitedX.resize(N, false);
    visitedY.resize(N, false);
    slack.resize(N, 0);

    for (int m = 0; m < N; m++) { // 设置男选手的期望值，等于他与最合适的女选手之间的期望和
        for (int w = 0; w < N; w++) {
            ex[m] = max(ex[m], total[m][w]);
        }
    }

    // 为每一个男选手寻找搭档
    for (int u = 0; u < N; u++) {

        // 每次为新的男选手寻找匹配时，重置所有女选手松弛量为最大值,因为后面要找最小
        for (int v = 0; v < N; v++) {
            slack[v] = INT_MAX;
        }

        while (1) { // 进入循环，直到匹配完成
            for (int i = 0; i < N; i++) { // 对于每一个男性选手的匹配过程都要重置两边选手的访问标记
                visitedX[i] = false;
                visitedY[i] = false;
            }

            if (dfs(u)) { // 找到搭档就可以退出循环了
                break;
            }
            // 没有找到的话，需要调整期望值
            int d = INT_MAX; // d表示需要调整的期望大小
            for (int v = 0; v < N; v++) { // 找最小松弛量
                if (!visitedY[v]) { // 从未访问的女选手中找最小slack作为d值
                    d = min(d, slack[v]);
                }
            }

            for (int j = 0; j < N; j++) {
                if (visitedX[j]) { // 本轮参与匹配的男选手减少期望值
                    ex[j] -= d;
                }

                if (visitedY[j]) { // 本轮参与匹配的女选手增加期望值
                    ey[j] += d;
                } else { // 本轮未参与的女选手slack降低
                    slack[j] -= d;
                }
            }
        }
    }

    // 上面做完了匹配的任务，下面是计算结果了
    int res = 0; // 分别计算总，男，女满意度
    int mSat = 0;
    int fSat = 0;
    MatchingResult result(N);

    for (int i = 0; i < N; i++) {
        if (matchX[i] == -1 || matchY[i] == -1) // 理论上不会没有匹配，以防万一跳过未被匹配的项
            continue;
        res += total[i][matchX[i]];
        mSat += matrix.mf[i][matchX[i]];
        fSat += matrix.fm[i][matchY[i]];
        result.pairs[i] = matchX[i];
    }
    result.totalScore = res;
    result.maleScore = mSat;
    result.femaleScore = fSat;

    return result;
}

/**
 * @brief 暴力搜索求解最优匹配
 * @param matrix 满意度矩阵
 * @return 匹配结果（保证是全局最优解）
 *
 * 算法流程：
 * 1. 生成女运动员索引的所有排列（0,1,2,...,n-1）
 * 2. 对于每种排列p，表示男i匹配女p[i]
 * 3. 计算该匹配的总满意度
 * 4. 保留最优解
 *
 * 时间复杂度：O(n! * n²)，实际中n≤8时可用
 */

// 暴力搜索
MatchingResult bruteForce(const SatisfactionMatrix& matrix)
{
    int n = matrix.n;

    // 构建总满意度矩阵（与匈牙利算法完全相同）
    vector<vector<int>> totalMatrix(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            totalMatrix[i][j] = matrix.mf[i][j] + matrix.fm[j][i];
        }
    }

    cout << "[暴力搜索] 使用的总满意度矩阵:" << endl;
    for (int i = 0; i < n; i++) {
        cout << "   ";
        for (int j = 0; j < n; j++) {
            cout << setw(4) << totalMatrix[i][j] << " ";
        }
        cout << endl;
    }

    // 初始化最优结果
    MatchingResult bestResult(n);
    bestResult.totalScore = INT_MIN;

    // 生成初始排列
    vector<int> permutation(n);
    for (int i = 0; i < n; i++) {
        permutation[i] = i;
    }

    // 枚举所有排列
    long long permutationCount = 0;
    long long maxPermutations = 1000000; // 最多检查100万种排列

    // 进度显示
    if (n <= 8) {
        cout << "[暴力搜索] 开始搜索 " << tgamma(n + 1) << " 种排列..." << endl;
    } else {
        cout << "[暴力搜索] 警告: n=" << n << " 较大，限制最多检查 "
             << maxPermutations << " 种排列" << endl;
    }

    do {
        // 计算当前排列的总满意度
        int totalScore = 0;
        int maleScore = 0;
        int femaleScore = 0;

        for (int i = 0; i < n; i++) {
            int j = permutation[i];
            maleScore += matrix.mf[i][j];
            femaleScore += matrix.fm[j][i];
            totalScore += totalMatrix[i][j];
        }

        // 更新最优解
        if (totalScore > bestResult.totalScore) {
            bestResult.totalScore = totalScore;
            bestResult.maleScore = maleScore;
            bestResult.femaleScore = femaleScore;
            bestResult.pairs = permutation;
        }

        permutationCount++;

        // 显示进度（每10%显示一次）
        if (n <= 8) {
            long long totalPerms = static_cast<long long>(tgamma(n + 1));
            if (totalPerms > 1000 && permutationCount % (totalPerms / 10) == 0) {
                int progress = (permutationCount * 100) / totalPerms;
                cout << "[暴力搜索] 进度: " << progress << "% ("
                     << permutationCount << "/" << totalPerms << ")" << endl;
            }
        }

        // 安全限制
        if (permutationCount >= maxPermutations) {
            cout << "[暴力搜索] 达到限制，提前终止搜索" << endl;
            break;
        }

    } while (next_permutation(permutation.begin(), permutation.end()));

    cout << "[暴力搜索] 完成！检查了 " << permutationCount << " 种排列" << endl;
    cout << "[暴力搜索] 最优总满意度: " << bestResult.totalScore << endl;

    return bestResult;
}
