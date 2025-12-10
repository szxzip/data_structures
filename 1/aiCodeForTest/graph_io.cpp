#include "header.h"

std::unique_ptr<Graph> GraphIO::createFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return nullptr;
    }

    int V, E;
    file >> V >> E;

    auto graph = std::make_unique<Graph>(V);

    for (int i = 0; i < E; i++) {
        int u, v;
        file >> u >> v;
        graph->addEdge(u, v);
    }

    file.close();
    return graph;
}

std::unique_ptr<Graph> GraphIO::createRandom(int vertices, double density)
{
    auto graph = std::make_unique<Graph>(vertices);

    std::mt19937 rng(time(nullptr));
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // 确保图连通（生成一棵生成树）
    for (int i = 1; i < vertices; i++) {
        std::uniform_int_distribution<int> parentDist(0, i - 1);
        int parent = parentDist(rng);
        graph->addEdge(i, parent);
    }

    // 根据密度添加额外边
    int maxEdges = vertices * (vertices - 1) / 2;
    int targetEdges = static_cast<int>(density * maxEdges);
    int currentEdges = vertices - 1;

    while (currentEdges < targetEdges) {
        int u = rng() % vertices;
        int v = rng() % vertices;

        if (u != v && !graph->edgeExists(u, v)) {
            graph->addEdge(u, v);
            currentEdges++;
        }
    }

    return graph;
}

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
        for (int neighbor : graph.getAdjacencyList()[i]) {
            if (i < neighbor) { // 避免重复保存
                file << i << " " << neighbor << std::endl;
            }
        }
    }

    file.close();
    return true;
}
