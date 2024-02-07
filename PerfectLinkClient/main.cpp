#include "mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Block::loadPicture();
    Player::loadPicture();
    Select::loadPicture();
    MainWindow w;
    w.show();
    return a.exec();
}
