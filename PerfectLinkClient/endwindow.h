#pragma once

#include <QWidget>
#include "ui_endwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EndWindowClass; };
QT_END_NAMESPACE

class EndWindow : public QWidget
{
	Q_OBJECT

public:
	EndWindow(QWidget *parent = nullptr);
	~EndWindow();

private:
	Ui::EndWindowClass *ui;
};
