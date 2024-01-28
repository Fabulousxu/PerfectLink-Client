#pragma once

#include <QtWidgets/QWidget>
#include "ui_mainwindow.h"
#include "gamewindow.h"
#include "startwindow.h"
#include "homewindow.h"
#include "socket.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void onLoginSuccess(quint64 id, const QString &nickname);
    void onLogoffSuccess();

private:
    Ui::MainWindowClass *ui;
    Socket *socket;
    StartWindow *startWindow;
    HomeWindow *homeWindow;
    GameWindow *gameWindow;

    struct PlayerMessage {
        QString nickname;
        quint64 id;
    } playerMessage;
};
