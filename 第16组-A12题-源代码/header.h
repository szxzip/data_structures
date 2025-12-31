#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <gtk/gtk.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Graph {
private:
    int V; // 顶点数
    vector<vector<int>> adj; // 邻接表
    vector<bool> visited; // 是否发现
    vector<int> disc; // 发现时间（时间戳）
    vector<int> low; // 最小非父可达节点
    vector<int> parent; // 父节点
    vector<bool> articulationPoints; // 关节点
    int time; // DFS时间计数器

    // B
    void DFS(int u); // 深度优先遍历

public:
    // A
    Graph(int vertices); // 创建指定顶点数的图
    void addEdge(int u, int v); // 添加边
    void removeEdge(int u, int v); // 删除边
    bool edgeExists(int u, int v) const; // 检查边是否存在

    int getVertexCount() const { return V; } // 获取顶点数
    int getEdgeCount() const; // 获取边数
    const vector<vector<int>>& getAdjacencyList() const { return adj; } // 获取邻接表
    void printGraph() const; // 打印图结构

    // B
    void findArticulationPoints(); // 查找并标记关节点
    vector<int> getArticulationPoints() const; // 获取关节点列表
    int countArticulationPoints() const; // 统计关节点数量
    bool isArticulationPoint(int vertex) const; // 判断是否为关节点

    // A
    bool convertToNonArticulation(int vertex); // 改造关节点为非关节点
};

Graph* createRandom(int vertices, double density = 0.3); // 随机创建

/* *
 * 读写文件 (C)
 *
 * sample.txt:
 * 顶点数
 * 顶点 顶点 （即一条边）
 * 顶点 顶点
 * ...
 */
Graph* createFromFile(const string& filename);
bool saveToFile(const Graph& graph, const string& filename);

// GUI
void update_graph_display(); // 更新图显示
bool draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data); // 绘图
void create_new_graph(GtkWidget* widget, gpointer data); // 创建新图
void load_graph_from_file(GtkWidget* widget, gpointer data); // 从文件加载图
void save_graph_to_file(GtkWidget* widget, gpointer data); // 保存图到文件
void find_articulation_points(GtkWidget* widget, gpointer data); // 查找关节点
void count_articulation_points(GtkWidget* widget, gpointer data); // 统计关节点
void convert_articulation_point(GtkWidget* widget, gpointer data); // 改造关节点
void initialize_gui(int argc, char** argv); // 初始化GUI

// 模板函数，可参考

template <typename T>
T my_min(T a, T b)
{
    return a < b ? a : b;
}

template <typename T>
int my_find(const vector<T>& vec, const T& value)
{
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] == value)
            return i;
    }
    return -1;
}
