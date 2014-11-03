#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"
#include <QtGui/QMainWindow>
#include "ui_MainWindow.h"
#include <string>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include "ImageProcess.h"
#include "Util.h"
using namespace std;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

	void init();
	//显示ImageList
	void showImageList(QString path);
	//显示左侧影像
	void showLeftImage();
	//显示右侧影像
	void showRightImage();
	//显示左边影像的特征点
	void showLeftImageFeature();
	//显示右边影像的特征点
	void showRightImageFeature();
	//显示匹配结果
	void showMatching();
	//显示拼接结果
	void showAlignment();

	/*界面显示的逻辑控制*/
	//1.未经过打开文件目录，其他按钮不能操作，state = 0
	//2.未选择影像，不能进行影像操作，state = 1
	//3.已选择影像，state = 2;
	//4.已选择影像，经过特征提取，state = 3;
	//5.已选择影像，经过特征提取，经过特征匹配，state = 4;
	//6.已选择影像，经过特征提取，经过特征匹配，经过影像对齐，state = 5;
	void controlGUI(int state);

	private slots:
		//打开指定文件夹
		void slotOpenDirectory();
		//显示影像列表
		void slotShowImageList();
		//显示选择的影像
		void slotShowSelectedImage(QTreeWidgetItem * item, int column);
		//提取特征点
		void slotExtractFeature();
		//匹配特征点
		void slotMatchFeature();
		//对齐影像
		void slotAlignImage();
		//显示特征点
		void slotShowFeaturePoint();
		//显示匹配
		void slotShowMatching();
		//显示对齐结果
		void slotShowAlignment();
		//显示控制台输出
		void slotShowConsole();
		//显示对齐结果(action)
		void slotViewAlignment();
		//保存结果
		void slotSave();
		
signals:
		//显示影像列表信号
		void signalShowImageList();
		//显示特征点信号
		void signalShowFeaturePoint();
		//显示匹配信号
		void signalShowMatching();
		//显示对齐结果信号
		void signalShowAlignment();
		//显示控制台输出信号
		void signalShowConsole();

private:
	Ui::MainWindowClass ui;

	ImageProcess imageProcess;
	Util util;
	QString leftImagePath;					//左边影像路径
	QString rightImagePath;					//右边影像路径
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

	/************************************************************************/
	/*							资源路径                                    */
	/************************************************************************/
	QString folderPath;
	QString consoleText;					//控制台文本内容

	/************************************************************************/
	/*							操作标志                                     */
	/************************************************************************/
	bool isOpenFlag;						//true表示已经打开文件目录
	bool isSelectedImageFlag;				//true表示已经选择影像
	bool isOverlapFlag;						//true表示选择的两张影像有重叠区域
	bool isExtractedFlag;					//ture表示已经提取特征
	bool isMatchedFlag;						//true表示已经匹配特征
	bool isAlignedFlag;						//true表示已经对齐影像
};

#endif // MAINWINDOW_H
