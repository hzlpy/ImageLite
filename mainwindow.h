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
	//��ʾImageList
	void showImageList(QString path);
	//��ʾ���Ӱ��
	void showLeftImage();
	//��ʾ�Ҳ�Ӱ��
	void showRightImage();
	//��ʾ���Ӱ���������
	void showLeftImageFeature();
	//��ʾ�ұ�Ӱ���������
	void showRightImageFeature();
	//��ʾƥ����
	void showMatching();
	//��ʾƴ�ӽ��
	void showAlignment();

	/*������ʾ���߼�����*/
	//1.δ�������ļ�Ŀ¼��������ť���ܲ�����state = 0
	//2.δѡ��Ӱ�񣬲��ܽ���Ӱ�������state = 1
	//3.��ѡ��Ӱ��state = 2;
	//4.��ѡ��Ӱ�񣬾���������ȡ��state = 3;
	//5.��ѡ��Ӱ�񣬾���������ȡ����������ƥ�䣬state = 4;
	//6.��ѡ��Ӱ�񣬾���������ȡ����������ƥ�䣬����Ӱ����룬state = 5;
	void controlGUI(int state);

	private slots:
		//��ָ���ļ���
		void slotOpenDirectory();
		//��ʾӰ���б�
		void slotShowImageList();
		//��ʾѡ���Ӱ��
		void slotShowSelectedImage(QTreeWidgetItem * item, int column);
		//��ȡ������
		void slotExtractFeature();
		//ƥ��������
		void slotMatchFeature();
		//����Ӱ��
		void slotAlignImage();
		//��ʾ������
		void slotShowFeaturePoint();
		//��ʾƥ��
		void slotShowMatching();
		//��ʾ������
		void slotShowAlignment();
		//��ʾ����̨���
		void slotShowConsole();
		//��ʾ������(action)
		void slotViewAlignment();
		//������
		void slotSave();
		
signals:
		//��ʾӰ���б��ź�
		void signalShowImageList();
		//��ʾ�������ź�
		void signalShowFeaturePoint();
		//��ʾƥ���ź�
		void signalShowMatching();
		//��ʾ�������ź�
		void signalShowAlignment();
		//��ʾ����̨����ź�
		void signalShowConsole();

private:
	Ui::MainWindowClass ui;

	ImageProcess imageProcess;
	Util util;
	QString leftImagePath;					//���Ӱ��·��
	QString rightImagePath;					//�ұ�Ӱ��·��
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

	/************************************************************************/
	/*							��Դ·��                                    */
	/************************************************************************/
	QString folderPath;
	QString consoleText;					//����̨�ı�����

	/************************************************************************/
	/*							������־                                     */
	/************************************************************************/
	bool isOpenFlag;						//true��ʾ�Ѿ����ļ�Ŀ¼
	bool isSelectedImageFlag;				//true��ʾ�Ѿ�ѡ��Ӱ��
	bool isOverlapFlag;						//true��ʾѡ�������Ӱ�����ص�����
	bool isExtractedFlag;					//ture��ʾ�Ѿ���ȡ����
	bool isMatchedFlag;						//true��ʾ�Ѿ�ƥ������
	bool isAlignedFlag;						//true��ʾ�Ѿ�����Ӱ��
};

#endif // MAINWINDOW_H
