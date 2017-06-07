#ifndef RGBREADER_H
#define RGBREADER_H

#include <QtWidgets/QMainWindow>
#include "ui_rgbreader.h"
#include <QMutex>

class RGBReader : public QMainWindow
{
	Q_OBJECT

public:
	RGBReader(QWidget *parent = 0);
	~RGBReader();

private slots:
	void clickOK(bool);
	void clickFile(bool);

private:
	void paintEvent(QPaintEvent * event);

private:
	Ui::RGBReaderClass ui;
	QString sRgbPath;
	int iInWidth;
	int iInHeight;
	QMutex oMutex;
	QImage oImage;
};

#endif // RGBREADER_H
