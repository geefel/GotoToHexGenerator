/*
hexErg[0]: F0 	Es folgt eine SysEx Meldung 	immer F0
hexErg[1]: 7F 	Beginn der Meldung              immer 7F
hexErg[2]: 01 	Device ID 1                     MSC Empfangskanal in Setup -> Vorstellung
hexErg[3]: 02 	SysEx Meldung ist MSC           muss immer 02 sein
hexErg[4]: 01 	Befehlsformat,                  01 steht für Licht 	muss bei Sender und Empfänger gleich sein (Ausnahme 7F = All Types)
hexErg[5]: 01 	Befehl
                    01 = Go, Cue starten
                    02 = Stop, Cue pausieren
                    03 = Resume, Cue fortsetzen
hexErg[n]:      34 2E 35 00 32 	Wert zum Befehl, hier Cue 2/4.5 	Erklärung siehe unten
F7 	Ende der Meldung 	immer F7
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QSysInfo>
#include <iostream>
#include <QColor>
#include <QMenuBar>
#include <QMenu>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->inID->setText("1");
    setErrorNr(0);

#ifdef __linux__
    ui->centralwidget->setStyleSheet("QWidget {color: rgb(255, 255, 255); background-color: #306CC4; \
                                     font-family: Sans Serif; font-size: 22px;}");
#elif _WIN32
    ui->centralwidget->setStyleSheet("QWidget {color: rgb(255, 255, 255); background-color: #306CC4; \
                                      font-family: Sans Serif; font-size: 22px;}");
#endif

    QString QLineEditStyle = "QLineEdit {color: rgb(0, 0, 0); background-color: rgb(255, 255, 255); \
                                      border: 2px solid #8f8f91; border-radius: 6px}";
    ui->inQ_Nr->setStyleSheet(QLineEditStyle);
    ui->inQ_List->setStyleSheet(QLineEditStyle);
    ui->inQ_Path->setStyleSheet(QLineEditStyle);
    ui->inID->setStyleSheet(QLineEditStyle);
    ui->outHex->setStyleSheet(QLineEditStyle);

    ui->labelErklaerung_Dez->setStyleSheet("QLabel {font-size: 16px}");
    ui->labelErklaerung_Copy->setStyleSheet("QLabel {font-size: 16px}");

    QString QPushButtonStyle = "QPushButton {border: 2px solid #8f8f91; border-radius: 6px; color: rgb(0, 0, 0); \
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde); \
            min-width: 80px;}";
    ui->butGoto->setStyleSheet(QPushButtonStyle);
    ui->butStop->setStyleSheet(QPushButtonStyle);
    ui->butResume->setStyleSheet(QPushButtonStyle);
    ui->butClose->setStyleSheet(QPushButtonStyle);

    //https://medium.com/genymobile/how-c-lambda-expressions-can-improve-your-qt-code-8cd524f4ed9f
    //Warning: Pass a context object as 3rd connect parameter [clazy-connect-3arg-lambda]
    //In this version, we pass "this" as a context to connect().
    //It won't affect the execution of our lambda, but when monitor is deleted,
    //Qt will notice and will disconnect Worker::progress() from our lambda.
    connect(ui->butGoto,   &QPushButton::clicked, this, [=]() {emit gotoStopResumeButClicked(1);});

    //connect(ui->butGoto, &QPushButton::keyPressEvent(Qt::Key_Return), this, [=]() {emit gotoStopResumeButClicked(1);});
    connect(ui->butStop,   &QPushButton::clicked, this, [=]() {emit gotoStopResumeButClicked(2);});
    connect(ui->butResume, &QPushButton::clicked, this, [=]() {emit gotoStopResumeButClicked(3);});
    connect(ui->butClose,  &QPushButton::clicked, this, [=]() {emit this->close();});
    connect(ui->actionClose, &QAction::triggered, this, [=]() {emit this->close();});
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::gotoStopResumeButClicked(int butNr) {
    QString inputStr = "";
//    float inputFloat = 0;
//    int inputInt = 0;
    int hexErgStelle = 0;

    hexErg[hexErgStelle] = "F0";                                //HexStart

    hexErg[++hexErgStelle] = "7F";                              //Beginn

    inputStr = ui->inID->text();                                //Pult-ID  00-6F individal ID, 70 - 7E Group ID 7F Broadcast
//    inputInt = inputStr.toInt();
//    if ((inputFloat == 0) || (inputFloat > 0x7F))
//        setErrorNr(PULT_ID_ERR);
    hexErg[++hexErgStelle] = stringToHexString(inputStr);

    hexErg[++hexErgStelle] = "02";                              //SysEx Meldung, hier: MSC, max 0x7F

    hexErg[++hexErgStelle] = "01";                              //Befehlsformat, hier: für Lichtpult

    hexErg[++hexErgStelle] = stringToHexString(inputStr.setNum(butNr)); //Befehl: Goto? Stop? Resume?

    inputStr = ui->inQ_Nr->text();                              //Cue-Nr
//    inputFloat = inputStr.toFloat();
//    if ((inputFloat == 0.0) || (inputFloat > 999.999))
//        setErrorNr(Q_NR_ERR);
    intstringToAscii(hexErg, hexErgStelle, inputStr);

    inputStr = ui->inQ_List->text();                            //Listen-Nr
    if (inputStr.size()) {
//        inputFloat = inputStr.toFloat();
//        if ((inputFloat == 0.0) || (inputFloat > 999.999))
//            setErrorNr(Q_LIST_ERR);
        hexErg[++hexErgStelle] = "00";                          //"00" ist Trenner zw. Q-Nr, Q-List und Q-Path
        intstringToAscii(hexErg, hexErgStelle, inputStr);

        inputStr = ui->inQ_Path->text();                        //Path-Nr
        if (inputStr.size()) {
//            inputFloat = inputStr.toFloat();
//            if ((inputFloat == 0.0) || (inputFloat > 999.999))
//                setErrorNr(Q_PATH_ERR);
            hexErg[++hexErgStelle] = "00";                      //"00" ist Trenner zw. Q-Nr, Q-List und Q-Path
            intstringToAscii(hexErg, hexErgStelle, inputStr);
        }
    }

    ergStr = "";                                                //bis dahin alles sammeln
    for (int i = 0; i < hexErgStelle + 1; ++i) {
        ergStr += hexErg[i] + " ";
    }

    ergStr += "F7";                                             //End of Hex

//    if (getErrorNr() == 0) {
        ui->outHex->setText(ergStr);                            //Ergebnis darstellen
        copyButClicked(ergStr);                                 //ins Clipboard kopieren
//    }
//    else {
//        ui->outHex->setText(errorText[errorNr]);
//        setErrorNr(0);
//    }
}

void MainWindow::setErrorNr(int err) {
   errorNr = err;
}

int MainWindow::getErrorNr() {
    return errorNr;
}

/*
    QClipboard::Clipboard  indicates that data should be stored and retrieved from the global clipboard. Ubuntu, Win
    QClipboard::Selection  indicates that data should be stored and retrieved from the global mouse selection. Support for Selection is provided only on systems with a global mouse selection (e.g. X11).
    QClipboard::FindBuffer indicates that data should be stored and retrieved from the Find buffer. This mode is used for holding search strings on macOS.
*/
void MainWindow::copyButClicked(QString &clip) {
    QClipboard *clipboard = QGuiApplication::clipboard();

#ifdef __linux__
    clipboard->setText(clip, QClipboard::Clipboard);
#endif
#ifdef _WIN32
    clipboard->setText(clip, QClipboard::Clipboard);
#endif
#ifdef __APPLE__
    clipboard->setText(clip, QClipboard::FindBuffer);
#endif
}

void MainWindow::intstringToAscii(QString ergArr[MAX_HEX], int &numErg, QString inStr) {
    int laengeStr = inStr.size();
    for (int i = 0; i < laengeStr; ++i) {
        if ((inStr[i] != ".") && (inStr[i] != ",")) //Auf Punkt oder Komma prüfen
            ergArr[++numErg] = "3" + inStr[i];
        else
            ergArr[++numErg] = "2E";
    }
}

QString MainWindow::stringToHexString(QString in) {
    QString out = "00";
    int i = in.toInt(NULL);
    if (i > 0x7F) {         //PultID-Error
        setErrorNr(1);
    }
    else {
        out[0] = deciIntToHexChar(i / 16);
        out[1] = deciIntToHexChar(i % 16);
    }
    return out;
}

char MainWindow::deciIntToHexChar(int deci) {
    switch (deci) {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
        default: return 'X';
    }
}
