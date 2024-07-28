#include "endwindow.h"

EndWindow::EndWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::EndWindowClass()) {
  ui->setupUi(this);
  setParent(parent);
  setFixedSize(parent->size());
  move(0, 0);
  setAttribute(Qt::WA_StyledBackground);
  connect(ui->backButton, &QPushButton::clicked, this,
          &EndWindow::onBackButton);

  rankDisplay.append(ui->rank1Display);
  rankDisplay.append(ui->rank2Display);
  rankDisplay.append(ui->rank3Display);
  rankDisplay.append(ui->rank4Display);
  selfLabel.append(ui->self1Label);
  selfLabel.append(ui->self2Label);
  selfLabel.append(ui->self3Label);
  selfLabel.append(ui->self4Label);
  nicknameLabel.append(ui->nickname1Label);
  nicknameLabel.append(ui->nickname2Label);
  nicknameLabel.append(ui->nickname3Label);
  nicknameLabel.append(ui->nickname4Label);
  scoreLabel.append(ui->score1Label);
  scoreLabel.append(ui->score2Label);
  scoreLabel.append(ui->score3Label);
  scoreLabel.append(ui->score4Label);
}

void EndWindow::onGameEnd(const QVector<QPair<QString, int>> &rank, int self) {
  for (int i = 0; i < 4; ++i) {
    if (i < rank.size()) {
      nicknameLabel[i]->setText(rank[i].first);
      scoreLabel[i]->setText(QString::number(rank[i].second));
      rankDisplay[i]->show();
    } else {
      rankDisplay[i]->hide();
    }
    if (i == self - 1) {
      selfLabel[i]->show();
    } else {
      selfLabel[i]->hide();
    }
  }
}

void EndWindow::onBackButton() { emit backToHome(); }
