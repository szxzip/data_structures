#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// 图类定义
class Graph {
private:
    int V; // 顶点数
    std::vector<std::vector<int>> adj; // 邻接表
    std::vector<bool> visited; // 是否发现
    std::vector<int> disc; // 发现时间
    std::vector<int> low; // 最早可访问的祖先
    std::vector<int> parent; // 父节点
    std::vector<bool> articulationPoints; // 关节点
    int time; // DFS时间计数器

    void DFS(int u); // 深度优先遍历（私有辅助函数）

public:
    Graph(int vertices); // 创建指定顶点数的图

    void addEdge(int u, int v); // 添加边
    void removeEdge(int u, int v); // 删除边
    bool edgeExists(int u, int v) const; // 检查边是否存在

    void findArticulationPoints(); // 查找所有关节点
    std::vector<int> getArticulationPoints() const; // 获取关节点列表
    int countArticulationPoints() const; // 统计关节点数量
    bool isArticulationPoint(int vertex) const; // 判断是否为关节点
    bool convertToNonArticulation(int vertex); // 改造关节点为非关节点

    int getVertexCount() const { return V; } // 获取顶点数
    int getEdgeCount() const; // 获取边数
    void printGraph() const; // 打印图结构
    const std::vector<std::vector<int>>& getAdjacencyList() const { return adj; }

    // 清除图
    void clear()
    {
        adj.clear();
        V = 0;
        articulationPoints.clear();
    }

    // 重置图大小
    void resize(int vertices)
    {
        V = vertices;
        adj.resize(V);
        articulationPoints.resize(V, false);
    }
};

// 图输入/输出类
class GraphIO {
public:
    static std::unique_ptr<Graph> createFromFile(const std::string& filename);
    static std::unique_ptr<Graph> createRandom(int vertices, double density = 0.3);
    static bool saveToFile(const Graph& graph, const std::string& filename);
};

// 函数声明，用于在main.cpp中调用
void update_graph_display();
