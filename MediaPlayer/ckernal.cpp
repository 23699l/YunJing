#include "ckernal.h"
#include"TcpClientMediator.h"
#include"packdef.h"
#include<QCryptographicHash>
#include<QFile>
#include<QFileInfo>
#include<QCoreApplication>
#include<QFileInfo>
#include<QSettings>
#include <windows.h>
#include<QMessageBox>
#include<QTime>
#include<QDir>
#define MD5_KEY 12345
static QByteArray GetMD5(QString val){
    QCryptographicHash hash(QCryptographicHash::Md5);
    QString tmp=QString("%1_%2").arg(val).arg(MD5_KEY);
    hash.addData(tmp.toStdString().c_str());
    QByteArray bt=hash.result();
    return bt.toHex();
}

Ckernal::Ckernal(QObject *parent) : QObject(parent)
{
    qsrand(QTime(0,0,0).msecsTo( QTime::currentTime() ));
    qRegisterMetaType<Hobby>("Hobby");
    m_id=0;
    //loadInitFile();
    Deal_Protocl_Arr();
    m_pMediator=new TcpClientMediator;
    m_loginDia=new loginDialog;
    m_onlinedialog=new onlinedialog;
    m_playerdialog=new PlayerDialog;
    m_uploaddialog=new UpLoadDialog;
    connect(m_onlinedialog,SIGNAL(SIG_ShowUpLoadDialog()),
            this,SLOT(slot_ShowUpLoadDialog()));
    connect(m_playerdialog,SIGNAL(SIG_ShowOnlineDialog()),this,SLOT(slot_ShowOnlineDialog()));//展示online窗口
    m_loginDia->show();
    connect(m_loginDia,SIGNAL(SIG_registerCommit(QString,QString,Hobby)),
            this,SLOT(slot_registerCommit(QString,QString,Hobby)));
    connect(m_loginDia,SIGNAL(SIG_LoginCommit(QString,QString)),
            this,SLOT(slot_LoginCommit(QString,QString)));
    connect(m_pMediator,SIGNAL(SIG_ReadyData(unsigned int, char*, int)),
            this,SLOT(slot_dealClientData(unsigned int,char* ,int)));
    m_pMediator->OpenNet(_SERVER_IP,_PORT_);
    m_playerdialog->hide();
    connect(m_playerdialog,SIGNAL(SIG_close()),this,SLOT(slot_destroy()));
    connect(this,SIGNAL(SIG_updateProcess(qint64, qint64)),
            m_uploaddialog,SLOT(slot_updateProcess(qint64,qint64)));//上传进度
    connect(m_onlinedialog,SIGNAL(SIG_DownLoad_rq()),
            this,SLOT(slot_DownLoad_rq()));//点击刷新
    connect(m_onlinedialog,SIGNAL(SIG_labelClicked(QString)),
            this,SLOT(slot_labelClicked(QString)));//点击播放url

    m_uploadworker=new UpLoadWorker;
    m_thread=new QThread;
    connect(m_uploaddialog,SIGNAL(SIG_UpLoadFile(QString,QString,Hobby)),
          m_uploadworker,SLOT(slot_UpLoadFile(QString,QString,Hobby)));//上传文件
    m_uploadworker->moveToThread(m_thread);
    m_thread->start();
    m_SendFIleBlock_mark=0;
}

Ckernal::~Ckernal()
{
    if(m_uploadworker){
        delete m_uploadworker;
        m_uploaddialog=nullptr;
    }
    if(m_thread){
        m_thread->quit();
        m_thread->wait(100);
        if(m_thread->isRunning()){
            m_thread->terminate();
            m_thread->wait(100);
        }
        delete m_thread;
        m_thread=nullptr;
    }
}

void Ckernal::loadInitFile()
{
    m_ip="196.152.1.56";
    m_port="8080";
    QString File_path=QCoreApplication::applicationDirPath()+"/config.ini";
    QFileInfo file(File_path);

    if(file.exists()){
        //文件存在
        QSettings setting(File_path,QSettings::IniFormat);
        //打开组
        setting.beginGroup("net");
        QVariant strIP=setting.value("ip");
        QVariant strPort=setting.value("port");
        if(!strIP.toString().isEmpty())m_ip=strIP.toString();
        if(!strPort.toString().isEmpty())m_port=strPort.toString();
        setting.endGroup();
    }else{
        //不存在
        //没有会创建
        QSettings setting(File_path,QSettings::IniFormat);
        //打开组
        setting.beginGroup("net");
        setting.setValue("ip",m_ip);
        setting.setValue("port",m_port);
        setting.endGroup();
    }
    qDebug()<<"m_ip:"<<m_ip<<" "<<"m_port:"<<m_port;
}

Ckernal *Ckernal::GetInstance()
{
    static Ckernal kernal;
    return &kernal;
}

void Ckernal::Deal_Protocl_Arr()
{
    memset(Fun_PtrArr,0,sizeof(Fun_PtrArr));
    Fun_PtrArr[_DEF_PACK_LOGIN_RS        -_DEF_PACK_BASE-1]=&Ckernal::Deal_Login_RS;//登入请求
    Fun_PtrArr[_DEF_PACK_REGISTER_RS     -_DEF_PACK_BASE-1]=&Ckernal::Deal_Register_RS;//注册回复
    Fun_PtrArr[DEF_PACK_UPLOAD_RS     -_DEF_PACK_BASE-1]=&Ckernal::Deal_UploadFile_RS;
    Fun_PtrArr[DEF_PACK_UPLOADFILERQ_RS     -_DEF_PACK_BASE-1]=&Ckernal::Deal_UploadRq_Rs;
    Fun_PtrArr[DEF_PACK_DOWNLOAD_RS     -_DEF_PACK_BASE-1]=&Ckernal::Deal_DownLoad_Rs;//处理下载回复
    Fun_PtrArr[DEF_PACK_FILEBLOCK_RQ     -_DEF_PACK_BASE-1]=&Ckernal::Deal_FileBlock_Rq;//处理文件块
}

//注册回复
void Ckernal::Deal_Register_RS(unsigned int lSendIP , char* buf , int nlen)
{
    STRU_REGISTER_RS* temp=(STRU_REGISTER_RS*)buf;
    if((temp->result)== register_success){
        QMessageBox::about(m_loginDia,"注册结果","注册成功");
    }else if(user_is_exist==(temp->result)){
        QMessageBox::about(m_loginDia,"注册结果","昵称已经存在");
    }
}
//登入回复
void Ckernal::Deal_Login_RS(unsigned int lSendIP , char* buf , int nlen)
{
    STRU_LOGIN_RS* temp=(STRU_LOGIN_RS*)buf;
    if((temp->result)==login_success){
        QMessageBox::about(m_loginDia,"提示","登入成功");
        m_loginDia->hide();
        m_playerdialog->showNormal();
        m_id=temp->userid;
        //发下载文件请求
        STRU_DOWNLOAD_RQ download_rq;
        download_rq.m_nUserId=m_id;
        m_pMediator->SendData(123,(char *)&download_rq,sizeof(download_rq));
    }else if(password_error==(temp->result)){
        QMessageBox::about(m_loginDia,"登入结果","账号或者密码错误");
    }else if(user_not_exist==(temp->result)){
        QMessageBox::about(m_loginDia,"登入结果","用户不存在");
    }
}
//文件请求回复
void Ckernal::Deal_UploadFile_RS(unsigned int lSendIP, char *buf, int nlen)
{
    STRU_UPLOAD_RS *upload_rs=(STRU_UPLOAD_RS*)buf;
    if(upload_rs->m_nResult==1){
        QMessageBox::about(m_uploaddialog,"提示","上传完成");
    }
}

void Ckernal::Deal_UploadRq_Rs(unsigned int lSendIP, char *buf, int nlen)//上传文件请求回复
{
    STRU_UPLOADRQ_RS* up_rs=(STRU_UPLOADRQ_RS*)buf;
    if(up_rs->mark==upload_mark){
        m_SendFIleBlock_mark=1;
    }
}
//处理下载回复
void Ckernal::Deal_DownLoad_Rs(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug()<<__FUNCTION__;
    STRU_DOWNLOAD_RS* up_rs=(STRU_DOWNLOAD_RS*)buf;
    FileInfo *fileInfo=new FileInfo;

    //把gif写保存下来
    fileInfo->videoId=up_rs->m_nVideoId;//真正的视频id
    fileInfo->fileName=up_rs->m_szFileName;
    fileInfo->fileId=up_rs->m_nFileId;//显示在控件上的ID
    fileInfo->rtmp=QString("rtmp://%1/vod%2").arg("192.168.203.133:1935").arg(up_rs->m_rtmp);
    qDebug()<<"up_rs->m_rtmp:"<<up_rs->m_rtmp;
    QDir dir;
    if(!dir.exists(QDir::currentPath()+"/temp/")){//如果不存在
        dir.mkpath(QDir::currentPath()+"/temp/");
    }
    fileInfo->filePath=QString("./temp/%1").arg(up_rs->m_szFileName);
    fileInfo->filePos=0;
    fileInfo->fileSize=up_rs->m_nFileSize;
    fileInfo->pFile=new QFile(fileInfo->filePath);
    if(fileInfo->pFile->open(QIODevice::WriteOnly)){
        m_FileIDTOFileInfo[fileInfo->videoId]=fileInfo;
    }else{
        qDebug()<<__FUNCTION__<<"打开文件失败";
        delete fileInfo;
        fileInfo=nullptr;
    }
}
void Ckernal::Deal_FileBlock_Rq(unsigned int lSendIP, char *buf, int nlen)
{
    STRU_FILEBLOCK_RQ *fileblock_rq=(STRU_FILEBLOCK_RQ *)buf;
    if(m_FileIDTOFileInfo.find(fileblock_rq->m_nFileId)!=m_FileIDTOFileInfo.end()){
        auto fileInfo=m_FileIDTOFileInfo[fileblock_rq->m_nFileId];
        int res=fileInfo->pFile->write(fileblock_rq->m_szFileContent,fileblock_rq->m_nBlockLen);
        (fileInfo->filePos)+=res;
        if((fileInfo->filePos)>=(fileInfo->fileSize)){
            //文件写完了
            fileInfo->pFile->close();
            m_FileIDTOFileInfo.erase(fileInfo->fileId);
            auto pb_play=m_onlinedialog->GetMyMovieLable(fileInfo->fileId);
            auto LastMovie=pb_play->movie();
            if(LastMovie!=nullptr&&LastMovie->isValid())
            {
                delete LastMovie;
            }
            QMovie *movie=new QMovie(fileInfo->filePath);
            pb_play->setMovie(movie);
            pb_play->setRtmpUrl(fileInfo->rtmp);
            //回收info
            delete fileInfo;
            fileInfo=NULL;
        }
    }
}
void Ckernal::slot_destroy()
{
    if(m_playerdialog){
        delete m_playerdialog;
        m_playerdialog=nullptr;
        qDebug()<<"回收完成";
    }
}

void Ckernal::slot_dealClientData(unsigned int lSendIP , char* buf , int nlen)
{
    //该写分发中心了
    int pro=(*(int*)buf);
    int index=pro-_DEF_PACK_BASE-1;
    if(index>=0&&index<_DEF_PACK_COUNT){//下标是否越界
        if(Fun_PtrArr[index]){
            (this->*Fun_PtrArr[index])(lSendIP,buf,nlen);
        }else{
            qDebug()<<"指针为空"<<endl;
            return;
        }
    }else{
        qDebug()<<"下标越界"<<endl;
        return;
    }
}
//注册
void Ckernal::slot_registerCommit(QString name, QString password,Hobby hy)
{
    STRU_REGISTER_RQ register_rq;
    strcpy_s(register_rq.name,_MAX_SIZE,name.toStdString().c_str());
    QByteArray password_hash=GetMD5(password);
    memcpy(register_rq.password,password_hash.data(),password_hash.length());
    register_rq.dance   =hy.dance  ;
    register_rq.edu     =hy.edu    ;
    register_rq.energy  =hy.energy ;
    register_rq.food    =hy.food   ;
    register_rq.funny   =hy.funny  ;
    register_rq.music   =hy.music  ;
    register_rq.outside =hy.outside;
    register_rq.video   =hy.video  ;
    m_pMediator->SendData(0,(char *)&register_rq,sizeof(register_rq));
}
//登入
void Ckernal::slot_LoginCommit(QString name, QString password)
{
    qDebug()<<__FUNCTION__;
    QByteArray password_hash=GetMD5(password);
    STRU_LOGIN_RQ login_rq;
    strcpy(login_rq.name,name.toStdString().c_str());
    memcpy(login_rq.password,password_hash.data(),password_hash.length());
    m_pMediator->SendData(0,(char *)&login_rq,sizeof(login_rq));
}
//展示Online窗口
void Ckernal::slot_ShowOnlineDialog()
{
    m_onlinedialog->show();
}

void Ckernal::slot_ShowUpLoadDialog()
{
    m_uploaddialog->show();
    m_uploaddialog->Clear();
}

void Ckernal::slot_DownLoad_rq()
{
    qDebug()<<__FUNCTION__;
    STRU_DOWNLOAD_RQ download_rq;
    download_rq.m_nUserId=m_id;
    m_pMediator->SendData(123,(char *)&download_rq,sizeof(download_rq));
}

void Ckernal::slot_labelClicked(QString url)
{
    m_playerdialog->playUrl(url);
}

//上传文件槽
void UpLoadWorker::slot_UpLoadFile(QString GifPath, QString VideoPath, Hobby hy)
{
    qDebug()<<"点击上传文件";
    Ckernal::GetInstance()->UpLoadFile(VideoPath,hy,GifPath);
    Ckernal::GetInstance()->UpLoadFile(GifPath,hy);
}

//上传文件函数
void Ckernal::UpLoadFile(QString path,Hobby hy,QString GIFPath){
    QFileInfo fileInfo(path);
    std::string strName=fileInfo.fileName().toStdString();
    const char *file_name=strName.c_str();
    //先发请求头
    STRU_UPLOAD_RQ upload_rq;
    if(!GIFPath.isEmpty()){
        QFileInfo fileInfo(GIFPath);
        strcpy_s(upload_rq.m_szGifName,_MAX_PATH,fileInfo.fileName().toLocal8Bit().data());
    }
    upload_rq.m_UserId=m_id;
    upload_rq.m_nFileId=qrand()%10000;
    upload_rq.m_nFileSize=fileInfo.size();
    strcpy_s(upload_rq.m_szFileName,_MAX_PATH,file_name);
    QByteArray bt=path.right(path.length()-path.lastIndexOf('.')-1).toLatin1();
    memcpy(upload_rq.m_szFileType,bt.data(),bt.length());
    memcpy(upload_rq.m_szHobby,&hy,sizeof(hy));
    m_pMediator->SendData(123,(char *)&upload_rq,sizeof(upload_rq));
    while(m_SendFIleBlock_mark==0){
        Sleep(1);
    }
    FileInfo fi;
    fi.fileId=upload_rq.m_nFileId;
    fi.fileName=upload_rq.m_szFileName;
    fi.filePath=path;
    fi.filePos=0;
    fi.fileSize=upload_rq.m_nFileSize;
    fi.pFile=new QFile(path);
    STRU_FILEBLOCK_RQ blockrq;
    blockrq.m_nUserId=m_id;
    if(fi.pFile->open(QIODevice::ReadOnly)){
        while(1){
            int read=fi.pFile->read(blockrq.m_szFileContent,_MAX_PATH);
            blockrq.filePos=fi.filePos;
            fi.filePos+=read;
            blockrq.m_nBlockLen=read;
            blockrq.m_nFileId=upload_rq.m_nFileId;
            m_pMediator->SendData(123,(char *)&blockrq,sizeof(blockrq));
            Q_EMIT SIG_updateProcess(fi.filePos,fi.fileSize);
            if(fi.filePos>=fi.fileSize){
                fi.pFile->close();
                delete fi.pFile;
                fi.pFile=NULL;
                break;
            }
        }
    }
    m_SendFIleBlock_mark=0;
}
