#ifndef USERMANAGEMENTDIALOG_H
#define USERMANAGEMENTDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include "database.h"

class UserManagementDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UserManagementDialog(QWidget *parent = nullptr);

private slots:
    void loadUsers();
    void onAddUser();
    void onDeleteUser();

private:
    void setupUI();

    QTableWidget *m_userTable;
    QPushButton *m_addBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_refreshBtn;
};

#endif // USERMANAGEMENTDIALOG_H
