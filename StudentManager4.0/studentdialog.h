#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QPushButton>
#include "database.h"

class StudentDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StudentDialog(QWidget *parent = nullptr);

    void setStudent(const StudentInfo &stu);
    StudentInfo getStudent() const;

private slots:
    void onSubmit();

private:
    void setupUI();
    bool validateInput();

    QLineEdit *m_studentIdEdit;
    QLineEdit *m_nameEdit;
    QComboBox *m_genderCombo;
    QSpinBox *m_ageSpin;
    QLineEdit *m_classEdit;
    QLineEdit *m_phoneEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_addressEdit;
    QDateEdit *m_enrollmentDateEdit;

    StudentInfo m_originalStu;
    bool m_isEditMode = false;
};

#endif // STUDENTDIALOG_H
