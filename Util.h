#pragma once
#include <QString>
#include <string>
#include <QImage>
#include <QDebug>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
class Util
{
public:
	Util(void);
	~Util(void);
	/************************************************************************/
	/*									类型转换								*/
	/************************************************************************/
	//string 转换为 QString
	QString s2q(const string &s);
	//QString 转换为 string
	string q2s(const QString &qs);
	//cv::Mat 转换为 QImage
	QImage Mat2QImage(const Mat &mat);
};

