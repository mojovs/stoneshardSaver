#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui
{
class PreviewDialog;
}

class PreviewDialog : public QDialog
{
    Q_OBJECT

  private:
    QString imgPath = "";

  public:
    explicit PreviewDialog(QWidget *parent = nullptr);
    ~PreviewDialog();
    void SetImagePath(const QString path);
    QString GetImagePath();

  protected:
    void mousePressEvent(QMouseEvent *event);
  private slots:
    //检测鼠标点击，就关闭窗口

  private:
    Ui::PreviewDialog *ui;
};

#endif // PREVIEWDIALOG_H
