#include "header.h"

// ============================================
// 暴力搜索算法实现
// ============================================

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
MatchingResult BruteForceSolver::solve(const SatisfactionMatrix& matrix)
{
    auto start_time = chrono::high_resolution_clock::now();

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

    // 记录结束时间
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    cout << "[暴力搜索] 完成！检查了 " << permutationCount << " 种排列" << endl;
    cout << "[暴力搜索] 用时: " << duration.count() << " ms" << endl;
    cout << "[暴力搜索] 最优总满意度: " << bestResult.totalScore << endl;

    return bestResult;
}

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
    if (n == 0) {
        cout << "[匈牙利算法] 空矩阵，返回空结果" << endl;
        return MatchingResult(0);
    }

    cout << "\n[匈牙利算法] ====== 开始求解 ======" << endl;
    cout << "[匈牙利算法] 问题规模: n = " << n << endl;

    // ====== 步骤1：构建总满意度矩阵 ======
    vector<vector<int>> totalMatrix(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            totalMatrix[i][j] = matrix.mf[i][j] + matrix.fm[j][i];
        }
    }

    // 显示总满意度矩阵（调试用）
    cout << "[匈牙利算法] 总满意度矩阵:" << endl;
    for (int i = 0; i < n; i++) {
        cout << "  ";
        for (int j = 0; j < n; j++) {
            cout << setw(4) << totalMatrix[i][j] << " ";
        }
        cout << endl;
    }

    // ====== 步骤2：转换为最小权匹配问题 ======
    // 匈牙利算法解决最小权匹配，我们需要将最大权转换为最小权
    cost = totalMatrix;

    // 找到总满意度矩阵中的最大值
    int maxWeight = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (totalMatrix[i][j] > maxWeight) {
                maxWeight = totalMatrix[i][j];
            }
        }
    }
    cout << "[匈牙利算法] 最大权重值: " << maxWeight << endl;

    // 转换公式：cost[i][j] = maxWeight - totalMatrix[i][j]
    // 这样最大值变为0，最小值变为正数，求最小权匹配
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cost[i][j] = maxWeight - totalMatrix[i][j];
        }
    }

    // 显示成本矩阵（调试用）
    cout << "[匈牙利算法] 转换后的成本矩阵 (最大权-" << maxWeight << "):" << endl;
    for (int i = 0; i < n; i++) {
        cout << "  ";
        for (int j = 0; j < n; j++) {
            cout << setw(4) << cost[i][j] << " ";
        }
        cout << endl;
    }

    // ====== 步骤3：初始化匈牙利算法数据结构 ======
    lx.assign(n, 0); // X部顶标
    ly.assign(n, 0); // Y部顶标
    matchX.assign(n, -1); // X部匹配结果，matchX[i]=j表示男i匹配女j
    matchY.assign(n, -1); // Y部匹配结果，matchY[j]=i表示女j匹配男i
    visitedX.assign(n, false); // X部访问标记
    visitedY.assign(n, false); // Y部访问标记
    slack.assign(n, 0); // 松弛量

    // 初始化顶标：lx[i] = max(cost[i][j])，即行最大值
    for (int i = 0; i < n; i++) {
        lx[i] = *max_element(cost[i].begin(), cost[i].end());
    }

    cout << "[匈牙利算法] 初始化顶标 lx: ";
    for (int i = 0; i < n; i++)
        cout << lx[i] << " ";
    cout << endl;

    // ====== 步骤4：匈牙利算法主循环 ======
    cout << "[匈牙利算法] 开始主循环，为每个X部顶点寻找匹配..." << endl;

    for (int u = 0; u < n; u++) {
        if (u > 0) {
            cout << "[匈牙利算法] 处理男运动员 " << u + 1 << "..." << endl;
        }

        // 初始化slack为无穷大
        slack.assign(n, INT_MAX);

        int attempt = 0;
        while (true) {
            attempt++;
            visitedX.assign(n, false);
            visitedY.assign(n, false);

            // 使用DFS在相等子图中寻找增广路
            if (dfs(u)) {
                // 找到增广路，成功匹配
                if (u > 0 && attempt > 1) {
                    cout << "  [迭代 " << attempt << "] 找到增广路，匹配成功" << endl;
                }
                break;
            }

            // 如果没有找到增广路，需要调整顶标
            if (attempt == 1) {
                cout << "  [迭代 " << attempt << "] 未找到增广路，调整顶标..." << endl;
            }

            updateLabels();
        }
    }

    // ====== 步骤5：输出匹配结果和调试信息 ======
    cout << "[匈牙利算法] 算法完成，匹配结果如下:" << endl;
    cout << "  男运动员 | 匹配的女运动员" << endl;
    cout << "  --------|--------------" << endl;

    for (int i = 0; i < n; i++) {
        if (matchX[i] != -1) {
            cout << "    男" << setw(2) << i + 1 << "   ->   女"
                 << setw(2) << matchX[i] + 1
                 << "   (原满意度: " << totalMatrix[i][matchX[i]]
                 << " = " << matrix.mf[i][matchX[i]] << "(男) + "
                 << matrix.fm[matchX[i]][i] << "(女))" << endl;
        } else {
            cout << "    男" << setw(2) << i + 1 << "   ->   未匹配" << endl;
        }
    }

    // ====== 步骤6：构建返回结果 ======
    MatchingResult result(n);
    result.totalScore = 0;
    result.maleScore = 0;
    result.femaleScore = 0;

    for (int i = 0; i < n; i++) {
        if (matchX[i] != -1) {
            int j = matchX[i];
            result.pairs[i] = j;
            result.totalScore += totalMatrix[i][j]; // 总满意度
            result.maleScore += matrix.mf[i][j]; // 男方满意度
            result.femaleScore += matrix.fm[j][i]; // 女方满意度
        }
    }

    // ====== 步骤7：验证匹配的完整性 ======
    bool completeMatch = true;
    for (int i = 0; i < n; i++) {
        if (matchX[i] == -1) {
            completeMatch = false;
            cout << "[警告] 男运动员 " << i + 1 << " 未匹配！" << endl;
        }
    }

    if (completeMatch) {
        cout << "[匈牙利算法] ✓ 找到完美匹配（所有人都有搭档）" << endl;
    } else {
        cout << "[匈牙利算法] ✗ 未找到完美匹配" << endl;
    }

    // 检查一一对应关系
    vector<bool> femaleUsed(n, false);
    bool oneToOne = true;
    for (int i = 0; i < n; i++) {
        if (matchX[i] != -1) {
            int j = matchX[i];
            if (j < 0 || j >= n) {
                cout << "[错误] 无效的女运动员索引: " << j << endl;
                oneToOne = false;
            } else if (femaleUsed[j]) {
                cout << "[错误] 女运动员 " << j + 1 << " 被重复匹配！" << endl;
                oneToOne = false;
            } else {
                femaleUsed[j] = true;
            }
        }
    }

    // 检查是否有女运动员未匹配
    for (int j = 0; j < n; j++) {
        if (!femaleUsed[j] && matchY[j] == -1) {
            cout << "[警告] 女运动员 " << j + 1 << " 未匹配！" << endl;
        }
    }

    if (oneToOne) {
        cout << "[匈牙利算法] ✓ 匹配满足一一对应关系" << endl;
    }

    // ====== 步骤8：输出最终统计信息 ======
    cout << "[匈牙利算法] ====== 求解完成 ======" << endl;
    cout << "[匈牙利算法] 总满意度: " << result.totalScore << endl;
    cout << "[匈牙利算法] 男方满意度: " << result.maleScore << endl;
    cout << "[匈牙利算法] 女方满意度: " << result.femaleScore << endl;
    cout << "[匈牙利算法] 平均每人满意度: "
         << (n > 0 ? result.totalScore / (2.0 * n) : 0) << endl;

    // 计算匹配方案的方差（衡量公平性）
    if (n > 0) {
        double avgPerPair = result.totalScore / (double)n;
        double variance = 0.0;
        for (int i = 0; i < n; i++) {
            if (matchX[i] != -1) {
                double diff = totalMatrix[i][matchX[i]] - avgPerPair;
                variance += diff * diff;
            }
        }
        variance /= n;
        cout << "[匈牙利算法] 匹配方案方差: " << variance
             << " (值越小越公平)" << endl;
    }

    return result;
}
