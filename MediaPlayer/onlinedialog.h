#ifndef ONLINEDIALOG_H
#define ONLINEDIALOG_H

#include <QDialog>
#include<qmymovielabel.h>
namespace Ui {
class onlinedialog;
}

class onlinedialog : public QDialog
{
    Q_OBJECT
public:
    explicit onlinedialog(QWidget *parent = nullptr);
    QMyMovieLabel* GetMyMovieLable(int FileId);
    ~onlinedialog();
private:
    Ui::onlinedialog *ui;
private slots:

    void slot_btnClicked();
    void on_pb_upload_clicked();
    void on_pb_tvShow_clicked();

    void on_pb_RecommendVideo_clicked();

    void on_pb_fresh_clicked();

signals:
    void SIG_ShowUpLoadDialog();
    void SIG_DownLoad_rq();
    void SIG_labelClicked(QString url);
};

#endif // ONLINEDIALOG_H
