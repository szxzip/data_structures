#include <algorithm>
#include <cairo.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <gtk/gtk.h>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

// 颜色结构体
struct Color {
    double r, g, b;
};

// 顶点结构体（用于图形显示）
struct Vertex {
    int id;
    double x, y;
    bool isArticulation;
    Color color;
};

// 边结构体
struct Edge {
    int from, to;
};

// 图类
class Graph {
private:
    int V; // 顶点数
    vector<vector<int>> adj; // 邻接表
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

        for (int v : adj[u]) {
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
    Graph(int vertices = 0)
        : V(vertices)
        , time(0)
    {
        if (vertices > 0) {
            init(vertices);
        }
    }

    void init(int vertices)
    {
        V = vertices;
        adj.resize(V);
        visited.resize(V, false);
        disc.resize(V, -1);
        low.resize(V, -1);
        parent.resize(V, -1);
        isArticulation.resize(V, false);
    }

    void addEdge(int u, int v)
    {
        if (u >= 0 && u < V && v >= 0 && v < V) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    vector<int> findArticulationPoints()
    {
        fill(visited.begin(), visited.end(), false);
        fill(disc.begin(), disc.end(), -1);
        fill(low.begin(), low.end(), -1);
        fill(parent.begin(), parent.end(), -1);
        fill(isArticulation.begin(), isArticulation.end(), false);
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

    int getVertexCount() const { return V; }
    const vector<vector<int>>& getAdjacencyList() const { return adj; }
    vector<int> getNeighbors(int v) const
    {
        if (v >= 0 && v < V)
            return adj[v];
        return {};
    }

    bool checkIfArticulation(int v)
    {
        findArticulationPoints();
        return (v >= 0 && v < V) ? isArticulation[v] : false;
    }

    // 尝试移除关节点
    bool removeArticulation(int v)
    {
        if (v < 0 || v >= V || !isArticulation[v])
            return false;

        vector<int> neighbors = adj[v];
        if (neighbors.size() < 2)
            return false;

        // 在邻居之间添加边
        bool added = false;
        for (int i = 0; i < neighbors.size(); i++) {
            for (int j = i + 1; j < neighbors.size(); j++) {
                int u1 = neighbors[i];
                int u2 = neighbors[j];

                // 检查边是否已存在
                bool exists = false;
                for (int n : adj[u1]) {
                    if (n == u2) {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    addEdge(u1, u2);
                    added = true;
                }
            }
        }

        return added;
    }

    void clear()
    {
        V = 0;
        adj.clear();
        visited.clear();
        disc.clear();
        low.clear();
        parent.clear();
        isArticulation.clear();
    }
};

// 全局变量
Graph g;
vector<Vertex> vertices;
vector<Edge> edges;
vector<int> articulationPoints;
GtkWidget* window;
GtkWidget* drawing_area;
GtkWidget* status_label;
GtkWidget* info_label;
bool showArticulations = true;
bool showLabels = true;
int selectedVertex = -1;
double canvas_width = 800;
double canvas_height = 600;

// 颜色定义
const Color VERTEX_NORMAL = { 0.3, 0.5, 0.8 };
const Color VERTEX_ARTICULATION = { 1.0, 0.3, 0.3 };
const Color VERTEX_SELECTED = { 0.2, 0.8, 0.2 };
const Color EDGE_COLOR = { 0.5, 0.5, 0.5 };

// 生成随机布局
void generateLayout()
{
    vertices.clear();
    int V = g.getVertexCount();

    // 生成圆形布局
    double center_x = canvas_width / 2;
    double center_y = canvas_height / 2;
    double radius = min(canvas_width, canvas_height) * 0.35;

    for (int i = 0; i < V; i++) {
        double angle = 2 * M_PI * i / V;
        Vertex v;
        v.id = i;
        v.x = center_x + radius * cos(angle);
        v.y = center_y + radius * sin(angle);
        v.isArticulation = false;
        v.color = VERTEX_NORMAL;
        vertices.push_back(v);
    }

    // 更新关节点状态
    articulationPoints = g.findArticulationPoints();
    for (int ap : articulationPoints) {
        if (ap < vertices.size()) {
            vertices[ap].isArticulation = true;
            vertices[ap].color = VERTEX_ARTICULATION;
        }
    }

    // 构建边列表
    edges.clear();
    const auto& adj = g.getAdjacencyList();
    for (int i = 0; i < V; i++) {
        for (int neighbor : adj[i]) {
            if (i < neighbor) { // 避免重复
                Edge e;
                e.from = i;
                e.to = neighbor;
                edges.push_back(e);
            }
        }
    }
}

// 查找最近的顶点
int findVertexAt(double x, double y, double threshold = 20)
{
    for (const auto& v : vertices) {
        double dx = v.x - x;
        double dy = v.y - y;
        if (dx * dx + dy * dy < threshold * threshold) {
            return v.id;
        }
    }
    return -1;
}

// 绘图函数
gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data)
{
    // 设置背景色
    cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
    cairo_paint(cr);

    // 绘制边
    cairo_set_source_rgb(cr, EDGE_COLOR.r, EDGE_COLOR.g, EDGE_COLOR.b);
    cairo_set_line_width(cr, 2);

    for (const auto& e : edges) {
        if (e.from < vertices.size() && e.to < vertices.size()) {
            cairo_move_to(cr, vertices[e.from].x, vertices[e.from].y);
            cairo_line_to(cr, vertices[e.to].x, vertices[e.to].y);
            cairo_stroke(cr);
        }
    }

    // 绘制顶点
    for (const auto& v : vertices) {
        // 选择颜色
        Color color;
        if (selectedVertex == v.id) {
            color = VERTEX_SELECTED;
        } else if (showArticulations && v.isArticulation) {
            color = VERTEX_ARTICULATION;
        } else {
            color = v.color;
        }

        // 绘制圆
        cairo_set_source_rgb(cr, color.r, color.g, color.b);
        cairo_arc(cr, v.x, v.y, 20, 0, 2 * M_PI);
        cairo_fill_preserve(cr);

        // 绘制边框
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 2);
        cairo_stroke(cr);

        // 绘制标签
        if (showLabels) {
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(cr, 14);

            string label = to_string(v.id);
            cairo_text_extents_t extents;
            cairo_text_extents(cr, label.c_str(), &extents);

            cairo_move_to(cr, v.x - extents.width / 2, v.y + extents.height / 2);
            cairo_show_text(cr, label.c_str());
        }
    }

    return FALSE;
}

// 鼠标点击事件
gboolean button_press_callback(GtkWidget* widget, GdkEventButton* event, gpointer data)
{
    if (event->button == GDK_BUTTON_PRIMARY) {
        selectedVertex = findVertexAt(event->x, event->y);

        // 更新状态标签
        if (selectedVertex >= 0) {
            bool isAP = g.checkIfArticulation(selectedVertex);
            string status = "顶点 " + to_string(selectedVertex) + " (" + (isAP ? "关节点" : "非关节点") + ")" + " - 邻居: ";

            auto neighbors = g.getNeighbors(selectedVertex);
            for (size_t i = 0; i < neighbors.size(); i++) {
                status += to_string(neighbors[i]);
                if (i < neighbors.size() - 1)
                    status += ", ";
            }

            gtk_label_set_text(GTK_LABEL(info_label), status.c_str());
        }

        gtk_widget_queue_draw(drawing_area);
    }
    return TRUE;
}

// 创建示例图1
void create_example_graph1()
{
    g.init(7);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(3, 4);
    g.addEdge(3, 5);
    g.addEdge(4, 5);
    g.addEdge(5, 6);

    generateLayout();

    string status = "示例图1已加载。关节点: ";
    auto aps = g.findArticulationPoints();
    for (size_t i = 0; i < aps.size(); i++) {
        status += to_string(aps[i]);
        if (i < aps.size() - 1)
            status += ", ";
    }
    gtk_label_set_text(GTK_LABEL(status_label), status.c_str());
    gtk_widget_queue_draw(drawing_area);
}

// 创建示例图2（双连通图）
void create_example_graph2()
{
    g.init(5);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(1, 2);
    g.addEdge(1, 4);
    g.addEdge(2, 3);
    g.addEdge(3, 4);

    generateLayout();

    auto aps = g.findArticulationPoints();
    if (aps.empty()) {
        gtk_label_set_text(GTK_LABEL(status_label), "示例图2已加载。这是一个双连通图，没有关节点。");
    }
    gtk_widget_queue_draw(drawing_area);
}

// 创建树状图
void create_tree_graph()
{
    g.init(6);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);

    generateLayout();

    string status = "树状图已加载。关节点: ";
    auto aps = g.findArticulationPoints();
    for (size_t i = 0; i < aps.size(); i++) {
        status += to_string(aps[i]);
        if (i < aps.size() - 1)
            status += ", ";
    }
    gtk_label_set_text(GTK_LABEL(status_label), status.c_str());
    gtk_widget_queue_draw(drawing_area);
}

// 随机生成图
void create_random_graph()
{
    srand(time(NULL));
    int vertices = rand() % 8 + 5; // 5-12个顶点
    int edges = vertices + rand() % (vertices * 2);

    g.init(vertices);

    // 确保连通性：先生成树
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

    generateLayout();

    string status = "随机图已生成。顶点数: " + to_string(vertices) + ", 边数: " + to_string(edges) + "。关节点: ";
    auto aps = g.findArticulationPoints();
    if (aps.empty()) {
        status += "无";
    } else {
        for (size_t i = 0; i < aps.size(); i++) {
            status += to_string(aps[i]);
            if (i < aps.size() - 1)
                status += ", ";
        }
    }
    gtk_label_set_text(GTK_LABEL(status_label), status.c_str());
    gtk_widget_queue_draw(drawing_area);
}

// 查找关节点
void find_articulations()
{
    articulationPoints = g.findArticulationPoints();

    // 更新顶点颜色
    for (auto& v : vertices) {
        v.isArticulation = false;
        v.color = VERTEX_NORMAL;
    }

    for (int ap : articulationPoints) {
        if (ap < vertices.size()) {
            vertices[ap].isArticulation = true;
            vertices[ap].color = VERTEX_ARTICULATION;
        }
    }

    string status = "关节点查找完成。找到 " + to_string(articulationPoints.size()) + " 个关节点";
    if (!articulationPoints.empty()) {
        status += ": ";
        for (size_t i = 0; i < articulationPoints.size(); i++) {
            status += to_string(articulationPoints[i]);
            if (i < articulationPoints.size() - 1)
                status += ", ";
        }
    }

    gtk_label_set_text(GTK_LABEL(status_label), status.c_str());
    gtk_widget_queue_draw(drawing_area);
}

// 移除选中的关节点
void remove_articulation()
{
    if (selectedVertex >= 0 && g.checkIfArticulation(selectedVertex)) {
        if (g.removeArticulation(selectedVertex)) {
            // 重新布局
            generateLayout();

            string status = "顶点 " + to_string(selectedVertex) + " 已从关节点移除。";
            gtk_label_set_text(GTK_LABEL(status_label), status.c_str());
            gtk_widget_queue_draw(drawing_area);
        } else {
            gtk_label_set_text(GTK_LABEL(status_label), "移除关节点失败。");
        }
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "请先选择一个关节点。");
    }
}

// 切换关节点显示
void toggle_articulations(GtkToggleButton* button, gpointer data)
{
    showArticulations = gtk_toggle_button_get_active(button);
    gtk_widget_queue_draw(drawing_area);
}

// 切换标签显示
void toggle_labels(GtkToggleButton* button, gpointer data)
{
    showLabels = gtk_toggle_button_get_active(button);
    gtk_widget_queue_draw(drawing_area);
}

// 从文件加载图
void load_from_file()
{
    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new("打开图文件",
        GTK_WINDOW(window),
        action,
        "_取消",
        GTK_RESPONSE_CANCEL,
        "_打开",
        GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        ifstream file(filename);
        if (file.is_open()) {
            int vertices, edges;
            file >> vertices >> edges;

            g.init(vertices);
            for (int i = 0; i < edges; i++) {
                int u, v;
                file >> u >> v;
                g.addEdge(u, v);
            }
            file.close();

            generateLayout();
            string status = "从文件 " + string(filename) + " 加载成功。";
            gtk_label_set_text(GTK_LABEL(status_label), status.c_str());
            gtk_widget_queue_draw(drawing_area);
        } else {
            gtk_label_set_text(GTK_LABEL(status_label), "无法打开文件。");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// 保存图到文件
void save_to_file()
{
    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;

    dialog = gtk_file_chooser_dialog_new("保存图",
        GTK_WINDOW(window),
        action,
        "_取消",
        GTK_RESPONSE_CANCEL,
        "_保存",
        GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        ofstream file(filename);
        if (file.is_open()) {
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
            gtk_label_set_text(GTK_LABEL(status_label), "图已保存到文件。");
        } else {
            gtk_label_set_text(GTK_LABEL(status_label), "无法保存文件。");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// 创建菜单栏
GtkWidget* create_menu_bar()
{
    GtkWidget* menu_bar = gtk_menu_bar_new();

    // 文件菜单
    GtkWidget* file_menu = gtk_menu_new();
    GtkWidget* file_item = gtk_menu_item_new_with_label("文件");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);

    GtkWidget* load_item = gtk_menu_item_new_with_label("打开文件");
    g_signal_connect(load_item, "activate", G_CALLBACK(load_from_file), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), load_item);

    GtkWidget* save_item = gtk_menu_item_new_with_label("保存文件");
    g_signal_connect(save_item, "activate", G_CALLBACK(save_to_file), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);

    GtkWidget* sep1 = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), sep1);

    GtkWidget* exit_item = gtk_menu_item_new_with_label("退出");
    g_signal_connect(exit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), exit_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    // 图菜单
    GtkWidget* graph_menu = gtk_menu_new();
    GtkWidget* graph_item = gtk_menu_item_new_with_label("图");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(graph_item), graph_menu);

    GtkWidget* example1_item = gtk_menu_item_new_with_label("示例图1");
    g_signal_connect(example1_item, "activate", G_CALLBACK(create_example_graph1), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), example1_item);

    GtkWidget* example2_item = gtk_menu_item_new_with_label("示例图2（双连通）");
    g_signal_connect(example2_item, "activate", G_CALLBACK(create_example_graph2), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), example2_item);

    GtkWidget* tree_item = gtk_menu_item_new_with_label("树状图");
    g_signal_connect(tree_item, "activate", G_CALLBACK(create_tree_graph), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), tree_item);

    GtkWidget* random_item = gtk_menu_item_new_with_label("随机图");
    g_signal_connect(random_item, "activate", G_CALLBACK(create_random_graph), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), random_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), graph_item);

    // 操作菜单
    GtkWidget* action_menu = gtk_menu_new();
    GtkWidget* action_item = gtk_menu_item_new_with_label("操作");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(action_item), action_menu);

    GtkWidget* find_item = gtk_menu_item_new_with_label("查找关节点");
    g_signal_connect(find_item, "activate", G_CALLBACK(find_articulations), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(action_menu), find_item);

    GtkWidget* remove_item = gtk_menu_item_new_with_label("移除选中的关节点");
    g_signal_connect(remove_item, "activate", G_CALLBACK(remove_articulation), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(action_menu), remove_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), action_item);

    return menu_bar;
}

// 创建工具栏
GtkWidget* create_toolbar()
{
    GtkWidget* toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

    // 查找关节点按钮
    GtkToolItem* find_item = gtk_tool_button_new_from_stock(GTK_STOCK_FIND);
    g_signal_connect(G_OBJECT(find_item), "clicked", G_CALLBACK(find_articulations), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), find_item, -1);

    // 移除关节点按钮
    GtkToolItem* remove_item = gtk_tool_button_new_from_stock(GTK_STOCK_REMOVE);
    g_signal_connect(G_OBJECT(remove_item), "clicked", G_CALLBACK(remove_articulation), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), remove_item, -1);

    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), -1);

    // 示例图按钮
    GtkToolItem* example1_item = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(example1_item), "示例1");
    g_signal_connect(G_OBJECT(example1_item), "clicked", G_CALLBACK(create_example_graph1), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), example1_item, -1);

    GtkToolItem* random_item = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(random_item), "随机");
    g_signal_connect(G_OBJECT(random_item), "clicked", G_CALLBACK(create_random_graph), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), random_item, -1);

    return toolbar;
}

// 创建控制面板
GtkWidget* create_control_panel()
{
    GtkWidget* frame = gtk_frame_new("控制");
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    // 显示选项
    GtkWidget* show_articulations = gtk_check_button_new_with_label("显示关节点");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(show_articulations), TRUE);
    g_signal_connect(show_articulations, "toggled", G_CALLBACK(toggle_articulations), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), show_articulations, FALSE, FALSE, 0);

    GtkWidget* show_labels = gtk_check_button_new_with_label("显示标签");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(show_labels), TRUE);
    g_signal_connect(show_labels, "toggled", G_CALLBACK(toggle_labels), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), show_labels, FALSE, FALSE, 0);

    // 图操作按钮
    GtkWidget* find_button = gtk_button_new_with_label("查找关节点");
    g_signal_connect(find_button, "clicked", G_CALLBACK(find_articulations), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), find_button, FALSE, FALSE, 0);

    GtkWidget* remove_button = gtk_button_new_with_label("移除关节点");
    g_signal_connect(remove_button, "clicked", G_CALLBACK(remove_articulation), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), remove_button, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(frame), vbox);
    return frame;
}

// 创建信息面板
GtkWidget* create_info_panel()
{
    GtkWidget* frame = gtk_frame_new("图信息");
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    GtkWidget* label = gtk_label_new("点击顶点查看详细信息");
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    info_label = gtk_label_new("选择顶点...");
    gtk_label_set_line_wrap(GTK_LABEL(info_label), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), info_label, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(frame), vbox);
    return frame;
}

// 创建状态栏
GtkWidget* create_status_bar()
{
    GtkWidget* status_bar = gtk_statusbar_new();
    status_label = gtk_label_new("就绪。请加载或创建一个图。");
    gtk_container_add(GTK_CONTAINER(status_bar), status_label);
    return status_bar;
}

// 主函数
int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    // 创建主窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "关节点查找系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 创建主垂直容器
    GtkWidget* main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    // 添加菜单栏
    GtkWidget* menu_bar = create_menu_bar();
    gtk_box_pack_start(GTK_BOX(main_vbox), menu_bar, FALSE, FALSE, 0);

    // 添加工具栏
    GtkWidget* toolbar = create_toolbar();
    gtk_box_pack_start(GTK_BOX(main_vbox), toolbar, FALSE, FALSE, 0);

    // 创建水平容器用于绘图区和控制面板
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vbox), hbox, TRUE, TRUE, 0);

    // 创建绘图区
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 700, 500);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_callback), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "button-press-event", G_CALLBACK(button_press_callback), NULL);

    // 添加滚动窗口包装绘图区
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), drawing_area);
    gtk_box_pack_start(GTK_BOX(hbox), scrolled_window, TRUE, TRUE, 0);

    // 创建右侧控制面板
    GtkWidget* right_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), right_vbox, FALSE, FALSE, 0);

    // 添加控制面板
    GtkWidget* control_panel = create_control_panel();
    gtk_box_pack_start(GTK_BOX(right_vbox), control_panel, FALSE, FALSE, 0);

    // 添加信息面板
    GtkWidget* info_panel = create_info_panel();
    gtk_box_pack_start(GTK_BOX(right_vbox), info_panel, FALSE, FALSE, 0);

    // 添加状态栏
    GtkWidget* status_bar = create_status_bar();
    gtk_box_pack_start(GTK_BOX(main_vbox), status_bar, FALSE, FALSE, 0);

    // 显示所有部件
    gtk_widget_show_all(window);

    // 加载初始示例图
    create_example_graph1();

    // 启动GTK主循环
    gtk_main();

    return 0;
}
