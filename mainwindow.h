#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#define MAX_HEX 128
#define Q_NR_ERR 1
#define Q_LIST_ERR 2
#define Q_PATH_ERR 3
#define PULT_ID_ERR 4

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void copyButClicked(QString &clip);
    void intstringToAscii(QString ergArr[MAX_HEX], int &numErg, QString inStr);
    QString stringToHexString(QString in);
    char deciIntToHexChar(int deci);
    void setErrorNr(int err);
    int getErrorNr();

    Ui::MainWindow *ui;
    QString hexErg[MAX_HEX];
    QString ergStr;
    int errorNr;

    QString errorText[5]
    {
        "",
        "Q-Nr: ungültig",
        "Q-List: ungültig",
        "Q-Path: ungültig",
        "Pult-ID: ungültig",
    };

private slots:
    void gotoStopResumeButClicked(int butNr);
};
#endif // MAINWINDOW_H
