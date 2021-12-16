#include "treeview.h"
#include <QHeaderView>
#include <QDir>
#include "myfilewatcher.h"
#include <QDebug>
#include <QMenu>
#include <QThread>
#include "backgrounddelegate.h"
#include "previewdialog.h"
#pragma execution_character_set("utf-8")

// 设置数据类型
#define PATH_ROLE Qt::UserRole + 1
#define TYPE_ROLE Qt::UserRole + 2
TreeView::TreeView(QWidget *parent) : QTreeView(parent)
{
    //获取角色数
    charactersNum = MyFileWatcher::GetCharactersNum();
    //创建存档目录结构
    CreateTemp();
    //初始化视图
    InitView();
    //文件夹监控
    aWatcher = new MyFileWatcher(this);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    //绑定：检测到文件变化
    connect(aWatcher, SIGNAL(exitSaveAdd(int)), this, SLOT(on_aNewSaveAdd(int)));
    //绑定鼠标菜单
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_treeViewMenu(QPoint)));
    //绑定：弹出图片显示对话框
    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_doubleClicked(QModelIndex)));
}

/*--析构--*/
TreeView::~TreeView()
{
    qDebug() << "TreeView delete";
}

void TreeView::InitView()
{
    model = new QStandardItemModel(4, 2);
    //设置头
    QHeaderView *header = this->header();
    //设置表头为自适应
    header->setSectionResizeMode(QHeaderView::Stretch);
    model->setHeaderData(0, Qt::Horizontal, "文件夹");
    model->setHeaderData(1, Qt::Horizontal, "预览图片");
    //显示设置
    //设置拖拽模式
    this->setEditTriggers(QAbstractItemView::NoEditTriggers); //不允许编辑
    this->setDragDropMode(QAbstractItemView::DragDrop);
    this->setDragEnabled(true);
    //获取目前有几个角色
    int charaCount = charactersNum;
    for (int i = 1; i < charaCount + 1; i++)
    {
        //角色根节点
        QStandardItem *charaXItem = new QStandardItem("角色" + QString::number(i));
        this->setItemDelegateForRow(charaXItem->index().row(), new BackgroundDelegate(this));
        //根节点属性设置为parent
        charaXItem->setData("parent", TYPE_ROLE);
        charaXItem->setData("#bcf000", Qt::ForegroundRole);
        //添加图片到i1栏，
        //导入图片
        //   QString imgPath = MyFileWatcher::GetPreviewImage(i, 0);

        //   QPixmap pix(imgPath);
        //   QPixmap pixAdjust        = pix.scaled(400, 200, Qt::IgnoreAspectRatio);
        //   QIcon *icon              = new QIcon(pixAdjust);
        //   QStandardItem *imageItem = new QStandardItem(*icon, "");

        //导入系列存档
        // 1.获取角色路径
        QString charPath = MyFileWatcher::GetPaths().value("progChaPath") + QString::number(i);
        QDir charDir(charPath);
        if (!charDir.exists())
        {
            qDebug() << "char Dir nt exists:[" << __FUNCTION__;
            return;
        }
        // 2.看当前这个目录下的文件夹
        //用名称来排序
        QFileInfoList infoList = charDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::Reversed);
        //如果里面没有存档，那就什么也不做
        if (infoList.count() < 1)
        {
        }
        else
        {
            //获取文件夹
            for (int i = 0; i < infoList.count(); i++)
            {
                QString folderName = infoList.at(i).baseName(); //单纯的文件夹名
                //这个文件夹代表的路径
                QString folderPath  = infoList.at(i).absoluteFilePath();
                QStandardItem *item = new QStandardItem(folderName);
                item->setData(folderPath, Qt::UserRole + 1); //设置data
                charaXItem->appendRow(item);
            }
        }

        model->setItem(i - 1, 0, charaXItem); //添加进模型
        // model->setItem(i - 1, 1, imageItem);  //添加进模型
    }

    //设置模型
    this->setModel(model);
}
/*--在当前路径下建立一个存档文件夹--*/
bool TreeView::CreateTemp()
{
    //获取当前路径
    QString curPath = QDir::currentPath();
    QDir curDir(curPath);
    //当前路径下创建一个文件加
    curDir.mkdir("saves");

    curPath = curPath + "/saves";
    curDir.setPath(curPath);
    //创建角色文件夹
    for (int i = 1; i < charactersNum + 1; i++)
    {
        curDir.mkdir("character" + QString::number(i));
    }
    return true;
}
/*--槽：接受到文件夹改变--*/
void TreeView::on_aNewSaveAdd(int charaNum)
{
    qDebug() << "on_aNewSaveAdd";
    QString curPath = MyFileWatcher::GetPaths().value("progChaPath") + QString::number(charaNum);
    QDir curDir(curPath);
    QString timeFolder = MyFileWatcher::GetDateTimeString();
    curDir.mkdir(timeFolder); //创建一个时间文件夹
    curPath = curPath + "/" + timeFolder;
    //设置当前路径

    //原来的路径
    QMap<QString, QString> pathMap = aWatcher->pathsMap;
    QString charaPath              = pathMap.value("charaXPath") + QString::number(charaNum);
    QString prePath                = charaPath + "/exitsave_1";
    QString preMapFile             = charaPath + "/character.map";
    //备份文件夹到当下
    MyFileWatcher::Copy(prePath, curPath);
    MyFileWatcher::Copy(preMapFile, curPath);
    //添加到模型下面

    QStandardItem *charaXItem = model->item(charaNum - 1, 0);
    //再创建一个item
    QStandardItem *timeItem = new QStandardItem(timeFolder);
    //设置路径数据
    timeItem->setData(curPath, Qt::UserRole + 1);
    timeItem->setData("child", TYPE_ROLE);
    //设置颜色
    timeItem->setData("#8fbc8f", Qt::ForegroundRole);
    charaXItem->insertRow(0, timeItem); //在头部插入项
}
/*--树型视图 右键菜单--*/
void TreeView::on_treeViewMenu(const QPoint &point)
{
    qDebug() << "Create menu ";
    QMenu menu;
    QModelIndex curIndex = this->indexAt(point);
    //返回一行的姐妹
    QModelIndex index = curIndex.sibling(curIndex.row(), 0);
    if (index.isValid())
    {
        menu.addAction("展开", this, SLOT(on_treeMenuExpand(bool)));
        menu.addAction("折叠", this, SLOT(on_treeMenuCollapse(bool)));
        //添加分隔
        menu.addSeparator();
        menu.addAction("加载存档", this, SLOT(on_treeMenuLoadSave()));
        menu.addAction("删除存档", this, SLOT(on_treeMenuDelSave()));
    }
    menu.exec(QCursor::pos());
}

/*--槽:右键展开--*/
void TreeView::on_treeMenuExpand(bool checked)
{
    QModelIndex curIndex = this->currentIndex();
    QModelIndex index    = curIndex.sibling(curIndex.row(), 0);
    if (index.isValid())
    {
        this->expand(index);
    }
}

/*--槽:右键折叠--*/
void TreeView::on_treeMenuCollapse(bool checked)
{
    QModelIndex curIndex = this->currentIndex();
    QModelIndex index    = curIndex.sibling(curIndex.row(), 0);
    if (index.isValid())
    {
        this->collapse(index);
    }
}

/*--槽:加载菜单--*/
void TreeView::on_treeMenuLoadSave()
{
    qDebug() << "Load save";
    // 1.获取鼠标点击的item内容
    QModelIndex curIndex = this->currentIndex();
    QModelIndex index    = curIndex.sibling(curIndex.row(), 0);
    QString folderStr;
    if (index.isValid())
    {
        folderStr = model->itemFromIndex(index)->text();
    }
    // 2.获取是哪个角色
    int charaNum = index.parent().row() + 1;

    QString src = MyFileWatcher::GetPaths().value("progChaPath") + QString::number(charaNum) + QDir::separator() + folderStr;
    QString dst = MyFileWatcher::GetPaths().value("charaXPath") + QString::number(charaNum);
    QString saveFolder = src + QDir::separator() + "exitsave_1";
    QString mapPath    = src + QDir::separator() + "character.map";
    //取消对文件的监控
    aWatcher->RemovePath(dst + "/character.map");
    // 3.加载存档的时候，让文件监控不检测信号
    // 4.开始复制路径
    MyFileWatcher::Copy(saveFolder, dst);
    MyFileWatcher::Copy(mapPath, dst);
    QThread::msleep(500);
    //等待文件拷贝完成才打开信号

    // 3.恢复文件监控的信号
    aWatcher->AddPath(dst + "/character.map");
}

/*--槽:删除存档菜单--*/
void TreeView::on_treeMenuDelSave()
{
    QModelIndex curIndex = this->currentIndex();
    QModelIndex index    = curIndex.sibling(curIndex.row(), 0);
    QString folderStr;
    if (index.isValid())
    {
        folderStr = model->itemFromIndex(index)->text();
        qDebug() << index.row() << index.column();
    }
    // 2.获取是哪个角色
    int charaNum = index.parent().row() + 1;
    // 3.获取到对应文件夹
    QString src = MyFileWatcher::GetPaths().value("progChaPath") + QString::number(charaNum) + QDir::separator() + folderStr;
    // 4.删除文件
    QDir aDir(src);
    if (!aDir.exists())
    {
        qDebug() << "Src is not Exist:" << src;
        return;
    }
    aDir.removeRecursively();
    // 4.从模型中删除
    model->removeRow(index.row(), index.parent());
}

/*--槽：双击显示图片--*/
void TreeView::on_doubleClicked(const QModelIndex &index)
{
    //如果为根节点，则不执行
    if (index.data(TYPE_ROLE) == "parent")
    {
        return;
    }
    // 1.构建对话框
    PreviewDialog *prevDialog = new PreviewDialog(this);
    // 2.找到图片路径
    if (index.column() != 0)
    {
        qDebug() << "The item is empty";
        return;
    }
    QString imgPath =
        model->itemFromIndex(index)->data(Qt::UserRole + 1).toString() + QDir::separator() + "exitsave_1/preview.png";
    prevDialog->SetImagePath(imgPath);
    // 3.执行函数
    prevDialog->exec();
}
