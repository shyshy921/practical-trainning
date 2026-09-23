#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString getUserRole() const { return m_role; }
    QString getUsername() const { return m_username; }

private slots:
    void onLogin();

private:
    void setupUI();

    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QString m_role;
    QString m_username;
};

#endif // LOGINDIALOG_H
