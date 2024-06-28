#include "playerdialog.h"
#include "ui_playerdialog.h"
#define _DEF_PATH "E:/ProjectClass/image/1.mp4"
PlayerDialog::PlayerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PlayerDialog)
{
    ui->setupUi(this);
    m_player=new VideoPlayer;
    connect(m_player,SIGNAL(SIG_getOneImage(QImage)),this,SLOT(solt_setIMage(QImage)));
    slot_PlayerStateChanged(PlayerState::Stop);
    connect(m_player,SIGNAL(SIG_PlayerStateChanged(int)),this,SLOT(slot_PlayerStateChanged(int)));
    connect( m_player, SIGNAL( SIG_TotalTime(qint64)) ,this ,SLOT( slot_getTotalTime(qint64)) );

    connect(&m_timer,SIGNAL(timeout()),this,SLOT(slot_TimerTimeOut()));
    m_timer.setInterval(500);
    //安装事件过滤器，让该对象成为被观察
    ui->horizontalSlider->installEventFilter(this);
}

PlayerDialog::~PlayerDialog()
{
    delete m_player;
    delete ui;
}

#include<QFileDialog>
void PlayerDialog::on_pb_start_clicked()//点击开始播放
{
    QString path=QFileDialog::getOpenFileName(this,"open file","./",
                 "视频文件 (*.flv *.rmvb *.avi *.MP4 *.mkv);; 所有文件(*.*);;");
    if(path.isEmpty()){
        return ;
    }
    if(m_player->playerState()!=PlayerState::Stop){
        m_player->stop(true);
    }
    m_player->setFileName(path);

    slot_PlayerStateChanged(PlayerState::Playing);
}
void PlayerDialog::solt_setIMage(QImage image){
    ui->wdg_show->slot_setImage(image);
}

void PlayerDialog::on_pb_resume_clicked()
{
    if(m_player->playerState()!=PlayerState::Pause){
        return ;
    }
    m_player->play();
    ui->pb_resume->hide();
    ui->pb_pause->show();
}


void PlayerDialog::on_pb_pause_clicked()
{
    if(m_player->playerState()!=PlayerState::Playing){
        return ;
    }
    m_player->pause();
    ui->pb_resume->show();
    ui->pb_pause->hide();
}

void PlayerDialog::on_pb_stop_clicked()
{
    m_player->stop(true);
}
//播放状态切换槽函数
void PlayerDialog::slot_PlayerStateChanged(int state)
{
    switch( state )
    {
    case PlayerState::Stop:
        qDebug()<< "VideoPlayer::Stop";
        m_timer.stop();
        ui->horizontalSlider->setValue(0);
        ui->lb_totaltime->setText("00:00:00");
        ui->lb_currtime->setText("00:00:00");
        ui->pb_pause->hide();
        ui->pb_resume->show();
        this->update();
        isStop = true;
        break;
    case PlayerState::Playing:
        qDebug()<< "VideoPlayer::Playing";
        ui->pb_resume->hide();
        ui->pb_pause->show();
        m_timer.start();
        this->update();
        isStop = false;
        break;
    }
}

//void PlayerDialog::on_pb_start_4_clicked(){}
void PlayerDialog::slot_getTotalTime(qint64 uSec)
{
    qint64 Sec = uSec/1000000;
    ui->horizontalSlider->setRange(0,Sec);//精确到秒
    QString hStr = QString("00%1").arg(Sec/3600);
    QString mStr = QString("00%1").arg(Sec/60);
    QString sStr = QString("00%1").arg(Sec%60);
    QString str =
            QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
    ui->lb_totaltime->setText(str);
}
void PlayerDialog::slot_TimerTimeOut()
{
    if (QObject::sender() == &m_timer)
    {
        qint64 Sec = m_player->getCurrentTime()/1000000;
        ui->horizontalSlider->setValue(Sec);
        QString hStr = QString("00%1").arg(Sec/3600);
        QString mStr = QString("00%1").arg(Sec/60%60);
        QString sStr = QString("00%1").arg(Sec%60);
        QString str =
                QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
        ui->lb_currtime->setText(str);
        if(ui->horizontalSlider->value() == ui->horizontalSlider->maximum()
                && m_player->playerState() == PlayerState::Stop)
        {
            //slot_PlayerStateChanged( PlayerState::Stop );
            m_player->stop(true);

        }else if(ui->horizontalSlider->value() + 1 ==
                 ui->horizontalSlider->maximum()
                 && m_player->playerState() == PlayerState::Stop)
        {
            //slot_PlayerStateChanged( PlayerState::Stop );
            m_player->stop(true);
        }
    }
}
#include<QStyle>
#include<QMouseEvent>
bool PlayerDialog::eventFilter(QObject* obj, QEvent * event)
{
    if (obj == ui->horizontalSlider) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            int min=ui->horizontalSlider->minimum();
            int max=ui->horizontalSlider->maximum();
            int value = QStyle::sliderValueFromPosition(
            min, max, mouseEvent->pos().x(), ui->horizontalSlider->width());
            m_timer.stop();
            ui->horizontalSlider->setValue(value);
            m_player->seek((qint64)value*1000000); //value 秒
            m_timer.start();
            return true;
        } else {
            return false;
        }
    }
    // pass the event on to the parent class
    return QDialog::eventFilter(obj, event);
}
#include<QMessageBox>
void PlayerDialog::closeEvent(QCloseEvent *event)
{

    if(QMessageBox::question(this,"提示","是否要退出应用")==QMessageBox::Yes){
        event->accept();
        Q_EMIT SIG_close();
    }else{
        //忽略
        event->ignore();
    }
}

//展示onlin窗口
void PlayerDialog::on_pushButton_clicked()
{
    Q_EMIT SIG_ShowOnlineDialog();
}

void PlayerDialog::playUrl(QString url)
{
    m_player->stop(true);
    m_player->setFileName(url);
    slot_PlayerStateChanged(PlayerState::Playing);

}

