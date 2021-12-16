#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QSysInfo>
#include <treeview.h>
#include <QVBoxLayout>

#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("晶石存档器 -mojovs");
    //获取model
    model = new QStandardItemModel(ui->treeView->model);
    //设置文件监控器

    charactersNum = MyFileWatcher::GetCharactersNum();
    pathMap       = MyFileWatcher::GetPaths();

    //绑定信号
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::IsGameRunning()
{
    //创建进程
    QProcess *process = new QProcess;
    process->start("tasklist", QStringList() << "/FI"
                                             << "imagename eq StoneShard.exe");

    process->waitForFinished();

    QString outputStr = QString::fromLocal8Bit(process->readAllStandardOutput());
    qDebug() << outputStr;
    if (!outputStr.contains("StoneShard.exe"))
    {
        return false;
    }
    return true;
}

bool MainWindow::FindSavesData()
{
    QString gameDataPath = pathMap.value("rootPath");

    QDir aDir(gameDataPath);
    if (aDir.exists())
    {
        return true;
    }
    return false;
}

//解析
bool MainWindow::ParseSaves(QString &filePah)
{
    QDir characterDir(filePah);
    if (!characterDir.exists())
    {
        qDebug() << "cant find characterX folder";
        return false;
    }

    QFileInfoList infoList = characterDir.entryInfoList(QDir::Dirs); //单纯获取目录
    return true;
}
