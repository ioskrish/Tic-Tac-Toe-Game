#ifndef TICTACTOESERVER_H
#define TICTACTOESERVER_H
#include <QTcpServer>
#include "tictactoemainwindow.h"

class TicTacToeMainWindow;

class TicTacToeServer : public QTcpServer
{
public:

    Q_OBJECT

public:
    TicTacToeServer(TicTacToeMainWindow* pHelloServer,QObject*parent=0);
    TicTacToeMainWindow* m_pMainWindow;
    void addMessage(QString Msg);
    TicTacToeServer* m_pBoxServer;

private slots:
    void readyRead();
    void disconnected();

protected:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;
private:
    QSet<QTcpSocket*> clients;
};

#endif // TICTACTOESERVER_H
