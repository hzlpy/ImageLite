#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	/*��ʼ��*/
	init();
	/*connect*/
	connect(ui.action,SIGNAL(triggered()),this,SLOT(slotOpenDirectory()));
	connect(this,SIGNAL(signalShowImageList()),this,SLOT(slotShowImageList()));
	/*ѡ��Ӱ�񣬲���ʾ*/
	connect(ui.treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),this,SLOT(slotShowSelectedImage(QTreeWidgetItem*, int)));
	/*������ȡ������ʾ���*/
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(slotExtractFeature()));//������ȡ
	connect(this,SIGNAL(signalShowFeaturePoint()),this,SLOT(slotShowFeaturePoint()));
	/*����ƥ�䣬����ʾ���*/
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(slotMatchFeature()));//����ƥ��
	connect(this,SIGNAL(signalShowMatching()),this,SLOT(slotShowMatching()));
	/*Ӱ����룬����ʾ���*/
	connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(slotAlignImage()));//Ӱ�����
	connect(this,SIGNAL(signalShowAlignment()),this,SLOT(slotShowAlignment()));

	connect(this,SIGNAL(signalShowConsole()),this,SLOT(slotShowConsole()));

	connect(ui.action_2,SIGNAL(triggered()),this,SLOT(slotViewAlignment()));
}

MainWindow::~MainWindow()
{

}

/*****************************************************************************
    *  @brief    : init ��ʼ��
    *  @author   : Zhangle
    *  @date     : 2014/10/24 19:50
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::init()
{
	this->showMaximized();
	ui.treeWidget->setHeaderLabel(tr("Ӱ��Ŀ¼"));
	ui.tabWidget->setCurrentIndex(0);//ָ��tabwedget�еĵ�ǰtab

	leftImagePath = "";							//���Ӱ��·��
	rightImagePath = "";						//�ұ�Ӱ��·��
	leftImageMat = NULL;						//���Ӱ�񣬾���洢
	rightImageMat = NULL;						//�ұ�Ӱ�񣬾���洢
	featureType = "";							//����������
	leftImageDesc = NULL;						//���Ӱ���������������
	rightImageDesc = NULL;						//�ұ�Ӱ���������������
	globalHomo = NULL;							//Homography
	matchMat = NULL;							//ƥ����
	xoffset = 0;								//Ӱ��ӳ���x��������Ҫ��λ��
	yoffset = 0;								//Ӱ��ӳ���y��������Ҫ��λ��
	alfa = 0;
	lambda = 0;
	folderPath = "";
}

/*****************************************************************************
    *  @brief    : showImageList ��ȡ·��path������Ӱ�񣬲���һ��QList����
    *  @author   : Zhangle
    *  @date     : 2014/10/21 20:48
    *  @version  : ver 1.0
    *  @inparam  : path ����·��
    *  @outparam : QList 
*****************************************************************************/
void MainWindow::showImageList(QString path)
{
	//QList<QTreeWidgetItem *> treeWidgetItemList;
	//��ȡqPath·���µ��ļ�
	QString qPath;
	qPath = path;
	QStringList fileList;
	QDir dir(qPath);
	if (!dir.exists())
	{
		return;
	}
	dir.setFilter(QDir::Dirs|QDir::Files);
	dir.setSorting(QDir::Name | QDir::Reversed);
	QFileInfoList list = dir.entryInfoList();
	//ѭ��
	int i = 0;
	int filecount = 0;
	QTreeWidgetItem *imageItem = new QTreeWidgetItem(ui.treeWidget,QStringList(QString("Ӱ���б�")));
	do 
	{
		QFileInfo fileInfo = list.at(i);
		if(fileInfo.fileName() == "." || fileInfo.fileName()== "..") 
		{ 
			//qDebug()<<"filedir="<<fileInfo.fileName();
			i++;
			continue;
		}
		bool bisDir=fileInfo.isDir();
		if (bisDir)
		{
		}
		else
		{
			QString currentFileName = fileInfo.fileName();
			bool Reght=currentFileName.endsWith(".JPG", Qt::CaseInsensitive);
			if(Reght) 
			{ 
 				filecount++;
 				QTreeWidgetItem *imageItem_1 = new QTreeWidgetItem(imageItem,QStringList(currentFileName)); //�ӽڵ�
				imageItem_1->setIcon(0,QIcon(folderPath+currentFileName));
				imageItem->addChild(imageItem_1); //����ӽڵ�
			} 
			i++;
		}
	} while (i<list.size());
	ui.treeWidget->expandAll(); //���ȫ��չ��
}

/*****************************************************************************
    *  @brief    : showLeftImage ��ʾӰ��
    *  @author   : Zhangle
    *  @date     : 2014/10/21 21:59
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showLeftImage()
{
	Mat mat = leftImageMat.clone();
	QImage qimg = util.Mat2QImage(mat);
	ui.label->clear();
	int label_width = ui.label->width();
	int label_height = ui.label->height();
	int image_width = qimg.width();
	int image_height = qimg.height();
	int scaled_width, scaled_height;
	util.getScaledRatio(image_width,image_height,label_width,label_height,&scaled_width,&scaled_height);

	ui.label->setPixmap(QPixmap::fromImage(qimg).scaled(scaled_width,scaled_height,Qt::KeepAspectRatio));
}
/*****************************************************************************
    *  @brief    : showRightImage ��ʾ�Ҳ�ͼ��
    *  @author   : Zhangle
    *  @date     : 2014/10/22 14:35
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showRightImage()
{
	Mat mat = rightImageMat.clone();
	QImage qimg = util.Mat2QImage(mat);
	ui.label_2->clear();

	int label_width = ui.label_2->width();
	int label_height = ui.label_2->height();
	int image_width = qimg.width();
	int image_height = qimg.height();
	int scaled_width, scaled_height;
	util.getScaledRatio(image_width,image_height,label_width,label_height,&scaled_width,&scaled_height);

	ui.label_2->setPixmap(QPixmap::fromImage(qimg).scaled(scaled_width,scaled_height,Qt::KeepAspectRatio));
}
/*****************************************************************************
    *  @brief    : showLeftImageFeature ��ʾ������
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:31
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showLeftImageFeature()
{
	Mat warpImageMat = leftImageMat.clone();
	/*���������ԭӰ����ʾ����*/
	drawKeypoints(warpImageMat,leftImageKeyPoints,warpImageMat,Scalar(0,0,255));//�������㻭��Ӱ����
	QImage qimg = util.Mat2QImage(warpImageMat); //��Matת��ΪQImage

	int label_width = ui.label_3->width();
	int label_height = ui.label_3->height();
	int image_width = qimg.width();
	int image_height = qimg.height();
	int scaled_width, scaled_height;
	util.getScaledRatio(image_width,image_height,label_width,label_height,&scaled_width,&scaled_height);

	ui.label_3->clear();
	ui.label_3->setPixmap(QPixmap::fromImage(qimg).scaled(scaled_width,scaled_height,Qt::KeepAspectRatio));
}
/*****************************************************************************
    *  @brief    : showRightImageFeature ��ʾ������
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:48
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showRightImageFeature()
{
	Mat referImageMat = rightImageMat.clone();
	/*���������ԭӰ����ʾ����*/
	drawKeypoints(referImageMat,rightImageKeyPoints,referImageMat,Scalar(0,255,0));//�������㻭��Ӱ����
	QImage qimg = util.Mat2QImage(referImageMat); //��Matת��ΪQImage

	int label_width = ui.label_4->width();
	int label_height = ui.label_4->height();
	int image_width = qimg.width();
	int image_height = qimg.height();
	int scaled_width, scaled_height;
	util.getScaledRatio(image_width,image_height,label_width,label_height,&scaled_width,&scaled_height);

	ui.label_4->clear();
	ui.label_4->setPixmap(QPixmap::fromImage(qimg).scaled(scaled_width,scaled_height,Qt::KeepAspectRatio));
}
/*****************************************************************************
    *  @brief    : showMatching ��ʾƥ����
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:48
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showMatching()
{
	Mat matchImage = matchMat.clone();//��¡һ��ƥ����matchMat���������ں�������
	QImage qimg = util.Mat2QImage(matchImage); 

	int label_width = ui.label_5->width();
	int label_height = ui.label_5->height();
	int image_width = qimg.width();
	int image_height = qimg.height();
	int scaled_width, scaled_height;
	util.getScaledRatio(image_width,image_height,label_width,label_height,&scaled_width,&scaled_height);
	qDebug() << "label_5 " <<scaled_height << scaled_width << endl;
	ui.label_5->clear();
	ui.label_5->setPixmap(QPixmap::fromImage(qimg).scaled(scaled_width,scaled_height,Qt::KeepAspectRatio));
	ui.label_5->setGeometry((label_width-scaled_width)/2,1,scaled_width,label_height);
}
/*****************************************************************************
    *  @brief    : showAlignment ��ʾ����ƴ�ӽ��
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:49
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showAlignment()
{
	Mat alignMat = alignResult.clone();
	QImage qimg = util.Mat2QImage(alignMat);

	int label_width = ui.label_6->width();
	int label_height = ui.label_6->height();
	int image_width = qimg.width();
	int image_height = qimg.height();
	int scaled_width, scaled_height;
	util.getScaledRatio(image_width,image_height,label_width,label_height,&scaled_width,&scaled_height);
	qDebug() << "label_6 " <<scaled_height << scaled_width << endl;
	ui.label_6->clear();
	ui.label_6->setPixmap(QPixmap::fromImage(qimg).scaled(scaled_width,scaled_height,Qt::KeepAspectRatio));
	ui.label_6->setGeometry((label_width-scaled_width)/2,1,scaled_width,label_height);
}

						/************************************************************************/
						/*								�ۺ���ʵ��							    */
						/************************************************************************/
/*****************************************************************************
    *  @brief    : slotOpenDirectory
    *  @author   : Zhangle
    *  @date     : 2014/10/24 13:13
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotOpenDirectory()
{
	//�����ļ���ѡ�񴰿ڣ�ѡ���ȷ��
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		"/home",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	folderPath = dir+"\\";
	ui.treeWidget->setHeaderLabel(folderPath);
	consoleText = "�� "+folderPath+" Ŀ¼";
	signalShowConsole();
	signalShowImageList();
	
}

/*****************************************************************************
    *  @brief    : slotShowImageList
    *  @author   : Zhangle
    *  @date     : 2014/10/24 14:04
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotShowImageList()
{
	showImageList(folderPath);
}

/*****************************************************************************
    *  @brief    : slotShowSelectedImage
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:54
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotShowSelectedImage(QTreeWidgetItem * qTreeWidgetItem, int column)
{
	QTreeWidgetItem *parent = qTreeWidgetItem->parent();
	if (NULL == parent)
	{
		return;
	}
	int col = parent->indexOfChild(qTreeWidgetItem);
	QString qText = qTreeWidgetItem->text(0);
	QString qPath = folderPath+qText;
	consoleText = "ѡ��"+qText;
	if (1 == col % 2)	//���
	{
		leftImageMat = imread(util.q2s(qPath));	//��ȡ���Ӱ�񣬴�Ϊmat��ʽ
		showLeftImage();
	}
	else				//�Ҳ�
	{
		rightImageMat = imread(util.q2s(qPath));	//��ȡ�ұ�Ӱ�񣬴�Ϊmat��ʽ
		showRightImage();
	}
	signalShowConsole();
	ui.tabWidget->setCurrentIndex(0);
}

/*****************************************************************************
    *  @brief    : slotExtractFeature 
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:58
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotExtractFeature()
{
	consoleText = "��ʼ��ȡ����..";
	//������ʾ����̨����ź�
	signalShowConsole();
	int col = ui.comboBox->currentIndex();
	featureType = ui.comboBox->currentText();
	qDebug() << "featureType = " << featureType << endl;
	//��ȡ����
	imageProcess.leftImageMat = this->leftImageMat;
	imageProcess.rightImageMat = this->rightImageMat;
	imageProcess.processStep = 1;
	imageProcess.featureType = featureType;
	imageProcess.start();
	imageProcess.wait();
	//imageProcess.extractFeature();
	consoleText = "��ȡ��������..";
	//������ʾ����̨����ź�
	signalShowConsole();

	leftImageKeyPoints = imageProcess.leftImageKeyPoints;
	rightImageKeyPoints = imageProcess.rightImageKeyPoints;

	//������ʾ�����ź�
	signalShowFeaturePoint();
	consoleText = "��ʾ����..";
	//������ʾ����̨����ź�
	signalShowConsole();
}

/*****************************************************************************
    *  @brief    : slotMatchFeature
    *  @author   : Zhangle
    *  @date     : 2014/10/22 19:27
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotMatchFeature()
{
	consoleText = "��ʼƥ������..";
	signalShowConsole();
	//ƥ������
	imageProcess.processStep = 2;
	imageProcess.start();
	imageProcess.wait();
	//imageProcess.matchFeature();
	this->matchMat = imageProcess.matchMat;
	consoleText = "ƥ����������..";
	signalShowConsole();
	//������ʾƥ�������ź�
	signalShowMatching();
	consoleText = "��ʾƥ����..";
	signalShowConsole();
}

void MainWindow::slotAlignImage()
{
	//(����)ƴ��ͼƬ
	imageProcess.processStep = 3;
	imageProcess.start();
	imageProcess.wait();
	//imageProcess.alignImage();
	this->alignResult = imageProcess.alignResult;
	//������ʾƴ�ӽ���ź�
	signalShowAlignment();
}
/*****************************************************************************
    *  @brief    : slotShowFeaturePoint
    *  @author   : Zhangle
    *  @date     : 2014/10/23 8:56
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotShowFeaturePoint()
{
	showLeftImageFeature();
	showRightImageFeature();
	ui.tabWidget->setCurrentIndex(1);
}
/*****************************************************************************
    *  @brief    : signalShowMatching
    *  @author   : Zhangle
    *  @date     : 2014/10/23 8:56
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotShowMatching()
{
	showMatching();
	ui.tabWidget->setCurrentIndex(2);
}
/*****************************************************************************
    *  @brief    : slotShowStitching
    *  @author   : Zhangle
    *  @date     : 2014/10/23 9:10
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotShowAlignment()
{
	showAlignment();
	ui.tabWidget->setCurrentIndex(3);
}
/*****************************************************************************
    *  @brief    : slotShowConsole ��ʾ����̨����
    *  @author   : Zhangle
    *  @date     : 2014/10/24 16:37
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::slotShowConsole()
{
	ui.plainTextEdit->appendPlainText(consoleText);
	
}

void MainWindow::slotViewAlignment()
{
	imshow("",graphcut);
}
