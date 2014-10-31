#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	/*初始化*/
	init();
	/*connect*/
	connect(ui.action,SIGNAL(triggered()),this,SLOT(slotOpenDirectory()));
	connect(this,SIGNAL(signalShowImageList()),this,SLOT(slotShowImageList()));
	/*选定影像，并显示*/
	connect(ui.treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),this,SLOT(slotShowSelectedImage(QTreeWidgetItem*, int)));
	/*特征提取，并显示结果*/
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(slotExtractFeature()));//特征提取
	connect(this,SIGNAL(signalShowFeaturePoint()),this,SLOT(slotShowFeaturePoint()));
	/*特征匹配，并显示结果*/
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(slotMatchFeature()));//特征匹配
	connect(this,SIGNAL(signalShowMatching()),this,SLOT(slotShowMatching()));
	/*影像对齐，并显示结果*/
	connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(slotAlignImage()));//影像对齐
	connect(this,SIGNAL(signalShowAlignment()),this,SLOT(slotShowAlignment()));

	connect(this,SIGNAL(signalShowConsole()),this,SLOT(slotShowConsole()));

	connect(ui.action_2,SIGNAL(triggered()),this,SLOT(slotViewAlignment()));
}

MainWindow::~MainWindow()
{

}

/*****************************************************************************
    *  @brief    : init 初始化
    *  @author   : Zhangle
    *  @date     : 2014/10/24 19:50
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::init()
{
	this->showMaximized();
	ui.treeWidget->setHeaderLabel(tr("影像目录"));
	ui.tabWidget->setCurrentIndex(0);//指定tabwedget中的当前tab

	leftImagePath = "";							//左边影像路径
	rightImagePath = "";						//右边影像路径
	leftImageMat = NULL;						//左边影像，矩阵存储
	rightImageMat = NULL;						//右边影像，矩阵存储
	featureType = "";							//特征点类型
	leftImageDesc = NULL;						//左边影像的特征点描述子
	rightImageDesc = NULL;						//右边影像的特征点描述子
	globalHomo = NULL;							//Homography
	matchMat = NULL;							//匹配结果
	xoffset = 0;								//影像映射后x方向上需要的位移
	yoffset = 0;								//影像映射后y方向上需要的位移
	alfa = 0;
	lambda = 0;
	folderPath = "";
}

/*****************************************************************************
    *  @brief    : showImageList 获取路径path下所有影像，并以一个QList返回
    *  @author   : Zhangle
    *  @date     : 2014/10/21 20:48
    *  @version  : ver 1.0
    *  @inparam  : path 绝对路径
    *  @outparam : QList 
*****************************************************************************/
void MainWindow::showImageList(QString path)
{
	//QList<QTreeWidgetItem *> treeWidgetItemList;
	//获取qPath路径下的文件
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
	//循环
	int i = 0;
	int filecount = 0;
	QTreeWidgetItem *imageItem = new QTreeWidgetItem(ui.treeWidget,QStringList(QString("影像列表")));
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
 				QTreeWidgetItem *imageItem_1 = new QTreeWidgetItem(imageItem,QStringList(currentFileName)); //子节点
				imageItem_1->setIcon(0,QIcon(folderPath+currentFileName));
				imageItem->addChild(imageItem_1); //添加子节点
			} 
			i++;
		}
	} while (i<list.size());
	ui.treeWidget->expandAll(); //结点全部展开
}

/*****************************************************************************
    *  @brief    : showLeftImage 显示影像
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
    *  @brief    : showRightImage 显示右侧图像
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
    *  @brief    : showLeftImageFeature 显示特征点
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:31
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showLeftImageFeature()
{
	Mat warpImageMat = leftImageMat.clone();
	/*把特征点和原影像显示出来*/
	drawKeypoints(warpImageMat,leftImageKeyPoints,warpImageMat,Scalar(0,0,255));//把特征点画在影像上
	QImage qimg = util.Mat2QImage(warpImageMat); //将Mat转换为QImage

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
    *  @brief    : showRightImageFeature 显示特征点
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:48
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showRightImageFeature()
{
	Mat referImageMat = rightImageMat.clone();
	/*把特征点和原影像显示出来*/
	drawKeypoints(referImageMat,rightImageKeyPoints,referImageMat,Scalar(0,255,0));//把特征点画在影像上
	QImage qimg = util.Mat2QImage(referImageMat); //将Mat转换为QImage

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
    *  @brief    : showMatching 显示匹配结果
    *  @author   : Zhangle
    *  @date     : 2014/10/22 15:48
    *  @version  : ver 1.0
    *  @inparam  : 
    *  @outparam :  
*****************************************************************************/
void MainWindow::showMatching()
{
	Mat matchImage = matchMat.clone();//克隆一个匹配结果matchMat副本，用于后续操作
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
    *  @brief    : showAlignment 显示最后的拼接结果
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
						/*								槽函数实现							    */
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
	//弹出文件夹选择窗口，选择后，确定
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		"/home",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	folderPath = dir+"\\";
	ui.treeWidget->setHeaderLabel(folderPath);
	consoleText = "打开 "+folderPath+" 目录";
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
	consoleText = "选择"+qText;
	if (1 == col % 2)	//左侧
	{
		leftImageMat = imread(util.q2s(qPath));	//读取左边影像，存为mat格式
		showLeftImage();
	}
	else				//右侧
	{
		rightImageMat = imread(util.q2s(qPath));	//读取右边影像，存为mat格式
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
	consoleText = "开始提取特征..";
	//发出显示控制台输出信号
	signalShowConsole();
	int col = ui.comboBox->currentIndex();
	featureType = ui.comboBox->currentText();
	qDebug() << "featureType = " << featureType << endl;
	//提取特征
	imageProcess.leftImageMat = this->leftImageMat;
	imageProcess.rightImageMat = this->rightImageMat;
	imageProcess.processStep = 1;
	imageProcess.featureType = featureType;
	imageProcess.start();
	imageProcess.wait();
	//imageProcess.extractFeature();
	consoleText = "提取特征结束..";
	//发出显示控制台输出信号
	signalShowConsole();

	leftImageKeyPoints = imageProcess.leftImageKeyPoints;
	rightImageKeyPoints = imageProcess.rightImageKeyPoints;

	//发出显示特征信号
	signalShowFeaturePoint();
	consoleText = "显示特征..";
	//发出显示控制台输出信号
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
	consoleText = "开始匹配特征..";
	signalShowConsole();
	//匹配特征
	imageProcess.processStep = 2;
	imageProcess.start();
	imageProcess.wait();
	//imageProcess.matchFeature();
	this->matchMat = imageProcess.matchMat;
	consoleText = "匹配特征结束..";
	signalShowConsole();
	//发出显示匹配结果的信号
	signalShowMatching();
	consoleText = "显示匹配结果..";
	signalShowConsole();
}

void MainWindow::slotAlignImage()
{
	//(对齐)拼接图片
	imageProcess.processStep = 3;
	imageProcess.start();
	imageProcess.wait();
	//imageProcess.alignImage();
	this->alignResult = imageProcess.alignResult;
	//发出显示拼接结果信号
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
    *  @brief    : slotShowConsole 显示控制台内容
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
