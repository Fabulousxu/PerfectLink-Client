#pragma once

#include <QWidget>
#include "ui_createroomwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateRoomWindowClass; };
QT_END_NAMESPACE

class CreateRoomWindow : public QWidget
{
	Q_OBJECT

public:
	CreateRoomWindow(QWidget *parent = nullptr);
	~CreateRoomWindow();

private:
	Ui::CreateRoomWindowClass *ui;
};
