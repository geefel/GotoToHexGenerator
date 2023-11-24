#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <QString>

QString changeIntToHex(int in);
QString changeStrToHex(QString in);
char intToHex(int i);
char charToHex(char c);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
