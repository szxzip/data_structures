#include "header.h"

/**
 * @brief HungarianAlgorithm构造函数
 */
HungarianAlgorithm::HungarianAlgorithm() { }

/**
 * @brief DFS函数 - 寻找增广路
 */
bool HungarianAlgorithm::dfs(int u)
{
    visitedX[u] = true;

    for (int v = 0; v < n; v++) {
        if (!visitedY[v]) {
            int gap = lx[u] + ly[v] - cost[u][v];

            if (gap == 0) { // 在相等子图中
                visitedY[v] = true;

                if (matchY[v] == -1 || dfs(matchY[v])) {
                    matchX[u] = v;
                    matchY[v] = u;
                    return true;
                }
            } else {
                slack[v] = min(slack[v], gap);
            }
        }
    }

    return false;
}

/**
 * @brief 更新顶标函数
 */
void HungarianAlgorithm::updateLabels()
{
    int delta = INT_MAX;

    // 计算最小slack值
    for (int v = 0; v < n; v++) {
        if (!visitedY[v]) {
            delta = min(delta, slack[v]);
        }
    }

    // 更新X部顶标
    for (int u = 0; u < n; u++) {
        if (visitedX[u]) {
            lx[u] -= delta;
        }
    }

    // 更新Y部顶标
    for (int v = 0; v < n; v++) {
        if (visitedY[v]) {
            ly[v] += delta;
        }
    }
}

/**
 * @brief 解决最大权匹配问题
 */
MatchingResult HungarianAlgorithm::solve(const SatisfactionMatrix& matrix)
{
    n = matrix.n;
    if (n == 0)
        return MatchingResult(0);

    // 构建总满意度矩阵
    vector<vector<int>> totalMatrix(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            totalMatrix[i][j] = matrix.mf[i][j] + matrix.fm[j][i];
        }
    }

    // 转换为最小权匹配问题
    cost = totalMatrix;
    int maxWeight = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            maxWeight = max(maxWeight, totalMatrix[i][j]);
        }
    }

    // 转换为最小权匹配问题
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cost[i][j] = maxWeight - totalMatrix[i][j];
        }
    }

    // 初始化顶标
    lx.assign(n, 0);
    ly.assign(n, 0);
    matchX.assign(n, -1);
    matchY.assign(n, -1);

    // 初始化顶标：lx[i] = max(cost[i][j])
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            lx[i] = max(lx[i], cost[i][j]);
        }
    }

    // 匈牙利算法主循环
    for (int u = 0; u < n; u++) {
        slack.assign(n, INT_MAX);

        while (true) {
            visitedX.assign(n, false);
            visitedY.assign(n, false);

            if (dfs(u)) {
                break; // 找到增广路
            }

            // 更新顶标，扩展相等子图
            updateLabels();
        }
    }

    // 构建匹配结果
    MatchingResult result(n);
    result.totalScore = 0;
    result.maleScore = 0;
    result.femaleScore = 0;

    for (int i = 0; i < n; i++) {
        if (matchX[i] != -1) {
            int j = matchX[i];
            result.pairs[i] = j;
            result.totalScore += totalMatrix[i][j];
            result.maleScore += matrix.mf[i][j];
            result.femaleScore += matrix.fm[j][i];
        }
    }

    return result;
}
