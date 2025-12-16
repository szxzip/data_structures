#include "header.h"

// 从文件读入图
Graph* GraphIO::createFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl; // 标准错误输出流
        return nullptr; // 空指针
    }

    int V, E; // 顶点数、边数
    file >> V >> E;

    Graph* graph = new Graph(V);

    // 读取、添加边
    for (int i = 0; i < E; i++) { // 边数超出，则抛弃多余行
        int u, v;
        if (!(file >> u >> v)) { // 检查是否少行
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
        int parent = Utils::randomInt(0, i - 1);
        graph->addEdge(i, parent);
    }

    // 根据密度添加额外边
    int maxEdges = vertices * (vertices - 1) / 2;
    int targetEdges = static_cast<int>(density * maxEdges);
    int currentEdges = vertices - 1;

    while (currentEdges < targetEdges) {
        int u = Utils::randomInt(0, vertices - 1);
        int v = Utils::randomInt(0, vertices - 1);

        if (u != v && !graph->edgeExists(u, v)) {
            graph->addEdge(u, v);
            currentEdges++;
        }
    }

    return graph;
}

// 写入图到文件
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
        const std::vector<int>& neighbors = graph.getAdjacencyList()[i]; // 将该点邻接表存入向量
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
