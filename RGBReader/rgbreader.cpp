#include "rgbreader.h"
#include <qDebug>
#include <qPainter>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>

#define MAX_HEIGHT 646
#define MAX_WIDTH 1056

class AutoLocker{

public:
	AutoLocker(QMutex *mutex)
		:mutex_(mutex)
	{
		if (mutex_)
		{
			mutex_->lock();
		}
	}

	~AutoLocker()
	{
		if (mutex_)
		{
			mutex_->unlock();
		}
	}

private:
	QMutex *mutex_;
};

RGBReader::RGBReader(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	qDebug() << "1";
	connect(ui.pushButtonFile, SIGNAL(clicked(bool)), this, SLOT(clickFile(bool)));
	connect(ui.pushButtonOK, SIGNAL(clicked(bool)), this, SLOT(clickOK(bool)));
}

RGBReader::~RGBReader()
{

}

void RGBReader::clickOK(bool) {
	qDebug() << "click OK";

	iInHeight = ui.lineEditHeight->text().toInt();
	iInWidth = ui.lineEditWidth->text().toInt();

	if (sRgbPath.endsWith(".jpg", Qt::CaseInsensitive)) {
		iInWidth = MAX_WIDTH;
		iInHeight = MAX_HEIGHT;
		AutoLocker al(&oMutex);
		oImage.load(sRgbPath);
		oImage = oImage.convertToFormat(QImage::Format_RGB888);
	}
	else {
		if (iInWidth <= 0 || iInHeight <= 0) {
			QMessageBox::warning(this, QApplication::translate("", "", 0), QApplication::translate("", "宽高设置错误", 0));
			return;
		}

		if (iInWidth > MAX_WIDTH) {
			iInWidth = MAX_WIDTH;
		}
		if (iInHeight > MAX_HEIGHT) {
			iInHeight = MAX_HEIGHT;
		}

		FILE *fp = NULL;
		fopen_s(&fp, sRgbPath.toLocal8Bit(), "rb+");
		if (!fp) {
			QMessageBox::warning(this, QApplication::translate("", "", 0), QApplication::translate("", "文件不存在或者有中文", 0));
			return;
		}

		fseek(fp, 0, SEEK_END);
		long totalLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		unsigned char *pData = new unsigned char[totalLen];
		if (!pData) {
			fclose(fp);
			QMessageBox::warning(this, QApplication::translate("", "", 0), QApplication::translate("", "没有内存", 0));
			return;
		}

		if (0 == fread(pData, 1, totalLen, fp)) {
			fclose(fp);
			QMessageBox::warning(this, QApplication::translate("", "", 0), QApplication::translate("", "没有内存", 0));
			return;
		}

		fclose(fp);

		AutoLocker al(&oMutex);
		oImage = QImage(pData, iInWidth, iInHeight, QImage::Format_RGB16).copy();
		delete[] pData;
	}

	update();
}

void RGBReader::paintEvent(QPaintEvent * event)
{
	AutoLocker al(&oMutex);
    QPainter painter(this);
	QRect displayRect_(2, 152, iInWidth, iInHeight);
    painter.drawImage(displayRect_, oImage);
    return QWidget::paintEvent(event);
}

void RGBReader::clickFile(bool) {
	qDebug() << "click File";
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(QApplication::translate("", "打开rgb文件", 0));
	fileDialog->setDirectory(".");
	if (fileDialog->exec() == QDialog::Accepted) 
	{
		sRgbPath = fileDialog->selectedFiles()[0];
		QString info = "select rgb path:" + sRgbPath;
		qDebug(info.toStdString().c_str());

		ui.lineEditFile->setText(sRgbPath);
	}
	else 
	{
		qDebug("path = NULL");
	}
}
