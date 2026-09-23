#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include "studentmodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &username, const QString &role, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 学生操作
    void onAddStudent();
    void onEditStudent();
    void onDeleteStudent();
    void onRefresh();
    void onSearch(const QString &keyword);

    // 导入导出
    void onExportCsv();
    void onBatchImport();

    // 统计图表
    void onShowCharts();

    // 主题切换
    void onToggleTheme();

    // 数据库备份还原
    void onBackupDatabase();
    void onRestoreDatabase();

    // 用户管理（仅管理员）
    void onUserManagement();

    // 表格双击编辑
    void onTableDoubleClicked(const QModelIndex &index);

    // 表格选择变化
    void onSelectionChanged();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupCentralWidget();
    void setupConnections();

    void applyTheme(bool dark);
    void loadStyleSheet(const QString &path);
    void applyPermissions();

    void updateStatusBar();

    // UI组件
    QTableView *m_tableView;
    StudentModel *m_model;
    QLineEdit *m_searchEdit;

    // 工具栏按钮
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_exportBtn;
    QPushButton *m_importBtn;
    QPushButton *m_chartBtn;

    // 状态栏
    QLabel *m_statusLabel;
    QLabel *m_countLabel;
    QLabel *m_roleLabel;

    // 状态
    QString m_username;
    QString m_role;
    bool m_isDarkTheme = false;
};

#endif // MAINWINDOW_H
