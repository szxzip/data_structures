#include "header.h"

// 全局变量
GtkWidget* window = nullptr;
GtkWidget* drawing_area = nullptr;
GtkWidget* info_label = nullptr;
GtkWidget* status_label = nullptr;
GtkWidget* articulation_list = nullptr;
GtkWidget* vertex_entry = nullptr;
GtkWidget* density_scale = nullptr;

Graph* currentGraph = nullptr; // 当前图
vector<int> currentArticulations; // 当前关节点
vector<pair<double, double>> vertexPositions; // GUI 顶点位置
vector<pair<int, int>> edgeList; // 当前边列表

// 更新图显示
void update_graph_display()
{
    if (!currentGraph)
        return;

    // 计算顶点位置（圆形布局）
    vertexPositions.clear(); // 清空顶点位置
    int V = currentGraph->getVertexCount(); // 顶点数
    int centerX = 300, centerY = 250, radius = 180; // 圆心及半径

    for (int i = 0; i < V; i++) {
        double angle = 2 * 3.141592653589793 * i / V; // 等分圆周
        double x = centerX + radius * cos(angle);
        double y = centerY + radius * sin(angle);
        vertexPositions.push_back(make_pair(x, y)); // 构造后写入
    }

    // 获取边列表
    edgeList.clear(); // 清空边列表
    const vector<vector<int>>& adj = currentGraph->getAdjacencyList();
    for (int i = 0; i < V; i++) { // 遍历顶点
        for (size_t j = 0; j < adj[i].size(); j++) { // 遍历相邻顶点
            int neighbor = adj[i][j]; // 相邻顶点
            if (i < neighbor) { // 顶点小于相邻顶点，避免重复
                edgeList.push_back(make_pair(i, neighbor)); // 写入边
            }
        }
    }

    // 更新信息标签
    stringstream info;
    info << "顶点数: " << V << " | 边数: " << currentGraph->getEdgeCount() << " | 关节点数: " << currentArticulations.size();

    gtk_label_set_text(GTK_LABEL(info_label), info.str().c_str());

    // 重绘图
    gtk_widget_queue_draw(drawing_area);
}

// 绘图
bool draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data)
{
    // 清空背景
    cairo_set_source_rgb(cr, 0.08, 0.09, 0.1);
    cairo_paint(cr);

    if (!currentGraph)
        return FALSE;

    // 绘制边
    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    cairo_set_line_width(cr, 2);

    for (size_t i = 0; i < edgeList.size(); i++) {
        int u = edgeList[i].first;
        int v = edgeList[i].second;

        double x1 = vertexPositions[u].first;
        double y1 = vertexPositions[u].second;
        double x2 = vertexPositions[v].first;
        double y2 = vertexPositions[v].second;

        cairo_move_to(cr, x1, y1);
        cairo_line_to(cr, x2, y2);
        cairo_stroke(cr);
    }

    // 绘制顶点
    for (int i = 0; i < currentGraph->getVertexCount(); i++) {
        double x = vertexPositions[i].first;
        double y = vertexPositions[i].second;

        // 判断是否为关节点
        bool isAP = false;
        for (size_t j = 0; j < currentArticulations.size(); j++) {
            if (currentArticulations[j] == i) {
                isAP = true;
                break;
            }
        }

        if (isAP) {
            // 关节点
            cairo_set_source_rgb(cr, 0.39, 0.32, 0.21);
        } else {
            // 普通顶点
            cairo_set_source_rgb(cr, 0.12, 0.14, 0.15);
        }

        cairo_arc(cr, x, y, 15, 0, 2 * 3.141592653589793);
        cairo_fill(cr);

        // 顶点边框
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_set_line_width(cr, 1);
        cairo_arc(cr, x, y, 15, 0, 2 * 3.141592653589793);
        cairo_stroke(cr);

        // 顶点标签
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 12);

        string label = to_string(i);
        cairo_text_extents_t extents;
        cairo_text_extents(cr, label.c_str(), &extents);
        cairo_move_to(cr, x - extents.width / 2, y + extents.height / 2);
        cairo_show_text(cr, label.c_str());
    }

    return FALSE;
}

// 创建新图
void create_new_graph(GtkWidget* widget, gpointer data)
{
    GtkWidget* dialog = gtk_dialog_new_with_buttons("创建新图",
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        "确定", GTK_RESPONSE_OK,
        "取消", GTK_RESPONSE_CANCEL,
        NULL);

    GtkWidget* content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // 顶点数输入
    GtkWidget* vertex_label = gtk_label_new("顶点数:");
    GtkWidget* vertex_spin = gtk_spin_button_new_with_range(3, 20, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(vertex_spin), 8);

    // 密度选择
    GtkWidget* density_label = gtk_label_new("密度:");
    density_scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.1, 0.9, 0.1);
    gtk_range_set_value(GTK_RANGE(density_scale), 0.3);

    gtk_grid_attach(GTK_GRID(grid), vertex_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), vertex_spin, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), density_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), density_scale, 1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(content), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int vertices = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(vertex_spin));
        double density = gtk_range_get_value(GTK_RANGE(density_scale));

        // 删除旧图
        if (currentGraph) {
            delete currentGraph;
        }

        currentGraph = createRandom(vertices, density);
        currentArticulations.clear();

        // 更新显示
        update_graph_display();

        // 清空关节点列表
        GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(articulation_list)));
        gtk_list_store_clear(store);

        stringstream msg;
        msg << "创建了随机图，顶点数: " << vertices << ", 边数: " << currentGraph->getEdgeCount();
        gtk_label_set_text(GTK_LABEL(status_label), msg.str().c_str());
    }

    gtk_widget_destroy(dialog);
}

// 从文件加载图
void load_graph_from_file(GtkWidget* widget, gpointer data)
{
    GtkWidget* dialog = gtk_file_chooser_dialog_new("打开图文件",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "取消", GTK_RESPONSE_CANCEL,
        "打开", GTK_RESPONSE_ACCEPT,
        NULL);

    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "文本文件 (*.txt)");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        // 删除旧图
        if (currentGraph) {
            delete currentGraph;
        }

        currentGraph = createFromFile(filename);
        if (currentGraph) {
            currentArticulations.clear();
            update_graph_display();

            // 清空关节点列表
            GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(articulation_list)));
            gtk_list_store_clear(store);

            stringstream msg;
            msg << "从文件加载图成功，顶点数: " << currentGraph->getVertexCount()
                << ", 边数: " << currentGraph->getEdgeCount();
            gtk_label_set_text(GTK_LABEL(status_label), msg.str().c_str());
        } else {
            gtk_label_set_text(GTK_LABEL(status_label), "加载文件失败！");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// 保存图到文件
void save_graph_to_file(GtkWidget* widget, gpointer data)
{
    if (!currentGraph) {
        gtk_label_set_text(GTK_LABEL(status_label), "没有图可保存！");
        return;
    }

    GtkWidget* dialog = gtk_file_chooser_dialog_new("保存图文件",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "取消", GTK_RESPONSE_CANCEL,
        "保存", GTK_RESPONSE_ACCEPT,
        NULL);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "graph.txt");

    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "文本文件 (*.txt)");
    gtk_file_filter_add_pattern(filter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (saveToFile(*currentGraph, filename)) {
            gtk_label_set_text(GTK_LABEL(status_label), "图保存成功！");
        } else {
            gtk_label_set_text(GTK_LABEL(status_label), "保存文件失败！");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// 查找关节点
void find_articulation_points(GtkWidget* widget, gpointer data)
{
    if (!currentGraph) {
        gtk_label_set_text(GTK_LABEL(status_label), "请先创建或加载图！");
        return;
    }

    currentGraph->findArticulationPoints();
    currentArticulations = currentGraph->getArticulationPoints();

    // 更新显示
    update_graph_display();

    // 更新关节点列表
    gtk_list_store_clear(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(articulation_list))));

    GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(articulation_list)));
    for (size_t i = 0; i < currentArticulations.size(); i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        string vertex_str = to_string(currentArticulations[i]);
        gtk_list_store_set(store, &iter, 0, vertex_str.c_str(), -1);
    }

    stringstream msg;
    msg << "找到 " << currentArticulations.size() << " 个关节点";
    gtk_label_set_text(GTK_LABEL(status_label), msg.str().c_str());
}

// 统计关节点
void count_articulation_points(GtkWidget* widget, gpointer data)
{
    if (!currentGraph) {
        gtk_label_set_text(GTK_LABEL(status_label), "请先创建或加载图！");
        return;
    }

    int count = currentGraph->countArticulationPoints();
    stringstream msg;
    msg << "关节点总数: " << count;

    // 显示消息对话框
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", msg.str().c_str());
    gtk_window_set_title(GTK_WINDOW(dialog), "统计结果");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// 改造关节点
void convert_articulation_point(GtkWidget* widget, gpointer data)
{
    if (!currentGraph) {
        gtk_label_set_text(GTK_LABEL(status_label), "请先创建或加载图！");
        return;
    }

    const char* vertex_text = gtk_entry_get_text(GTK_ENTRY(vertex_entry));
    if (strlen(vertex_text) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "请输入顶点编号！");
        return;
    }

    int vertex = stoi(vertex_text);

    if (vertex < 0 || vertex >= currentGraph->getVertexCount()) {
        gtk_label_set_text(GTK_LABEL(status_label), "顶点编号无效！");
        return;
    }

    if (!currentGraph->isArticulationPoint(vertex)) {
        stringstream msg;
        msg << "顶点 " << vertex << " 不是关节点！";
        gtk_label_set_text(GTK_LABEL(status_label), msg.str().c_str());
        return;
    }

    if (currentGraph->convertToNonArticulation(vertex)) {
        // 重新查找关节点
        currentGraph->findArticulationPoints();
        currentArticulations = currentGraph->getArticulationPoints();

        // 更新显示
        update_graph_display();

        // 更新关节点列表
        gtk_list_store_clear(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(articulation_list))));

        GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(articulation_list)));
        for (size_t i = 0; i < currentArticulations.size(); i++) {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            string vertex_str = to_string(currentArticulations[i]);
            gtk_list_store_set(store, &iter, 0, vertex_str.c_str(), -1);
        }

        stringstream msg;
        msg << "成功将顶点 " << vertex << " 改造为非关节点";
        gtk_label_set_text(GTK_LABEL(status_label), msg.str().c_str());
    } else {
        stringstream msg;
        msg << "改造顶点 " << vertex << " 失败";
        gtk_label_set_text(GTK_LABEL(status_label), msg.str().c_str());
    }
}

// 初始化GUI
void initialize_gui(int argc, char** argv)
{
    gtk_init(&argc, &argv);

    // 创建主窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "无向连通图关节点求解系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 创建主垂直盒子
    GtkWidget* main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    // 信息标签
    info_label = gtk_label_new("欢迎使用关节点求解系统");
    gtk_label_set_xalign(GTK_LABEL(info_label), 0);
    GtkWidget* info_frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(info_frame), info_label);
    gtk_box_pack_start(GTK_BOX(main_vbox), info_frame, FALSE, FALSE, 5);

    // 创建水平盒子，左侧是控制面板，右侧是绘图区域
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vbox), hbox, TRUE, TRUE, 0);

    // 左侧控制面板
    GtkWidget* control_frame = gtk_frame_new("控制面板");
    gtk_widget_set_size_request(control_frame, 250, -1);

    GtkWidget* control_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(control_vbox), 10);
    gtk_container_add(GTK_CONTAINER(control_frame), control_vbox);

    // 关节点列表
    GtkWidget* list_frame = gtk_frame_new("关节点列表");
    GtkWidget* list_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(list_vbox), 5);

    // 创建列表
    GtkListStore* store = gtk_list_store_new(1, G_TYPE_STRING);
    articulation_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("顶点", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(articulation_list), column);

    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 150);
    gtk_container_add(GTK_CONTAINER(scrolled_window), articulation_list);

    gtk_box_pack_start(GTK_BOX(list_vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(list_frame), list_vbox);
    gtk_box_pack_start(GTK_BOX(control_vbox), list_frame, TRUE, TRUE, 0);

    // 改造关节点区域
    GtkWidget* convert_frame = gtk_frame_new("改造关节点");
    GtkWidget* convert_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(convert_vbox), 10);

    GtkWidget* convert_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget* vertex_label = gtk_label_new("顶点:");
    vertex_entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(vertex_entry), 3);
    gtk_entry_set_width_chars(GTK_ENTRY(vertex_entry), 5);

    gtk_box_pack_start(GTK_BOX(convert_hbox), vertex_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(convert_hbox), vertex_entry, TRUE, TRUE, 0);

    GtkWidget* convert_button = gtk_button_new_with_label("改造");

    gtk_box_pack_start(GTK_BOX(convert_vbox), convert_hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(convert_vbox), convert_button, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(convert_frame), convert_vbox);
    gtk_box_pack_start(GTK_BOX(control_vbox), convert_frame, FALSE, FALSE, 0);

    // 操作按钮区域
    GtkWidget* button_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget* new_button = gtk_button_new_with_label("新建随机图");
    GtkWidget* open_button = gtk_button_new_with_label("打开图文件");
    GtkWidget* save_button = gtk_button_new_with_label("保存图文件");
    GtkWidget* find_button = gtk_button_new_with_label("查找关节点");
    GtkWidget* count_button = gtk_button_new_with_label("统计关节点");

    gtk_box_pack_start(GTK_BOX(button_vbox), new_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_vbox), open_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_vbox), save_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_vbox), find_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_vbox), count_button, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(control_vbox), button_vbox, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(hbox), control_frame, FALSE, FALSE, 0);

    // 右侧绘图区域
    GtkWidget* drawing_frame = gtk_frame_new("图显示");
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 600, 500);
    gtk_container_add(GTK_CONTAINER(drawing_frame), drawing_area);
    gtk_box_pack_start(GTK_BOX(hbox), drawing_frame, TRUE, TRUE, 0);

    // 状态栏
    status_label = gtk_label_new("就绪");
    gtk_label_set_xalign(GTK_LABEL(status_label), 0);
    GtkWidget* status_frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(status_frame), status_label);
    gtk_box_pack_start(GTK_BOX(main_vbox), status_frame, FALSE, FALSE, 5);

    // 连接信号
    g_signal_connect(new_button, "clicked", G_CALLBACK(create_new_graph), NULL);
    g_signal_connect(open_button, "clicked", G_CALLBACK(load_graph_from_file), NULL);
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_graph_to_file), NULL);
    g_signal_connect(find_button, "clicked", G_CALLBACK(find_articulation_points), NULL);
    g_signal_connect(count_button, "clicked", G_CALLBACK(count_articulation_points), NULL);
    g_signal_connect(convert_button, "clicked", G_CALLBACK(convert_articulation_point), NULL);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_callback), NULL);

    // 显示所有控件
    gtk_widget_show_all(window);
}

// 主函数
int main(int argc, char** argv)
{
    initialize_gui(argc, argv);
    gtk_main();

    // 清理内存
    if (currentGraph) {
        delete currentGraph;
    }

    return 0;
}
