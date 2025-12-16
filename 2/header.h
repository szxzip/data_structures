#ifndef HEADER_H
#define HEADER_H

#include <algorithm>
#include <climits>
#include <fstream>
#include <gtk/gtk.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// 矩阵数据结构（B）
class SatisfactionMatrix {
public:
    vector<vector<int>> mf; // 男对女满意度矩阵 (n x n)
    vector<vector<int>> fm; // 女对男满意度矩阵 (n x n)
    int n; // 运动员数量

    SatisfactionMatrix(int size = 0); // 构造函数
    void resize(int size); // 调整矩阵大小
    void randomGenerate(int maxScore = 100); // 随机生成矩阵
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
    vector<vector<int>> cost; // 成本矩阵（用于最小权匹配）
    vector<int> lx, ly; // 顶标（X部和Y部顶点的标签值）
    vector<int> matchX, matchY; // 匹配结果 matchX[i]=j表示男i匹配女j
    vector<bool> visitedX, visitedY; // DFS搜索时的访问标记
    vector<int> slack; // 松弛量，用于顶标调整
    int n; // 问题规模
    bool dfs(int u); // DFS深度优先搜索，在相等子图中寻找增广路
    void updateLabels(); // 更新顶标函数，当找不到增广路时调整顶标

public:
    HungarianAlgorithm(); // 构造函数
    MatchingResult solve(const SatisfactionMatrix& matrix); // 计算配对结果
};

// GUI (C)
extern GtkWidget* male_grid; // 男对女矩阵显示网格
extern GtkWidget* female_grid; // 女对男矩阵显示网格
extern GtkWidget* result_textview; // 结果显示文本框
extern SatisfactionMatrix current_matrix; // 当前处理的矩阵数据
extern MatchingResult current_result; // 当前匹配结果
extern GtkWidget* n_spinner; // 运动员数量选择器

/**
 * 读写文件 (C)
 * 文件格式：
 * 第一行：n（运动员数量）
 * n行：男对女矩阵（n×n）
 * n行：女对男矩阵（n×n）
 */
bool loadMatrixFromFile(const string& filename, SatisfactionMatrix& matrix);
void saveMatrixToFile(const string& filename, const SatisfactionMatrix& matrix);

void activate(GtkApplication* app, gpointer user_data); // GTK应用程序激活回调函数
void on_generate_clicked(GtkWidget* widget, gpointer data); // "生成随机矩阵"按钮回调函数
void on_solve_clicked(GtkWidget* widget, gpointer data); // "求解最佳组合"按钮回调函数
void on_load_clicked(GtkWidget* widget, gpointer data); // "加载文件"按钮回调函数
void on_save_clicked(GtkWidget* widget, gpointer data); // "保存文件"按钮回调函数
void display_matrix_in_grid(GtkWidget* grid, const vector<vector<int>>& matrix, int start_row, const string& title);
void update_result_display(GtkWidget* textview, const MatchingResult& result); // 更新结果显示文本框

#endif // HEADER_H
