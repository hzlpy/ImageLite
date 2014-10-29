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
	/*����·����ȡӰ��*/
	Mat readImage(string path);
	/*����·���洢ͼ��*/
	bool writeImage(Mat mat, string, int type);
	/*��left��right����Ӱ�����ƴ�ӡ�right��ΪreferenceӰ��left���б任*/
	void stitchTwoImage(Mat mat_left, Mat mat_right);

	/*���������*/
	vector<KeyPoint> detectKeypoints(Mat img);
	/*��ȡ������*/
	Mat extractorDescriptors(Mat img, vector<KeyPoint> v_keypoint);

	/*ƥ�䷽��*/
	vector<DMatch> matchDescriptors(Mat descriptor_left, Mat descriptor_right );
	vector<DMatch> bestMatch(Mat descriptor_left, Mat descriptor_right);
	void knnMatch(Mat& queryDescriptor, Mat& trainDescriptor, vector<vector<DMatch>>& knn_matches, vector<DMatch>& matches, int k);   // K����ƥ��

	/*��ƥ����еõ����Ӱ����ұ�Ӱ���е�����������(x,y)*/
	void getPointsfromMatches(vector<KeyPoint> v_keypoint_left, vector<KeyPoint> v_keypoint_right,vector<DMatch> matcher ,vector<Point2f> &point_left, vector<Point2f> &point_right);

	/*ƥ����ƥ����еõ����Ӱ����ұ�Ӱ���е�keypoint*/
	void getKeypointsfromMatches(vector<KeyPoint> v_keypoint_left, vector<KeyPoint> v_keypoint_right,vector<DMatch> matches ,vector<KeyPoint> &warp_matched_keypoints, vector<KeyPoint> &reference_mathced_keypoints);
	/*��mat�ϻ���*/
	void drawPoints(Mat mat, vector<Point2f> v_point, Scalar scalar);
	void drawPoints(Mat mat, vector<Point> v_point, Scalar scalar);
	/*�������*/
	void drawPloygon(Mat mat, vector<Point> corner, Scalar scalar);

	/*����leftӰ����ĸ���ӳ�䵽rightӰ���ϵ�����*/
	vector<Point> getWarpedCorner(Mat h, Mat img);

	/*���hӳ����ͼ��*/
	Mat getWarpedImage(Mat img, Mat H, vector<Point> v_warped_corner, int &xoffset, int &yoffset);

	/*���ȫ��ͼ*/
	Mat getPanorama(Mat warped_img, Mat referenced_img, int xoffset, int yoffset);

	/*��ȡwarp_img��referenced_img֮����ص���������*/
	bool getOverlap(Mat warp_img, Mat referenced_img, Mat H, vector<Point> &v_overlap_warped_corner, vector<Point> &v_overlap_warp_corner, vector<Point> &v_over_referenced_corner);

	/*��ȡ�ĸ���Χ�ɵ��ı��ε���Χ��������*/
	vector<Point> getOuterRectangle(vector<Point> warped_corner);
	/*��ȡ������ı��ε���Χ��������*/
	void getOuterRectangle(vector<Point> &poly_corner, vector<Point> &outer_rect);

	bool refineMatchesWithHomography(const vector<KeyPoint>& queryKeypoints,    
		const vector<KeyPoint>& trainKeypoints,     
		float reprojectionThreshold,    
		vector<DMatch>& matches,    
		cv::Mat& homography  );

	/*������󽻼�*/
	bool PolygonClip(vector<Point> v_left_corner, vector<Point> v_right_corner, vector<Point> &v_overlap_corner);
	//  The function will return YES if the point x,y is inside the polygon, or
	//  NO if it is not.  If the point is exactly on the edge of the polygon,
	//  then the function may return YES or NO.
	bool IsPointInPolygon(vector<Point> poly, Point pt);
	/*����a���ڵ�b������a�ڵ�b˳ʱ�뷽�򣬷���true;���򷵻�false*/
	bool PointCmp(const Point &a,const Point &b,const Point &center);

	void ClockwiseSortPoints(std::vector<Point> &vPoints);

	/*�ų�ʵ��*/
	bool IsRectCross(const Point &p1,const Point &p2,const Point &q1,const Point &q2);
	/*�����ж�*/
	bool IsLineSegmentCross(const Point &pFirst1,const Point &pFirst2,const Point &pSecond1,const Point &pSecond2);

	bool GetCrossPoint(const Point &p1,const Point &p2,const Point &q1,const Point &q2,long &x,long &y);
	/*��ȡ�ص������е����ص���Ŀ*/
	void getPixelCountfromOverlap(int width, int height, vector<Point> &poly, int &count, vector<Point> &innerPixel);
private:
	Ptr<FeatureDetector> m_detector;
	Ptr<DescriptorExtractor> m_extractor;  
	Ptr<DescriptorMatcher> m_matcher;  

	string m_detectType;  
	string m_extractType;  
	string m_matchType;

	Mat leftImageMat;						//���Ӱ�񣬾���洢
	Mat rightImageMat;						//�ұ�Ӱ�񣬾���洢
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
};

