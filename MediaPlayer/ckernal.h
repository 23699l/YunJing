#ifndef CKERNAL_H
#define CKERNAL_H
#include <QObject>
#include "playerdialog.h"
#include"logindialog.h"
#include"onlinedialog.h"
#include"uploaddialog.h"
#include<QThread>
#include <map>
#include"packdef.h"
using namespace std;
class INetMediator;
class Ckernal;
typedef void (Ckernal::* FUN_PTR)(unsigned int,char*,int);
class UpLoadWorker:public QObject{
    Q_OBJECT
public slots:
    void slot_UpLoadFile(QString GifPath,QString VideoPath,Hobby hy);
};
class Ckernal : public QObject
{
    Q_OBJECT
private:
    explicit Ckernal(QObject *parent = nullptr);
    Ckernal(const Ckernal&){}
    ~Ckernal();
    void loadInitFile();
public:

    //提高全局接口
    static Ckernal* GetInstance();
    void Deal_Protocl_Arr();//绑定协议头数组
    void Deal_Register_RS(unsigned int lSendIP , char* buf , int nlen);
    void Deal_Login_RS(unsigned int lSendIP , char* buf , int nlen);
    void Deal_UploadFile_RS(unsigned int lSendIP , char* buf , int nlen);//文件请求
    void Deal_UploadRq_Rs(unsigned int lSendIP , char* buf , int nlen);//上传文件请求回复
    void Deal_index(char* data,int len,long from);
    void Deal_DownLoad_Rs(unsigned int lSendIP , char* buf , int nlen);//处理文件下载回
    void Deal_FileBlock_Rq(unsigned int lSendIP , char* buf , int nlen);//处理文件块
public:
    void UpLoadFile();
signals:
    void SIG_updateProcess(qint64 cur, qint64 max);
    void SIG_playUrl(QString url);
private:
    PlayerDialog *  m_playerdialog;
    QString         m_ip;
    QString         m_port;
    INetMediator*   m_pMediator;
    loginDialog*    m_loginDia;
    int m_id;
    FUN_PTR         Fun_PtrArr[1024];
    onlinedialog*   m_onlinedialog;
    UpLoadDialog*   m_uploaddialog;
    UpLoadWorker*   m_uploadworker;
    QThread     *   m_thread;
    bool m_SendFIleBlock_mark;

    map<int,FileInfo* > m_FileIDTOFileInfo;
private slots:
    void slot_destroy();
    void slot_dealClientData(unsigned int lSendIP , char* buf , int nlen);
    void slot_registerCommit(QString name,QString password,Hobby hy);//注册
    void slot_LoginCommit(QString name,QString password);//登入
    void slot_ShowOnlineDialog();//展示online窗口
    void slot_ShowUpLoadDialog();
    void slot_DownLoad_rq();
    void slot_labelClicked(QString url);

public:
    void UpLoadFile(QString path,Hobby hy,QString GIFPath="");
};

#endif // CKERNAL_H
