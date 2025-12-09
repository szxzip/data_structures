#include <algorithm>
#include <cairo.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <gtk/gtk.h>
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
using namespace std;

// ==============================
// 第一部分：图数据结构和算法模块
// ==============================
class Graph {
private:
    int V;
    vector<vector<int>> adj;

public:
    Graph(int vertices = 0)
        : V(vertices)
    {
        adj.resize(V);
    }

    void init(int vertices)
    {
        V = vertices;
        adj.resize(V);
    }

    void addEdge(int u, int v)
    {
        if (u < V && v < V) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    const vector<vector<int>>& getAdjacencyList() const { return adj; }
    int getVertexCount() const { return V; }
    vector<int> getNeighbors(int v) const
    {
        if (v >= 0 && v < V)
            return adj[v];
        return {};
    }

    int getEdgeCount() const
    {
        int count = 0;
        for (int i = 0; i < V; i++) {
            count += adj[i].size();
        }
        return count / 2;
    }

    void clear()
    {
        V = 0;
        adj.clear();
    }
};

// 关节点查找类
class ArticulationFinder {
private:
    const Graph* graph;
    vector<bool> visited;
    vector<int> disc;
    vector<int> low;
    vector<int> parent;
    vector<bool> isArticulation;
    int time;

    void DFS(int u)
    {
        int children = 0;
        visited[u] = true;
        disc[u] = low[u] = ++time;

        for (int v : graph->getNeighbors(u)) {
            if (!visited[v]) {
                children++;
                parent[v] = u;
                DFS(v);
                low[u] = min(low[u], low[v]);

                if (parent[u] == -1 && children > 1) {
                    isArticulation[u] = true;
                }
                if (parent[u] != -1 && low[v] >= disc[u]) {
                    isArticulation[u] = true;
                }
            } else if (v != parent[u]) {
                low[u] = min(low[u], disc[v]);
            }
        }
    }

public:
    vector<int> findArticulationPoints(const Graph& g)
    {
        graph = &g;
        int V = g.getVertexCount();

        visited.resize(V, false);
        disc.resize(V, -1);
        low.resize(V, -1);
        parent.resize(V, -1);
        isArticulation.resize(V, false);
        time = 0;

        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                DFS(i);
            }
        }

        vector<int> points;
        for (int i = 0; i < V; i++) {
            if (isArticulation[i]) {
                points.push_back(i);
            }
        }
        return points;
    }
};

// 图生成器
class GraphGenerator {
public:
    static Graph generateRandomGraph(int vertices, int edges)
    {
        srand(time(NULL));
        Graph g(vertices);

        // 先确保连通性：生成树
        for (int i = 1; i < vertices; i++) {
            int parent = rand() % i;
            g.addEdge(i, parent);
        }

        // 添加额外边
        for (int i = vertices - 1; i < edges; i++) {
            int u = rand() % vertices;
            int v = rand() % vertices;
            if (u != v) {
                g.addEdge(u, v);
            }
        }
        return g;
    }

    static Graph createExample1()
    {
        Graph g(7);
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(1, 2);
        g.addEdge(1, 3);
        g.addEdge(1, 4);
        g.addEdge(3, 4);
        g.addEdge(3, 5);
        g.addEdge(4, 5);
        g.addEdge(5, 6);
        return g;
    }

    static Graph createExample2()
    {
        Graph g(5);
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(0, 3);
        g.addEdge(1, 2);
        g.addEdge(1, 4);
        g.addEdge(2, 3);
        g.addEdge(3, 4);
        return g;
    }

    static Graph createTree(int vertices)
    {
        Graph g(vertices);
        for (int i = 1; i < vertices; i++) {
            int parent = rand() % i;
            g.addEdge(i, parent);
        }
        return g;
    }
};

// ==============================
// 第二部分：文件操作和数据处理模块
// ==============================
class FileManager {
public:
    static Graph loadFromFile(const string& filename)
    {
        ifstream file(filename);
        if (!file) {
            throw runtime_error("无法打开文件: " + filename);
        }

        int vertices, edges;
        file >> vertices >> edges;

        Graph g(vertices);
        for (int i = 0; i < edges; i++) {
            int u, v;
            file >> u >> v;
            g.addEdge(u, v);
        }

        file.close();
        return g;
    }

    static bool saveToFile(const Graph& g, const string& filename)
    {
        ofstream file(filename);
        if (!file)
            return false;

        int V = g.getVertexCount();
        int edgeCount = 0;
        const auto& adj = g.getAdjacencyList();

        for (int i = 0; i < V; i++) {
            for (int neighbor : adj[i]) {
                if (i < neighbor)
                    edgeCount++;
            }
        }

        file << V << " " << edgeCount << endl;

        for (int i = 0; i < V; i++) {
            for (int neighbor : adj[i]) {
                if (i < neighbor) {
                    file << i << " " << neighbor << endl;
                }
            }
        }

        file.close();
        return true;
    }

    static bool validateGraphFile(const string& filename)
    {
        ifstream file(filename);
        if (!file)
            return false;

        int vertices, edges;
        if (!(file >> vertices >> edges)) {
            file.close();
            return false;
        }

        for (int i = 0; i < edges; i++) {
            int u, v;
            if (!(file >> u >> v)) {
                file.close();
                return false;
            }
            if (u < 0 || u >= vertices || v < 0 || v >= vertices) {
                file.close();
                return false;
            }
        }

        file.close();
        return true;
    }
};

// 图修改器
class GraphModifier {
public:
    static bool removeArticulation(Graph& g, int vertex)
    {
        auto neighbors = g.getNeighbors(vertex);
        if (neighbors.size() < 2)
            return false;

        // 在邻居之间添加边
        bool added = false;
        for (size_t i = 0; i < neighbors.size(); i++) {
            for (size_t j = i + 1; j < neighbors.size(); j++) {
                int u = neighbors[i];
                int v = neighbors[j];

                // 检查边是否已存在
                auto uNeighbors = g.getNeighbors(u);
                bool exists = find(uNeighbors.begin(), uNeighbors.end(), v) != uNeighbors.end();

                if (!exists) {
                    g.addEdge(u, v);
                    added = true;
                }
            }
        }
        return added;
    }
};

// ==============================
// 第三部分：GUI界面模块
// ==============================
struct VertexDisplay {
    int id;
    double x, y;
    bool isArticulation;
    bool isSelected;
};

struct EdgeDisplay {
    int from, to;
};

// 全局变量
Graph currentGraph;
ArticulationFinder finder;
vector<VertexDisplay> verticesDisplay;
vector<EdgeDisplay> edgesDisplay;
vector<int> currentArticulations;

GtkWidget* window;
GtkWidget* drawing_area;
GtkWidget* status_label;
GtkWidget* info_label;
GtkWidget* vertex_count_label;
GtkWidget* edge_count_label;
GtkWidget* articulation_count_label;

bool showArticulations = true;
bool showLabels = true;
int selectedVertex = -1;

// 颜色定义
const double VERTEX_COLOR[3] = { 0.3, 0.5, 0.8 };
const double ARTICULATION_COLOR[3] = { 1.0, 0.3, 0.3 };
const double SELECTED_COLOR[3] = { 0.2, 0.8, 0.2 };
const double EDGE_COLOR[3] = { 0.5, 0.5, 0.5 };

// 布局函数
void layoutVertices()
{
    verticesDisplay.clear();
    edgesDisplay.clear();

    int V = currentGraph.getVertexCount();
    const auto& adj = currentGraph.getAdjacencyList();

    // 圆形布局
    double center_x = 400;
    double center_y = 300;
    double radius = 250;

    for (int i = 0; i < V; i++) {
        VertexDisplay vd;
        vd.id = i;
        double angle = 2 * M_PI * i / V;
        vd.x = center_x + radius * cos(angle);
        vd.y = center_y + radius * sin(angle);
        vd.isArticulation = false;
        vd.isSelected = (i == selectedVertex);
        verticesDisplay.push_back(vd);
    }

    // 构建边
    for (int i = 0; i < V; i++) {
        for (int neighbor : adj[i]) {
            if (i < neighbor) {
                EdgeDisplay ed;
                ed.from = i;
                ed.to = neighbor;
                edgesDisplay.push_back(ed);
            }
        }
    }

    // 标记关节点
    currentArticulations = finder.findArticulationPoints(currentGraph);
    for (int ap : currentArticulations) {
        if (ap < verticesDisplay.size()) {
            verticesDisplay[ap].isArticulation = true;
        }
    }
}

// 绘图回调函数
gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data)
{
    // 背景
    cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
    cairo_paint(cr);

    // 绘制边
    cairo_set_source_rgb(cr, EDGE_COLOR[0], EDGE_COLOR[1], EDGE_COLOR[2]);
    cairo_set_line_width(cr, 2);

    for (const auto& edge : edgesDisplay) {
        if (edge.from < verticesDisplay.size() && edge.to < verticesDisplay.size()) {
            const auto& v1 = verticesDisplay[edge.from];
            const auto& v2 = verticesDisplay[edge.to];
            cairo_move_to(cr, v1.x, v1.y);
            cairo_line_to(cr, v2.x, v2.y);
            cairo_stroke(cr);
        }
    }

    // 绘制顶点
    for (const auto& vd : verticesDisplay) {
        // 选择颜色
        const double* color;
        if (vd.isSelected) {
            color = SELECTED_COLOR;
        } else if (showArticulations && vd.isArticulation) {
            color = ARTICULATION_COLOR;
        } else {
            color = VERTEX_COLOR;
        }

        // 绘制圆形
        cairo_set_source_rgb(cr, color[0], color[1], color[2]);
        cairo_arc(cr, vd.x, vd.y, 20, 0, 2 * M_PI);
        cairo_fill_preserve(cr);

        // 边框
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 2);
        cairo_stroke(cr);

        // 标签
        if (showLabels) {
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr, 14);

            string label = to_string(vd.id);
            cairo_text_extents_t extents;
            cairo_text_extents(cr, label.c_str(), &extents);

            cairo_move_to(cr, vd.x - extents.width / 2, vd.y + extents.height / 2);
            cairo_show_text(cr, label.c_str());
        }
    }

    return FALSE;
}

// 鼠标点击事件
gboolean button_press_callback(GtkWidget* widget, GdkEventButton* event, gpointer data)
{
    if (event->button == GDK_BUTTON_PRIMARY) {
        double click_x = event->x;
        double click_y = event->y;

        // 查找点击的顶点
        selectedVertex = -1;
        for (const auto& vd : verticesDisplay) {
            double dx = vd.x - click_x;
            double dy = vd.y - click_y;
            if (dx * dx + dy * dy < 400) { // 半径20的平方
                selectedVertex = vd.id;
                break;
            }
        }

        // 更新显示
        layoutVertices();
        gtk_widget_queue_draw(drawing_area);

        // 更新信息
        if (selectedVertex >= 0) {
            string info = "顶点 " + to_string(selectedVertex);
            bool isAP = false;
            for (int ap : currentArticulations) {
                if (ap == selectedVertex) {
                    isAP = true;
                    break;
                }
            }
            info += (isAP ? " (关节点)" : " (非关节点)");

            auto neighbors = currentGraph.getNeighbors(selectedVertex);
            info += "\n邻居: ";
            for (size_t i = 0; i < neighbors.size(); i++) {
                info += to_string(neighbors[i]);
                if (i < neighbors.size() - 1)
                    info += ", ";
            }

            gtk_label_set_text(GTK_LABEL(info_label), info.c_str());
        }
    }
    return TRUE;
}

// 更新状态栏
void updateStatus(const string& message)
{
    gtk_label_set_text(GTK_LABEL(status_label), message.c_str());
}

// 更新统计信息
void updateStatistics()
{
    int vertices = currentGraph.getVertexCount();
    int edges = currentGraph.getEdgeCount();
    int articulations = currentArticulations.size();

    string vtext = "顶点数: " + to_string(vertices);
    string etext = "边数: " + to_string(edges);
    string atext = "关节点数: " + to_string(articulations);

    gtk_label_set_text(GTK_LABEL(vertex_count_label), vtext.c_str());
    gtk_label_set_text(GTK_LABEL(edge_count_label), etext.c_str());
    gtk_label_set_text(GTK_LABEL(articulation_count_label), atext.c_str());
}

// 按钮回调函数
void load_example1(GtkWidget* widget, gpointer data)
{
    currentGraph = GraphGenerator::createExample1();
    layoutVertices();
    updateStatus("示例图1已加载");
    updateStatistics();
    gtk_widget_queue_draw(drawing_area);
}

void load_example2(GtkWidget* widget, gpointer data)
{
    currentGraph = GraphGenerator::createExample2();
    layoutVertices();
    updateStatus("示例图2已加载（双连通图）");
    updateStatistics();
    gtk_widget_queue_draw(drawing_area);
}

void generate_random(GtkWidget* widget, gpointer data)
{
    currentGraph = GraphGenerator::generateRandomGraph(8, 12);
    layoutVertices();
    updateStatus("随机图已生成");
    updateStatistics();
    gtk_widget_queue_draw(drawing_area);
}

void find_articulations(GtkWidget* widget, gpointer data)
{
    if (currentGraph.getVertexCount() == 0) {
        updateStatus("请先加载图");
        return;
    }

    currentArticulations = finder.findArticulationPoints(currentGraph);
    layoutVertices();

    string msg = "找到 " + to_string(currentArticulations.size()) + " 个关节点";
    if (!currentArticulations.empty()) {
        msg += ": ";
        for (size_t i = 0; i < currentArticulations.size(); i++) {
            msg += to_string(currentArticulations[i]);
            if (i < currentArticulations.size() - 1)
                msg += ", ";
        }
    }

    updateStatus(msg);
    updateStatistics();
    gtk_widget_queue_draw(drawing_area);
}

void remove_articulation(GtkWidget* widget, gpointer data)
{
    if (selectedVertex < 0) {
        updateStatus("请先选择一个顶点");
        return;
    }

    bool isAP = false;
    for (int ap : currentArticulations) {
        if (ap == selectedVertex) {
            isAP = true;
            break;
        }
    }

    if (!isAP) {
        updateStatus("选中的顶点不是关节点");
        return;
    }

    if (GraphModifier::removeArticulation(currentGraph, selectedVertex)) {
        currentArticulations = finder.findArticulationPoints(currentGraph);
        layoutVertices();
        updateStatus("已尝试移除关节点 " + to_string(selectedVertex));
        updateStatistics();
        gtk_widget_queue_draw(drawing_area);
    } else {
        updateStatus("移除关节点失败");
    }
}

void load_from_file(GtkWidget* widget, gpointer data)
{
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "打开图文件",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_取消", GTK_RESPONSE_CANCEL,
        "_打开", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        try {
            currentGraph = FileManager::loadFromFile(filename);
            layoutVertices();
            updateStatus("文件加载成功: " + string(filename));
            updateStatistics();
            gtk_widget_queue_draw(drawing_area);
        } catch (const exception& e) {
            updateStatus("加载失败: " + string(e.what()));
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void save_to_file(GtkWidget* widget, gpointer data)
{
    if (currentGraph.getVertexCount() == 0) {
        updateStatus("图为空，无法保存");
        return;
    }

    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "保存图文件",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_取消", GTK_RESPONSE_CANCEL,
        "_保存", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (FileManager::saveToFile(currentGraph, filename)) {
            updateStatus("文件保存成功: " + string(filename));
        } else {
            updateStatus("保存失败");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void toggle_show_articulations(GtkWidget* widget, gpointer data)
{
    showArticulations = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    gtk_widget_queue_draw(drawing_area);
}

void toggle_show_labels(GtkWidget* widget, gpointer data)
{
    showLabels = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    gtk_widget_queue_draw(drawing_area);
}

// 创建界面
void create_gui()
{
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "关节点查找系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 主垂直布局
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // 菜单栏
    GtkWidget* menu_bar = gtk_menu_bar_new();
    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* file_item = gtk_menu_item_new_with_label("文件");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);

    GtkWidget* load_item = gtk_menu_item_new_with_label("打开文件");
    g_signal_connect(load_item, "activate", G_CALLBACK(load_from_file), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), load_item);

    GtkWidget* save_item = gtk_menu_item_new_with_label("保存文件");
    g_signal_connect(save_item, "activate", G_CALLBACK(save_to_file), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);

    GtkWidget* sep = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), sep);

    GtkWidget* exit_item = gtk_menu_item_new_with_label("退出");
    g_signal_connect(exit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), exit_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // 水平布局：绘图区 + 控制面板
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    // 绘图区
    GtkWidget* frame = gtk_frame_new(NULL);
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 700, 500);
    gtk_container_add(GTK_CONTAINER(frame), drawing_area);
    gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_callback), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "button-press-event", G_CALLBACK(button_press_callback), NULL);

    // 右侧控制面板
    GtkWidget* right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(hbox), right_panel, FALSE, FALSE, 0);

    // 统计信息框
    GtkWidget* stats_frame = gtk_frame_new("统计信息");
    GtkWidget* stats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(stats_box), 10);

    vertex_count_label = gtk_label_new("顶点数: 0");
    edge_count_label = gtk_label_new("边数: 0");
    articulation_count_label = gtk_label_new("关节点数: 0");

    gtk_box_pack_start(GTK_BOX(stats_box), vertex_count_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(stats_box), edge_count_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(stats_box), articulation_count_label, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(stats_frame), stats_box);
    gtk_box_pack_start(GTK_BOX(right_panel), stats_frame, FALSE, FALSE, 0);

    // 控制按钮框
    GtkWidget* control_frame = gtk_frame_new("控制");
    GtkWidget* control_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(control_box), 10);

    GtkWidget* example1_btn = gtk_button_new_with_label("示例图1");
    GtkWidget* example2_btn = gtk_button_new_with_label("示例图2");
    GtkWidget* random_btn = gtk_button_new_with_label("随机图");
    GtkWidget* find_btn = gtk_button_new_with_label("查找关节点");
    GtkWidget* remove_btn = gtk_button_new_with_label("移除关节点");

    g_signal_connect(example1_btn, "clicked", G_CALLBACK(load_example1), NULL);
    g_signal_connect(example2_btn, "clicked", G_CALLBACK(load_example2), NULL);
    g_signal_connect(random_btn, "clicked", G_CALLBACK(generate_random), NULL);
    g_signal_connect(find_btn, "clicked", G_CALLBACK(find_articulations), NULL);
    g_signal_connect(remove_btn, "clicked", G_CALLBACK(remove_articulation), NULL);

    gtk_box_pack_start(GTK_BOX(control_box), example1_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(control_box), example2_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(control_box), random_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(control_box), find_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(control_box), remove_btn, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(control_frame), control_box);
    gtk_box_pack_start(GTK_BOX(right_panel), control_frame, FALSE, FALSE, 0);

    // 显示选项框
    GtkWidget* options_frame = gtk_frame_new("显示选项");
    GtkWidget* options_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(options_box), 10);

    GtkWidget* show_ap_check = gtk_check_button_new_with_label("显示关节点");
    GtkWidget* show_labels_check = gtk_check_button_new_with_label("显示标签");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(show_ap_check), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(show_labels_check), TRUE);

    g_signal_connect(show_ap_check, "toggled", G_CALLBACK(toggle_show_articulations), NULL);
    g_signal_connect(show_labels_check, "toggled", G_CALLBACK(toggle_show_labels), NULL);

    gtk_box_pack_start(GTK_BOX(options_box), show_ap_check, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(options_box), show_labels_check, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(options_frame), options_box);
    gtk_box_pack_start(GTK_BOX(right_panel), options_frame, FALSE, FALSE, 0);

    // 信息框
    GtkWidget* info_frame = gtk_frame_new("顶点信息");
    GtkWidget* info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(info_box), 10);

    info_label = gtk_label_new("点击顶点查看详细信息");
    gtk_label_set_line_wrap(GTK_LABEL(info_label), TRUE);
    gtk_box_pack_start(GTK_BOX(info_box), info_label, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(info_frame), info_box);
    gtk_box_pack_start(GTK_BOX(right_panel), info_frame, FALSE, FALSE, 0);

    // 状态栏
    GtkWidget* status_frame = gtk_frame_new(NULL);
    status_label = gtk_label_new("就绪");
    gtk_container_add(GTK_CONTAINER(status_frame), status_label);
    gtk_box_pack_start(GTK_BOX(vbox), status_frame, FALSE, FALSE, 0);
}

// 主函数
int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    create_gui();

    // 加载初始示例图
    load_example1(NULL, NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
