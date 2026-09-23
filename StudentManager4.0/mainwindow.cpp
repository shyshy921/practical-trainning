#include "mainwindow.h"
#include "database.h"
#include "studentdialog.h"
#include "studentdelegate.h"
#include "chartwidget.h"
#include "dataexport.h"
#include "usermanagementdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QFile>
#include <QDateTime>

MainWindow::MainWindow(const QString &username, const QString &role, QWidget *parent)
    : QMainWindow(parent), m_username(username), m_role(role)
{
    setupUI();
    loadStyleSheet(":/style.qss");
    m_model->loadData();
    updateStatusBar();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setWindowTitle("学生信息管理系统 v1.0");
    resize(1100, 650);
    setMinimumSize(800, 500);

    setupMenuBar();
    setupCentralWidget();
    setupToolBar();
    setupStatusBar();
    setupConnections();
    applyPermissions();
}

// ==================== 菜单栏 ====================
void MainWindow::setupMenuBar()
{
    QMenuBar *mb = menuBar();

    // 文件菜单
    QMenu *fileMenu = mb->addMenu("文件(&F)");

    QAction *exportAction = fileMenu->addAction("导出 CSV (&E)");
    exportAction->setShortcut(QKeySequence("Ctrl+E"));

    // 批量导入、备份、还原仅管理员可见
    QAction *importAction = nullptr;
    QAction *backupAction = nullptr;
    QAction *restoreAction = nullptr;

    if (m_role == "admin") {
        importAction = fileMenu->addAction("批量导入 CSV (&I)");
        importAction->setShortcut(QKeySequence("Ctrl+I"));

        fileMenu->addSeparator();

        backupAction = fileMenu->addAction("备份数据库 (&B)");
        restoreAction = fileMenu->addAction("还原数据库 (&R)");
    }

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("退出 (&Q)");
    exitAction->setShortcut(QKeySequence("Alt+F4"));

    // 视图菜单
    QMenu *viewMenu = mb->addMenu("视图(&V)");
    QAction *themeAction = viewMenu->addAction("切换主题 (深色/浅色) (&T)");
    themeAction->setShortcut(QKeySequence("Ctrl+T"));

    QAction *chartAction = viewMenu->addAction("数据统计图表 (&C)");
    chartAction->setShortcut(QKeySequence("Ctrl+D"));

    // 管理菜单（仅管理员可见）
    if (m_role == "admin") {
        QMenu *adminMenu = mb->addMenu("管理(&M)");
        QAction *userMgrAction = adminMenu->addAction("用户管理 (&U)");
        userMgrAction->setShortcut(QKeySequence("Ctrl+U"));
        connect(userMgrAction, &QAction::triggered, this, &MainWindow::onUserManagement);
    }

    // 帮助菜单
    QMenu *helpMenu = mb->addMenu("帮助(&H)");
    QAction *aboutAction = helpMenu->addAction("关于 (&A)");

    // 连接信号
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportCsv);
    if (importAction)
        connect(importAction, &QAction::triggered, this, &MainWindow::onBatchImport);
    if (backupAction)
        connect(backupAction, &QAction::triggered, this, &MainWindow::onBackupDatabase);
    if (restoreAction)
        connect(restoreAction, &QAction::triggered, this, &MainWindow::onRestoreDatabase);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(themeAction, &QAction::triggered, this, &MainWindow::onToggleTheme);
    connect(chartAction, &QAction::triggered, this, &MainWindow::onShowCharts);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "关于",
            "<h3>学生信息管理系统 v1.0</h3>"
            "<p>基于 Qt5/C++ 开发的教务管理应用</p>"
            "<hr>"
            "<p><b>核心技术栈:</b></p>"
            "<ul>"
            "<li>Qt Model/View 架构</li>"
            "<li>SQLite 数据库</li>"
            "<li>多线程数据导出</li>"
            "<li>QPainter 自定义图表</li>"
            "<li>信号槽机制</li>"
            "<li>QSS 主题切换</li>"
            "</ul>"
        );
    });
}

// ==================== 顶部操作栏 ====================
void MainWindow::setupToolBar()
{
    // 所有按钮已在 setupCentralWidget 中作为顶部操作栏创建，此处不再使用 QToolBar
}

// ==================== 中央区域 ====================
void MainWindow::setupCentralWidget()
{
    QWidget *central = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(12, 12, 12, 8);
    layout->setSpacing(10);

    // ========== 第一行：操作按钮 ==========
    QHBoxLayout *topBar = new QHBoxLayout;
    topBar->setSpacing(8);

    // 左侧操作按钮组
    m_addBtn = new QPushButton("添加学生");
    m_addBtn->setObjectName("btnExport");
    m_addBtn->setFixedHeight(38);

    m_editBtn = new QPushButton("编辑");
    m_editBtn->setEnabled(false);
    m_editBtn->setFixedHeight(38);

    m_deleteBtn = new QPushButton("删除");
    m_deleteBtn->setObjectName("btnDelete");
    m_deleteBtn->setEnabled(false);
    m_deleteBtn->setFixedHeight(38);

    QFrame *sep1 = new QFrame;
    sep1->setFrameShape(QFrame::VLine);
    sep1->setStyleSheet("color: #D1D5DB;");

    m_exportBtn = new QPushButton("导出CSV");
    m_exportBtn->setObjectName("btnExport");
    m_exportBtn->setFixedHeight(38);

    m_importBtn = new QPushButton("批量导入");
    m_importBtn->setObjectName("btnExport");
    m_importBtn->setFixedHeight(38);

    QFrame *sep2 = new QFrame;
    sep2->setFrameShape(QFrame::VLine);
    sep2->setStyleSheet("color: #D1D5DB;");

    m_chartBtn = new QPushButton("统计图表");
    m_chartBtn->setStyleSheet(
        "QPushButton { background-color: #F59E0B; color: white; font-size: 14px; font-weight: bold;"
        "border-radius: 8px; padding: 8px 18px; }"
        "QPushButton:hover { background-color: #D97706; }");
    m_chartBtn->setFixedHeight(38);

    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setStyleSheet(
        "QPushButton { background-color: #E5E7EB; color: #374151; font-size: 14px; font-weight: bold;"
        "border-radius: 8px; padding: 8px 18px; }"
        "QPushButton:hover { background-color: #D1D5DB; }");
    refreshBtn->setFixedHeight(38);

    topBar->addWidget(m_addBtn);
    topBar->addWidget(m_editBtn);
    topBar->addWidget(m_deleteBtn);
    topBar->addWidget(sep1);
    topBar->addWidget(m_exportBtn);
    topBar->addWidget(m_importBtn);
    topBar->addWidget(sep2);
    topBar->addWidget(m_chartBtn);
    topBar->addWidget(refreshBtn);
    topBar->addStretch();

    layout->addLayout(topBar);

    // ========== 第二行：搜索栏，独占一行 ==========
    QHBoxLayout *searchBar = new QHBoxLayout;
    searchBar->setSpacing(8);

    QLabel *searchLabel = new QLabel("搜索:");
    searchLabel->setStyleSheet("font-weight: bold; font-size: 15px; color: #374151;");

    m_searchEdit = new QLineEdit;
    m_searchEdit->setObjectName("searchBox");
    m_searchEdit->setPlaceholderText("输入学号/姓名/班级/电话搜索...");
    m_searchEdit->setFixedHeight(38);

    searchBar->addWidget(searchLabel);
    searchBar->addWidget(m_searchEdit, 1);  // stretch factor 1，填满整行

    layout->addLayout(searchBar);

    // ========== 数据表格 ==========
    m_model = new StudentModel(this);

    m_tableView = new QTableView;
    m_tableView->setModel(m_model);
    m_tableView->setItemDelegate(new StudentDelegate(this));
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setShowGrid(true);
    m_tableView->verticalHeader()->setVisible(true);
    m_tableView->verticalHeader()->setDefaultSectionSize(42);
    m_tableView->setSortingEnabled(false);

    // 列宽设置
    QHeaderView *header = m_tableView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setStretchLastSection(true);
    m_tableView->setColumnWidth(StudentModel::ColId, 50);
    m_tableView->setColumnWidth(StudentModel::ColStudentId, 90);
    m_tableView->setColumnWidth(StudentModel::ColName, 80);
    m_tableView->setColumnWidth(StudentModel::ColGender, 50);
    m_tableView->setColumnWidth(StudentModel::ColAge, 50);
    m_tableView->setColumnWidth(StudentModel::ColClassName, 160);
    m_tableView->setColumnWidth(StudentModel::ColPhone, 110);
    m_tableView->setColumnWidth(StudentModel::ColEmail, 160);

    layout->addWidget(m_tableView);
    setCentralWidget(central);

    // 按钮信号连接
    connect(m_addBtn, &QPushButton::clicked, this, &MainWindow::onAddStudent);
    connect(m_editBtn, &QPushButton::clicked, this, &MainWindow::onEditStudent);
    connect(m_deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteStudent);
    connect(m_exportBtn, &QPushButton::clicked, this, &MainWindow::onExportCsv);
    connect(m_importBtn, &QPushButton::clicked, this, &MainWindow::onBatchImport);
    connect(m_chartBtn, &QPushButton::clicked, this, &MainWindow::onShowCharts);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefresh);
}

// ==================== 状态栏 ====================
void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("就绪");
    m_countLabel = new QLabel;
    m_roleLabel = new QLabel;

    QString roleText = (m_role == "admin") ? "管理员" : "普通用户";
    m_roleLabel->setText(QString("当前用户: %1 [%2]").arg(m_username).arg(roleText));
    m_roleLabel->setStyleSheet(
        QString("color: %1; font-weight: bold; padding: 0 14px; font-size: 13px;")
            .arg(m_role == "admin" ? "#F87171" : "#A5B4FC")
    );

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_countLabel);
    statusBar()->addPermanentWidget(m_roleLabel);
}
// ==================== 信号连接 ====================
void MainWindow::setupConnections()
{
    // 搜索框实时搜索 — 搜索框现在在 central widget 中, 不再受 QToolBar 干扰
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearch);

    // 表格双击编辑
    connect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::onTableDoubleClicked);

    // 表格选择变化
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onSelectionChanged);
}

// ==================== 学生CRUD操作 ====================
void MainWindow::onAddStudent()
{
    if (m_role != "admin") {
        QMessageBox::warning(this, "权限不足", "普通用户无权添加学生信息!");
        return;
    }

    StudentDialog dlg(this);
    dlg.setWindowTitle("添加学生信息");

    // 预设空数据
    StudentInfo emptyStu;
    dlg.setStudent(emptyStu);

    if (dlg.exec() == QDialog::Accepted) {
        StudentInfo stu = dlg.getStudent();
        if (Database::instance()->addStudent(stu)) {
            m_model->loadData();
            updateStatusBar();
            m_statusLabel->setText("已添加学生: " + stu.name);
        } else {
            QMessageBox::critical(this, "错误", "添加学生失败，学号可能重复!");
        }
    }
}

void MainWindow::onEditStudent()
{
    if (m_role != "admin") {
        QMessageBox::warning(this, "权限不足", "普通用户无权修改学生信息!");
        return;
    }

    QModelIndex index = m_tableView->currentIndex();
    if (!index.isValid()) return;

    int row = index.row();
    StudentInfo stu = m_model->studentAt(row);

    StudentDialog dlg(this);
    dlg.setStudent(stu);

    if (dlg.exec() == QDialog::Accepted) {
        StudentInfo updated = dlg.getStudent();
        if (Database::instance()->updateStudent(updated)) {
            m_model->loadData();
            updateStatusBar();
            m_statusLabel->setText("已更新学生: " + updated.name);
        } else {
            QMessageBox::critical(this, "错误", "更新学生信息失败!");
        }
    }
}

void MainWindow::onDeleteStudent()
{
    if (m_role != "admin") {
        QMessageBox::warning(this, "权限不足", "普通用户无权删除学生信息!");
        return;
    }

    QModelIndex index = m_tableView->currentIndex();
    if (!index.isValid()) return;

    int row = index.row();
    StudentInfo stu = m_model->studentAt(row);

    int ret = QMessageBox::question(this, "确认删除",
                                     QString("确定要删除学生 \"%1\" (%2) 吗？\n此操作不可恢复!")
                                         .arg(stu.name).arg(stu.studentId),
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (Database::instance()->deleteStudent(stu.id)) {
            m_model->loadData();
            updateStatusBar();
            m_statusLabel->setText("已删除学生: " + stu.name);
        } else {
            QMessageBox::critical(this, "错误", "删除学生失败!");
        }
    }
}

void MainWindow::onRefresh()
{
    m_searchEdit->clear();  // 刷新时清空搜索框，恢复显示全部
    m_model->loadData();
    updateStatusBar();
    m_statusLabel->setText("数据已刷新");
}

// ==================== 搜索 — setRowHidden 方案 ====================
// 不碰 model、不动数据、不发信号，只是告诉 QTableView 哪些行不显示
// 输入法焦点完全不受影响
void MainWindow::onSearch(const QString &keyword)
{
    QString kw = keyword.trimmed();
    int visibleCount = 0;

    // 停掉表格刷新，把所有 setRowHidden 打包成一次重绘，避免中途绘制打断 IME
    m_tableView->setUpdatesEnabled(false);

    for (int i = 0; i < m_model->rowCount(); i++) {
        if (kw.isEmpty()) {
            m_tableView->setRowHidden(i, false);
            visibleCount++;
        } else {
            QString sid   = m_model->data(m_model->index(i, StudentModel::ColStudentId), Qt::DisplayRole).toString();
            QString name  = m_model->data(m_model->index(i, StudentModel::ColName),      Qt::DisplayRole).toString();
            QString cls   = m_model->data(m_model->index(i, StudentModel::ColClassName),  Qt::DisplayRole).toString();
            QString phone = m_model->data(m_model->index(i, StudentModel::ColPhone),      Qt::DisplayRole).toString();

            bool match = sid.contains(kw, Qt::CaseInsensitive)
                      || name.contains(kw, Qt::CaseInsensitive)
                      || cls.contains(kw, Qt::CaseInsensitive)
                      || phone.contains(kw, Qt::CaseInsensitive);

            m_tableView->setRowHidden(i, !match);
            if (match) visibleCount++;
        }
    }

    m_tableView->setUpdatesEnabled(true);

    int total = Database::instance()->getTotalCount();
    m_countLabel->setText(QString("共 %1 条记录 | 总计 %2 人").arg(visibleCount).arg(total));
    m_countLabel->setStyleSheet("padding: 0 12px; font-weight: bold; color: #CBD5E1;");
}

void MainWindow::onTableDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    if (m_role != "admin") return;  // 普通用户双击不触发编辑
    onEditStudent();
}

void MainWindow::onSelectionChanged()
{
    bool hasSelection = m_tableView->selectionModel()->hasSelection();
    m_editBtn->setEnabled(hasSelection);
    m_deleteBtn->setEnabled(hasSelection);
}

// ==================== 导入导出 ====================
void MainWindow::onExportCsv()
{
    if (m_model->rowCount() == 0) {
        QMessageBox::information(this, "提示", "没有数据可导出!");
        return;
    }

    QString defaultName = QString("学生信息_%1.csv")
                              .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QString filePath = QFileDialog::getSaveFileName(
        this, "导出CSV文件", defaultName, "CSV 文件 (*.csv);;所有文件 (*)");

    if (filePath.isEmpty()) return;

    // 收集所有数据
    QList<StudentInfo> allData;
    for (int i = 0; i < m_model->rowCount(); i++) {
        allData.append(m_model->studentAt(i));
    }

    // 同步导出（学生数量少，无需多线程）
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString errorMsg;
    bool ok = exportStudentsToCsv(filePath, allData, errorMsg);
    QApplication::restoreOverrideCursor();

    if (ok) {
        QMessageBox::information(this, "导出成功",
            QString("成功导出 %1 条学生记录到:\n%2").arg(allData.size()).arg(filePath));
        m_statusLabel->setText("导出完成: " + filePath);
    } else {
        QMessageBox::critical(this, "导出失败", errorMsg);
    }
}

void MainWindow::onBatchImport()
{
    if (m_role != "admin") {
        QMessageBox::warning(this, "权限不足", "普通用户无权批量导入数据!");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        this, "选择CSV文件", "", "CSV 文件 (*.csv);;所有文件 (*)");

    if (filePath.isEmpty()) return;

    // 同步导入
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString errorMsg;
    int skipped = 0;
    int count = importStudentsFromCsv(filePath, errorMsg, skipped);
    QApplication::restoreOverrideCursor();

    if (count >= 0) {
        QString msg = QString("成功导入 %1 条记录").arg(count);
        if (skipped > 0) {
            msg += QString("，跳过 %1 条（学号已存在）").arg(skipped);
        }
        QMessageBox::information(this, "导入完成", msg);
        m_model->loadData();
        updateStatusBar();
        m_statusLabel->setText(QString("批量导入: 新增 %1 条，跳过 %2 条").arg(count).arg(skipped));
    } else {
        QMessageBox::critical(this, "导入失败", errorMsg);
    }
}

// ==================== 图表 ====================
void MainWindow::onShowCharts()
{
    if (m_model->rowCount() == 0) {
        QMessageBox::information(this, "提示", "没有学生数据，请先添加记录!");
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("数据统计分析");
    dlg.resize(860, 600);
    dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    ChartWidget *chart = new ChartWidget(&dlg);
    chart->setDarkTheme(m_isDarkTheme);
    layout->addWidget(chart);

    QPushButton *refreshBtn = new QPushButton("刷新数据");
    layout->addWidget(refreshBtn);
    connect(refreshBtn, &QPushButton::clicked, chart, &ChartWidget::updateData);

    dlg.exec();
}

// ==================== 主题切换 ====================
void MainWindow::onToggleTheme()
{
    m_isDarkTheme = !m_isDarkTheme;
    applyTheme(m_isDarkTheme);
    m_statusLabel->setText(m_isDarkTheme ? "已切换到深色主题" : "已切换到浅色主题");
}

void MainWindow::applyTheme(bool dark)
{
    if (dark) {
        qApp->setStyleSheet(R"(
            /* ===== 深色主题 (Premium Dark) ===== */
            * {
                font-family: "Microsoft YaHei", "PingFang SC", "Segoe UI", sans-serif;
                font-size: 14px;
            }

            QMainWindow { background-color: #0F1119; }
            QDialog { background-color: #161822; }

            QMenuBar {
                background-color: #0A0C14;
                color: #D1D5DB;
                padding: 5px 10px;
                font-size: 14px;
                border-bottom: 1px solid #1E2030;
            }
            QMenuBar::item { padding: 7px 18px; border-radius: 5px; font-size: 14px; }
            QMenuBar::item:selected { background-color: #1E2030; color: #FFFFFF; }
            QMenu {
                background-color: #1A1C28;
                color: #E5E7EB;
                border: 1px solid #2D3148;
                border-radius: 8px;
                padding: 8px 4px;
                margin-top: 6px;
            }
            QMenu::item { padding: 9px 36px 9px 22px; border-radius: 5px; font-size: 14px; }
            QMenu::item:selected { background-color: #252740; color: #A5B4FC; }
            QMenu::separator { background-color: #2D3148; height: 1px; margin: 5px 10px; }

            QToolBar {
                background-color: #161822;
                border-bottom: 1px solid #1E2030;
                spacing: 10px;
                padding: 8px 16px;
            }

            QTableView {
                background-color: #1A1C28;
                alternate-background-color: #151720;
                selection-background-color: #252740;
                selection-color: #E0E7FF;
                gridline-color: #1E2030;
                border: 1px solid #1E2030;
                border-radius: 10px;
                color: #E5E7EB;
                font-size: 14px;
                outline: none;
            }
            QTableView::item { padding: 10px 14px; }
            QTableView::item:selected { background-color: #3730A3; color: #E0E7FF; }
            QHeaderView::section {
                background-color: #151720;
                color: #9CA3AF;
                padding: 12px 10px;
                border: none;
                border-bottom: 2px solid #6366F1;
                font-weight: bold;
                font-size: 14px;
            }
            QHeaderView::section:hover { background-color: #1E2030; }

            QPushButton {
                background-color: #6366F1;
                color: white;
                border: none;
                border-radius: 8px;
                padding: 10px 26px;
                font-size: 14px;
                font-weight: bold;
                min-width: 80px;
            }
            QPushButton:hover { background-color: #5558E3; }
            QPushButton:pressed { background-color: #484BD1; }
            QPushButton:disabled { background-color: #2D3148; color: #6B7280; }
            QPushButton#btnDelete { background-color: #DC2626; }
            QPushButton#btnDelete:hover { background-color: #B91C1C; }
            QPushButton#btnDelete:pressed { background-color: #991B1B; }
            QPushButton#btnExport { background-color: #059669; }
            QPushButton#btnExport:hover { background-color: #047857; }
            QPushButton#btnExport:pressed { background-color: #065F46; }

            QLineEdit, QComboBox, QSpinBox, QDateEdit {
                background-color: #151720;
                color: #E5E7EB;
                border: 1.5px solid #2D3148;
                border-radius: 8px;
                padding: 10px 14px;
                font-size: 14px;
                min-height: 22px;
                selection-background-color: #6366F1;
            }
            QLineEdit:focus, QComboBox:focus-visible, QSpinBox:focus, QDateEdit:focus {
                border: 2px solid #818CF8;
                background-color: #1A1C28;
            }
            QLineEdit:hover, QComboBox:hover, QSpinBox:hover, QDateEdit:hover {
                border-color: #4B5563;
            }
            QComboBox {
                padding: 10px 34px 10px 14px;
            }
            QComboBox::drop-down, QDateEdit::drop-down { border: none; width: 30px; }
            QComboBox QAbstractItemView {
                background-color: #1A1C28;
                border: 1px solid #2D3148;
                border-radius: 8px;
                selection-background-color: #252740;
                color: #E5E7EB;
                padding: 6px;
                font-size: 14px;
                outline: none;
            }
            QSpinBox::up-button, QSpinBox::down-button {
                border: none;
                width: 24px;
                background-color: #1E2030;
            }

            QGroupBox {
                border: 1px solid #2D3148;
                border-radius: 10px;
                margin-top: 18px;
                padding: 24px 18px 14px 18px;
                background-color: #161822;
                color: #E5E7EB;
                font-size: 15px;
                font-weight: bold;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 18px;
                padding: 0 10px;
                color: #A5B4FC;
                font-size: 15px;
            }

            QLabel { color: #D1D5DB; font-size: 14px; }

            QStatusBar {
                background-color: #0A0C14;
                border-top: none;
                font-size: 13px;
                color: #6B7280;
                padding: 6px 10px;
                min-height: 26px;
            }
            QStatusBar::item { border: none; }

            QLineEdit#searchBox {
                border: 2px solid #2D3148;
                border-radius: 22px;
                padding: 10px 18px 10px 40px;
                font-size: 14px;
                min-width: 300px;
                background-color: #151720;
                color: #E5E7EB;
            }
            QLineEdit#searchBox:focus {
                border: 2px solid #818CF8;
                background-color: #1A1C28;
            }

            QTabWidget::pane {
                border: 1px solid #2D3148;
                border-radius: 10px;
                background-color: #1A1C28;
            }
            QTabBar::tab {
                background-color: #151720;
                border: 1px solid #2D3148;
                padding: 10px 22px;
                margin-right: 3px;
                color: #9CA3AF;
                font-size: 14px;
            }
            QTabBar::tab:selected { background-color: #6366F1; color: white; border-color: #6366F1; }
            QTabBar::tab:hover:!selected { background-color: #252740; color: #A5B4FC; }

            QProgressBar {
                background-color: #2D3148;
                border: none;
                border-radius: 8px;
                height: 24px;
                text-align: center;
                font-size: 13px;
                color: #D1D5DB;
            }
            QProgressBar::chunk { background-color: #6366F1; border-radius: 8px; }

            QScrollBar:vertical { background-color: #151720; width: 10px; border-radius: 5px; margin: 3px; }
            QScrollBar::handle:vertical { background-color: #374151; border-radius: 5px; min-height: 36px; }
            QScrollBar::handle:vertical:hover { background-color: #4B5563; }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
            QScrollBar:horizontal { background-color: #151720; height: 10px; border-radius: 5px; margin: 3px; }
            QScrollBar::handle:horizontal { background-color: #374151; border-radius: 5px; min-width: 36px; }
            QScrollBar::handle:horizontal:hover { background-color: #4B5563; }
            QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }

            QToolTip {
                background-color: #374151;
                color: #F9FAFB;
                border: 1px solid #4B5563;
                border-radius: 8px;
                padding: 8px 14px;
                font-size: 13px;
            }

            QCalendarWidget { background-color: #1A1C28; }

            /* 用户管理表格 */
            QTableWidget {
                background-color: #1A1C28;
                alternate-background-color: #151720;
                gridline-color: #1E2030;
                border: 1px solid #1E2030;
                border-radius: 10px;
                font-size: 14px;
                color: #E5E7EB;
                outline: none;
            }
            QTableWidget::item { padding: 8px 12px; }
            QTableWidget::item:selected { background-color: #3730A3; color: #E0E7FF; }
        )");
    } else {
        // 浅色主题
        loadStyleSheet(":/style.qss");
    }
}

void MainWindow::loadStyleSheet(const QString &path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString style = file.readAll();
        qApp->setStyleSheet(style);
        file.close();
    }
}

// ==================== 数据库备份还原 ====================
void MainWindow::onBackupDatabase()
{
    if (m_role != "admin") {
        QMessageBox::warning(this, "权限不足", "普通用户无权备份数据库!");
        return;
    }

    QString defaultName = QString("student_system_backup_%1.db")
                              .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QString filePath = QFileDialog::getSaveFileName(
        this, "备份数据库", defaultName, "数据库文件 (*.db);;所有文件 (*)");

    if (filePath.isEmpty()) return;

    if (Database::instance()->backupDatabase(filePath)) {
        QMessageBox::information(this, "备份成功",
                                  QString("数据库已备份到:\n%1").arg(filePath));
        m_statusLabel->setText("数据库备份完成");
    } else {
        QMessageBox::critical(this, "备份失败", "数据库备份失败!");
    }
}

void MainWindow::onRestoreDatabase()
{
    if (m_role != "admin") {
        QMessageBox::warning(this, "权限不足", "普通用户无权还原数据库!");
        return;
    }

    int ret = QMessageBox::warning(this, "还原数据库",
                                    "还原数据库将覆盖当前所有数据！\n确定要继续吗？",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    QString filePath = QFileDialog::getOpenFileName(
        this, "选择备份文件", "", "数据库文件 (*.db);;所有文件 (*)");

    if (filePath.isEmpty()) return;

    if (Database::instance()->restoreDatabase(filePath)) {
        QMessageBox::information(this, "还原成功", "数据库已还原! 请重启程序以生效。");
        m_model->loadData();
        updateStatusBar();
    } else {
        QMessageBox::critical(this, "还原失败", "数据库还原失败!");
    }
}

// ==================== 用户管理 ====================
void MainWindow::onUserManagement()
{
    UserManagementDialog dlg(this);
    dlg.exec();
}

// ==================== 权限控制 ====================
void MainWindow::applyPermissions()
{
    bool isAdmin = (m_role == "admin");

    // 工具栏按钮：普通用户隐藏 增/删/改/导入
    m_addBtn->setVisible(isAdmin);
    m_editBtn->setVisible(isAdmin);
    m_deleteBtn->setVisible(isAdmin);
    m_importBtn->setVisible(isAdmin);

    // 普通用户禁止双击编辑
    if (!isAdmin) {
        disconnect(m_tableView, &QTableView::doubleClicked, this, &MainWindow::onTableDoubleClicked);
    }
}

// ==================== 辅助函数 ====================
void MainWindow::updateStatusBar()
{
    int count = m_model->rowCount();
    int total = Database::instance()->getTotalCount();
    m_countLabel->setText(QString("共 %1 条记录 | 总计 %2 人").arg(count).arg(total));
    m_countLabel->setStyleSheet("padding: 0 12px; font-weight: bold; color: #CBD5E1;");
}
