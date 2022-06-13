#ifndef TICTACTOEMAINWINDOW_H
#define TICTACTOEMAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include "tictactoeserver.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TicTacToeMainWindow;
}
QT_END_NAMESPACE

class Cell
{
public:
    QRect           cellRect;
    QString         cellText;
    Cell()
    {
        cellText="";
    }
};

class TicTacToeServer;

class TicTacToeMainWindow : public QMainWindow
{
    Q_OBJECT
    QTcpSocket *m_pClientSocket;
    TicTacToeServer* m_pBoxServer;

public:
    Cell CellArray[3][3];
    QString  Message;
    int sizeOfGrid = 3;

    TicTacToeMainWindow(QWidget *parent = nullptr);
    ~TicTacToeMainWindow();
    void DisplayStatusMessage(QString Message);
    void DisplayRemotePCMessage(QString remoteMove);
    void SendMovesToRemotePC(QString Message);
    void displayError(QAbstractSocket::SocketError socketError);

    void WinningLogicCheck();

    void NewGame();

    bool alreadyClicked = false;

    int statusX[10][10];

    int statusO[10][10];

    int clickCount = 0;

protected:

    void paintEvent(QPaintEvent *);

    void mousePressEvent(QMouseEvent* ev);

    QString CreateMessage();

private slots:
    void on_pushButtonStart_clicked();

    void on_pushButtonConnect_clicked();

private:
    Ui::TicTacToeMainWindow *ui;
};

#endif // TICTACTOEMAINWINDOW_H
