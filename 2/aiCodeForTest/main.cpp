#include "header.h"

// 创建矩阵输入页面
GtkWidget* create_matrix_page(const char* title, AppData* app_data,
    GtkWidget*** entries)
{
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_add(GTK_CONTAINER(scrolled_window), grid);

    int n = app_data->n;
    *entries = (GtkWidget**)g_malloc(n * n * sizeof(GtkWidget*));

    // 创建标签和输入框
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            GtkWidget* label = gtk_label_new(g_strdup_printf("M%d-F%d", i + 1, j + 1));
            gtk_grid_attach(GTK_GRID(grid), label, j * 2, i, 1, 1);

            GtkWidget* entry = gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(entry), 3);
            gtk_entry_set_text(GTK_ENTRY(entry), "0");
            gtk_grid_attach(GTK_GRID(grid), entry, j * 2 + 1, i, 1, 1);

            (*entries)[i * n + j] = entry;
        }
    }

    return scrolled_window;
}

// 创建矩阵输入条目
void create_matrix_entries(AppData* app_data, int n)
{
    // 清除现有的页面
    while (gtk_notebook_get_n_pages(GTK_NOTEBOOK(app_data->notebook)) > 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(app_data->notebook), 0);
    }

    // 释放旧的内存
    clear_matrix_entries(app_data);

    // 创建男运动员对女运动员的满意度矩阵页面
    GtkWidget* page1 = create_matrix_page("男对女满意度", app_data, &app_data->mf_entries);
    gtk_notebook_append_page(GTK_NOTEBOOK(app_data->notebook), page1,
        gtk_label_new("男对女满意度"));

    // 创建女运动员对男运动员的满意度矩阵页面
    GtkWidget* page2 = create_matrix_page("女对男满意度", app_data, &app_data->fm_entries);
    gtk_notebook_append_page(GTK_NOTEBOOK(app_data->notebook), page2,
        gtk_label_new("女对男满意度"));

    // 显示所有控件
    gtk_widget_show_all(app_data->notebook);
}

// 清除矩阵条目
void clear_matrix_entries(AppData* app_data)
{
    if (app_data->mf_entries) {
        g_free(app_data->mf_entries);
        app_data->mf_entries = NULL;
    }
    if (app_data->fm_entries) {
        g_free(app_data->fm_entries);
        app_data->fm_entries = NULL;
    }
}

// 创建主窗口
GtkWidget* create_main_window(AppData* app_data)
{
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "羽毛球运动员最佳配对系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // 标题
    GtkWidget* title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label),
        "<span size='x-large' weight='bold'>羽毛球运动员最佳配对系统</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 10);

    // 控制面板
    GtkWidget* control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), control_box, FALSE, FALSE, 10);

    GtkWidget* n_label = gtk_label_new("运动员数量 (N):");
    gtk_box_pack_start(GTK_BOX(control_box), n_label, FALSE, FALSE, 5);

    app_data->spin_n = gtk_spin_button_new_with_range(1, 8, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app_data->spin_n), 3);
    g_signal_connect(app_data->spin_n, "value-changed",
        G_CALLBACK(on_n_changed), app_data);
    gtk_box_pack_start(GTK_BOX(control_box), app_data->spin_n, FALSE, FALSE, 5);

    GtkWidget* calculate_btn = gtk_button_new_with_label("计算最佳配对");
    g_signal_connect(calculate_btn, "clicked",
        G_CALLBACK(on_calculate_clicked), app_data);
    gtk_box_pack_start(GTK_BOX(control_box), calculate_btn, FALSE, FALSE, 5);

    // 分隔线
    GtkWidget* separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 10);

    // 笔记本（标签页）
    app_data->notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), app_data->notebook, TRUE, TRUE, 5);

    // 分隔线
    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 10);

    // 结果区域
    GtkWidget* result_frame = gtk_frame_new("配对结果");
    gtk_box_pack_start(GTK_BOX(vbox), result_frame, TRUE, TRUE, 5);

    GtkWidget* result_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(result_vbox), 10);
    gtk_container_add(GTK_CONTAINER(result_frame), result_vbox);

    app_data->result_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app_data->result_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app_data->result_text), GTK_WRAP_WORD);

    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, -1, 200);
    gtk_container_add(GTK_CONTAINER(scrolled_window), app_data->result_text);
    gtk_box_pack_start(GTK_BOX(result_vbox), scrolled_window, TRUE, TRUE, 0);

    return window;
}

// 计算按钮回调函数
void on_calculate_clicked(GtkButton* button, gpointer user_data)
{
    AppData* app_data = (AppData*)user_data;
    int n = app_data->n;

    // 读取输入矩阵
    Matrix mf = MatrixUtils::createMatrix(n);
    Matrix fm = MatrixUtils::createMatrix(n);

    // 从GUI读取男对女满意度数据
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            const char* text = gtk_entry_get_text(GTK_ENTRY(app_data->mf_entries[i * n + j]));
            mf[i][j] = std::atoi(text);
        }
    }

    // 从GUI读取女对男满意度数据
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            const char* text = gtk_entry_get_text(GTK_ENTRY(app_data->fm_entries[i * n + j]));
            fm[i][j] = std::atoi(text);
        }
    }

    // 计算组合满意度矩阵
    Matrix combined = MatrixUtils::addMatrices(mf, fm);

    // 使用暴力搜索算法（n <= 8时可行）
    std::pair<std::vector<int>, int> result;
    if (n <= 8) {
        result = MatchingAlgorithms::bruteForceMatching(combined);
    } else {
        result = MatchingAlgorithms::hungarianAlgorithm(combined);
    }

    std::vector<int> matching = result.first;
    int totalSatisfaction = result.second;

    // 显示结果
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_data->result_text));
    gtk_text_buffer_set_text(buffer, "", -1);

    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);

    std::stringstream ss;
    ss << "最佳配对结果:\n\n";

    for (int i = 0; i < n; i++) {
        ss << "  男运动员 " << (i + 1) << "  ↔  女运动员 " << (matching[i] + 1) << "\n";
    }

    ss << "\n总满意度: " << totalSatisfaction << "\n\n";
    ss << "男对女满意度矩阵:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ss << mf[i][j] << "\t";
        }
        ss << "\n";
    }

    ss << "\n女对男满意度矩阵:\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ss << fm[i][j] << "\t";
        }
        ss << "\n";
    }

    gtk_text_buffer_insert(buffer, &iter, ss.str().c_str(), -1);

    // 保存结果到文件
    FileIO::saveResultToFile("matching_result.txt", matching, totalSatisfaction);
}

// N值改变回调函数
void on_n_changed(GtkSpinButton* spin, gpointer user_data)
{
    AppData* app_data = (AppData*)user_data;
    int new_n = gtk_spin_button_get_value_as_int(spin);

    if (new_n != app_data->n) {
        app_data->n = new_n;
        create_matrix_entries(app_data, new_n);
    }
}

int main(int argc, char* argv[])
{
    // 初始化GTK
    gtk_init(&argc, &argv);

    // 初始化应用数据
    AppData* app_data = new AppData();
    app_data->n = 3;
    app_data->mf_entries = NULL;
    app_data->fm_entries = NULL;

    // 创建主窗口
    GtkWidget* window = create_main_window(app_data);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 创建初始矩阵输入
    create_matrix_entries(app_data, app_data->n);

    // 显示窗口并运行主循环
    gtk_widget_show_all(window);
    gtk_main();

    // 清理内存
    clear_matrix_entries(app_data);
    delete app_data;

    return 0;
}
