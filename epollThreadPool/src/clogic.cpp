#include "clogic.h"
#include<list>
#include<string>
using namespace std;
void CLogic::setNetPackMap()
{
    NetPackMap(_DEF_PACK_REGISTER_RQ)    =&CLogic::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)       =&CLogic::LoginRq;
    NetPackMap(DEF_PACK_UPLOAD_RQ)       =&CLogic::UpLoadFileRq;
    NetPackMap(DEF_PACK_FILEBLOCK_RQ)    =&CLogic::UpLoadFileBlockRq;
    NetPackMap(DEF_PACK_DOWNLOAD_RQ)    =&CLogic::DealDownLoad_Rq;
}
#define _DEF_COUT_FUNC_    cout << "clientfd fd:"<< clientfd <<" "<< __func__ << endl;
//注册
void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    STRU_REGISTER_RQ* register_rq=(STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS register_rs;
    //查询用户是否存在
    list<string> lst;
    char sql[1024]="";
    sprintf(sql,"select id from t_UserData where name='%s';",register_rq->name);
    if(FALSE==m_sql->SelectMysql(sql,1,lst)){
        cout<<__FUNCTION__<<","<<"查询失败,sql:"<<sql<<endl;
    }
    //用户已经存在
    if(lst.size()>0){
        register_rs.result=user_is_exist;
    }else{
        //保存用户信息
        sprintf(sql,"insert into t_UserData (name,password,food,funny,ennegy,dance,music,video,outside,edu) values ('%s','%s',%d,%d,%d,%d,%d,%d,%d,%d);",
                register_rq->name,register_rq->password,register_rq->food,register_rq->funny,register_rq->energy,register_rq->dance,register_rq->music,register_rq->video,register_rq->outside,register_rq->edu);
        if(m_sql->UpdataMysql(sql)==FALSE){
            cout<<__FUNCTION__<<","<<"插入失败,sql:"<<sql<<endl;
        }
        sprintf(sql,"select id from t_UserData where name='%s';",register_rq->name);
        lst.clear();
        if(m_sql->SelectMysql(sql,1,lst)==FALSE){
            cout<<__FUNCTION__<<","<<"查询失败,sql:"<<sql<<endl;
        }
        if(lst.size()>0){
            register_rs.userId=stoi(lst.front());
        }
        //创建用户路径
        char user_path[1024]="";
        sprintf(user_path,"/home/colin/video/flv/%s/",register_rq->name);
        umask(0);
        mkdir(user_path,S_IRWXU|S_IRWXG|S_IRWXO);
        register_rs.result=register_success;
    }
    m_pKernel->SendData(clientfd,(char *)&register_rs,sizeof(register_rs));
}
//登录
void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
    STRU_LOGIN_RS login_rs;
    STRU_LOGIN_RQ *login_rq=(STRU_LOGIN_RQ *)szbuf;
    list<string> lst;
    char sql[1024]="";
    sprintf(sql,"select password,id from t_UserData where name='%s';",login_rq->name);
    //查询用户是否在数据库中
    if(FALSE==m_sql->SelectMysql(sql,2,lst)){
        cout<<__FUNCTION__<<","<<"查询失败,sql:"<<sql<<endl;
    }
    cout<<"lst.size():"<<lst.size()<<endl;
    if(lst.size()==2){
        //查看密码是否正确
        if(strcmp(lst.front().c_str(),login_rq->password)==0){
            lst.pop_front();
            int id=stoi(lst.front());
            lst.pop_front();
            //登入成功
            login_rs.result=login_success;
            cout<<"login_success"<<endl;
            login_rs.userid=id;
            m_pKernel->m_IdToSocket[id]=clientfd;
        }else{
            login_rs.result=password_error;
        }
    }else{//用户不存在
        login_rs.result=user_not_exist;
    }
    m_pKernel->SendData(clientfd,(char *)&login_rs,sizeof(login_rs));
}
//上传文件请求
void CLogic::UpLoadFileRq(sock_fd clientfd, char*szbuf, int nlen){
    STRU_UPLOAD_RQ* upload_rq=(STRU_UPLOAD_RQ*)szbuf;
    FileInfo *fileInfo=new FileInfo;
    fileInfo->m_WriteFIleSize=0;
    memcpy(fileInfo->m_Hobby,upload_rq->m_szHobby,_MAX_PATH);
    fileInfo->m_VideoID=0;
    fileInfo->m_nFileID=upload_rq->m_nFileId;
    fileInfo->m_nUserId=upload_rq->m_UserId;
    fileInfo->m_nFileSize=upload_rq->m_nFileSize;
    strcpy(fileInfo->m_szFileType,upload_rq->m_szFileType);
    strcpy(fileInfo->m_szFileName,upload_rq->m_szFileName);
    char sql_BUf[1024]="";
    list<string> lst;
    sprintf(sql_BUf,"select name from t_UserData where id=%d;",upload_rq->m_UserId);
    if(m_sql->SelectMysql(sql_BUf,1,lst)!=TRUE){
        cout<<__FUNCTION__<<"数据库查询失败 sql_BUf:"<<sql_BUf<<endl;
        delete upload_rq;
        return ;
    }
    if(lst.size()<=0){
        delete upload_rq;
        return;
    }
    strcpy(fileInfo->m_UserName,lst.front().c_str());
    sprintf(fileInfo->m_szFilePath,"/home/colin/video/flv/%s/%s",fileInfo->m_UserName,fileInfo->m_szFileName);

    if(strcmp(upload_rq->m_szFileType,"gif")!=0){
        strcpy(fileInfo->m_szGifName,upload_rq->m_szGifName);
        sprintf(fileInfo->m_szGifPath,"/home/colin/video/flv/%s/%s",fileInfo->m_UserName,fileInfo->m_szGifName);

    }
    sprintf(fileInfo->m_szRtmp,"//%s/%s",fileInfo->m_UserName,fileInfo->m_szFileName);
    fileInfo->pFile=fopen(fileInfo->m_szFilePath,"w");
    if(fileInfo->pFile==NULL){
        cout<<"打开文件失败"<<endl;
    }

    m_pKernel->m_FIleIdToFileInfo[upload_rq->m_nFileId]=fileInfo;
    STRU_UPLOADRQ_RS upload_ma;
    upload_ma.mark=1;
    SendData(clientfd,(char *)&upload_ma,sizeof(upload_ma));
}

//上传文件块请求
void CLogic::UpLoadFileBlockRq(sock_fd clientfd, char *szbuf, int nlen)
{

    char sql_BUf[1024]="";
    list<string> lst;
    STRU_FILEBLOCK_RQ* fileBlock_rq=(STRU_FILEBLOCK_RQ*)szbuf;
    if((m_pKernel->m_FIleIdToFileInfo).find(fileBlock_rq->m_nFileId)==(m_pKernel->m_FIleIdToFileInfo).end()){
        cout<<"寻找文件结构体失败"<<endl;
        return ;
    }
    auto fileInfo=(m_pKernel->m_FIleIdToFileInfo)[fileBlock_rq->m_nFileId];
    //调整文件指针
    fseek(fileInfo->pFile,fileBlock_rq->filePos,SEEK_SET);
    int res=fwrite(fileBlock_rq->m_szFileContent,1,fileBlock_rq->m_nBlockLen,fileInfo->pFile);
    if(res<=0){
        cout<<"写入文件失败"<<endl;
    }

    pthread_mutex_lock(&(m_pKernel->m_lock));
    (fileInfo->m_WriteFIleSize)+=res;
    pthread_mutex_unlock(&m_pKernel->m_lock);

    if(fileInfo->m_WriteFIleSize>=fileInfo->m_nFileSize){
        //写完了
        //把文件与gif写入数据库
        fclose(fileInfo->pFile);
        if(strcmp(fileInfo->m_szFileType,"gif")!=0){
            sprintf(sql_BUf,"insert into t_VideoInfo (userId,videoName,picName,videoPath,picPath,rtmp,dance,edu,ennegy,food,funny,music,outside,video,hotdegree) values (%d,'%s','%s','%s','%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d);"
                    ,fileInfo->m_nUserId,fileInfo->m_szFileName,fileInfo->m_szGifName,fileInfo->m_szFilePath,fileInfo->m_szGifPath,fileInfo->m_szRtmp
                    ,(fileInfo->m_Hobby)[0],(fileInfo->m_Hobby)[1],(fileInfo->m_Hobby)[2]
                    ,(fileInfo->m_Hobby)[3],(fileInfo->m_Hobby)[4],(fileInfo->m_Hobby)[5]
                    ,(fileInfo->m_Hobby)[6],(fileInfo->m_Hobby)[7],0);
            //写文件请求回复
            STRU_UPLOAD_RS upload_rs;
            upload_rs.m_nResult=1;
            SendData(clientfd,(char *)&upload_rs,sizeof(upload_rs));
            if(m_sql->UpdataMysql(sql_BUf)!=TRUE){
                cout<<__FUNCTION__<<"插入失败 sql_buf:"<<sql_BUf<<endl;
            }
        }
        delete fileInfo;
        fileInfo=nullptr;
        (m_pKernel->m_FIleIdToFileInfo).erase(fileBlock_rq->m_nFileId);
    }

}
//处理文件下载请求
void CLogic::DealDownLoad_Rq(sock_fd clientfd, char *szbuf, int nlen)
{
    //获取为下载视频的列表

    STRU_DOWNLOAD_RQ* download_rq=(STRU_DOWNLOAD_RQ*)szbuf;
    list<FileInfo*> lst;
    GetFileList(lst,download_rq->m_nUserId);
    STRU_DOWNLOAD_RS download_rs;

    //发下载回复和发送文件内容
    while(lst.size()>0){
        FileInfo* p_fileInfo=lst.front();
        lst.pop_front();
        strcpy(download_rs.m_rtmp,(p_fileInfo->m_szRtmp));
        download_rs.m_nFileId=p_fileInfo->m_nFileID;
        download_rs.m_nVideoId=p_fileInfo->m_VideoID;
        download_rs.m_nFileSize=p_fileInfo->m_nFileSize;
        strcpy(download_rs.m_szFileName,p_fileInfo->m_szFileName);
        //发生下载文件回复
        SendData(clientfd,(char *)&download_rs,sizeof (download_rs));
        //发生文件内容
        p_fileInfo->pFile=fopen(p_fileInfo->m_szFilePath,"r");
        if(p_fileInfo->pFile==NULL){
            cout<<__FUNCTION__<<"打开文件描述符失败"<<endl;
        }
        if(p_fileInfo->pFile){
            STRU_FILEBLOCK_RQ filebovk_rq;
            while(1){
                long long res=fread(filebovk_rq.m_szFileContent,1,_MAX_PATH,p_fileInfo->pFile);
                (p_fileInfo->m_Pos)+=res;
                filebovk_rq.m_nBlockLen=res;
                filebovk_rq.m_nFileId=p_fileInfo->m_VideoID;
                filebovk_rq.m_nUserId=download_rq->m_nUserId;
                SendData(clientfd,(char *)&filebovk_rq,sizeof(filebovk_rq));
                if((p_fileInfo->m_Pos)>=p_fileInfo->m_nFileSize){
                    //文件内容读完
                    fseek(p_fileInfo->pFile,0,SEEK_SET);
                    fclose(p_fileInfo->pFile);
                    delete p_fileInfo;
                    p_fileInfo=nullptr;
                    break;
                }
            }
        }

    }
}
void CLogic::GetFileList(list<FileInfo*> &FileInfo_lst,int userId){
    //查询用户没有看过的视频数量

    char sql_Buf[1024]="";
    sprintf(sql_Buf,"select count(videoId) from t_VideoInfo where t_VideoInfo.videoId not in ( select t_UserRecv.videoId from t_UserRecv where t_UserRecv.userId =%d);",userId);
    list<string> lst;
    if(TRUE!=m_sql->SelectMysql(sql_Buf,1,lst)){
        cout<<__FUNCTION__<<"查询失败 sql_Buf："<<sql_Buf<<endl;
        return;
    }
    if(lst.size()==0){
        cout<<__FUNCTION__<<"vedioCount_lst长度为0"<<endl;
        return ;
    }
    //如果用户没看过的数量为0
    if(0==stoi(lst.front())){
        sprintf(sql_Buf,"delete from t_UserRecv where userId = %d;",userId);
        //情况t_UserRecv表
        if(TRUE!=m_sql->UpdataMysql(sql_Buf)){
            cout<<__FUNCTION__<<"删除失败"<<endl;
        }
    }
    //查询用户没看给的10条
    lst.clear();
    sprintf(sql_Buf,"select videoId ,picName , picPath , rtmp  from t_VideoInfo where t_VideoInfo.videoId not in ( select t_UserRecv.videoId from t_UserRecv where t_UserRecv.userId = %d ) order by hotdegree desc limit 0,10;",userId);
    if(TRUE!=m_sql->SelectMysql(sql_Buf,4,lst)){
        cout<<__FUNCTION__<<"查询失败 sql_Buf："<<sql_Buf<<endl;
        return;
    }
    int nCount=1;
    while(!lst.empty()){
        FileInfo *p_FIleInfo=new FileInfo;
        p_FIleInfo->m_Pos=0;
        p_FIleInfo->m_nFileID=nCount++;
        p_FIleInfo->m_VideoID=stoi(lst.front());
        lst.pop_front();

        strcpy(p_FIleInfo->m_szFileName,lst.front().c_str());
        lst.pop_front();

        strcpy(p_FIleInfo->m_szFilePath,lst.front().c_str());
        lst.pop_front();

        strcpy(p_FIleInfo->m_szRtmp,lst.front().c_str());
        cout<<"p_FIleInfo->m_szRtmp:"<<p_FIleInfo->m_szRtmp<<endl;
        lst.pop_front();

        //文件大小
        p_FIleInfo->pFile=fopen(p_FIleInfo->m_szFilePath,"r");

        if(p_FIleInfo->pFile==NULL){

            cout<<__FUNCTION__<<"打开文件描述符失败,p_FIleInfo->m_szFilePath:"<<p_FIleInfo->m_szFilePath<<endl;
            return ;
        }
        fseek(p_FIleInfo->pFile,0,SEEK_END);
        p_FIleInfo->m_nFileSize=ftell(p_FIleInfo->pFile);
        fseek(p_FIleInfo->pFile,0,SEEK_SET);
        fclose(p_FIleInfo->pFile);

        p_FIleInfo->pFile=NULL;
        //插入文件信息链表
        FileInfo_lst.push_back(p_FIleInfo);
        //把推过的视频插入t_UserRecv表中‘
        sprintf(sql_Buf,"insert into t_UserRecv values(%d ,%d);",userId,p_FIleInfo->m_VideoID);
        if(TRUE!=m_sql->UpdataMysql(sql_Buf)){
            cout<<__FUNCTION__<<"插入失败 sql_Buf："<<sql_Buf<<endl;
            return ;
        }
    }
}
