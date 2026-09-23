#include "studentfilterproxymodel.h"
#include <QAbstractItemModel>

StudentFilterProxyModel::StudentFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

bool StudentFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QRegExp regex = filterRegExp();
    if (regex.isEmpty())
        return true;

    QAbstractItemModel *model = sourceModel();
    if (!model)
        return true;

    // 在 学号、姓名、班级、电话 四个列中模糊搜索
    for (int col : {1, 2, 5, 6}) {
        QModelIndex idx = model->index(sourceRow, col, sourceParent);
        QString text = model->data(idx).toString();
        if (text.contains(regex))
            return true;
    }
    return false;
}
