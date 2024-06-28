#include "onlinedialog.h"
#include "ui_onlinedialog.h"

onlinedialog::onlinedialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::onlinedialog)
{
    ui->setupUi(this);
    connect(ui->pb_tv1,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv2,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv3,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv4,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv5,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv6,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv7,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv8,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv9,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv10,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv11,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv12,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv13,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv14,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));
    connect(ui->pb_tv15,SIGNAL(clicked()),this,SLOT(slot_btnClicked()));

    connect(ui->pb_play1,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString)));
    connect(ui->pb_play2,SIGNAL(SIG_labelClicked(QString )),this,SIGNAL(SIG_labelClicked(QString )));
    connect(ui->pb_play3,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString )));
    connect(ui->pb_play4,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString )));
    connect(ui->pb_play5,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString )));
    connect(ui->pb_play6,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString )));
    connect(ui->pb_play7,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString )));
    connect(ui->pb_play8,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString )));
    connect(ui->pb_play9,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString )));
    connect(ui->pb_play10,SIGNAL(SIG_labelClicked(QString)),this,SIGNAL(SIG_labelClicked(QString )));

}

QMyMovieLabel* onlinedialog::GetMyMovieLable(int FileId)
{
    QString pb_paly=QString("pb_play%1").arg(FileId);
    return ui->sw_page->findChild<QMyMovieLabel *>(pb_paly);
}

onlinedialog::~onlinedialog()
{
    delete ui;
}
#include<QDebug>
void onlinedialog::slot_btnClicked()
{
    qDebug()<<__FUNCTION__;
}
//点击上传视频
void onlinedialog::on_pb_upload_clicked()
{
    Q_EMIT  SIG_ShowUpLoadDialog();
}


void onlinedialog::on_pb_tvShow_clicked()
{
    ui->sw_page->setCurrentIndex(1);
}


void onlinedialog::on_pb_RecommendVideo_clicked()
{
    ui->sw_page->setCurrentIndex(0);
}


void onlinedialog::on_pb_fresh_clicked()
{
    //发送下载文件请求
    Q_EMIT SIG_DownLoad_rq();
}

