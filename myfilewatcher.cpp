#include "myfilewatcher.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QDateTime>

#pragma execution_character_set("utf-8")
/*--构造--*/
MyFileWatcher::MyFileWatcher(QObject *parent)
{
    //先获取常用路径图
    pathsMap = GetPaths();
    //获取有几个角色
    characterNum = GetCharactersNum();

    //分别添加需要监控的路径
    for (int i = 1; i < characterNum + 1; i++)
    {
        QString charaXPath = pathsMap.value("charaXPath") + QString::number(i);
        //先判断角色X的文件夹是否存在
        if (!QDir(charaXPath).exists())
        {
            qDebug() << "No such path:" << charaXPath;
            return;
        }
        // mWatcher.addPath(charaXPath);                    //添加监控的路径
        mWatcher.addPath(charaXPath + "/character.map"); //添加监控的路径
    }

    //函数绑定
    connect(&mWatcher, SIGNAL(fileChanged(QString)), this, SLOT(on_fileChanged(QString)));
    connect(&mWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(on_directoryChanged(QString)));

    qDebug() << "Monitoring File:" << mWatcher.files();
    qDebug() << "Monitoring Directory:" << mWatcher.directories();
}

/*--移除监控的路径--*/
void MyFileWatcher::RemovePath(QString path)
{
    mWatcher.removePath(path);
}

/*--添加监控路径--*/
void MyFileWatcher::AddPath(QString path)
{
    mWatcher.addPath(path);
}

/*--槽:一旦文件夹中发生变化，就执行--*/
void MyFileWatcher::on_directoryChanged(const QString path)
{
    // qDebug() << "----------------------";
    // qDebug() << "Change direcotry is " << path;
}

/*--槽:一旦文件发生变化，就执行--*/
void MyFileWatcher::on_fileChanged(const QString path)
{
    qDebug() << "----------------------";
    qDebug() << "Change file is " << path;
    //通过路径判断当前是哪个角色
    //正则匹配
    QString regStr = "character_(\\w)";
    QRegExp reg(regStr);
    int pos = reg.indexIn(path);
    QString charaNum;
    if (pos > -1)
    {
        charaNum = reg.cap(1);
    }

    if (!isExitSaveExist(charaNum.toInt()))
    {
        qDebug() << "Cant find CHARACTER:[" << charaNum.toInt() << "] saver .[" << __FILE__ << __LINE__ << "]";
        return;
    }
    //找到存档发送信号
    qDebug() << "SIGNAL send";
    emit exitSaveAdd(charaNum.toInt());
}

/*--如果退出存档存在的话--*/
bool MyFileWatcher::isExitSaveExist(int charaNum)
{
    //构造某角色存档
    const QString savePath = pathsMap.value("charaXPath") + QString::number(charaNum) + "/exitsave_1/";

    QDir aDir(savePath);
    if (aDir.exists())
    {
        qDebug() << "Save find!!";
        return true;
    }
    return false;
}
/*--获取常用路径--*/
QMap<QString, QString> MyFileWatcher::GetPaths()
{
    QMap<QString, QString> tmpMap;
    QString homePath    = QDir::homePath();                       //用户路径
    QString rootPath    = homePath + "/AppData/Local/StoneShard"; //游戏存档路径
    QString charaV1Path = rootPath + "/characters_v1";
    // C:\Users\Jerry\AppData\Local\StoneShard\characters_v1\character_1
    QString charaXPath = charaV1Path + "/character_";

    //存档路径
    QString curPath       = QDir::currentPath();
    QString progSavesPath = curPath + "/saves";
    QString progChaPath   = progSavesPath + "/character";

    tmpMap.insert("homePath", homePath);
    tmpMap.insert("rootPath", rootPath);
    tmpMap.insert("charaV1Path", charaV1Path);
    tmpMap.insert("charaXPath", charaXPath);
    tmpMap.insert("curPath", curPath);
    tmpMap.insert("progSavesPath", progSavesPath);
    tmpMap.insert("progChaPath", progChaPath);
    return tmpMap;
}
/*--静态：解析有多少个角色存档--*/
int MyFileWatcher::GetCharactersNum()
{
    //获取常用路径
    QMap<QString, QString> pathsMap = GetPaths();
    //先检查有几个角色
    QString charaV1Path = pathsMap.value("charaV1Path");
    QDir charactersDir(charaV1Path);
    if (!charactersDir.exists())
    {
        qDebug() << "cant find characters_v1 folder";
        return false;
    }

    QFileInfoList infoList = charactersDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot); //单纯获取目录

    return infoList.size();
}
/*--递归拷贝文件夹--*/
void MyFileWatcher::CopyPath(QString src, QString dst)
{
    QDir dir(src);
    if (!dir.exists())
    {
        //如果是文件的话
        QFileInfo fileInfo(src);
        if (fileInfo.isFile())
        {
            //直接拷贝
            QString fileName = fileInfo.fileName();
            if (QFile::copy(src, dst + QDir::separator() + fileName))
            {
                qDebug() << "copy success";
            }
        }
        return;
    }

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString dst_path = dst + QDir::separator() + d;
        dir.mkpath(dst_path);
        CopyPath(src + QDir::separator() + d, dst_path);
    }

    foreach (QString f, dir.entryList(QDir::Files))
    {
        if (QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f))
        {
            qDebug() << "copy success";
        }
    }
}

void MyFileWatcher::Copy(QString src, QString dst)
{
    // 1.获取文件夹名
    QFileInfo srcInfo(src);
    QString srcFolderName = srcInfo.fileName();
    // 2.判断目的路径下面有无同名文件，有就删除目的路径
    QDir dstDir(dst);
    if (!dstDir.exists())
    {
        qDebug() << "Dir not exist! " << dst << __FUNCTION__;
        return;
    }
    //匹配文件名
    QStringList filterString;
    filterString << srcFolderName;
    QFileInfoList infoList = dstDir.entryInfoList(filterString, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    //如果匹配到了同名文件,全部删除
    if (infoList.count() > 0)
    {
        foreach (QFileInfo info, infoList)
        {
            //判断要删除的是文件还是文件夹
            if (info.isDir())
            {
                QDir aDir(info.absoluteFilePath());
                aDir.removeRecursively();
            }
            else if (info.isFile())
            {
                QFile aFile(info.absoluteFilePath());
                aFile.remove();
            }
        }
    }
    // 3.判断是文件夹
    if (srcInfo.isDir() && srcFolderName != "." && srcFolderName != "..")
    {
        // 4.在目的文件夹创建一个文件夹
        dstDir.mkdir(srcFolderName);
        // 5.构建实际路径
        QString dstPath = dst + QDir::separator() + srcFolderName;
        // 6.拷贝
        CopyPath(src, dstPath);
    }
    else
    {
        CopyPath(src, dst);
    }
}

/*--获取当前的时间--*/
QString MyFileWatcher::GetDateTimeString()
{
    QDateTime curTime = QDateTime::currentDateTime();

    return curTime.toString("hh点mm分ss秒yy年MM月dd日");
}

/*返回previewImage路径
 * @charaNum 第几个个角色
 * @num   	第几个文件夹
 */
QString MyFileWatcher::GetPreviewImage(int charaNum, int num)
{
    QString curPath    = QDir::currentPath();
    QString charaXPath = curPath + "/saves/character" + QString::number(charaNum);
    QDir aDir(charaXPath);
    if (!aDir.exists())
    {
        qDebug() << "Cant find characterPath:" << __FUNCTION__;
        return nullptr;
    }
    //找到当前路径下的第一个文件夹的图片
    QFileInfoList infoList = aDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
    if (infoList.count() < 1)
    {
        return nullptr;
    }

    QString imgPath = infoList.at(num).absoluteFilePath() + "/exitsave_1/preview.png";
    return imgPath;
}
