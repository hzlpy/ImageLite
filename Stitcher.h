#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
//#include "FilePath.h"
using namespace std;
using namespace cv;
class Stitcher
{
public:
	Stitcher(void);
	~Stitcher(void);

	Stitcher(const string& detectType, const string& extractType, const string& matchType);
	/*根据路径读取影像*/
	Mat readImage(string path);
	/*根据路径存储图像*/
	bool writeImage(Mat mat, string, int type);
	/*对left，right两张影像进行拼接。right作为reference影像，left进行变换*/
	void stitchTwoImage(Mat mat_left, Mat mat_right);

	/*检测特征点*/
	vector<KeyPoint> detectKeypoints(Mat img);
	/*提取描述子*/
	Mat extractorDescriptors(Mat img, vector<KeyPoint> v_keypoint);

	/*匹配方法*/
	vector<DMatch> matchDescriptors(Mat descriptor_left, Mat descriptor_right );
	vector<DMatch> bestMatch(Mat descriptor_left, Mat descriptor_right);
	void knnMatch(Mat& queryDescriptor, Mat& trainDescriptor, vector<vector<DMatch>>& knn_matches, vector<DMatch>& matches, int k);   // K近邻匹配

	/*从匹配对中得到左边影像和右边影像中的特征点坐标(x,y)*/
	void getPointsfromMatches(vector<KeyPoint> v_keypoint_left, vector<KeyPoint> v_keypoint_right,vector<DMatch> matcher ,vector<Point2f> &point_left, vector<Point2f> &point_right);

	/*匹配后从匹配对中得到左边影像和右边影像中的keypoint*/
	void getKeypointsfromMatches(vector<KeyPoint> v_keypoint_left, vector<KeyPoint> v_keypoint_right,vector<DMatch> matches ,vector<KeyPoint> &warp_matched_keypoints, vector<KeyPoint> &reference_mathced_keypoints);
	/*在mat上画点*/
	void drawPoints(Mat mat, vector<Point2f> v_point, Scalar scalar);
	void drawPoints(Mat mat, vector<Point> v_point, Scalar scalar);
	/*画多边形*/
	void drawPloygon(Mat mat, vector<Point> corner, Scalar scalar);

	/*计算left影像的四个点映射到right影像上的坐标*/
	vector<Point> getWarpedCorner(Mat h, Mat img);

	/*获得h映射后的图像*/
	Mat getWarpedImage(Mat img, Mat H, vector<Point> v_warped_corner, int &xoffset, int &yoffset);

	/*获得全景图*/
	Mat getPanorama(Mat warped_img, Mat referenced_img, int xoffset, int yoffset);

	/*获取warp_img与referenced_img之间的重叠区域坐标*/
	bool getOverlap(Mat warp_img, Mat referenced_img, Mat H, vector<Point> &v_overlap_warped_corner, vector<Point> &v_overlap_warp_corner, vector<Point> &v_over_referenced_corner);

	/*获取四个点围成的四边形的外围矩形坐标*/
	vector<Point> getOuterRectangle(vector<Point> warped_corner);
	/*获取多边形四边形的外围矩形坐标*/
	void getOuterRectangle(vector<Point> &poly_corner, vector<Point> &outer_rect);

	bool refineMatchesWithHomography(const vector<KeyPoint>& queryKeypoints,    
		const vector<KeyPoint>& trainKeypoints,     
		float reprojectionThreshold,    
		vector<DMatch>& matches,    
		cv::Mat& homography  );

	/*多边形求交集*/
	bool PolygonClip(vector<Point> v_left_corner, vector<Point> v_right_corner, vector<Point> &v_overlap_corner);
	//  The function will return YES if the point x,y is inside the polygon, or
	//  NO if it is not.  If the point is exactly on the edge of the polygon,
	//  then the function may return YES or NO.
	bool IsPointInPolygon(vector<Point> poly, Point pt);
	/*若点a大于点b，即点a在点b顺时针方向，返回true;否则返回false*/
	bool PointCmp(const Point &a,const Point &b,const Point &center);

	void ClockwiseSortPoints(std::vector<Point> &vPoints);

	/*排斥实验*/
	bool IsRectCross(const Point &p1,const Point &p2,const Point &q1,const Point &q2);
	/*跨立判断*/
	bool IsLineSegmentCross(const Point &pFirst1,const Point &pFirst2,const Point &pSecond1,const Point &pSecond2);

	bool GetCrossPoint(const Point &p1,const Point &p2,const Point &q1,const Point &q2,long &x,long &y);
	/*获取重叠区域中的像素点数目*/
	void getPixelCountfromOverlap(int width, int height, vector<Point> &poly, int &count, vector<Point> &innerPixel);
private:
	Ptr<FeatureDetector> m_detector;
	Ptr<DescriptorExtractor> m_extractor;  
	Ptr<DescriptorMatcher> m_matcher;  

	string m_detectType;  
	string m_extractType;  
	string m_matchType;

	Mat leftImageMat;						//左边影像，矩阵存储
	Mat rightImageMat;						//右边影像，矩阵存储
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
};

