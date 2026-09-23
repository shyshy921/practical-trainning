#ifndef STUDENTMODEL_H
#define STUDENTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "database.h"

class StudentModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        ColId = 0,
        ColStudentId,
        ColName,
        ColGender,
        ColAge,
        ColClassName,
        ColPhone,
        ColEmail,
        ColAddress,
        ColEnrollmentDate,
        ColCount
    };

    explicit StudentModel(QObject *parent = nullptr);

    // 必须实现的虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // 数据操作
    void loadData();
    void searchData(const QString &keyword);
    void appendStudent(const StudentInfo &stu);
    void updateStudentRow(int row, const StudentInfo &stu);
    void removeStudentRow(int row);
    StudentInfo studentAt(int row) const;

private:
    QList<StudentInfo> m_students;
    static const QStringList m_headers;
};

#endif // STUDENTMODEL_H
