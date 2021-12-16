#include "backgrounddelegate.h"

BackgroundDelegate::BackgroundDelegate(QWidget *parent) : QItemDelegate(parent) {}

QSize BackgroundDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index)
{
    return QSize(option.rect.width(), 100);
}
