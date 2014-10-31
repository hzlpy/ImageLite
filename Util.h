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
	/*									����ת��								*/
	/************************************************************************/
	//string ת��Ϊ QString
	QString s2q(const string &s);
	//QString ת��Ϊ string
	string q2s(const QString &qs);
	//cv::Mat ת��Ϊ QImage
	QImage Mat2QImage(const Mat &mat);
	//
	void getScaledRatio(int image_width, int image_height, int label_width, int label_height, int *scaled_width, int *scaled_height);
};

