#include <qfontdatabase.h>

#include <QtWidgets/QApplication>

#include "mainwindow.h"

extern QPixmap *background;

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QFontDatabase::addApplicationFont("./MinecraftAE.ttf");
  Block::loadPicture();
  Player::loadPicture();
  Select::loadPicture();
  background = new QPixmap(":/MainWindow/image/background.png");
  MainWindow w;
  w.show();
  return a.exec();
}
