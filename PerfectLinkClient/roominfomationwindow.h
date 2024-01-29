#pragma once

#include <QWidget>
#include "ui_roominfomationwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RoomInfomationWindowClass; };
QT_END_NAMESPACE

class RoomInfomationWindow : public QWidget
{
	Q_OBJECT

public:
	RoomInfomationWindow(QWidget *parent = nullptr);
	~RoomInfomationWindow();

private:
	Ui::RoomInfomationWindowClass *ui;
};
