#include "stdafx.h"
#include "TSR.h"
#include "TSRDlg.h"
#include "ROIPrepro.h"

CROIPrepro::CROIPrepro(void)
{
}


CROIPrepro::~CROIPrepro(void)
{
}

void CROIPrepro::TiltCorrect(IplImage *img) //Correct tilt of ROI 
{
	IplImage *src = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );
	IplImage *bimg = cvCreateImage( cvGetSize(img), img->depth, 1 ); 
	int i;
	int n_line;
	double t, tm;
    CvMemStorage* stor;  
    CvSeq* lines=NULL;
	CvPoint* Line;
	bool first = true; 

	cvCopy( img, src, NULL );
	ColorPro(src);
	cvCvtColor( src, bimg, CV_BGR2GRAY );
	cvThreshold( bimg, bimg, slider_pos, 255, CV_THRESH_BINARY );
	cvErode( bimg, bimg );  //Erode
    stor = cvCreateMemStorage(0);   
    lines = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor); 
    cvCanny (bimg, bimg, slider_pos, 200, 3); 
    lines = cvHoughLines2 (bimg, stor, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 30, 40, 20);	//Get all lines of the contour
	n_line = lines->total; 
	////Calculate the angles between each line and horizontal direction. Select the minimum value as the tilt angle of the whole ROI and correct it
    for (i = 0; i < n_line; i++)
    {  
        Line = (CvPoint*)cvGetSeqElem(lines, i); 
		if(Line[0].x == Line[1].x)
			continue;
		t = atan(float(Line[0].y-Line[1].y)/float(Line[0].x-Line[1].x));
		if(first == true)
		{
			tm = t;
			first = false;
		}
		if(abs(tm) > abs(t))
			tm = t;
	}
	float m[6];            
    CvMat M = cvMat( 2, 3, CV_32F, m );
	int  nx,ny; 
    float  ca,sa;
    int  xmin,xmax,ymin,ymax,sx,sy;
	ca = (float)cos(tm);
    sa = (float)sin(tm);
    nx = img->width;
    ny = img->height;
    xmin = xmax = ymin = ymax = 0;
    bound(nx-1,0,ca,sa,&xmin,&xmax,&ymin,&ymax);
    bound(0,ny-1,ca,sa,&xmin,&xmax,&ymin,&ymax);
    bound(nx-1,ny-1,ca,sa,&xmin,&xmax,&ymin,&ymax);
    sx = xmax-xmin+1;
    sy = ymax-ymin+1;
    IplImage* newImage;
    newImage=cvCreateImage(cvSize(sx,sy),img->depth,img->nChannels);
	cvFillImage(newImage,0);
    m[0] = ca;
    m[1] = sa;
    m[2] =-(float)xmin; 
    m[3] =-m[1];
    m[4] = m[0];
    m[5] =-(float)ymin;
    cvWarpAffine( img, newImage, &M,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(0) );
	cvResize(newImage, img,CV_INTER_LINEAR);
	cvReleaseImage(&bimg);
}

void CROIPrepro::bound(int x, int y, float ca, float sa, int *xmin, int *xmax, int *ymin, int *ymax)
{   
    int rx,ry;
    // Clockwise rotation
    rx = (int)floor(ca*(float)x+sa*(float)y);
    ry = (int)floor(-sa*(float)x+ca*(float)y);
    if (rx<*xmin) *xmin=rx; if (rx>*xmax) *xmax=rx;
    if (ry<*ymin) *ymin=ry; if (ry>*ymax) *ymax=ry;
}

void CROIPrepro::ColorPro(IplImage *img)  //RGB color segmentation
{
	CvScalar s;
	int i, j;
	double R, G, B, sum;

	for (i = 0; i < img->height; i++)  
	{  
		for (j = 0; j < img->width; j++)  
		{  
			s = cvGet2D( img, i, j ); //Get pixel value of (i,j)
			sum = s.val[0]+s.val[1]+s.val[2];
			B = s.val[0]/sum;
			G = s.val[1]/sum;
			R = s.val[2]/sum;
			s.val[0] = 255; //B
			s.val[1] = 255;  //G
			s.val[2] = 255;  //R
			if((R-G) > 0.08 && (R-B) > 0.08)//Red
				cvSet2D( img, i, j, s ); 
			else if((B-G) > 0.01 && (B-R) > 0.1)//Blue
				cvSet2D( img, i, j, s );
			else if((R-B) > 0.12 && (G-B) > 0.12)//Yellow
				cvSet2D( img, i, j, s );
			else
			{
				s.val[0] = 0; //B
				s.val[1] = 0;  //G
				s.val[2] = 0;  //R
				cvSet2D( img, i, j, s );
			}
		}  
	}  
}

void CROIPrepro::ROISegment(CString ImgAddre, IplImage *img) //Resegmentation of ROI
{
	IplImage *src = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );
	IplImage *bimg = cvCreateImage( cvGetSize(img), img->depth, 1 );  
	IplImage *dst;
	int i;
	int count;
    CvMemStorage* stor;  
    CvSeq* cont;
	CvPoint* PointArray;
	CvPoint p1, p2, p3, p4;
	bool first = true; 

	cvCopy( img, src, NULL );
	ColorPro(src);
	cvCvtColor( src, bimg, CV_BGR2GRAY );
	cvThreshold( bimg, bimg, slider_pos, 255, CV_THRESH_BINARY );
	cvErode( bimg, bimg );  //Erode
	stor = cvCreateMemStorage( 0 );
    cont = cvCreateSeq( CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor ); 
	cvFindContours( bimg, stor, &cont, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );  // Find all contours
	for( ; cont; cont = cont->h_next) 
	{
		count = cont->total;  
        PointArray = (CvPoint*)malloc( count*sizeof(CvPoint) );  
        cvCvtSeqToArray( cont, PointArray, CV_WHOLE_SEQ );  //Get all pixels of each contour
		if(first == true)
		{
			p1=p2=p3=p4=PointArray[0];
			first=false;
		}
		for(i = 0;i < count;i++)
		{
			if(p1.x > PointArray[i].x)
				p1 = PointArray[i];
			if(p2.x < PointArray[i].x)
				p2 = PointArray[i];
			if(p3.y > PointArray[i].y)
				p3 = PointArray[i];
			if(p4.y < PointArray[i].y)
				p4 = PointArray[i];
		}
	}
	CvSize rect = cvSize(p2.x-p1.x, p4.y-p3.y);
	cvSetImageROI(img,cvRect(p1.x, p3.y, p2.x-p1.x, p4.y-p3.y));
	dst = cvCreateImage(rect, img->depth, img->nChannels);
	cvCopy(img, dst); 
	cvSaveImage(ImgAddre, dst);
	cvResetImageROI(dst);
	cvReleaseImage(&dst);
	cvReleaseImage(&src);
	cvReleaseImage(&bimg);
}