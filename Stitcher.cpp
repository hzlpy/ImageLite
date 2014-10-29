#include "Stitcher.h"


Stitcher::Stitcher(void)
{
	m_detectType = "SURF";
	m_extractType = "SURF";
	m_matchType = "FlannBased"; //FruteForce,FlannBased
	initModule_nonfree();
}


Stitcher::~Stitcher(void)
{
}

Stitcher::Stitcher(const string& detectType, const string& extractType, const string& matchType)
{
	m_detectType = detectType;  
	m_extractType = extractType;  
	m_matchType = matchType;  
	initModule_nonfree(); 
}

/*****************************************************************************
    *  @brief    : stitchTwoImage 对两张图片进行拼接 (左侧图像映射到右侧)
    *  @author   : Zhangle
    *  @date     : 2014/10/13 21:37
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void Stitcher::stitchTwoImage(Mat mat_left, Mat mat_right)
{
	/*检测特征点*/
	int feature_count = 500;
	vector<KeyPoint> v_keypoint_left = detectKeypoints(mat_left);
	vector<KeyPoint> v_keypoint_right = detectKeypoints(mat_right);
	
	/*提取特征点描述子*/
	Mat descriptor_left = extractorDescriptors(mat_left,v_keypoint_left);
	Mat descriptor_right = extractorDescriptors(mat_right,v_keypoint_right);

	/*匹配*/
	vector<vector<DMatch>> knn_matches; 
	vector<DMatch> matches;
	knnMatch(descriptor_left,descriptor_right,knn_matches,matches,2);
	
	/*从匹配对中得到左边影像和右边影像匹配好的点*/
// 	vector<Point2f> v_point_left, v_point_right;
// 	getPointsfromMatches(v_keypoint_left,v_keypoint_right,matches,v_point_left,v_point_right);

// 	drawPoints(mat_left,v_point_left);
// 	drawPoints(mat_right,v_point_right);
// 	imwrite("../result/left_point.jpg",mat_left);
// 	imwrite("../result/right_point.jpg",mat_right);

	Mat img_matches;
	drawMatches(mat_left,v_keypoint_left,mat_right,v_keypoint_right,matches,img_matches);
	imwrite("../result/matches.jpg",img_matches);

	Mat global_h;
	float homographyReprojectionThreshold = 1.0;
	bool homographyFound = refineMatchesWithHomography(v_keypoint_left,v_keypoint_right,homographyReprojectionThreshold,matches,global_h); 

	/*获得映射后的四个角点坐标*/
	vector<Point> v_warped_corner = getWarpedCorner(global_h,mat_left);

	int xoffset = 0;
	int yoffset = 0;
	Mat warped_img = getWarpedImage(mat_left,global_h,v_warped_corner,xoffset,yoffset);

	Mat panorama = getPanorama(warped_img,mat_right,xoffset,yoffset);

	vector<Point> v_overlap_warped_corner, v_overlap_warp_corner, v_overlap_referenced_corner;
	getOverlap(mat_left,mat_right,global_h,v_overlap_warped_corner,v_overlap_warp_corner,v_overlap_referenced_corner);
	//drawPoints(panorama,v_overlap_warped_corner);
	//imwrite("overlap.jpg",panorama);
 	waitKey(0);
}

vector<KeyPoint> Stitcher::detectKeypoints(Mat img)
{
 	vector<KeyPoint> v_KeyPoints;
	m_detector = FeatureDetector::create(m_detectType);
	m_detector->detect(img, v_KeyPoints); 
	return v_KeyPoints;
}

Mat Stitcher::extractorDescriptors(Mat img, vector<KeyPoint> v_keyPoints)
{
 	Mat mat_descriptor;
	m_extractor = DescriptorExtractor::create(m_extractType);  
	m_extractor->compute(img, v_keyPoints, mat_descriptor); 
	return mat_descriptor;
}

vector<DMatch> Stitcher::matchDescriptors(Mat descriptor_left, Mat descriptor_right )
{
	vector<DMatch> matches;

 
	return matches;
}

vector<DMatch> Stitcher::bestMatch(Mat queryDescriptor, Mat trainDescriptor)
{
	vector<DMatch> matches;
	m_matcher = DescriptorMatcher::create(m_matchType);  
	m_matcher->add(std::vector<Mat>(1, trainDescriptor));  
	m_matcher->train();  
	m_matcher->match(queryDescriptor, matches);
	return matches;
}

void Stitcher::knnMatch(Mat& queryDescriptors, Mat& trainDescriptors, vector<vector<DMatch>>& knn_matches, vector<DMatch>& matches, int k)
{
	matches.clear();
	m_matcher = DescriptorMatcher::create(m_matchType);  
	const float minRatio = 1.f / 1.5f;    
	m_matcher->knnMatch(queryDescriptors,trainDescriptors,knn_matches,k);    
	for (size_t i=0; i<knn_matches.size(); i++)    
	{    
		const cv::DMatch& bestMatch = knn_matches[i][0];    
		const cv::DMatch& betterMatch = knn_matches[i][1];    
		float distanceRatio = bestMatch.distance / betterMatch.distance;    
		if (distanceRatio < minRatio)    
		{    
			matches.push_back(bestMatch);    
		}    
	}    
}
/*****************************************************************************
    *  @brief    : getPointsfromMatches 从描述子中得到对应的特征点
    *  @author   : Zhangle
    *  @date     : 2014/10/13 21:13
    *  @version  : ver 1.0
    *  @inparam  : v_keypoint_left 左边图像的特征描述子
	*  @inparam  : v_keypoint_right 右边图像的特征描述子
	*  @inparam  : matches 匹配对
	*  @inparam  : v_point_left 左边图像的特征点
	*  @inparam  : v_point_right 右边图像的特征点
    *  @outparam :  
*****************************************************************************/
void Stitcher::getPointsfromMatches(vector<KeyPoint> v_keypoint_left, vector<KeyPoint> v_keypoint_right, vector<DMatch> matches, vector<Point2f> &v_point_left, vector<Point2f> &v_point_right)
{
	vector<DMatch>::size_type matchCount = matches.size();
	for(int i=0; i != matchCount; ++i)
	{
		v_point_left.push_back(v_keypoint_left[matches[i].queryIdx].pt);
		v_point_right.push_back(v_keypoint_right[matches[i].trainIdx].pt);
	}
}

void Stitcher::getKeypointsfromMatches(vector<KeyPoint> v_keypoint_left, vector<KeyPoint> v_keypoint_right,vector<DMatch> matches ,vector<KeyPoint> &warp_matched_keypoints, vector<KeyPoint> &reference_mathced_keypoints)
{
// 	vector<DMatch>::size_type matchCount = matches.size();
// 	for(int i=0; i != matchCount; ++i)
// 	{
// 		int queryIdx = matches[i].queryIdx;
// 		int trainIdx = matches[i].trainIdx;
// 		warp_matched_keypoints.push_back(v_keypoint_left[matches[i].queryIdx]);
// 		reference_mathced_keypoints.push_back(v_keypoint_right[matches[i].trainIdx]);
// 	}
}

void Stitcher::drawPoints(Mat mat, vector<Point2f> v_point, Scalar scalar)
{
	Point2f point;
	
	for (vector<Point2f>::iterator iter=v_point.begin(); iter != v_point.end(); ++iter)
	{
		point = *iter;
		circle(mat,point,5,scalar,-1,8,0);
	}
}

void Stitcher::drawPoints(Mat mat, vector<Point> v_point, Scalar scalar)
{
	Point point;
	for (vector<Point>::iterator iter=v_point.begin(); iter != v_point.end(); ++iter)
	{
		point = *iter;
		circle(mat,point,5,scalar,-1,8,0);
	}
}

vector<Point> Stitcher::getWarpedCorner(Mat H, Mat img)
{
	vector<Point> v_img_corner,v_warped_corner;
	Point point;

	v_img_corner.push_back(cvPoint(1,1)); //左上
	v_img_corner.push_back(cvPoint(1,img.rows));//左下
	v_img_corner.push_back(cvPoint(img.cols,img.rows));//右下
	v_img_corner.push_back(cvPoint(img.cols,1));//右上

	for (vector<Point>::size_type i=0; i < v_img_corner.size(); i++)
	{
		double x = v_img_corner.at(i).x;
		double y = v_img_corner.at(i).y;

 		//double Z = 1./( H.at<double>(2,0)*x + H.at<double>(2,1)*y + H.at<double>(2,2) );
		double h33 = H.at<double>(2,2);
		double Z = 1/( H.at<double>(2,0)*x + H.at<double>(2,1)*y + H.at<double>(2,2) );  
 		double X = ( H.at<double>(0,0)*x + H.at<double>(0,1)*y + H.at<double>(0,2) )*Z;
 		double Y = ( H.at<double>(1,0)*x + H.at<double>(1,1)*y + H.at<double>(1,2) )*Z;

		point.x = cvRound(X);
		point.y = cvRound(Y);

		v_warped_corner.push_back(point);
	}
	return v_warped_corner;
}

Mat Stitcher::getWarpedImage(Mat img, Mat H, vector<Point> v_warped_corner, int &xoffset, int &yoffset)
{
	int warped_img_width = 0;
	int warped_img_height = 0;
	xoffset = yoffset = 0;

	Point LT = v_warped_corner.at(0);
	Point LB = v_warped_corner.at(1);
	Point RB = v_warped_corner.at(2);
	Point RT = v_warped_corner.at(3);

	vector<Point> rectangle = getOuterRectangle(v_warped_corner);
	warped_img_width = rectangle.at(2).x;
	warped_img_height = rectangle.at(2).y;
	Mat warped_img = Mat(rectangle.at(2).y,rectangle.at(2).x,img.type());
	//如果是正数，就不用平移
	if (LT.x < 0 || LB.x < 0)
	{
		if (LT.x < LB.x)
		{
			xoffset = 0 - LT.x;
		} 
		else
		{
			xoffset = 0 - LB.x;
		}
	}
	if (LT.y < 0 || RT.y < 0)
	{
		if (LT.y < RT.y)
		{
			yoffset = 0 - LT.y;
		} 
		else
		{
			yoffset = 0 - RT.y;
		}
	}

	//Mat warped_img = Mat(warped_img_height,warped_img_width + xoffset,img.type());

	Point newLT = Point(LT.x+xoffset,LT.y+yoffset);
	Point newLB = Point(LB.x+xoffset,LB.y+yoffset);
	Point newRB = Point(RB.x+xoffset,RB.y+yoffset);
	Point newRT = Point(RT.x+xoffset,RT.y+yoffset);

	Mat shftMat = (Mat_<double>(3,3)<<1.0,0,xoffset, 0,1.0,yoffset, 0,0,1.0);
 	warpPerspective(img,warped_img,shftMat*H,Size(warped_img_width,warped_img_height));
	//imwrite("../result/panorama2.jpg",warped_img);
	return warped_img;
}

Mat Stitcher::getPanorama(Mat warped_img, Mat referenced_img, int xoffset, int yoffset)
{
	int canvas_width, canvas_height;
	/*计算画布的宽高*/
	canvas_width = xoffset + referenced_img.cols; //cols表示width
	canvas_height = (warped_img.rows > referenced_img.rows+yoffset) ? warped_img.rows : (referenced_img.rows+yoffset);
	/*创建全景图*/
	//Mat panorama = Mat(canvas_height,canvas_width,referenced_img.type());
	Mat panorama = Mat::zeros(canvas_height,canvas_width,referenced_img.type());
	warped_img.copyTo(Mat(panorama,Rect(0,0,warped_img.cols,warped_img.rows)));
	imwrite("../result/panorama1.jpg",panorama);

	referenced_img.copyTo(Mat(panorama,Rect(xoffset,yoffset,referenced_img.cols,referenced_img.rows)));
	imwrite("../result/panorama2.jpg",panorama);
	return panorama;
}

bool Stitcher::getOverlap(Mat warp_img, Mat referenced_img, Mat H, vector<Point> &v_overlap_warped_corner, vector<Point> &v_overlap_warp_corner, vector<Point> &v_overlap_referenced_corner)
{
	bool flag = false;
	vector<Point> v_warp_img_corner, v_offset_warped_img_corner;
	vector<Point> v_referenced_img_corner, v_offset_referenced_img_corner;
	int xoffset,yoffset;
	xoffset = yoffset = 0;//初始化
	v_warp_img_corner.push_back(cvPoint(0,0)); //左上
	v_warp_img_corner.push_back(cvPoint(0,warp_img.rows));//左下
	v_warp_img_corner.push_back(cvPoint(warp_img.cols,warp_img.rows));//右下
	v_warp_img_corner.push_back(cvPoint(warp_img.cols,0));//右上

	v_referenced_img_corner.push_back(cv::Point(0,0));
	v_referenced_img_corner.push_back(cv::Point(0,referenced_img.rows));
	v_referenced_img_corner.push_back(cv::Point(referenced_img.cols,referenced_img.rows));
	v_referenced_img_corner.push_back(cv::Point(referenced_img.cols,0));

	/*计算warp_img通过H映射后的坐标值*/
	vector<Point> v_warped_corner = getWarpedCorner(H,warp_img);
	Point warped_LT = v_warped_corner.at(0);
	Point warped_LB = v_warped_corner.at(1);
	Point warped_RB = v_warped_corner.at(2);
	Point warped_RT = v_warped_corner.at(3);
	/*如果是正数，就不用平移*/
	if (warped_LT.x < 0 || warped_LB.x < 0)
	{
		if (warped_LT.x < warped_LB.x)
		{
			xoffset = 0 - warped_LT.x;
		} 
		else
		{
			xoffset = 0 - warped_LB.x;
		}
	}
	if (warped_LT.y < 0 || warped_RT.y < 0)
	{
		if (warped_LT.y < warped_RT.y)
		{
			yoffset = 0 - warped_LT.y;
		} 
		else
		{
			yoffset = 0 - warped_RT.y;
		}
	}

	Point new_warped_LT = Point(warped_LT.x+xoffset,warped_LT.y+yoffset);
	Point new_warped_LB = Point(warped_LB.x+xoffset,warped_LB.y+yoffset);
	Point new_warped_RB = Point(warped_RB.x+xoffset,warped_RB.y+yoffset);
	Point new_warped_RT = Point(warped_RT.x+xoffset,warped_RT.y+yoffset);

	v_offset_warped_img_corner.push_back(new_warped_LT);
	v_offset_warped_img_corner.push_back(new_warped_LB);
	v_offset_warped_img_corner.push_back(new_warped_RB);
	v_offset_warped_img_corner.push_back(new_warped_RT);

	Point referenced_LT = v_referenced_img_corner.at(0);
	Point referenced_LB = v_referenced_img_corner.at(1);
	Point referenced_RB = v_referenced_img_corner.at(2);
	Point referenced_RT = v_referenced_img_corner.at(3);

	Point new_referenced_LT = Point(referenced_LT.x+xoffset,referenced_LT.y+yoffset);
	Point new_referenced_LB = Point(referenced_LB.x+xoffset,referenced_LB.y+yoffset);
	Point new_referenced_RB = Point(referenced_RB.x+xoffset,referenced_RB.y+yoffset);
	Point new_referenced_RT = Point(referenced_RT.x+xoffset,referenced_RT.y+yoffset);

	v_offset_referenced_img_corner.push_back(new_referenced_LT);
	v_offset_referenced_img_corner.push_back(new_referenced_LB);
	v_offset_referenced_img_corner.push_back(new_referenced_RB);
	v_offset_referenced_img_corner.push_back(new_referenced_RT);

	Mat mat = imread("../result/panorama2.jpg");
	drawPoints(mat,v_offset_warped_img_corner,Scalar(0,0,255));
	drawPoints(mat,v_offset_referenced_img_corner,Scalar(0,0,255));
	drawPloygon(mat,v_offset_warped_img_corner,Scalar(255,255,255));
	drawPloygon(mat,v_offset_referenced_img_corner,Scalar(255,255,255));
	//imwrite("../result/overlap.jpg",mat);

	//计算warped后的图像与reference图像的交点，点是在warped后图像上的
	if (!PolygonClip(v_offset_warped_img_corner,v_offset_referenced_img_corner,v_overlap_warped_corner))
	{
		//flag = false;
		return flag;
	}
	drawPoints(mat,v_overlap_warped_corner,Scalar(0,255,0));
	imwrite("../result/overlap.jpg",mat);
	
	for (vector<Point>::iterator iter = v_overlap_warped_corner.begin(); iter != v_overlap_warped_corner.end(); ++iter )
	{
		/*得到参考图像上的重叠区域顶点*/
		Point p = *iter;
		Point p_referenced;
		p_referenced.x = p.x - xoffset;
		p_referenced.y = p.y - yoffset;
		v_overlap_referenced_corner.push_back(p_referenced);

		/*将warped的点反映射回warp图像上，得到其坐标*/
		Mat warpedMat = Mat::zeros(3,1,CV_64FC1);
		warpedMat.at<double>(0,0) = p.x-xoffset;
		warpedMat.at<double>(1,0) = p.y-yoffset;
		warpedMat.at<double>(2,0) = 1.0;

		Mat warpMat = H.inv() * warpedMat;
		Point over_warp_corner;
		//vector<Point> v_over_warp_corner;
		over_warp_corner.x = cvRound(warpMat.at<double>(0,0)/warpMat.at<double>(2,0));
		over_warp_corner.y = cvRound(warpMat.at<double>(1,0)/warpMat.at<double>(2,0));
		v_overlap_warp_corner.push_back(over_warp_corner);
	}

	//drawPloygon(referenced_img,v_overlap_referenced_corner,Scalar(0,0,255));
	//imwrite("../result/ref.jpg",referenced_img);
	return flag;
}

vector<Point> Stitcher::getOuterRectangle(vector<Point> v_warped_corner)
{
	Point LT = v_warped_corner.at(0);
	Point LB = v_warped_corner.at(1);
	Point RB = v_warped_corner.at(2);
	Point RT = v_warped_corner.at(3);

	int max_x,min_x,max_y,min_y;

	min_y = (LT.y < RT.y) ? LT.y : RT.y;
	max_y = (LB.y > RB.y) ? LB.y : RB.y;

	min_x = (LT.x < LB.x) ? LT.x : LB.x;
	max_x = (RT.x > RB.x) ? RT.x : RB.x;

	Point rect_LT = cvPoint(0,0);
	Point rect_LB = cvPoint(0,max_y-min_y);
	Point rect_RB = cvPoint(max_x-min_x, max_y-min_y);
	Point rect_RT = cvPoint(max_x-min_x, 0);

	vector<Point> rectangle;
	rectangle.push_back(rect_LT);
	rectangle.push_back(rect_LB);
	rectangle.push_back(rect_RB);
	rectangle.push_back(rect_RT);
	return rectangle;
}

/*****************************************************************************
    *  @brief    : refineMatchesWithHomography
    *  @author   : Zhangle
    *  @date     : 2014/10/13 21:10
    *  @version  : ver 1.0
    *  @inparam  : queryKeypoints 特征点描述子
	*  @inparam  : trainKeypoints 特征点描述子	
	*  @inparam  : reprojectionThreshold
	*  @inparam  : matches refine后的匹配对
	*  @inparam  : homography homography矩阵
    *  @outparam :  
*****************************************************************************/
bool Stitcher::refineMatchesWithHomography(
	const std::vector<cv::KeyPoint>& queryKeypoints,    
	const std::vector<cv::KeyPoint>& trainKeypoints,     
	float reprojectionThreshold,    
	std::vector<cv::DMatch>& matches,    
	cv::Mat& homography  )  
{  
	const int minNumberMatchesAllowed = 4;    
	if (matches.size() < minNumberMatchesAllowed)    
		return false;    
	// Prepare data for cv::findHomography    
	vector<Point2f> queryPoints; //描述子对应的特征点
	vector<Point2f> trainPoints;    
	//从描述子从得到特征点
	getPointsfromMatches(queryKeypoints,trainKeypoints,matches,queryPoints,trainPoints);
	// Find homography matrix and get inliers mask    
	std::vector<unsigned char> inliersMask(matches.size());    
	homography = cv::findHomography(queryPoints,     
		trainPoints,     
		CV_FM_RANSAC,     
		reprojectionThreshold,     
		inliersMask);    
	vector<DMatch> inliers;    
	for (size_t i=0; i<inliersMask.size(); i++)    
	{    
		if (inliersMask[i]) 
			inliers.push_back(matches[i]);    
	}    
	matches.swap(inliers);
	//Mat homoShow;
	//drawMatches(mat_left,queryKeypoints,mat_right,trainKeypoints,matches,homoShow,Scalar::all(-1),CV_RGB(255,255,255),Mat(),2);
	//imwrite("../result/homoshow.jpg",homoShow);
	return matches.size() > minNumberMatchesAllowed;   
}  

/*****************************************************************************
    *  @brief    : PolygonClip 多边形求交集
    *  @author   : Zhangle
    *  @date     : 2014/9/20 19:33
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
bool Stitcher::PolygonClip(vector<Point> v_left_corner, vector<Point> v_right_corner, vector<Point> &v_overlap_corner)
{
	if (v_left_corner.size() < 3 || v_right_corner.size() < 3)
	{
		return false;
	}

	long x,y;
	//计算多边形交点
	for (int i = 0;i < v_left_corner.size();i++)
	{
		int v_left_corner_next_idx = (i + 1) % v_left_corner.size();
		for (int j = 0;j < v_right_corner.size();j++)
		{
			int v_right_corner_next_idx = (j + 1) % v_right_corner.size();
			if (GetCrossPoint(v_left_corner[i],v_left_corner[v_left_corner_next_idx],
				v_right_corner[j],v_right_corner[v_right_corner_next_idx],
				x,y))
			{
				v_overlap_corner.push_back(cv::Point(x,y));
			}
		}
	}

	//计算多边形内部点
	for(int i = 0;i < v_left_corner.size();i++)
	{
		if (IsPointInPolygon(v_right_corner,v_left_corner[i]))
		{
			v_overlap_corner.push_back(v_left_corner[i]);
		}
	}
	for (int i = 0;i < v_right_corner.size();i++)
	{
		if (IsPointInPolygon(v_left_corner,v_right_corner[i]))
		{
			v_overlap_corner.push_back(v_right_corner[i]);
		}
	}

	if(v_overlap_corner.size() <= 0)
		return false;

	//点集排序 
	ClockwiseSortPoints(v_overlap_corner);
	return true;
}

bool Stitcher::IsPointInPolygon(vector<Point> poly,Point pt)
{
	int i,j;
	bool c = false;
	for (i = 0,j = poly.size() - 1;i < poly.size();j = i++)
	{
		if ((((poly[i].y <= pt.y) && (pt.y < poly[j].y)) ||
			((poly[j].y <= pt.y) && (pt.y < poly[i].y)))
			&& (pt.x < (poly[j].x - poly[i].x) * (pt.y - poly[i].y)/(poly[j].y - poly[i].y) + poly[i].x))
		{
			c = !c;
		}
	}
	return c;
}

//若点a大于点b,即点a在点b顺时针方向,返回true,否则返回false
/*****************************************************************************
    *  @brief    : PointCmp
    *  @author   : Zhangle
    *  @date     : 2014/10/8 14:39
    *  @version  : ver 1.0
    *  @inparam  : a,b,center 向量OA等于(a-center),向量OB等于(b-center)
    *  @outparam : true/false 如果det<0返回true, 
*****************************************************************************/
bool Stitcher::PointCmp(const Point &a,const Point &b,const Point &center)
{
// 	if (a.x >= 0 && b.x < 0)
// 		return true;
// 	if (a.x == 0 && b.x == 0)
// 		return a.y > b.y;
	if (a.x - center.x >= 0 && b.x - center.x < 0)
		return true;
	if (a.x - center.x < 0 && b.x - center.x >= 0)
		return false;
	if (a.x - center.x == 0 && b.x - center.x == 0) {
		if (a.y - center.y >= 0 || b.y - center.y >= 0)
			return a.y > b.y;
		return b.y > a.y;
	}
	//向量OA和向量OB的叉积det
	int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
	if (det < 0)//det<0，则向量OA与OB之间的夹角(OA转向OB)大于180°
		return true;
	if (det > 0)//det<0，则向量OA与OB之间的夹角小于180°
		return false;
	//向量OA和向量OB共线，以距离判断大小
	int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
	int d2 = (b.x - center.x) * (b.x - center.y) + (b.y - center.y) * (b.y - center.y);
	return d1 > d2;
}
void Stitcher::ClockwiseSortPoints(std::vector<Point> &vPoints)
{
	bool flag = false;
	//计算重心
	cv::Point center;
	double x = 0,y = 0;
	for (int i = 0;i < vPoints.size();i++)
	{
		x += vPoints[i].x;
		y += vPoints[i].y;
	}
	center.x = (int)x/vPoints.size();
	center.y = (int)y/vPoints.size();

	//冒泡排序
	for(int i = 0;i < vPoints.size() - 1;i++)
	{
		for (int j = 0;j < vPoints.size() - i - 1;j++)
		{
			flag = PointCmp(vPoints[j],vPoints[j+1],center);
			if (flag)
			{
				Point tmp = vPoints[j];
				vPoints[j] = vPoints[j + 1];
				vPoints[j + 1] = tmp;
			}
		}
	}
}

/*
 *	线段相交
 */
//排斥实验
bool Stitcher::IsRectCross(const Point &p1,const Point &p2,const Point &q1,const Point &q2)
{
	bool ret = min(p1.x,p2.x) <= max(q1.x,q2.x)    &&
		min(q1.x,q2.x) <= max(p1.x,p2.x) &&
		min(p1.y,p2.y) <= max(q1.y,q2.y) &&
		min(q1.y,q2.y) <= max(p1.y,p2.y);
	return ret;
}
//跨立判断
bool Stitcher::IsLineSegmentCross(const Point &pFirst1,const Point &pFirst2,const Point &pSecond1,const Point &pSecond2)
{
	long line1,line2;
	line1 = pFirst1.x * (pSecond1.y - pFirst2.y) +
		pFirst2.x * (pFirst1.y - pSecond1.y) +
		pSecond1.x * (pFirst2.y - pFirst1.y);
	line2 = pFirst1.x * (pSecond2.y - pFirst2.y) +
		pFirst2.x * (pFirst1.y - pSecond2.y) + 
		pSecond2.x * (pFirst2.y - pFirst1.y);
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
		return false;

	line1 = pSecond1.x * (pFirst1.y - pSecond2.y) +
		pSecond2.x * (pSecond1.y - pFirst1.y) +
		pFirst1.x * (pSecond2.y - pSecond1.y);
	line2 = pSecond1.x * (pFirst2.y - pSecond2.y) + 
		pSecond2.x * (pSecond1.y - pFirst2.y) +
		pFirst2.x * (pSecond2.y - pSecond1.y);
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
		return false;
	return true;
}

bool Stitcher::GetCrossPoint(const Point &p1,const Point &p2,const Point &q1,const Point &q2,long &x,long &y)
{
	if(IsRectCross(p1,p2,q1,q2))
	{
		if (IsLineSegmentCross(p1,p2,q1,q2))
		{
			//求交点
			long tmpLeft,tmpRight;
			tmpLeft = (q2.x - q1.x) * (p1.y - p2.y) - (p2.x - p1.x) * (q1.y - q2.y);
			tmpRight = (p1.y - q1.y) * (p2.x - p1.x) * (q2.x - q1.x) + q1.x * (q2.y - q1.y) * (p2.x - p1.x) - p1.x * (p2.y - p1.y) * (q2.x - q1.x);

			x = (int)((double)tmpRight/(double)tmpLeft);

			tmpLeft = (p1.x - p2.x) * (q2.y - q1.y) - (p2.y - p1.y) * (q1.x - q2.x);
			tmpRight = p2.y * (p1.x - p2.x) * (q2.y - q1.y) + (q2.x- p2.x) * (q2.y - q1.y) * (p1.y - p2.y) - q2.y * (q1.x - q2.x) * (p2.y - p1.y); 
			y = (int)((double)tmpRight/(double)tmpLeft);
			return true;
		}
	}
	return false;
}

void Stitcher::drawPloygon(Mat mat, vector<Point> corner, Scalar scalar)
{
	for (int i=0; i<corner.size()-1; i++)
	{
		line(mat,corner.at(i),corner.at(i+1),scalar,2);
	}
	line(mat,corner.at(corner.size()-1),corner.at(0),scalar,2);
}

Mat Stitcher::readImage(string path)
{
	Mat img = imread(path);
	return img;
}

bool Stitcher::writeImage(Mat mat, string input_path, int type)
{
// 	FilePath file;
// 	string output_path = file.setFilePath(input_path,type);
// 	return imwrite(output_path,mat);
	return true;
}

/*****************************************************************************
    *  @brief    : getPixelCountfromOverlap 获取重叠区域中的像素点数目
    *  @author   : Zhangle
    *  @date     : 2014/10/15 15:01
    *  @version  : ver 1.0
    *  @inparam  : width, 全景图的宽
	*  @inparam  : width, 全景图的高
	*  @inparam  : poly, 重叠区域的角点
	*  @inparam  : count, 像素点数目
    *  @outparam :  
*****************************************************************************/
void Stitcher::getPixelCountfromOverlap(int width, int height, vector<Point> &poly, int &count, vector<Point> &innerPixel)
{
	count = 0;
	int x = 0;//在全景图中水平方向(由左至右)的值
	int y = 0;//在全景图中竖直方向(由上到下)的值
	Point pixel;//初始化一个像素点
	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
		{
			pixel.x = x;
			pixel.y = y;
			if (IsPointInPolygon(poly,pixel))
			{
				count ++;
				innerPixel.push_back(pixel);
			}
		}
	}
}

void Stitcher::getOuterRectangle(vector<Point> &poly_corner, vector<Point> &outer_rect)
{
	int max_x,max_y,min_x,min_y;
	min_x = min_y = max_x = max_y = 0;
	//从角点中找出最大的x,y和最小的x,y
	for (int i=0; i<poly_corner.size(); i++)//先找出最大的x,y
	{
		int x = poly_corner.at(i).x;
		int y = poly_corner.at(i).y;
		if (x > max_x)
		{
			max_x = x;
		}
		if (y > max_y)
		{
			max_y = y;
		}
	}
	min_x = max_x;
	min_y = max_y;
	for (int i=0; i<poly_corner.size(); i++)//先找出最小的x,y
	{
		int x = poly_corner.at(i).x;
		int y = poly_corner.at(i).y;
		if (x < min_x)
		{
			min_x = x;
		}
		if (y < min_y)
		{
			min_y = y;
		}
	}
	Point LT,LB,RB,RT;
	LT.x = min_x;
	LT.y = min_y;
	LB.x = min_x;
	LB.y = max_y;
	RB.x = max_x;
	RB.y = max_y;
	RT.x = max_x;
	RT.y = min_y;
	outer_rect.push_back(LT);
	outer_rect.push_back(LB);
	outer_rect.push_back(RB);
	outer_rect.push_back(RT);
}

