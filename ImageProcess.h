#pragma once
#include <QThread>
#include "Stitcher.h"

class ImageProcess:public QThread
{
public:
	ImageProcess(void);
	~ImageProcess(void);

	Mat leftImageMat;						//���Ӱ�񣬾���洢
	Mat rightImageMat;						//�ұ�Ӱ�񣬾���洢
	QString featureType;					//����������
	vector<KeyPoint> leftImageKeyPoints;	//���Ӱ��Ĺؼ���
	vector<KeyPoint> rightImageKeyPoints;	//�ұ�Ӱ��Ĺؼ���
	Mat leftImageDesc;						//���Ӱ���������������
	Mat rightImageDesc;						//�ұ�Ӱ���������������
	vector<vector<DMatch>> knn_matches; 
	vector<DMatch> matches;
	Mat globalHomo;							//Homography
	vector<Point2f> leftFeaturePoint;		//�������Ӱ���ϵ�������
	vector<Point2f> rightFeaturePoint;		//�����ұ�Ӱ���ϵ�������
	Mat matchMat;							//ƥ����
	int xoffset;							//Ӱ��ӳ���x��������Ҫ��λ��
	int yoffset;							//Ӱ��ӳ���y��������Ҫ��λ��
	vector<Point> v_warped_corner;			//Ӱ����Hӳ����ĸ��ǵ������
	Mat warpedImage;						//ӳ����Ӱ��
	int alignMethod;						//���뷽����ֱ����ȫ��H���룬PTIS��SDIS
	Mat alignbyHResult;						//ȫ��H����Ľ��
	Mat alignbyPTISResult;					//PTIS����Ľ��
	Mat alignbySDISResult;					//SDIS����Ľ��
	Mat alignResult;
	Mat graphcut;
	Mat graphcut_and_cutline;
	double alfa;
	double lambda;
	int processStep;

	//��ȡ������
	void extractFeature();
	//ƥ��
	void matchFeature();
	//���루ƴ�ӣ�Ӱ��
	void alignImage();
	//ֱ����H����
	void alignbyH();
	//��PTIS��������
	void alignbyPTIS();
	//��SDIS��������
	void alignbySDIS();
	//�����i��tab���������ļ���ΪfileName
	void saveResult(int, string fileName);

private:
	Stitcher stitcher;

protected:
	void run();	

};

