#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <qfontdatabase.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont("./MinecraftAE.ttf");
    Block::loadPicture();
    Player::loadPicture();
    Select::loadPicture();
    MainWindow w;
    w.show();
    return a.exec();
}
