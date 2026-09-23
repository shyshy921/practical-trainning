#include "studentdelegate.h"
#include "studentmodel.h"

#include <QPainter>
#include <QApplication>
#include <QColor>

StudentDelegate::StudentDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void StudentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 性别列着色：男生靛蓝色，女生玫红色
    if (index.column() == StudentModel::ColGender) {
        QString text = index.data(Qt::DisplayRole).toString();
        if (text == "男") {
            opt.palette.setColor(QPalette::Text, QColor("#5B6AF0"));
        } else if (text == "女") {
            opt.palette.setColor(QPalette::Text, QColor("#F43F5E"));
        }
    }

    // 交替行背景由 QSS alternate-background-color 控制
    // 选中高亮由 QSS selection-background-color 控制
    // delegate 不做额外背景干预，保持一致性

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
}
