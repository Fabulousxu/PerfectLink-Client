#pragma once

#include <qlabel.h>

#include <QWidget>

#include "ui_endwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class EndWindowClass;
};
QT_END_NAMESPACE

class EndWindow : public QWidget {
  Q_OBJECT

 public:
  EndWindow(QWidget *parent = nullptr);
  ~EndWindow() { delete ui; }

 private:
  Ui::EndWindowClass *ui;
  QVector<QWidget *> rankDisplay;
  QVector<QLabel *> selfLabel;
  QVector<QLabel *> nicknameLabel;
  QVector<QLabel *> scoreLabel;

 public slots:
  void onGameEnd(const QVector<QPair<QString, int>> &rank, int self);

 private slots:
  void onBackButton();

 signals:
  void backToHome();
};
