#include "imageProcess.h"


ImageProcess::ImageProcess(void)
{
}


ImageProcess::~ImageProcess(void)
{
}
/*****************************************************************************
    *  @brief    : extractFeature
    *  @author   : Zhangle
    *  @date     : 2014/10/29 10:35
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void ImageProcess::extractFeature()
{
	/*���������*/
	int feature_count = 500;
	Mat warpImage = leftImageMat.clone();//��¡һ�����������������
	Mat referImage = rightImageMat.clone();
	/*���ؼ���*/
// 	if (!leftImageKeyPoints.empty())
// 	{
// 		leftImageKeyPoints.clear();
// 	}
// 	if (!rightImageKeyPoints.empty())
// 	{
// 		rightImageKeyPoints.clear();
// 	}
	leftImageKeyPoints = stitcher.detectKeypoints(warpImage);
	rightImageKeyPoints = stitcher.detectKeypoints(referImage);
	/*��ȡ������������*/
	leftImageDesc = stitcher.extractorDescriptors(warpImage,leftImageKeyPoints);
	rightImageDesc = stitcher.extractorDescriptors(referImage,rightImageKeyPoints);
}
/*****************************************************************************
    *  @brief    : matchFeature
    *  @author   : Zhangle
    *  @date     : 2014/10/29 10:35
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void ImageProcess::matchFeature()
{
	/*clear*/
	if (!leftFeaturePoint.empty())
	{
		leftFeaturePoint.clear();
	}
	if (!rightFeaturePoint.empty())
	{
		rightFeaturePoint.clear();
	}

	Mat warpImage = leftImageMat.clone();//��¡һ�����������������
	Mat referImage = rightImageMat.clone();
	/*ƥ��*/
	stitcher.knnMatch(leftImageDesc,rightImageDesc,knn_matches,matches,2);
	//��globalHomo
	float homographyReprojectionThreshold = 1.0;
	bool homographyFound = stitcher.refineMatchesWithHomography(leftImageKeyPoints,rightImageKeyPoints,homographyReprojectionThreshold,matches,globalHomo); 
	/*��ƥ����еõ����Ӱ����ұ�Ӱ��ƥ��õ�������*/
	stitcher.getPointsfromMatches(leftImageKeyPoints,rightImageKeyPoints,matches,leftFeaturePoint,rightFeaturePoint);
	vector<DMatch> inliers = matches;
	drawMatches(warpImage,leftImageKeyPoints,referImage,rightImageKeyPoints,inliers,matchMat);
}
/*****************************************************************************
    *  @brief    : alignImage
    *  @author   : Zhangle
    *  @date     : 2014/10/29 10:35
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void ImageProcess::alignImage()
{
	alignbyH();
}
/*****************************************************************************
    *  @brief    : alignbyH
    *  @author   : Zhangle
    *  @date     : 2014/10/29 10:35
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void ImageProcess::alignbyH()
{
	Mat warpImage = leftImageMat.clone();
	Mat referImage = rightImageMat.clone();
	/*���ӳ�����ĸ��ǵ�����*/
	v_warped_corner = stitcher.getWarpedCorner(globalHomo,warpImage);
	warpedImage = stitcher.getWarpedImage(warpImage,globalHomo,v_warped_corner,xoffset,yoffset);
	alignResult = stitcher.getPanorama(warpedImage,referImage,xoffset,yoffset);
	alignbyHResult = alignResult.clone();
}

void ImageProcess::run()
{
	switch(processStep)
	{
	case 1:
		//��ȡ����
		extractFeature();
		break;
	case 2:
		//ƥ������
		matchFeature();
		break;
	case 3:
		//����Ӱ��
		alignImage();
		break;
	}
}
/*****************************************************************************
    *  @brief    : saveResult ������
    *  @author   : Zhangle
    *  @date     : 2014/11/3 9:26
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void ImageProcess::saveResult(int i, string fileName)
{
	Mat savingMat;		//����һ����Ŵ��������ݵı���
	switch (i)
	{
	case 0:		//��ʱ��ʾtabwidget����ʾ���ǵ�1��tab����1��tab����ʾ���Ǵ򿪵�ԭʼӰ��
		//ԭʼӰ����Ҫ����
		break;
	case 1:		//��ʱ��ʾtabwidget����ʾ���ǵ�2��tab����2��tab����ʾ������ȡ������Ľ��
		//������ȡ������Ľ��
		break;
	case 2:		//��ʱ��ʾtabwidget����ʾ���ǵ�3��tab����3��tab����ʾ����ƥ��������Ľ��
		//����ƥ��������Ľ��
		savingMat = matchMat.clone();
		break;
	case 3:		//��ʱ��ʾtabwidget����ʾ���ǵ�4��tab����4��tab����ʾ���Ƕ����Ľ��
		//����Ӱ������Ľ��
		savingMat = alignResult.clone();
		break;
	}
	//д���ļ�
	imwrite(fileName,savingMat);
}
