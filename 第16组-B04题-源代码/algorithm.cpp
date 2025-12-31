#include "header.h"

HungarianAlgorithm::HungarianAlgorithm()
    : n(0)
{
    // 构造函数初始化
    cout << "HungarianAlgorithm 构造函数：完成！" << endl;
}

bool HungarianAlgorithm::dfs(int u)
{

    visitedX[u] = true; // 表示该男选手已访问

    for (int w = 0; w < n; w++) {
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
                cout << "深度优先搜索：完成！" << endl;
            }
        } else {
            slack[w] = min(slack[w], gap); // 更新这位女选手的松弛量，可以理解为还差多少期望可以找到一个搭档
        }
    }

    cout << "深度优先搜索：失败！" << endl;
    return false;
}

MatchingResult HungarianAlgorithm::solve(const SatisfactionMatrix& matrix)
{

    // 处理n=0边界情况
    if (matrix.n == 0) {
        MatchingResult result(0);
        return result;
    }

    n = matrix.n; // 问题规模为n人

    total.resize(n, vector<int>(n, 0)); // 构建满意度矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // 总满意度 = 男对女的满意度 + 女对男的满意度
            total[i][j] = matrix.mf[i][j] + matrix.fm[j][i];
        }
    }

    ex.resize(n, 0); // 初始化期望值数组
    ey.resize(n, 0);
    matchX.resize(n, -1); // 初始化匹配数组
    matchY.resize(n, -1);
    visitedX.resize(n, false);
    visitedY.resize(n, false);
    slack.resize(n, 0);

    for (int m = 0; m < n; m++) { // 设置男选手的期望值，等于他与最合适的女选手之间的期望和
        for (int w = 0; w < n; w++) {
            ex[m] = max(ex[m], total[m][w]);
        }
    }

    // 为每一个男选手寻找搭档
    for (int u = 0; u < n; u++) {

        // 每次为新的男选手寻找匹配时，重置所有女选手松弛量为最大值,因为后面要找最小
        for (int v = 0; v < n; v++) {
            slack[v] = 100;
        }

        while (1) { // 进入循环，直到匹配完成
            for (int i = 0; i < n; i++) { // 对于每一个男性选手的匹配过程都要重置两边选手的访问标记
                visitedX[i] = false;
                visitedY[i] = false;
            }

            if (dfs(u)) { // 找到搭档就可以退出循环了
                break;
            }
            // 没有找到的话，需要调整期望值
            int d = 100; // d表示需要调整的期望大小
            for (int v = 0; v < n; v++) { // 找最小松弛量
                if (!visitedY[v]) { // 从未访问的女选手中找最小slack作为d值
                    d = min(d, slack[v]);
                }
            }

            for (int j = 0; j < n; j++) {
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
    MatchingResult result(n);

    for (int i = 0; i < n; i++) {
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
