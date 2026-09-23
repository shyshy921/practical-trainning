#ifndef STUDENTFILTERPROXYMODEL_H
#define STUDENTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class StudentFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit StudentFilterProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // STUDENTFILTERPROXYMODEL_H
