#include "previewdialog.h"
#include "ui_previewdialog.h"
#include <QDir>
#include <QDebug>

#pragma execution_character_set("utf-8")
PreviewDialog::PreviewDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreviewDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("预览图片");
    //设置鼠标监控

    //绑定函数
}

PreviewDialog::~PreviewDialog()
{
    delete ui;
}

void PreviewDialog::SetImagePath(const QString path)
{
    QDir imgDir(path);
    if (!imgDir.exists())
    {
        qDebug() << "Dir dont exist!" << __FUNCTION__;
    }
    imgPath = path;
    //图片显示出来
    QLabel *pixlab = ui->labPix;
    //创建图片
    QPixmap pixmap;
    pixmap.load(imgPath);
    //设置图片大小
    pixlab->setPixmap(pixmap.scaled(this->width(), this->height()));
}

void PreviewDialog::mousePressEvent(QMouseEvent *event)
{
    //关闭窗口
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        this->close();
    }
}
