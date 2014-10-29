#include "Util.h"


Util::Util(void)
{
}


Util::~Util(void)
{
}
/*****************************************************************************
    *  @brief    : s2q
    *  @author   : Zhangle
    *  @date     : 2014/10/29 10:26
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
QString Util::s2q(const string &s)
{
	return QString(QString::fromLocal8Bit(s.c_str()));
}
/*****************************************************************************
    *  @brief    : q2s
    *  @author   : Zhangle
    *  @date     : 2014/10/29 10:26
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
string Util::q2s(const QString &qs)
{
	return string((const char *)qs.toLocal8Bit());
}
/*****************************************************************************
    *  @brief    : Mat2QImage
    *  @author   : Zhangle
    *  @date     : 2014/10/29 10:26
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
QImage Util::Mat2QImage(const Mat &mat)
{
	// 8-bits unsigned, NO. OF CHANNELS=1
	if(mat.type()==CV_8UC1)
	{
		// Set the color table (used to translate colour indexes to qRgb values)
		QVector<QRgb> colorTable;
		for (int i=0; i<256; i++)
			colorTable.push_back(qRgb(i,i,i));
		// Copy input Mat
		const uchar *qImageBuffer = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
		img.setColorTable(colorTable);
		return img;
	}
	// 8-bits unsigned, NO. OF CHANNELS=3
	if(mat.type()==CV_8UC3)
	{
		// Copy input Mat
		const uchar *qImageBuffer = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return img.rgbSwapped();
	}
	else
	{
		qDebug() << "ERROR: Mat could not be converted to QImage.";
		return QImage();
	}
}
