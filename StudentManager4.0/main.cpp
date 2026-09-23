#include <QApplication>
#include <QMessageBox>
#include "database.h"
#include "logindialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("StudentManageSystem");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Cheng");

    // 初始化数据库
    if (!Database::instance()->initialize()) {
        QMessageBox::critical(nullptr, "错误",
                              "数据库初始化失败!\n请检查程序运行目录权限。");
        return -1;
    }

    // 显示登录对话框
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0; // 用户取消登录
    }

    // 登录成功，显示主窗口
    MainWindow mainWindow(loginDialog.getUsername(), loginDialog.getUserRole());
    mainWindow.show();

    return app.exec();
}
