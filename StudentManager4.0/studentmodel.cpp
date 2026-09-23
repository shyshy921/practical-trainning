#include "studentmodel.h"

const QStringList StudentModel::m_headers = {
    "ID", "学号", "姓名", "性别", "年龄", "班级", "电话", "邮箱", "地址", "入学日期"
};

StudentModel::StudentModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int StudentModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_students.size();
}

int StudentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColCount;
}

QVariant StudentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_students.size())
        return QVariant();

    const StudentInfo &stu = m_students.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColId:             return stu.id;
        case ColStudentId:      return stu.studentId;
        case ColName:           return stu.name;
        case ColGender:         return stu.gender;
        case ColAge:            return stu.age;
        case ColClassName:      return stu.className;
        case ColPhone:          return stu.phone;
        case ColEmail:          return stu.email;
        case ColAddress:        return stu.address;
        case ColEnrollmentDate: return stu.enrollmentDate;
        default: return QVariant();
        }
    }

    if (role == Qt::TextAlignmentRole) {
        // 数值列居中
        if (index.column() == ColId || index.column() == ColAge) {
            return int(Qt::AlignCenter);
        }
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    // 性别颜色和背景色通过 StudentDelegate 处理，不在此返回 QColor

    return QVariant();
}

QVariant StudentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section >= 0 && section < m_headers.size())
            return m_headers.at(section);
    }
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section + 1; // 行号从1开始
    }
    return QVariant();
}

Qt::ItemFlags StudentModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void StudentModel::loadData()
{
    beginResetModel();
    m_students = Database::instance()->getAllStudents();
    endResetModel();
}

void StudentModel::searchData(const QString &keyword)
{
    beginResetModel();
    if (keyword.isEmpty()) {
        m_students = Database::instance()->getAllStudents();
    } else {
        m_students = Database::instance()->searchStudents(keyword);
    }
    endResetModel();
}

void StudentModel::appendStudent(const StudentInfo &stu)
{
    beginInsertRows(QModelIndex(), m_students.size(), m_students.size());
    m_students.append(stu);
    endInsertRows();
}

void StudentModel::updateStudentRow(int row, const StudentInfo &stu)
{
    if (row < 0 || row >= m_students.size()) return;
    m_students[row] = stu;
    emit dataChanged(index(row, 0), index(row, ColCount - 1));
}

void StudentModel::removeStudentRow(int row)
{
    if (row < 0 || row >= m_students.size()) return;
    beginRemoveRows(QModelIndex(), row, row);
    m_students.removeAt(row);
    endRemoveRows();
}

StudentInfo StudentModel::studentAt(int row) const
{
    if (row >= 0 && row < m_students.size())
        return m_students.at(row);
    return StudentInfo();
}
