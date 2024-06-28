#include "qmymovielabel.h"
#include "ui_qmymovielabel.h"
#include<QDebug>
QMyMovieLabel::QMyMovieLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMyMovieLabel)
{
    ui->setupUi(this);
    m_movie=nullptr;
    ui->lb_movie->installEventFilter(this);
}

QMyMovieLabel::~QMyMovieLabel()
{
    delete ui;
}

void QMyMovieLabel::setMovie(QMovie *movie)
{
    m_movie=movie;
    ui->lb_movie->setMovie(movie);
    movie->start();
    movie->stop();
}

void QMyMovieLabel::enterEvent(QEvent *event)
{
    if(m_movie){
        m_movie->start();
    }
}

void QMyMovieLabel::leaveEvent(QEvent *event)
{
    if(m_movie){
        m_movie->stop();
    }
}

void QMyMovieLabel::setRtmpUrl(QString url)
{
    m_rtmpUrl=url;
}

bool QMyMovieLabel::eventFilter(QObject *watched, QEvent *event)
{
    //动画点击
    if(watched==ui->lb_movie&&event->type()==QEvent::MouseButtonPress){
        Q_EMIT SIG_labelClicked(m_rtmpUrl);
        return true;
    }
}

QMovie *QMyMovieLabel::movie() const
{
    return m_movie;
}
