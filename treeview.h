#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QObject>
#include <QTreeView>
#include <QStandardItemModel>
#include "myfilewatcher.h"

class TreeView : public QTreeView
{
    Q_OBJECT
  public:
    QStandardItemModel *model;
    TreeView(QWidget *parent = nullptr);
    ~TreeView();

  private:
    int charactersNum       = 0;       //有几个角色存档
    MyFileWatcher *aWatcher = nullptr; //监控文件
    void InitView();                   //初始化图
    bool CreateTemp();                 //创建一个存储目录
  private slots:
    void on_aNewSaveAdd(int charaNum);         //当一个新存档增加
    void on_treeViewMenu(const QPoint &point); //树型视图 右键菜单
    void on_treeMenuExpand(bool checked);      //打开子节点
    void on_treeMenuCollapse(bool checked);    //折叠子节点
    void on_treeMenuLoadSave();                //加载存档
    void on_treeMenuDelSave();                 //删除存档
    void on_doubleClicked(const QModelIndex &index);
};

#endif // TREEVIEW_H
