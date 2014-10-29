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
	/*检测特征点*/
	int feature_count = 500;
	Mat warpImage = leftImageMat.clone();//克隆一个副本进行下面操作
	Mat referImage = rightImageMat.clone();
	/*检测关键点*/
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
	/*提取特征点描述子*/
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

	Mat warpImage = leftImageMat.clone();//克隆一个副本进行下面操作
	Mat referImage = rightImageMat.clone();
	/*匹配*/
	stitcher.knnMatch(leftImageDesc,rightImageDesc,knn_matches,matches,2);
	//求globalHomo
	float homographyReprojectionThreshold = 1.0;
	bool homographyFound = stitcher.refineMatchesWithHomography(leftImageKeyPoints,rightImageKeyPoints,homographyReprojectionThreshold,matches,globalHomo); 
	/*从匹配对中得到左边影像和右边影像匹配好的特征点*/
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
	/*获得映射后的四个角点坐标*/
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
		//提取特征
		extractFeature();
		break;
	case 2:
		//匹配特征
		matchFeature();
		break;
	case 3:
		//对齐影像
		alignImage();
		break;
	}
}
