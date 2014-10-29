#include "mainwindow.h"
#include <QtGui/QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
	/*��������*/
	QTextCodec *codec = QTextCodec::codecForName("GBK");//���1
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
