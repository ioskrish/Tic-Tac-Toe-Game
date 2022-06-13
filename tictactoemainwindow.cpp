#include "tictactoemainwindow.h"
#include "ui_tictactoemainwindow.h"
#include <QMessageBox>
#include <QTcpServer>
#include <QtGui>
#include <QPalette>

TicTacToeMainWindow::TicTacToeMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TicTacToeMainWindow)
{
    ui->setupUi(this);
    m_pClientSocket = new QTcpSocket(this);
}

TicTacToeMainWindow::~TicTacToeMainWindow()
{
    delete ui;
}
void TicTacToeMainWindow::paintEvent(QPaintEvent *pEvent)
{
    QWidget::paintEvent(pEvent);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen((QPen(Qt::darkMagenta,3)));


    for (int i = 0; i<sizeOfGrid; i++)
    {
        for (int j = 0; j<sizeOfGrid; j++)
        {
             CellArray[i][j].cellRect.setSize(QSize(100, 100));
             CellArray[i][j].cellRect.setTopLeft(QPoint(-50 + (100*(j+1)), -50 + (100*(i+1))));
        }
    }

    QFont newfont("Monospace",25);
    painter.setFont(newfont);

    for(int i = 0; i<sizeOfGrid; i++)
    {
        for(int j = 0; j<sizeOfGrid; j++)
        {
            painter.drawRect(CellArray[i][j].cellRect);
            painter.drawText(CellArray[i][j].cellRect.center(),CellArray[i][j].cellText);
        }
    }
}

QString TicTacToeMainWindow ::CreateMessage()
{
    Message="";
    for(int i=0;i<sizeOfGrid;i++)
    {
        for(int j=0;j<sizeOfGrid;j++)
        {
            Message+=CellArray[i][j].cellText;
        }
    }
    return Message;
}
void TicTacToeMainWindow::on_pushButtonStart_clicked()
{
    if(ui->pushButtonStart->text()=="Start")
    {
         m_pBoxServer= new TicTacToeServer(this);
        bool success = m_pBoxServer->listen(QHostAddress::Any, quint16(ui->textEditPort->toPlainText().toInt()));
        if(!success)
        {
            DisplayStatusMessage("Server failed...");
        }
        else
        {
            DisplayStatusMessage("Server Started...");
            this->setWindowTitle(this->windowTitle()+"|"+ui->textEditPort->toPlainText());
            ui->pushButtonStart->setText("Stop");
        }

    }
    else
    {
        m_pBoxServer->close();
        DisplayStatusMessage("Server Stopped...");
        ui->pushButtonStart->setText("Start");
    }

}

void TicTacToeMainWindow::on_pushButtonConnect_clicked()
{
    try
    {
        if(ui->pushButtonConnect->text()=="Connect")
        {
            m_pClientSocket->connectToHost(ui->textEditRemoteIPAddr->toPlainText(),quint16(ui->textEditRemotePort->toPlainText().toInt()) );
                connect(m_pClientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
            ui->pushButtonConnect->setText("Disconnect");
            DisplayStatusMessage("Connected to Host =>" + ui->textEditRemoteIPAddr->toPlainText() + ui->textEditRemotePort->toPlainText());
        }
        else
        {
            m_pClientSocket->disconnectFromHost();
            m_pClientSocket->close();
            ui->pushButtonConnect->setText("Connect");
            DisplayStatusMessage("Disconnected from Host =>" + ui->textEditRemoteIPAddr->toPlainText());
        }
    }
    catch(QException* ex)
    {
        DisplayStatusMessage(QString("Error in Connection=>") + QString(ex->what()));
    }

}

void TicTacToeMainWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);


    if (event->button() == Qt::LeftButton)
    {
        for(int i=0;i<sizeOfGrid;i++)
        {
            for(int j=0;j<sizeOfGrid;j++)
            {

                QRect widgetRect = CellArray[i][j].cellRect;
                widgetRect.moveTopLeft(this->parentWidget()->mapToGlobal(widgetRect.topLeft()));
                QPoint CurretPoint=event->pos();
                if(widgetRect.contains(CurretPoint) && alreadyClicked == false)
                {
                    clickCount++;
                    alreadyClicked = true;
                    CellArray[i][j].cellText="X";
                    statusX[i][j] = 1;

                    this->repaint();

                    if(i == 0 && j == 0){                   //The if statement's are used to send the position of the clicked grid
                        SendMovesToRemotePC("00");          //to the remote PC and thereby assigning and display "O" in the corresponding
                    }                                       //grids of remote PC for each click. It can also be possible if we can send
                    if(i == 0 && j == 1){                   //i and j values through SendMovesToRemotePC() function.
                        SendMovesToRemotePC("01");
                    }
                    if(i == 0 && j == 2){
                        SendMovesToRemotePC("02");
                    }
                    if(i == 1 && j == 0){
                        SendMovesToRemotePC("10");
                    }
                    if(i == 1 && j == 1){
                        SendMovesToRemotePC("11");
                    }
                    if(i == 1 && j == 2){
                        SendMovesToRemotePC("12");
                    }
                    if(i == 2 && j == 0){
                        SendMovesToRemotePC("20");
                    }
                    if(i == 2 && j == 1){
                        SendMovesToRemotePC("21");
                    }
                    if(i == 2 && j == 2){
                        SendMovesToRemotePC("22");
                    }
                    WinningLogicCheck();
                    return;

                }
                else if(alreadyClicked == true){
                    QMessageBox::information(this,tr("Warning"),tr("Wait for Opponent to play"));
                    return;
                }
            }
        }
    }

}
void TicTacToeMainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
            case QAbstractSocket::RemoteHostClosedError:
                break;
            case QAbstractSocket::HostNotFoundError:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The host was not found. Please check the "
                                            "host name and port settings."));
                break;
            case QAbstractSocket::ConnectionRefusedError:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The connection was refused by the peer. "
                                            "Make sure the fortune server is running, "
                                            "and check that the host name and port "
                                            "settings are correct."));
                break;
            default:
                QMessageBox::information(this, tr("Fortune Client"),
                                         tr("The following error occurred: %1.")
                                         .arg(m_pClientSocket->errorString()));
            }

}

void TicTacToeMainWindow::DisplayStatusMessage(QString Message)
{
    ui->labelResult->setText(Message);

}




void TicTacToeMainWindow::DisplayRemotePCMessage(QString remoteMove)
{

//    DisplayStatusMessage(remoteMove);

   if(remoteMove == "00"){
       CellArray[0][0].cellText="O";
       statusO[0][0]=1;
       clickCount++;
   }
   if(remoteMove == "01"){
       CellArray[0][1].cellText="O";
       statusO[0][1]=1;
       clickCount++;
   }
   if(remoteMove == "02"){
       CellArray[0][2].cellText="O";
       statusO[0][2]=1;
       clickCount++;
   }
   if(remoteMove == "10"){
       CellArray[1][0].cellText="O";
       statusO[1][0]=1;
       clickCount++;
   }
   if(remoteMove == "11"){
       CellArray[1][1].cellText="O";
       statusO[1][1]=1;
       clickCount++;
   }
   if(remoteMove == "12"){
       CellArray[1][2].cellText="O";
       statusO[1][2]=1;
       clickCount++;
   }
   if(remoteMove == "20"){
       CellArray[2][0].cellText="O";
       statusO[2][0]=1;
       clickCount++;
   }
   if(remoteMove == "21"){
       CellArray[2][1].cellText="O";
       statusO[2][1]=1;
       clickCount++;
   }
   if(remoteMove == "22"){
       CellArray[2][2].cellText="O";
       statusO[2][2]=1;
       clickCount++;
   }

   WinningLogicCheck();
   alreadyClicked = false;
   this->repaint();


}
void TicTacToeMainWindow::SendMovesToRemotePC(QString Message)
{
    //This is a simple protocol message ie Just storing only cell moves
    //User might need to expand the protocol struct
    //to accomodate other settings paramaters eg Message="GridSize"+"#"+"X or Y"+"#"+Message;
    //based on the requirements
    m_pClientSocket->write(QString(Message + "\n").toUtf8());
}




void TicTacToeMainWindow::WinningLogicCheck()
{
    if((statusX[0][0] == 1 && statusX[0][1] == 1 && statusX[0][2] == 1)||
       (statusX[1][0] == 1 && statusX[1][1] == 1 && statusX[1][2] == 1)||
       (statusX[2][0] == 1 && statusX[2][1] == 1 && statusX[2][2] == 1)||

       (statusX[0][0] == 1 && statusX[1][0] == 1 && statusX[2][0] == 1)||
       (statusX[0][1] == 1 && statusX[1][1] == 1 && statusX[2][1] == 1)||
       (statusX[0][2] == 1 && statusX[1][2] == 1 && statusX[2][2] == 1)||

       (statusX[0][0] == 1 && statusX[1][1] == 1 && statusX[2][2] == 1)||
       (statusX[0][2] == 1 && statusX[1][1] == 1 && statusX[2][0] == 1))
        {
            QMessageBox::information(this,tr("Result"),tr("You won !!"));
            NewGame();
        }
    else if((statusO[0][0] == 1 && statusO[0][1] == 1 && statusO[0][2] == 1)||
            (statusO[1][0] == 1 && statusO[1][1] == 1 && statusO[1][2] == 1)||
            (statusO[2][0] == 1 && statusO[2][1] == 1 && statusO[2][2] == 1)||

            (statusO[0][0] == 1 && statusO[1][0] == 1 && statusO[2][0] == 1)||
            (statusO[0][1] == 1 && statusO[1][1] == 1 && statusO[2][1] == 1)||
            (statusO[0][2] == 1 && statusO[1][2] == 1 && statusO[2][2] == 1)||

            (statusO[0][0] == 1 && statusO[1][1] == 1 && statusO[2][2] == 1)||
            (statusO[0][2] == 1 && statusO[1][1] == 1 && statusO[2][0] == 1))
        {
            QMessageBox::information(this,tr("Result"),tr("Sorry..Your opponent won !!"));
            NewGame();
        }

//    Check to know whether game is over or not

    else if(clickCount == 9)
        {
            QMessageBox::information(this,tr("Tic Tac Toe"),tr("Game Over!!"));
            NewGame();
        }
}

void TicTacToeMainWindow ::NewGame()
{
    clickCount = 0;
    alreadyClicked = false;
    for(int i=0;i<sizeOfGrid;i++)
    {
        for(int j=0;j<sizeOfGrid;j++)
        {
            CellArray[i][j].cellText = "";
        }
    }

    for(int i=0;i<sizeOfGrid;i++)
    {
        for(int j=0;j<sizeOfGrid;j++)
        {
            statusX[i][j] = 0;
        }
    }

    for(int i=0;i<sizeOfGrid;i++)
    {
        for(int j=0;j<sizeOfGrid;j++)
        {
            statusO[i][j] = 0;
        }
    }
}

