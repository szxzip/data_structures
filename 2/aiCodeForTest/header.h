#include <algorithm>
#include <climits>
#include <cmath>
#include <fstream>
#include <gtk/gtk.h>
#include <iomanip>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// 定义矩阵类型
using Matrix = std::vector<std::vector<int>>;

// AppData结构体
typedef struct {
    GtkWidget* window;
    GtkWidget* notebook;
    GtkWidget* spin_n;
    GtkWidget** mf_entries;
    GtkWidget** fm_entries;
    GtkWidget* result_text;
    int n;
} AppData;

// 矩阵工具类
class MatrixUtils {
public:
    static Matrix createMatrix(int n, int defaultValue = 0);
    static Matrix addMatrices(const Matrix& m1, const Matrix& m2);
    static int calculateTotalSatisfaction(const Matrix& combined,
        const std::vector<int>& matching);
    static void printMatrix(const Matrix& matrix);
    static std::string matrixToString(const Matrix& matrix);
    static bool isValidMatrix(const Matrix& matrix, int n);
};

// 匹配算法类
class MatchingAlgorithms {
public:
    // 匈牙利算法求解最大权重匹配
    static std::pair<std::vector<int>, int> hungarianAlgorithm(const Matrix& costMatrix);

    // 暴力搜索（用于小规模验证）
    static std::pair<std::vector<int>, int> bruteForceMatching(const Matrix& combinedMatrix);

private:
    // 匈牙利算法辅助函数
    static void step1(Matrix& matrix, int n);
    static void step2(const Matrix& matrix, Matrix& mask,
        std::vector<bool>& rowCover, std::vector<bool>& colCover,
        int n);
    static int step3(const Matrix& mask, std::vector<bool>& colCover, int n);
    static void step4(Matrix& matrix, Matrix& mask,
        std::vector<bool>& rowCover, std::vector<bool>& colCover,
        int path_row_0, int path_col_0, int n);
    static void step5(Matrix& mask, std::vector<bool>& rowCover,
        std::vector<bool>& colCover, int n);

    // 寻找零元素
    static std::pair<int, int> findZero(int row, int col, const Matrix& matrix,
        const std::vector<bool>& rowCover,
        const std::vector<bool>& colCover, int n);
};

// 文件IO类
class FileIO {
public:
    static bool readMatrixFromFile(const std::string& filename, Matrix& matrix, int n);
    static bool saveResultToFile(const std::string& filename,
        const std::vector<int>& matching,
        int totalSatisfaction);
};

// 验证类
class Validation {
public:
    static bool validateMatrixInput(const std::string& input, int& value);
    static bool validateN(const std::string& input, int& n);
};

// GUI函数声明
GtkWidget* create_main_window(AppData* app_data);
void on_calculate_clicked(GtkButton* button, gpointer user_data);
void on_n_changed(GtkSpinButton* spin, gpointer user_data);
void create_matrix_entries(AppData* app_data, int n);
void clear_matrix_entries(AppData* app_data);
GtkWidget* create_matrix_page(const char* title, AppData* app_data,
    GtkWidget*** entries);
