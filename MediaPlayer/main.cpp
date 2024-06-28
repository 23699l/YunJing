#include "playerdialog.h"

#include <QApplication>

#include"ckernal.h"
#include<QDebug>

#include<iostream>
using namespace std;
#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ckernal::GetInstance();

    return a.exec();
}
