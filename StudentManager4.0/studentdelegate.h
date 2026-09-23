#ifndef STUDENTDELEGATE_H
#define STUDENTDELEGATE_H

#include <QStyledItemDelegate>

class StudentDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StudentDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

#endif // STUDENTDELEGATE_H
