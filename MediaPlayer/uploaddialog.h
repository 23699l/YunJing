#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H
#include"videoplayer.h"
#include <QDialog>
#include"logindialog.h"
#include<QImage>
namespace Ui {
class UpLoadDialog;
}


class UpLoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpLoadDialog(QWidget *parent = nullptr);
    ~UpLoadDialog();
    QString SaveVideoJpg(QString FilePath);
    void Clear();
signals:

    //void SIG_GetOnePicture(QImage);
    void SIG_UpLoadFile(QString GifPath,QString VideoPath,Hobby hy);
private slots:
    void on_pb_view_clicked();
    void on_pb_startUpLoad_clicked();
    void slot_updateProcess(qint64 cur,qint64 max);
private:
    Ui::UpLoadDialog *ui;
    QString m_GifPath;
    QString m_VideoPath;

};

#endif // UPLOADDIALOG_H
