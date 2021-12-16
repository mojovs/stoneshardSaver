#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myfilewatcher.h"
#include <QMap>
#include <QStandardItemModel>
QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:

  private:
    int charactersNum = 0; //有几个角色存档
    QStandardItemModel *model;
    QMap<QString, QString> pathMap;

    bool IsGameRunning(); //游戏是否运行
    //找到档案
    bool FindSavesData();
    bool ParsePATH_Characters();
    bool ParseSaves(QString &filePah);

  private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
