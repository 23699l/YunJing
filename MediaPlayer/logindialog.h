#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
struct Hobby{
    char dance;
    char edu;
    char energy;
    char food;
    char funny;
    char music;
    char outside;
    char video;
};
namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();

private slots:
    void on_cb_all_clicked();

    void on_cb_allnot_clicked();

    void on_pb_registe_clicked();
    void on_pb_login_clicked();

signals:
    void SIG_registerCommit(QString name,QString password,Hobby hy);
    void SIG_LoginCommit(QString name,QString password);
private:
    Ui::loginDialog *ui;

};

#endif // LOGINDIALOG_H
