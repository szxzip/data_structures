#ifndef HEADER_H
#define HEADER_H

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <gtk/gtk.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

extern bool enable_verification; // 是否启用验证

// 矩阵数据结构（B）
class SatisfactionMatrix {
public:
    // 满意度取值为 0-100
    vector<vector<int>> mf; // 男对女满意度矩阵 (n x n)
    vector<vector<int>> fm; // 女对男满意度矩阵 (n x n)
    int n; // 运动员数量

    SatisfactionMatrix(int size = 0); // 构造函数
    void resize(int size); // 调整矩阵大小
    void randomGenerate(); // 随机生成矩阵
};

// 配对结果 (B)
class MatchingResult {
public:
    vector<int> pairs; // 配对结果，pairs[i]=j表示男i配女j
    int totalScore; // 总满意度（男+女）
    int maleScore; // 男方总满意度
    int femaleScore; // 女方总满意度
    MatchingResult(int size = 0); // 构造函数
};

// 匈牙利算法（A）
class HungarianAlgorithm {
private:
    vector<vector<int>> total; // 总的期望矩阵 就是把男女对彼此的期望相加
    vector<int> ex, ey; // 男女期望
    vector<int> matchX, matchY; // 匹配结果 matchX[i]=j表示男i匹配女j
    vector<bool> visitedX, visitedY; // DFS搜索时的访问标记
    vector<int> slack; // 松弛量，用于调整期望
    int n; // 问题规模
    bool dfs(int u); // DFS深度优先搜索，为某一位男队员匹配，返回成功与否

public:
    HungarianAlgorithm(); // 构造函数
    MatchingResult solve(const SatisfactionMatrix& matrix); // 计算配对结果
};

// 暴力搜索 (C)
MatchingResult* bruteSearch(const SatisfactionMatrix& matrix);

// 算法结果对比 (C)
struct AlgorithmComparison {
    MatchingResult hungarian; // 匈牙利算法结果
    MatchingResult bruteForce; // 暴力搜索结果
    int n; // 问题规模
    bool isOptimal; // 匈牙利算法是否找到最优解
    int scoreDifference; // 分数差值（暴力-匈牙利）

    AlgorithmComparison(int size = 0);
};

AlgorithmComparison* verify_hungarian_result(const SatisfactionMatrix& matrix, const MatchingResult& hungarian_result); // 验证匈牙利算法的结果

/* *
 * 读写文件 (C)
 *
 * sample.txt:
 * 第一行：n（运动员数量）
 * n行：男评价女，男行女列（n×n）
 * n行：女评价男，女行男列（n×n）
 */
bool loadMatrixFromFile(const string& filename, SatisfactionMatrix& matrix);
bool saveMatrixToFile(const string& filename, const SatisfactionMatrix& matrix);

// GUI (C)
void activate(GtkApplication* app, gpointer user_data); // GTK应用程序激活
void on_generate_clicked(GtkWidget* widget, gpointer data); // "生成随机矩阵"按钮
void on_solve_clicked(GtkWidget* widget, gpointer data); // "求解最佳组合"按钮
void on_load_clicked(GtkWidget* widget, gpointer data); // "加载文件"按钮
void on_save_clicked(GtkWidget* widget, gpointer data); // "保存文件"按钮
void display_matrix_in_grid(GtkWidget* grid, const vector<vector<int>>& matrix, int start_row, const string& title); // 在网格中显示矩阵
void update_result_display(GtkWidget* textview, const MatchingResult& result); // 更新结果显示
void update_result_display_with_comparison(GtkWidget* textview, const MatchingResult& result, const AlgorithmComparison& comparison); // 更新结果显示（带对比信息）

#endif // HEADER_H
