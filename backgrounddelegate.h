#ifndef BACKGROUNDDELEGATE_H
#define BACKGROUNDDELEGATE_H

#include <QItemDelegate>

class BackgroundDelegate : public QItemDelegate
{
    Q_OBJECT
  public:
    BackgroundDelegate(QWidget *parent = nullptr);
    //编辑时候的空间
    // virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index);

    //编辑的时候设置数据到上面创建的edit
    // virtual void setEditorData(QWidget *editor, const QModelIndex &index);
    // virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index);
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index);
    //设置编辑控件的位置和大小、样式等
    // virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index);
};

#endif // BACKGROUNDDELEGATE_H
