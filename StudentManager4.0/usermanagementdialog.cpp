#include "usermanagementdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLabel>

UserManagementDialog::UserManagementDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadUsers();
}

void UserManagementDialog::setupUI()
{
    setWindowTitle("用户管理");
    setFixedSize(580, 460);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // 标题
    QLabel *titleLabel = new QLabel("系统用户管理");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #1F2937; padding: 0 0 8px 0;");
    mainLayout->addWidget(titleLabel);

    // 用户表格
    m_userTable = new QTableWidget;
    m_userTable->setColumnCount(3);
    m_userTable->setHorizontalHeaderLabels({"用户名", "角色", "ID"});
    m_userTable->setColumnHidden(2, true); // 隐藏ID列
    m_userTable->horizontalHeader()->setStretchLastSection(true);
    m_userTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_userTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_userTable->setAlternatingRowColors(true);
    mainLayout->addWidget(m_userTable);

    // 按钮区
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(14);

    m_addBtn = new QPushButton("添加用户");
    m_addBtn->setObjectName("btnExport");
    m_addBtn->setStyleSheet("QPushButton { font-size: 14px; font-weight: bold; padding: 10px 22px; }");

    m_deleteBtn = new QPushButton("删除用户");
    m_deleteBtn->setObjectName("btnDelete");
    m_deleteBtn->setStyleSheet("QPushButton { font-size: 14px; font-weight: bold; padding: 10px 22px; }");

    m_refreshBtn = new QPushButton("刷新列表");
    m_refreshBtn->setStyleSheet(
        "QPushButton { background-color: #E5E7EB; color: #374151; font-size: 14px;"
        "font-weight: bold; padding: 10px 22px; border-radius: 8px; }"
        "QPushButton:hover { background-color: #D1D5DB; }"
    );

    QPushButton *closeBtn = new QPushButton("关闭");
    closeBtn->setStyleSheet("QPushButton { font-size: 14px; font-weight: bold; padding: 10px 22px; }");

    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_deleteBtn);
    btnLayout->addWidget(m_refreshBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);

    // 连接信号
    connect(m_addBtn, &QPushButton::clicked, this, &UserManagementDialog::onAddUser);
    connect(m_deleteBtn, &QPushButton::clicked, this, &UserManagementDialog::onDeleteUser);
    connect(m_refreshBtn, &QPushButton::clicked, this, &UserManagementDialog::loadUsers);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void UserManagementDialog::loadUsers()
{
    QList<UserInfo> users = Database::instance()->getAllUsers();
    m_userTable->setRowCount(users.size());

    for (int i = 0; i < users.size(); i++) {
        const UserInfo &u = users[i];
        m_userTable->setItem(i, 0, new QTableWidgetItem(u.username));

        // 角色显示美化
        QTableWidgetItem *roleItem = new QTableWidgetItem(
            u.role == "admin" ? "管理员" : "普通用户"
        );
        roleItem->setForeground(u.role == "admin" ? QColor("#e74c3c") : QColor("#3498db"));
        QFont font = roleItem->font();
        font.setBold(true);
        roleItem->setFont(font);
        m_userTable->setItem(i, 1, roleItem);

        m_userTable->setItem(i, 2, new QTableWidgetItem(QString::number(u.id)));
    }
}

void UserManagementDialog::onAddUser()
{
    // 弹出添加用户对话框
    QDialog dlg(this);
    dlg.setWindowTitle("添加新用户");
    dlg.setFixedSize(380, 260);

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(24, 20, 24, 16);
    QVBoxLayout *form = new QVBoxLayout;
    form->setSpacing(14);

    // 辅助: 创建对齐行
    auto createRow = [&dlg](const QString &labelText, QWidget *input) -> QHBoxLayout* {
        QHBoxLayout *row = new QHBoxLayout;
        row->setSpacing(12);
        QLabel *label = new QLabel(labelText, &dlg);
        label->setFixedSize(75, 40);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        label->setStyleSheet("font-size: 15px; font-weight: bold; color: #374151;");
        row->addWidget(label);
        row->addWidget(input, 1);
        return row;
    };

    QLineEdit *usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setFixedHeight(40);
    usernameEdit->setStyleSheet("font-size: 15px;");

    QLineEdit *passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedHeight(40);
    passwordEdit->setStyleSheet("font-size: 15px;");

    QComboBox *roleCombo = new QComboBox;
    roleCombo->addItems({"user", "admin"});
    roleCombo->setFixedHeight(40);
    roleCombo->setStyleSheet("font-size: 15px;");

    form->addLayout(createRow("用户名:", usernameEdit));
    form->addLayout(createRow("密码:", passwordEdit));
    form->addLayout(createRow("角色:", roleCombo));
    layout->addLayout(form);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->setStyleSheet("QPushButton { font-size: 14px; padding: 8px 24px; }");
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text().trimmed();
        QString role = roleCombo->currentText();

        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "提示", "用户名和密码不能为空!");
            return;
        }

        if (Database::instance()->userExists(username)) {
            QMessageBox::warning(this, "提示", "该用户名已存在!");
            return;
        }

        if (Database::instance()->addUser(username, password, role)) {
            QMessageBox::information(this, "成功", "用户添加成功!");
            loadUsers();
        } else {
            QMessageBox::critical(this, "错误", "添加用户失败!");
        }
    }
}

void UserManagementDialog::onDeleteUser()
{
    int row = m_userTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的用户!");
        return;
    }

    QString username = m_userTable->item(row, 0)->text();
    if (username == "admin") {
        QMessageBox::warning(this, "提示", "不能删除内置管理员账户!");
        return;
    }

    int ret = QMessageBox::question(this, "确认删除",
                                     QString("确定要删除用户 \"%1\" 吗？").arg(username),
                                     QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        int userId = m_userTable->item(row, 2)->text().toInt();
        if (Database::instance()->deleteUser(userId)) {
            QMessageBox::information(this, "成功", "用户已删除!");
            loadUsers();
        } else {
            QMessageBox::critical(this, "错误", "删除失败!");
        }
    }
}
