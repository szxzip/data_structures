#include "header.h"

// 从文件创建图
Graph* GraphIO::createFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return nullptr;
    }

    int V, E;
    file >> V >> E;

    Graph* graph = new Graph(V);

    // 读取、添加边
    for (int i = 0; i < E; i++) {
        int u, v;
        if (!(file >> u >> v)) {
            std::cerr << "错误：第 " << i + 1 << " 条边读取失败，检查文件是否规范！" << std::endl;
            file.close();
            delete graph;
            return nullptr;
        }
        graph->addEdge(u, v);
    }

    file.close();
    return graph;
}

// 随机创建图
Graph* GraphIO::createRandom(int vertices, double density)
{
    Graph* graph = new Graph(vertices);

    // 确保图连通（生成一棵生成树）
    for (int i = 1; i < vertices; i++) {
        int parent = randomInt(0, i - 1);
        graph->addEdge(i, parent);
    }

    // 根据密度添加额外边
    int maxEdges = vertices * (vertices - 1) / 2;
    int targetEdges = static_cast<int>(density * maxEdges);
    int currentEdges = vertices - 1;

    while (currentEdges < targetEdges) {
        int u = randomInt(0, vertices - 1);
        int v = randomInt(0, vertices - 1);

        if (u != v && !graph->edgeExists(u, v)) {
            graph->addEdge(u, v);
            currentEdges++;
        }
    }

    return graph;
}

// 保存图到文件
bool GraphIO::saveToFile(const Graph& graph, const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    int V = graph.getVertexCount();
    int E = graph.getEdgeCount();

    file << V << " " << E << std::endl;

    // 保存边
    for (int i = 0; i < V; i++) {
        const std::vector<int>& neighbors = graph.getAdjacencyList()[i];
        for (size_t j = 0; j < neighbors.size(); j++) {
            int neighbor = neighbors[j];
            if (i < neighbor) { // 避免重复保存
                file << i << " " << neighbor << std::endl;
            }
        }
    }

    file.close();
    return true;
}

// 工具函数实现
int randomInt(int min, int max)
{
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
    return min + (std::rand() % (max - min + 1));
}

double randomDouble()
{
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
    return static_cast<double>(std::rand()) / RAND_MAX;
}
