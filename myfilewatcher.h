#ifndef MYFILEWATCHER_H
#define MYFILEWATCHER_H
#include <QFileSystemWatcher>
#include <QMap>
class MyFileWatcher : public QObject
{
    Q_OBJECT
  public:
    QString rootPath = "";           //默认检查路径
    int characterNum = 1;            //当前角色
    QMap<QString, QString> pathsMap; //常用路径图

    MyFileWatcher(QObject *parent = nullptr);
    //移除监控的路径
    void RemovePath(QString path);
    //添加监控路径
    void AddPath(QString path);

    //解析部分
    static QMap<QString, QString> GetPaths();
    static int GetCharactersNum();
    static void CopyPath(QString src, QString dst); //拷贝某文件下面的内容
    static void Copy(QString src, QString dst);     //拷贝该文件夹的所有内容（包括该文件夹）
    static QString GetDateTimeString();
    static QString GetPreviewImage(int charaNum, int num);
  public slots:
    void on_directoryChanged(const QString path); //检查有文件夹改变
    void on_fileChanged(const QString path);      //检查有文件更改

  signals:
    /* 每当退出存档添加进来，就发送该信号
     * @charaNum 存档添加
     */
    void exitSaveAdd(int charaNum);

  private:
    QFileSystemWatcher mWatcher;
    //判断某个角色的退出存档是否存在
    bool isExitSaveExist(int charaNum);
};

#endif // MYFILEWATCHER_H
