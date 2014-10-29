#include "mainwindow.h"
#include <QtGui/QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
	/*±‡¬Î…Ë÷√*/
	QTextCodec *codec = QTextCodec::codecForName("GBK");//«Èøˆ1
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
