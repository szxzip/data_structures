#include <cmath>
#include <cstring>
#include <ctime> // 计时
#include <fstream>
#include <gtk/gtk.h>
#include <iostream>
#include <sstream> // 字符串流
#include <string>
#include <vector>

// 图类定义
class Graph {
private:
    int V; // 顶点数
    std::vector<std::vector<int>> adj; // 邻接表
    std::vector<bool> visited; // 是否发现
    std::vector<int> disc; // 发现时间（时间戳）
    std::vector<int> low; // 最小非父可达节点
    std::vector<int> parent; // 父节点
    std::vector<bool> articulationPoints; // 关节点
    int time; // DFS时间计数器

    // B
    void DFS(int u); // 深度优先遍历

public:
    Graph(int vertices); // 创建指定顶点数的图

    // A
    void addEdge(int u, int v); // 添加边
    void removeEdge(int u, int v); // 删除边
    bool edgeExists(int u, int v) const; // 检查边是否存在
    int getVertexCount() const { return V; } // 获取顶点数
    int getEdgeCount() const; // 获取边数
    void printGraph() const; // 打印图结构
    bool convertToNonArticulation(int vertex); // 改造关节点为非关节点

    // B
    void findArticulationPoints(); // 查找所有关节点
    std::vector<int> getArticulationPoints() const; // 获取关节点列表
    int countArticulationPoints() const; // 统计关节点数量
    bool isArticulationPoint(int vertex) const; // 判断是否为关节点

    // 辅助函数
    const std::vector<std::vector<int>>& getAdjacencyList() const { return adj; } // 获取邻接表
    void clear(); // 清除图
    void resize(int vertices); // 重置图大小
};

// 图输入/输出类
class GraphIO {
public:
    // C
    static Graph* createFromFile(const std::string& filename); // 从文件读入
    static Graph* createRandom(int vertices, double density = 0.3); // 随机创建
    static bool saveToFile(const Graph& graph, const std::string& filename); // 写入文件
};

// 模板函数，可参考

inline int randomInt(int min, int max)
{
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
    return min + (std::rand() % (max - min + 1));
}

inline double randomDouble()
{
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
    return static_cast<double>(std::rand()) / RAND_MAX;
}

template <typename T>
T min(T a, T b)
{
    return a < b ? a : b;
}

template <typename T>
int find(const std::vector<T>& vec, const T& value)
{
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i] == value)
            return i;
    }
    return -1;
}
