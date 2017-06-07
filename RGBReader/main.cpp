#include "rgbreader.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RGBReader w;
	w.show();
	return a.exec();
}
