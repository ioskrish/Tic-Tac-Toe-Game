#include "tictactoemainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("C:/Users/ACER/Downloads/icon2.png"));
    TicTacToeMainWindow w;
    w.show();
    return a.exec();
}
