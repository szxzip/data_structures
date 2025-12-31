#include "header.h"

// 全局变量定义
GtkWidget* male_grid; // 男对女矩阵显示网格
GtkWidget* female_grid; // 女对男矩阵显示网格
GtkWidget* result_textview; // 结果显示文本框
SatisfactionMatrix current_matrix; // 当前处理的矩阵数据
MatchingResult current_result; // 当前匹配结果
GtkWidget* n_spinner; // 运动员数量选择器

bool enable_verification = true; // 默认启用验证

// 在网格中显示矩阵
void display_matrix_in_grid(GtkWidget* grid, const vector<vector<int>>& matrix, int start_row, const string& title)
{
    // 清除旧内容
    GList* children = gtk_container_get_children(GTK_CONTAINER(grid));
    for (GList* child = children; child != NULL; child = child->next) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
    }
    g_list_free(children);

    int n = matrix.size();
    if (n == 0)
        return;

    // 添加标题
    GtkWidget* title_label = gtk_label_new(title.c_str());
    gtk_grid_attach(GTK_GRID(grid), title_label, 0, start_row, n + 1, 1);

    // 添加列标题
    for (int j = 0; j < n; j++) {
        string label_text = "女" + to_string(j + 1);
        GtkWidget* col_label = gtk_label_new(label_text.c_str());
        gtk_grid_attach(GTK_GRID(grid), col_label, j + 1, start_row + 1, 1, 1);
    }

    // 添加行标题和矩阵内容
    for (int i = 0; i < n; i++) {
        string row_label_text = "男" + to_string(i + 1);
        GtkWidget* row_label = gtk_label_new(row_label_text.c_str());
        gtk_grid_attach(GTK_GRID(grid), row_label, 0, start_row + 2 + i, 1, 1);

        for (int j = 0; j < n; j++) {
            string value = to_string(matrix[i][j]);
            GtkWidget* value_label = gtk_label_new(value.c_str());
            gtk_grid_attach(GTK_GRID(grid), value_label, j + 1, start_row + 2 + i, 1, 1);
        }
    }

    gtk_widget_show_all(grid);
}

// 更新结果显示
void update_result_display(GtkWidget* textview, const MatchingResult& result)
{
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer, "", -1);

    GtkTextIter iter;
    gtk_text_buffer_get_start_iter(buffer, &iter);

    // 匈牙利算法结果
    string result_text = "=== 匈牙利算法结果 ===\n\n";
    result_text += "最佳匹配:\n";

    for (size_t i = 0; i < result.pairs.size(); i++) {
        if (result.pairs[i] != -1) {
            result_text += "  男" + to_string(i + 1) + " ←→ 女" + to_string(result.pairs[i] + 1) + "\n";
        }
    }

    result_text += "\n满意度分数:\n";
    result_text += "  男方总满意度: " + to_string(result.maleScore) + "\n";
    result_text += "  女方总满意度: " + to_string(result.femaleScore) + "\n";
    result_text += "  总满意度: " + to_string(result.totalScore) + "\n";

    gtk_text_buffer_insert(buffer, &iter, result_text.c_str(), -1);
}

// 更新结果显示（有对比信息）
void update_result_display_with_comparison(GtkWidget* textview, const MatchingResult& result, const AlgorithmComparison& comparison)
{

    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer, "", -1);

    GtkTextIter iter;
    gtk_text_buffer_get_start_iter(buffer, &iter);

    // 匈牙利算法结果
    string result_text = "=== 匈牙利算法结果 ===\n\n";
    result_text += "最佳匹配:\n";

    for (size_t i = 0; i < result.pairs.size(); i++) {
        if (result.pairs[i] != -1) {
            result_text += "  男" + to_string(i + 1) + " ←→ 女" + to_string(result.pairs[i] + 1) + "\n";
        }
    }

    result_text += "\n满意度分数:\n";
    result_text += "  男方总满意度: " + to_string(result.maleScore) + "\n";
    result_text += "  女方总满意度: " + to_string(result.femaleScore) + "\n";
    result_text += "  总满意度: " + to_string(result.totalScore) + "\n";

    result_text += "\n=== 算法验证结果 ===\n\n";

    if (comparison.isOptimal) {
        result_text += "✓ 验证通过：匈牙利算法找到了最优解\n\n";
    } else {
        result_text += "✗ 验证未通过：匈牙利算法未找到最优解\n";
        result_text += "  暴力搜索最优解: " + to_string(comparison.bruteForce.totalScore) + "\n";
        result_text += "  分数差值: " + to_string(comparison.scoreDifference) + "\n\n";
    }

    result_text += "暴力搜索结果:\n";
    result_text += "  总满意度: " + to_string(comparison.bruteForce.totalScore) + "\n";

    gtk_text_buffer_insert(buffer, &iter, result_text.c_str(), -1);
}

// 生成随机矩阵按钮
void on_generate_clicked(GtkWidget* widget, gpointer data)
{
    int n = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(n_spinner));

    if (n <= 0 || n > 20) {
        GtkWidget* dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "运动员数量应在1-20之间");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    current_matrix.resize(n);
    current_matrix.randomGenerate();

    display_matrix_in_grid(male_grid, current_matrix.mf, 0, "男对女满意度矩阵");
    display_matrix_in_grid(female_grid, current_matrix.fm, 0, "女对男满意度矩阵");
}

// 求解按钮
void on_solve_clicked(GtkWidget* widget, gpointer data)
{
    if (current_matrix.n == 0) {
        GtkWidget* dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "请先生成或加载矩阵数据");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // 使用匈牙利算法求解
    HungarianAlgorithm solver;
    current_result = solver.solve(current_matrix);

    // 验证结果 & 更新GUI显示
    if (enable_verification == true && current_matrix.n <= 10) {
        AlgorithmComparison* comparison = verify_hungarian_result(current_matrix, current_result);
        update_result_display_with_comparison(result_textview, current_result, *comparison);
    } else {
        update_result_display(result_textview, current_result);
    }
}

// 加载文件按钮
void on_load_clicked(GtkWidget* widget, gpointer data)
{
    GtkWidget* dialog = gtk_file_chooser_dialog_new("选择文件",
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "取消", GTK_RESPONSE_CANCEL,
        "打开", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (loadMatrixFromFile(filename, current_matrix)) {
            display_matrix_in_grid(male_grid, current_matrix.mf, 0, "男对女满意度矩阵");
            display_matrix_in_grid(female_grid, current_matrix.fm, 0, "女对男满意度矩阵");
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(n_spinner), current_matrix.n);
        } else {
            GtkWidget* error_dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "加载文件失败，请检查文件格式");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// 保存文件按钮
void on_save_clicked(GtkWidget* widget, gpointer data)
{
    if (current_matrix.n == 0) {
        GtkWidget* dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "没有数据可保存，请先生成或加载矩阵");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    GtkWidget* dialog = gtk_file_chooser_dialog_new("保存文件",
        NULL,
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "取消", GTK_RESPONSE_CANCEL,
        "保存", GTK_RESPONSE_ACCEPT,
        NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        saveMatrixToFile(filename, current_matrix);

        GtkWidget* success_dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "文件保存成功");
        gtk_dialog_run(GTK_DIALOG(success_dialog));
        gtk_widget_destroy(success_dialog);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// 验证开关按钮
void on_verify_toggled(GtkToggleButton* button, gpointer data)
{
    enable_verification = gtk_toggle_button_get_active(button);

    // 在按钮上显示当前状态
    if (enable_verification) {
        gtk_button_set_label(GTK_BUTTON(button), "✓ 验证已启用");
    } else {
        gtk_button_set_label(GTK_BUTTON(button), "✗ 验证已禁用");
    }
}

// GTK应用程序激活
void activate(GtkApplication* app, gpointer user_data)
{
    // 创建主窗口
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "羽毛球运动员最佳组合系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // 创建主垂直布局
    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // 控制面板
    GtkWidget* control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), control_box, FALSE, FALSE, 0);

    // 运动员数量选择
    GtkWidget* n_label = gtk_label_new("运动员数量 (N):");
    gtk_box_pack_start(GTK_BOX(control_box), n_label, FALSE, FALSE, 0);

    // 创建 SpinButton
    GtkAdjustment* adj = gtk_adjustment_new(4, 1, 20, 1, 5, 0);
    n_spinner = gtk_spin_button_new(adj, 1, 0);
    gtk_box_pack_start(GTK_BOX(control_box), n_spinner, FALSE, FALSE, 0);

    // 按钮
    GtkWidget* generate_button = gtk_button_new_with_label("生成随机矩阵");
    g_signal_connect(generate_button, "clicked", G_CALLBACK(on_generate_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(control_box), generate_button, FALSE, FALSE, 0);

    GtkWidget* solve_button = gtk_button_new_with_label("求解最佳组合");
    g_signal_connect(solve_button, "clicked", G_CALLBACK(on_solve_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(control_box), solve_button, FALSE, FALSE, 0);

    GtkWidget* load_button = gtk_button_new_with_label("加载文件");
    g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(control_box), load_button, FALSE, FALSE, 0);

    GtkWidget* save_button = gtk_button_new_with_label("保存文件");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(control_box), save_button, FALSE, FALSE, 0);

    GtkWidget* verify_toggle = gtk_toggle_button_new_with_label("✓ 验证已启用");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(verify_toggle), enable_verification);
    g_signal_connect(verify_toggle, "toggled", G_CALLBACK(on_verify_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(control_box), verify_toggle, FALSE, FALSE, 0);

    // 创建水平布局放置两个矩阵
    GtkWidget* matrix_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), matrix_box, TRUE, TRUE, 0);

    // 男对女矩阵网格
    GtkWidget* male_frame = gtk_frame_new("男对女满意度矩阵");
    gtk_box_pack_start(GTK_BOX(matrix_box), male_frame, TRUE, TRUE, 0);

    GtkWidget* male_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(male_frame), male_scroll);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(male_scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    male_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(male_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(male_grid), 5);
    gtk_container_add(GTK_CONTAINER(male_scroll), male_grid);

    // 女对男矩阵网格
    GtkWidget* female_frame = gtk_frame_new("女对男满意度矩阵");
    gtk_box_pack_start(GTK_BOX(matrix_box), female_frame, TRUE, TRUE, 0);

    GtkWidget* female_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(female_frame), female_scroll);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(female_scroll),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    female_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(female_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(female_grid), 5);
    gtk_container_add(GTK_CONTAINER(female_scroll), female_grid);

    // 结果显示区域
    GtkWidget* result_frame = gtk_frame_new("匹配结果");
    gtk_box_pack_start(GTK_BOX(main_box), result_frame, FALSE, FALSE, 0);
    gtk_widget_set_size_request(result_frame, -1, 350);

    GtkWidget* result_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(result_frame), result_scroll);

    result_textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(result_textview), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(result_textview), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(result_scroll), result_textview);

    // 初始生成一个矩阵
    on_generate_clicked(NULL, NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char** argv)
{
    GtkApplication* app;
    int status;

    app = gtk_application_new("org.gtk.athlete.matching", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
