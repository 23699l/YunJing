#ifndef PLAYERDIALOG_H
#define PLAYERDIALOG_H

#include <QDialog>
#include"videoplayer.h"
#include<QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class PlayerDialog; }
QT_END_NAMESPACE

class PlayerDialog : public QDialog
{
    Q_OBJECT

public:
    PlayerDialog(QWidget *parent = nullptr);
    ~PlayerDialog();
signals:
    void SIG_close();
    void SIG_ShowOnlineDialog();
private slots:
    void on_pb_start_clicked();
    void solt_setIMage(QImage image);
    void on_pb_resume_clicked();

    void on_pb_pause_clicked();

    void on_pb_stop_clicked();

    void slot_PlayerStateChanged(int state);
    void slot_getTotalTime(qint64 uSec);

    void slot_TimerTimeOut();
    //事件过滤器
    bool eventFilter(QObject* obj, QEvent * event);
    void on_pushButton_clicked();

private:
    Ui::PlayerDialog *ui;
    VideoPlayer* m_player;
    QTimer m_timer;
    //停止状态
    bool isStop;
public:
    //关闭事件：
    void closeEvent(QCloseEvent* event);
    void playUrl(QString url);
};
#endif // PLAYERDIALOG_H
