#include "logindialog.h"
#include "database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QApplication>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void LoginDialog::setupUI()
{
    setWindowTitle("学生信息管理系统 - 登录");
    setFixedSize(460, 440);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 顶部标题区域
    QWidget *headerWidget = new QWidget;
    headerWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "stop:0 #1B1F35, stop:0.5 #252C48, stop:1 #312E81);"
    );
    headerWidget->setFixedHeight(130);
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setAlignment(Qt::AlignCenter);

    QLabel *iconLabel = new QLabel;
    iconLabel->setText("学生管理系统");
    iconLabel->setStyleSheet("color: white; font-size: 26px; font-weight: bold; letter-spacing: 2px;");
    iconLabel->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(iconLabel);

    QLabel *subtitle = new QLabel("Student Information Management System");
    subtitle->setStyleSheet("color: rgba(255,255,255,0.5); font-size: 13px; margin-top: 4px;");
    subtitle->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(subtitle);

    mainLayout->addWidget(headerWidget);

    // 表单区域
    QWidget *formWidget = new QWidget;
    QVBoxLayout *formLayout = new QVBoxLayout(formWidget);
    formLayout->setContentsMargins(44, 34, 44, 24);
    formLayout->setSpacing(18);

    // 用户名
    QLabel *userLabel = new QLabel("用户名");
    userLabel->setStyleSheet("font-weight: bold; font-size: 15px; color: #374151;");
    m_usernameEdit = new QLineEdit;
    m_usernameEdit->setPlaceholderText("请输入用户名");
    m_usernameEdit->setFixedHeight(44);
    m_usernameEdit->setStyleSheet("font-size: 15px;");
    formLayout->addWidget(userLabel);
    formLayout->addWidget(m_usernameEdit);

    // 密码
    QLabel *pwdLabel = new QLabel("密码");
    pwdLabel->setStyleSheet("font-weight: bold; font-size: 15px; color: #374151;");
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setFixedHeight(44);
    m_passwordEdit->setStyleSheet("font-size: 15px;");
    formLayout->addWidget(pwdLabel);
    formLayout->addWidget(m_passwordEdit);

    // 登录按钮
    m_loginBtn = new QPushButton("登  录");
    m_loginBtn->setFixedHeight(48);
    m_loginBtn->setCursor(Qt::PointingHandCursor);
    m_loginBtn->setStyleSheet(
        "QPushButton { background-color: #5B6AF0; color: white; border-radius: 10px;"
        "font-size: 16px; font-weight: bold; letter-spacing: 4px; }"
        "QPushButton:hover { background-color: #4C5CE6; }"
        "QPushButton:pressed { background-color: #3D4ED4; }"
    );
    formLayout->addSpacing(6);
    formLayout->addWidget(m_loginBtn);

    // 提示信息
    QLabel *hint = new QLabel("默认账户: admin / admin123  (管理员)\n         user  / user123    (普通用户)");
    hint->setStyleSheet("color: #9CA3AF; font-size: 12px; padding: 12px;"
                        "background-color: #F1F5F9; border-radius: 8px;");
    hint->setAlignment(Qt::AlignCenter);
    hint->setWordWrap(true);
    formLayout->addWidget(hint);

    mainLayout->addWidget(formWidget);

    // 底部
    mainLayout->addStretch();

    // 连接信号
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
    connect(m_usernameEdit, &QLineEdit::returnPressed, [this]() {
        m_passwordEdit->setFocus();
    });

    // 全局样式
    setStyleSheet("QDialog { background-color: #F5F6FA; border-radius: 12px; }");
}

void LoginDialog::onLogin()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空!");
        return;
    }

    QString role;
    if (Database::instance()->verifyLogin(username, password, role)) {
        m_username = username;
        m_role = role;
        accept();
    } else {
        QMessageBox::critical(this, "登录失败", "用户名或密码错误，请重试!");
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}
