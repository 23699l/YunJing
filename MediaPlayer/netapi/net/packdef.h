#pragma once
#include<memory.h>
#include<QString>
#include<QFile>
#define _SERVER_IP ("192.168.203.133")
#define _PORT_ (8000)
#define _DEF_BUFFER         (4096)
#define _DEF_CONTENT_SIZE	(1024)
#define _MAX_SIZE           (40)
#define _MAX_PATH           (260)

//自定义协议   先写协议头 再写协议结构
//登录 注册 获取好友信息 添加好友 聊天 发文件 下线请求
#define _DEF_PACK_BASE	(10000)
#define _DEF_PACK_COUNT (100)

//注册
#define _DEF_PACK_REGISTER_RQ	(_DEF_PACK_BASE + 0 )
#define _DEF_PACK_REGISTER_RS	(_DEF_PACK_BASE + 1 )
//登录
#define _DEF_PACK_LOGIN_RQ      (_DEF_PACK_BASE + 2 )
#define _DEF_PACK_LOGIN_RS      (_DEF_PACK_BASE + 3 )
//文件传输
#define DEF_PACK_UPLOAD_RQ      (_DEF_PACK_BASE + 4 )
#define DEF_PACK_UPLOAD_RS      (_DEF_PACK_BASE + 5 )
#define DEF_PACK_FILEBLOCK_RQ   (_DEF_PACK_BASE + 6 )
#define DEF_PACK_UPLOADFILERQ_RS   (_DEF_PACK_BASE + 7 )//上传文件请求回复
//添加协议头
#define DEF_PACK_DOWNLOAD_RQ    (_DEF_PACK_BASE + 7 )
#define DEF_PACK_DOWNLOAD_RS    (_DEF_PACK_BASE + 8 )

//返回的结果
//注册请求的结果
#define user_is_exist		(0)
#define register_success	(1)
//登录请求的结果
#define user_not_exist		(0)
#define password_error		(1)
#define login_success		(2)
//文件请求回复
#define upload_mark         (1)//客户端接收到上传标记，开始上传文件块

typedef int PackType;

//协议结构
//注册
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ():type(_DEF_PACK_REGISTER_RQ)
    {
        memset( tel  , 0, sizeof(tel));
        memset( name  , 0, sizeof(name));
        memset( password , 0, sizeof(password) );
    }
    //需要手机号码 , 密码, 昵称
    PackType type;
    char tel[_MAX_SIZE];
    char name[_MAX_SIZE];
    char password[_MAX_SIZE];
    int dance;
    int edu;
    int energy;
    int food;
    int funny;
    int music;
    int outside;
    int video;
}STRU_REGISTER_RQ;
//注册回复
typedef struct STRU_REGISTER_RS
{
    //回复结果
    STRU_REGISTER_RS(): type(_DEF_PACK_REGISTER_RS) , result(register_success),userId(-1)
    {
    }
    PackType type;
    int userId;
    int result;

}STRU_REGISTER_RS;

//登录
typedef struct STRU_LOGIN_RQ
{
    //登录需要: 手机号 密码
    STRU_LOGIN_RQ():type(_DEF_PACK_LOGIN_RQ)
    {
        memset( tel , 0, sizeof(tel) );
        memset( password , 0, sizeof(password) );
        memset( name , 0, sizeof(name) );
    }
    PackType type;
    char tel[_MAX_SIZE];
    char name[_MAX_SIZE];
    char password[_MAX_SIZE];

}STRU_LOGIN_RQ;
//登入回复
typedef struct STRU_LOGIN_RS
{
    // 需要 结果 , 用户的id
    STRU_LOGIN_RS(): type(_DEF_PACK_LOGIN_RS) , result(login_success),userid(0)
    {
    }
    PackType type;
    int result;
    int userid;

}STRU_LOGIN_RS;
//上传文件请求
typedef struct STRU_UPLOAD_RQ
{
    STRU_UPLOAD_RQ()
    {
        m_nType = DEF_PACK_UPLOAD_RQ;
        m_nFileId = 0;
        m_UserId = 0;
        m_nFileSize=0;
        memset(m_szFileType ,0,_MAX_PATH);
        memset(m_szGifName,0,_MAX_PATH);
        memset(m_szFileName ,0,_MAX_PATH);
        memset(m_szHobby ,0,_MAX_PATH);
    }
    PackType m_nType; //包类型
    int m_UserId; //用于查数据库, 获取用户名字, 拼接路径
    int m_nFileId; //区分不同文件, 可采用 md5 或 随机数 用户同时只能传一个所以相同概率较低
    long long m_nFileSize; //文件大小, 用于文件传输结束
    char m_szHobby[_MAX_PATH];//喜好标签
    char m_szFileName[_MAX_PATH]; //文件名, 用于存储文件
    char m_szGifName[_MAX_PATH]; //动画名字, 方便直接写入数据库
    char m_szFileType[_MAX_PATH]; //用于区分视频和图片
}STRU_UPLOAD_RQ;
//上传文件请求回复
typedef struct STRU_UPLOADRQ_RS
{
    STRU_UPLOADRQ_RS()
    {
        m_nType = DEF_PACK_UPLOADFILERQ_RS;
        mark = 0;
    }
    PackType m_nType; //包类型
    bool mark;
}STRU_UPLOADRQ_RS;
//上传完成回复
typedef struct STRU_UPLOAD_RS
{
    STRU_UPLOAD_RS()
    {
        m_nType = DEF_PACK_UPLOAD_RS;
        m_nResult = 0;
    }
    PackType m_nType; //包类型
    int m_nResult;
}STRU_UPLOAD_RS;
//文件块请求
typedef struct STRU_FILEBLOCK_RQ
{
    STRU_FILEBLOCK_RQ()
    {
        m_nType = DEF_PACK_FILEBLOCK_RQ;
        m_nUserId = 0;
        m_nFileId =0;
        m_nBlockLen =0;
        filePos=0;
        memset(m_szFileContent,0,_MAX_PATH);
    }
    PackType m_nType; //包类型
    long long filePos;
    int m_nUserId; //用户 ID
    int m_nFileId; //文件 id 用于区分文件
    int m_nBlockLen; //文件写入大小
    char m_szFileContent[_MAX_PATH];
}STRU_FILEBLOCK_RQ;

//下载文件请求
typedef struct STRU_DOWNLOAD_RQ
{
    STRU_DOWNLOAD_RQ()
    {
        m_nType = DEF_PACK_DOWNLOAD_RQ;
        m_nUserId = 0;
    }
    PackType m_nType; //包类型
    int m_nUserId; //用户 ID
}STRU_DOWNLOAD_RQ;
//下载文件回复
typedef struct STRU_DOWNLOAD_RS
{
    STRU_DOWNLOAD_RS()
    {
        m_nType = DEF_PACK_DOWNLOAD_RS;
        m_nFileId = 0;
        memset(m_szFileName , 0 ,_MAX_PATH);
        memset(m_rtmp , 0 ,_MAX_PATH);
    }
    PackType m_nType; //包类型
    int m_nFileId;
    int64_t m_nFileSize;
    int m_nVideoId;
    char m_szFileName[_MAX_PATH];
    char m_rtmp[_MAX_PATH]; // 播放地址 如//1/103.MP3 用户本地需要转化为 rtmp://服务器 ip/app 名/ + 这个字符串 //本项目 app 名为 vod
}STRU_DOWNLOAD_RS;

//文件信息
typedef struct FileInfo
{
    int fileId;
    int videoId;
    long long filePos;
    long long fileSize;
    QString filePath;
    QString fileName;
    QString rtmp;
    QFile *pFile;
}FileInfo;






