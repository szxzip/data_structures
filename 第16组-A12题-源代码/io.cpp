#include "header.h"

// 随机创建图
Graph* createRandom(int vertices, double density)
{
    Graph* graph = new Graph(vertices);

    // 生成连通图（最小密度）
    for (int i = 1; i < vertices; i++) { // 遍历顶点（除了首个顶点）
        int parent = rand() % i; // 每个顶点与后方顶点随机连一条线
        graph->addEdge(i, parent);
    }

    // 根据密度添加额外边
    int maxEdges = vertices * (vertices - 1) / 2; // 最大可能边数
    int targetEdges = static_cast<int>(density * maxEdges);
    int currentEdges = vertices - 1; // 最小可能边数，也是当前边数

    while (currentEdges < targetEdges) {
        int x = rand() % vertices;
        int y = rand() % vertices;

        if (x != y && !graph->edgeExists(x, y)) { // 避免自环、重复边
            graph->addEdge(x, y);
            currentEdges++;
        }
    }

    cout << "随机创建图：完成！" << endl;
    return graph;
}

// 从文件读入图
Graph* createFromFile(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << filename << endl; // 错误输出到命令行
        return nullptr; // 空指针
    }

    int V = 0; // 顶点数
    file >> V; // 若开头不合规，则之后的判定会拒绝通过

    // 顶点数 3-20
    if (V < 3 || V > 20) {
        cerr << "错误：无效的顶点数！" << endl;
        file.close();
        return nullptr;
    }

    Graph* graph = new Graph(V);

    int lineNumber = 1; // 当前行号
    string line;
    getline(file, line); // 清除第一行换行符（和多余字符，如果有的话）

    // 读入边
    while (getline(file, line)) {
        lineNumber++;

        // 使用字符串流解析行
        istringstream iss(line);
        vector<int> values;
        int value;

        // 写入临时数组（仅能写入整数）
        while (iss >> value) {
            values.push_back(value);
        }

        // 检查每行元素数量，跳过多数/少数行、空行
        if (values.size() != 2) {
            cerr << "警告：第 " << lineNumber << " 行格式不正确！已跳过此行。" << endl;
            continue;
        }

        int x = values[0];
        int y = values[1];

        // 检查顶点编号是否有效
        if (x < 0 || x >= V || y < 0 || y >= V) {
            cerr << "警告：第 " << lineNumber << " 行包含非法顶点编号！已跳过此行。" << endl;
            continue;
        }

        if (x == y) {
            cerr << "警告：第 " << lineNumber << " 行是自环！已跳过此行。" << endl;
            continue;
        }

        graph->addEdge(x, y);
    }

    file.close();
    cout << "从文件读入图：完成！" << endl;
    return graph;
}

// 写入图到文件
bool saveToFile(const Graph& graph, const string& filename)
{
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "错误：无法创建文件 " << filename << endl;
        return false;
    }

    // 写入顶点
    int V = graph.getVertexCount();
    file << V << endl;

    // 写入边
    for (int i = 0; i < V; i++) { // 遍历顶点
        const vector<int>& neighbors = graph.getAdjacencyList()[i]; // 读取该点邻接表
        for (size_t j = 0; j < neighbors.size(); j++) { // 遍历相邻顶点
            int neighbor = neighbors[j]; // 相邻顶点
            if (i < neighbor) { // 顶点小于相邻顶点，避免重复
                file << i << " " << neighbor << endl;
            }
        }
    }

    file.close();
    cout << "写入图到文件：完成！" << endl;
    return true;
}
