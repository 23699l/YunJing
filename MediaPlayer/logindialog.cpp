#include "logindialog.h"
#include "ui_logindialog.h"

loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    //注册Hobby
    qRegisterMetaType<Hobby>("Hobby");
}

loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::on_cb_all_clicked()
{
    ui->cb_all->setChecked(true);
    ui->cb_allnot->setChecked(false);
    ui->cb_dance->setChecked(true);
    ui->cb_edu->setChecked(true);
    ui->cb_energy->setChecked(true);
    ui->cb_food->setChecked(true);
    ui->cb_funny->setChecked(true);
    ui->cb_music->setChecked(true);
    ui->cb_outside->setChecked(true);
    ui->cb_video->setChecked(true);
}


void loginDialog::on_cb_allnot_clicked()
{
    ui->cb_all->setChecked(false);
    ui->cb_allnot->setChecked(true);
    ui->cb_dance->setChecked(false);
    ui->cb_edu->setChecked(false);
    ui->cb_energy->setChecked(false);
    ui->cb_food->setChecked(false);
    ui->cb_funny->setChecked(false);
    ui->cb_music->setChecked(false);
    ui->cb_outside->setChecked(false);
    ui->cb_video->setChecked(false);
}

#include<QMessageBox>
//注册
void loginDialog::on_pb_registe_clicked()
{
    QString name =ui->le_registe_username->text();
    QString password=ui->le_registe_password->text();
    QString confirm=ui->le_registe_password_again->text();
    if(name.isEmpty()||name.remove(" ").isEmpty()||password.isEmpty()||password.remove(" ").isEmpty()
            ||confirm.isEmpty()||confirm.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","账号或密码不能为空");
        return ;
    }
    //正则表达式判断 name
    QRegExp reg("a-zA-Z0-9{1,10}");
    //bool res=reg.exactMatch(name);
    //if(!res){
    //    QMessageBox::information(this,"提示","用户名非法，要使用数字或字母");
    //    return;
    //}
    if(name.length()>10||password.length()>20){
        QMessageBox::information(this,"提示","用户名称或密码太长");
    }
    if(password!=confirm){
        QMessageBox::information(this,"提示","两次密码不一致");
    }
    Hobby hoby;
    hoby.dance      =ui->cb_dance  ->isChecked()?50:0;
    hoby.edu        =ui->cb_edu    ->isChecked()?50:0;
    hoby.energy     =ui->cb_energy ->isChecked()?50:0;
    hoby.food       =ui->cb_food   ->isChecked()?50:0;
    hoby.funny      =ui->cb_funny  ->isChecked()?50:0;
    hoby.music      =ui->cb_music  ->isChecked()?50:0;
    hoby.outside    =ui->cb_outside->isChecked()?50:0;
    hoby.video      =ui->cb_video  ->isChecked()?50:0;
    Q_EMIT SIG_registerCommit(name,password,hoby);
}

//点击登入
void loginDialog::on_pb_login_clicked()
{
    QString name =ui->le_username->text();
    QString password=ui->le_password->text();
    if(name.isEmpty()||name.remove(" ").isEmpty()||password.isEmpty()||password.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","账号或密码不能为空");
        return ;
    }
    //正则表达式判断 name
    QRegExp reg("a-zA-Z0-9{1,10}");
    //bool res=reg.exactMatch(name);
    //if(!res){
    //    QMessageBox::information(this,"提示","用户名非法，要使用数字或字母");
    //    return;
    //}
    if(name.length()>10||password.length()>20){
        QMessageBox::information(this,"提示","用户名称或密码太长");
    }
    Q_EMIT SIG_LoginCommit(name,password);
}

