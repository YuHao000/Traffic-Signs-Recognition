#include "stdafx.h"
#include "TSR.h"
#include "TSRDlg.h"
#include "fstream"  
#include "SVM.h"
using namespace std;  
using namespace cv;


CSVM::CSVM(void)
{
}


CSVM::~CSVM(void)
{
}

CString CSVM::CirSVM(IplImage *img) //Circle Signs have blue and red colors in China
{
	int id = 100;
	CvScalar s;
	double R,G,B,sum;
	bool TrainFinish = false;
	CString Circle="";
	for (int i = 0;i < img->height;i++)  //Select SVM based on the color of the sign
	{  
		for (int j = 0;j < img->width;j++)  
		{  
			s = cvGet2D( img, i, j ); //Get pixel value of (i, j)
			sum = s.val[0]+s.val[1]+s.val[2]; //B,G,R
			B = s.val[0]/sum;
			G = s.val[1]/sum;
			R = s.val[2]/sum;
			if((B-G) > 0.01 && (B-R) > 0.1)//Blue
			{
				ImgPath="D:/新建文件夹/ellipse/blue/hb.txt";
				SVMPath="..\\HOG_SVM_ELLIPSE_BLUE.xml";
				if(BlueTrain == false) //If the blue SVM has not been trained yet
				{
					SVMLearn();
					BlueTrain=true;
				}
				id=SVMDetect(img);
				TrainFinish=true;
				break;
			}
			else if((R-G) > 0.08 && (R-B) > 0.08)//Red
			{
				ImgPath="D:/新建文件夹/ellipse/red/hb.txt";
				SVMPath="..\\HOG_SVM_ELLIPSE_RED.xml";
				if(RedTrain==false)
				{
					SVMLearn();
					BlueTrain=true;
				}
				id=SVMDetect(img);
				TrainFinish=true;
				break;
			}
		}
		if(TrainFinish == true)
			break;;
	}
	//Chinese traffic signs
	if(id == 0)
		Circle = "向左转弯";
	if(id == 1)
		Circle = "向右转弯";
	if(id == 2)
		Circle = "环岛行驶";
	if(id == 3)
		Circle = "靠右侧道路行驶";
	if(id == 4)
		Circle = "靠左侧道路行驶";
	if(id == 5)
		Circle = "鸣喇叭";
	if(id == 6)
		Circle = "直行和向右转弯";
	if(id == 7)
		Circle = "禁止直行";
	if(id == 8)
		Circle = "禁止向左向右转弯";
	if(id == 9)
		Circle = "禁止机动车通行";
	if(id == 10)
		Circle = "禁止向左转弯";
	if(id == 11)
		Circle = "禁止驶入";
	if(id == 100)
		Circle = "无效区域";
	return Circle;
}

CString CSVM::TriSVM(IplImage *img) //Triangle signs have yellow and red colors in China
{
	int id = 100;
	CvScalar s;
	double R,G,B,sum;
	CString Triangle = "";
	bool TrainFinish=false;
	for (int i = 0;i < img->height;i++)  //Select SVM based on the color of the sign
	{  
		for (int j = 0;j < img->width;j++)  
		{  
			s = cvGet2D( img, i, j ); //Get pixel value of (i,j)
			sum = s.val[0]+s.val[1]+s.val[2]; //B,G,R
			B = s.val[0]/sum;
			G = s.val[1]/sum;
			R = s.val[2]/sum;
			if((R-B) > 0.12 && (G-B) > 0.12)//Yellow
			{
				ImgPath="D:/新建文件夹/triangle/yell/hb.txt";
				SVMPath="..\\HOG_SVM_TRIANGLE_YELL.xml";
				if(YellTrain==false)
				{
					SVMLearn();
					YellTrain=true;
				}
				id=SVMDetect(img);
				TrainFinish=true;
				break;
			}
			else if((R-G) > 0.08 && (R-B) > 0.08)//Red
			{
				id = 0;   //Red triangle sign only has one type
				break;
			}
		}
		if(TrainFinish == true)
			break;;
	}
	//Chinese traffic signs
	if(id == 0)
		Triangle = "减速慢行";
	if(id == 1)
		Triangle = "注意行人";
	if(id == 2)
		Triangle = "路面不平";
	if(id == 100)
		Triangle = "无效区域";
	return Triangle;
}

CString CSVM::RecSVM(IplImage *img) //Rectangle signs only have blue color
{
	int id = 100;
	CvScalar s;
	double R,G,B,sum;
	CString Rectangle;
	bool TrainFinish=false;
	for (int i = 0;i < img->height;i++)  //Select SVM based on the color
	{  
		for (int j = 0;j < img->width;j++)  
		{  
			s = cvGet2D( img, i, j ); //Get pixel value of (i,j)
			sum = s.val[0]+s.val[1]+s.val[2]; //B,G,R
			B = s.val[0]/sum;
			G = s.val[1]/sum;
			R = s.val[2]/sum;
			if((B-G) > 0.01 && (B-R) > 0.1)//Blue
			{
				ImgPath="D:/新建文件夹/rectangle/blue/hb.txt";
				SVMPath="..\\HOG_SVM_RECTANGLE_BLUE.xml";
				if(BlueTrain == false)
				{
					SVMLearn();
					BlueTrain=true;
				}
				id=SVMDetect(img);
				TrainFinish=true;
				break;
			}
		}
		if(TrainFinish == true)
			break;;
	}
	//Chinese traffic signs
	if(id == 0)
		Rectangle = "允许掉头";
	if(id == 1)
		Rectangle = "停车场";
	if(id == 2)
		Rectangle = "人行横道";
	if(id == 100)
		Rectangle = "无效区域";
	return Rectangle;
}

void CSVM::SVMLearn()
{
	vector<string> img_path;//Input filename variable     
    vector<int> img_catg;      
    int nLine = 0;      
    string buf;      
    ifstream svm_data( ImgPath );///The path of folder that stores sample photos      
    unsigned long n;       
    while( svm_data )
    {      
        if( getline( svm_data, buf ) )      
        {      
            nLine ++;      
            if( nLine % 2 == 0 )//Odd-numbered line is path, even-numbered line is label   
            {      
                 img_catg.push_back( atoi( buf.c_str() ) );
            }      
            else      
            {      
                img_path.push_back( buf );      
            }      
        }      
    }      
    svm_data.close();
    CvMat *data_mat, *res_mat;      
    int nImgNum = nLine / 2; //Number of samples      
    data_mat = cvCreateMat( nImgNum, 324, CV_32FC1 );     
    cvSetZero( data_mat );      
    res_mat = cvCreateMat( nImgNum, 1, CV_32FC1 );       
    cvSetZero( res_mat );      
    
	IplImage* src;      
    IplImage* trainImg=cvCreateImage(cvSize(28,28),8,3);
    //Process HOG features    
    for( string::size_type i = 0; i != img_path.size(); i++ )      
    {      
            src=cvLoadImage(img_path[i].c_str(),1);      
            if( src == NULL )        
                return;           
            cvResize(src,trainImg);       
            HOGDescriptor *hog=new HOGDescriptor(cvSize(28,28),cvSize(14,14),cvSize(7,7),cvSize(7,7),9);        
            vector<float>descriptors(1764);//Store results       
            hog->compute(trainImg, descriptors,Size(1,1), Size(0,0)); //Calculate Hog features       
            n=0;      
            for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)      
            {      
                cvmSet(data_mat,i,n,*iter);//Store Hog features   
                n++;      
            }         
            cvmSet( res_mat, i, 0, img_catg[i] ); 
    }                     
    CvSVM svm = CvSVM();        
    CvSVMParams param;    
    CvTermCriteria criteria;        
    criteria = cvTermCriteria( CV_TERMCRIT_EPS, 1000, FLT_EPSILON );        
    param = CvSVMParams( CvSVM::C_SVC, CvSVM::RBF, 10.0, 0.09, 1.0, 10.0, 0.5, 1.0, NULL, criteria );                
    svm.train( data_mat, res_mat, NULL, NULL, param );//Train       
    //Store trained SVM        
    svm.save( SVMPath );   
	cvReleaseMat( &data_mat );      
    cvReleaseMat( &res_mat ); 
	cvReleaseImage(&trainImg);
	cvReleaseImage(&src);
}

int CSVM::SVMDetect(IplImage *img) //Sign detection using SVM
{
	CvSVM svm = CvSVM(); 
	svm.load( SVMPath );
	IplImage* trainImg=cvCreateImage(cvSize(28,28),8,3);
	cvZero(trainImg);
	cvResize(img,trainImg); 
	HOGDescriptor *hog=new HOGDescriptor(cvSize(28,28),cvSize(14,14),cvSize(7,7),cvSize(7,7),9);
	vector<float>descriptors(1764);
	hog->compute(trainImg, descriptors,Size(1,1), Size(0,0)); //Calculate Hog features  
	CvMat* SVMtrainMat=cvCreateMat(1,descriptors.size(),CV_32FC1); 
	int n=0;
	for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
	{
		cvmSet(SVMtrainMat,0,n,*iter); 
		n++;
	}
	int ret = svm.predict(SVMtrainMat); //Detect
	cvReleaseImage(&trainImg);  
	cvReleaseImage(&img);
	return ret;
}
