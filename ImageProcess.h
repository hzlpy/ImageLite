#pragma once
#include <QThread>
#include "Stitcher.h"

class ImageProcess:public QThread
{
public:
	ImageProcess(void);
	~ImageProcess(void);

	Mat leftImageMat;						//左边影像，矩阵存储
	Mat rightImageMat;						//右边影像，矩阵存储
	QString featureType;					//特征点类型
	vector<KeyPoint> leftImageKeyPoints;	//左边影像的关键点
	vector<KeyPoint> rightImageKeyPoints;	//右边影像的关键点
	Mat leftImageDesc;						//左边影像的特征点描述子
	Mat rightImageDesc;						//右边影像的特征点描述子
	vector<vector<DMatch>> knn_matches; 
	vector<DMatch> matches;
	Mat globalHomo;							//Homography
	vector<Point2f> leftFeaturePoint;		//落在左边影像上的特征点
	vector<Point2f> rightFeaturePoint;		//落在右边影像上的特征点
	Mat matchMat;							//匹配结果
	int xoffset;							//影像映射后x方向上需要的位移
	int yoffset;							//影像映射后y方向上需要的位移
	vector<Point> v_warped_corner;			//影像用H映射后四个角点的坐标
	Mat warpedImage;						//映射后的影像
	int alignMethod;						//对齐方法：直接用全局H对齐，PTIS，SDIS
	Mat alignbyHResult;						//全局H对齐的结果
	Mat alignbyPTISResult;					//PTIS对齐的结果
	Mat alignbySDISResult;					//SDIS对齐的结果
	Mat alignResult;
	Mat graphcut;
	Mat graphcut_and_cutline;
	double alfa;
	double lambda;
	int processStep;

	//提取特征点
	void extractFeature();
	//匹配
	void matchFeature();
	//对齐（拼接）影像
	void alignImage();
	//直接用H对齐
	void alignbyH();
	//用PTIS方法对齐
	void alignbyPTIS();
	//用SDIS方法对齐
	void alignbySDIS();
	//保存第i的tab处理结果，文件名为fileName
	void saveResult(int, string fileName);

private:
	Stitcher stitcher;

protected:
	void run();	

};

