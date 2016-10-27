#include "stdafx.h"
#include "TSR.h"
#include "TSRDlg.h"
#include "ROIDetect.h"
using namespace std;  
using namespace cv;  


CROIDetect::CROIDetect(void)
{
}


CROIDetect::~CROIDetect(void)
{
}

void CROIDetect::RGBMark(IplImage *img)  //RGB color segmentation
{
	CvScalar s;
	int i, j;
	double R, G, B, sum;

	for (i = 0; i < img->height; i++)  
	{  
		for (j = 0; j < img->width; j++)  
		{  
			s = cvGet2D( img, i, j ); //Get the pixel of point(i,j)
			sum = s.val[0]+s.val[1]+s.val[2];
			B = s.val[0]/sum;
			G = s.val[1]/sum;
			R = s.val[2]/sum;
			s.val[0] = 255; //B
			s.val[1] = 255;  //G
			s.val[2] = 255;  //R
			if((R-G) > 0.08 && (R-B) > 0.08)//Red
				cvSet2D( img, i, j, s );  //Set the pixel of (i,j)
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

void CROIDetect::SizeDeJug(IplImage *img)  //Filter acceptable ROI based on the size of their contours
{
	CvMemStorage* stor;  
    CvSeq* cont; 
	CvRect rect;
	CvBox2D box;
	IplImage *rsp = cvCreateImage(cvGetSize(img),img->depth,img->nChannels); //Copy original image
	IplImage *bimg = cvCreateImage(cvGetSize(img),img->depth,1);//Binary processing

	cvCopy( img, rsp, NULL );
	cvCvtColor( img, bimg, CV_BGR2GRAY);
	cvThreshold( bimg, bimg, slider_pos, 255, CV_THRESH_BINARY );
	cvDilate( bimg, bimg );
	cvErode( bimg, bimg );
	stor = cvCreateMemStorage( 0 );
    cont = cvCreateSeq( CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor ); 
	cvFindContours( bimg, stor, &cont, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );  //Get all contours in the image
	cvZero( img );
	for( ; cont; cont = cont->h_next) 
	{
		rect = cvBoundingRect( cont,1 ); 
		box = cvMinAreaRect2( cont, NULL );  
		if(rect.height > 220 || rect.width > 220) //If the contour is overlarge, unacceptable   
			continue;
		if(rect.height < 30 || rect.width < 30) //If the contour is oversmall, unacceptable
			continue;
		if(box.size.height > box.size.width*2 || box.size.width > box.size.height*2)
			continue;
		cvSetImageROI( rsp, rect );
		cvSetImageROI( img, rect );
		cvCopy( rsp, img );
		cvResetImageROI( img );
		cvResetImageROI( rsp );
	}
	cvReleaseImage( &bimg );
	cvReleaseImage( &rsp );
}

void CROIDetect::CircleDec(IplImage *bimg, IplImage *src) //Define if ellipse areas exist
{
	IplImage* srcD = cvCreateImage( cvGetSize(src), src->depth, src->nChannels ); //Copy the original image
	IplImage* cont_img = cvCreateImage( cvGetSize(bimg), bimg->depth, bimg->nChannels );  //cont_img is the image after finding contours processing
	IplImage* pDest; //ROI
	ROI EllipReg[10];
	CvMemStorage* stor;  
    CvSeq* cont;  
    CvPoint* PointArray;
	CvPoint p1, p2;  //p1 and p2 are two points of each contour that are the farthest away from each other
	CvPoint cenTmp;
	int n; //The number of accumulator
	int i, j, a, maxN;
	int count;  //The total amount of pixels of each contour
	int tmp[4];
	double dd, ff, rTmp;
	bool OverLap;  //Define if the new ROI is overlapped with other found ROIs 
	string ImageBase ="..\\";
	string ImageAddres;

	cvCopy( src, srcD, NULL );
	cvCopy( bimg, cont_img, NULL );
	CirNum=0;
    stor = cvCreateMemStorage(0);
    cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor); 
    cvFindContours( cont_img, stor, &cont, sizeof(CvContour),CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    for( ; cont; cont = cont->h_next)  //If there are acceptable ellipses existing in the contours
	{ 
        count = cont->total; 
        PointArray = (CvPoint*)malloc( count*sizeof(CvPoint) );  
        cvCvtSeqToArray( cont, PointArray, CV_WHOLE_SEQ );  //cont is an array that contains all pixels of the contour
		tmp[0] = 0, tmp[1] = 0, tmp[2] = 0, tmp[3] = 0;
		Circal bArray[500];  //Accumulator
		n=0;
        for(i=0; i<count; i++)  //Find two points that are the farthest away from each other
        {  
			if(PointArray[tmp[0]].x>PointArray[i].x)  //tmp1.x is the smallest
				tmp[0]=i;
			if(PointArray[tmp[1]].y>PointArray[i].y)  //tmp2.y is the smallest
				tmp[1]=i;
			if(PointArray[tmp[2]].x<PointArray[i].x)   //tmp3.x is the largest
				tmp[2]=i;
			if(PointArray[tmp[3]].y<PointArray[i].y)   //tmp4.y is the largest
				tmp[3]=i;
		}
		dd = pow(PointArray[tmp[0]].x-PointArray[tmp[3]].x,2)+pow(PointArray[tmp[0]].y-PointArray[tmp[3]].y,2);
		p1 = PointArray[tmp[0]];
		p2 = PointArray[tmp[3]];
		for(i=0; i<4; i++)
		{
			for(j=3; j>i; j--)
			{
				if((pow(PointArray[tmp[i]].x-PointArray[tmp[j]].x,2)+pow(PointArray[tmp[i]].y-PointArray[tmp[j]].y,2)) > dd)
				{
					dd = pow(PointArray[tmp[i]].x-PointArray[tmp[j]].x,2)+pow(PointArray[tmp[i]].y-PointArray[tmp[j]].y,2);
					p1 = PointArray[tmp[i]];
					p2 = PointArray[tmp[j]];
				}
			}
		}
		if(dd < 900)
			continue;
		for(i=0; i<count; i++)
		{
			if((PointArray[i].x==p1.x && PointArray[i].y==p1.y) || (PointArray[i].x==p2.x && PointArray[i].y==p2.y)) //The third point cannot be the same as p1 or p2
				continue;
			dd = pow(p2.x,2)+pow(p2.y,2)-pow(p1.x,2)-pow(p1.y,2);
			ff = pow(PointArray[i].x,2)+pow(PointArray[i].y,2)-pow(p2.x,2)-pow(p2.y,2);
			if((p2.y-p1.y)*(PointArray[i].x-p2.x) == (p2.x-p1.x)*(PointArray[i].y-p2.y))
				continue;
			//cenTmp store the center point while p1, p2 and the third point are on the same circle
			cenTmp.x = (int)((2*(p2.y-p1.y)*ff-2*(PointArray[i].y-p2.y)*dd)/(4*(p2.y-p1.y)*(PointArray[i].x-p2.x)-4*(p2.x-p1.x)*(PointArray[i].y-p2.y))); 
			cenTmp.y = (int)((2*(PointArray[i].x-p2.x)*dd-2*(p2.x-p1.x)*ff)/(4*(p2.y-p1.y)*(PointArray[i].x-p2.x)-4*(p2.x-p1.x)*(PointArray[i].y-p2.y)));
			rTmp = pow((cenTmp.x-p1.x)*(cenTmp.x-p1.x)+(cenTmp.y-p1.y)*(cenTmp.y-p1.y),0.5);
			if(rTmp < 15) //If the radius is oversmall, this circle is unacceptable
				continue;
			for(j=0; j<n; j++) //Calculate the number of other points on this contour that are also on this circle
			{
				if(bArray[j].r <= rTmp*1.1 && bArray[j].r >= rTmp*0.9) //When the point is close to the circle, define it is on the circle
				{
					++bArray[j].num;
					break;
				}
			}
			if(j==n)
			{
				bArray[n].r=rTmp;
				bArray[n].center=cenTmp;
				bArray[n].num=1;
				++n;
			}
        }
		maxN=0;
		for(i=0; i<n; i++)
		{
			if(bArray[maxN].num < bArray[i].num)
				maxN = i;
		}
		if(bArray[maxN].num < count*0.7) //If the number of points on this circle is less than the 70% of total amount of points on this contour, this circle is not the acceptable shape of this contour
			continue;
		OverLap = false;
		EllipReg[CirNum].size = cvSize((int)(bArray[maxN].r*2.2),(int)(bArray[maxN].r*2.2));//Get the size of ROI
		EllipReg[CirNum].strPoint.x = (int)(bArray[maxN].center.x-bArray[maxN].r*1.1);
		if(EllipReg[CirNum].strPoint.x < 0)
			EllipReg[CirNum].strPoint.x = 0;
		EllipReg[CirNum].strPoint.y = (int)(bArray[maxN].center.y-bArray[maxN].r*1.1);
		if(EllipReg[CirNum].strPoint.y < 0)
			EllipReg[CirNum].strPoint.y = 0;
		for(a = 0; a < CirNum; a++) //Define if the ROI is overlapped with preivous ones
		{
			dd = pow(EllipReg[CirNum].strPoint.x-EllipReg[a].strPoint.x,2)+pow(EllipReg[CirNum].strPoint.y-EllipReg[a].strPoint.y,2);
			if(dd < (EllipReg[a].size.height*EllipReg[a].size.height*2))
			{
				OverLap = true;
				break;
			}
		}
		if(OverLap == true)
			break;
		++CirNum;
	}
	//Store the ROI
	for(int i = 0; i < min(CirNum,4); i++)
	{
		ImageAddres = ImageBase + "ellip" + (char)(i + 48) +".jpg";
		cvSetImageROI(srcD,cvRect(EllipReg[i].strPoint.x,EllipReg[i].strPoint.y,EllipReg[i].size.width,EllipReg[i].size.height));
		pDest = cvCreateImage(EllipReg[i].size,src->depth,src->nChannels);
		cvCopy(srcD,pDest);
		cvSaveImage(ImageAddres.data(), pDest);
		cvResetImageROI(pDest);
		cvReleaseImage(&pDest);
	}  
	cvReleaseImage(&cont_img);
	cvReleaseImage(&srcD);
}

void CROIDetect::TriangleDec(IplImage *bimg, IplImage *src) //Define if triangle areas exist
{
	IplImage* srcD = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
    IplImage* cannyImg = cvCreateImage(cvGetSize(srcD),IPL_DEPTH_8U,1);  //Canny contour image
	IplImage* pDest;
    CvMemStorage* stor;  
    CvSeq* lines=NULL;
	ROI inte_reg[10];
	CvPoint* line1, * line2, * line3;
	CvPoint p1,p2;  //Unconnected two endpoints
	int n_line;    //The total amount of lines on the contour
	int i, j, k, a;  
	int yMin, yMax, xMin, xMax;
	double ff1, ff2, ff3, ff4, ff5, ff6, ff7, t, c, dd;
	bool OverLap;
	string ImageBase ="..\\";
	string ImageAddres;

    TriNum = 0;
	cvCopy(src,srcD,NULL);
	stor = cvCreateMemStorage(0);  
    lines = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor); 
    cvCanny (bimg, cannyImg, slider_pos, 200, 3); 
    lines = cvHoughLines2 (cannyImg, stor, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 30, 40, 20);	
	n_line = lines->total; 
	if(n_line < 3) //If the number of lines is less than 3, then the contour cannot be a triangle
		n_line=0;
    for (i = 0; i < n_line; i++)  
    {  
        line1 = (CvPoint*)cvGetSeqElem(lines, i); //line1 is one possible line of a triangle
		for(j = i+1; j < n_line; j++)
		{
			line2 = (CvPoint*)cvGetSeqElem(lines, j); //line2 is the second possible line of the triangle
			//Calculate the  Euclidean distance of endpoints of the two lines, if it is less than 10, then line2 is unacceptable
			ff1 = pow(line1[0].x-line2[0].x,2)+pow(line1[0].y-line2[0].y,2);
			ff2 = pow(line1[0].x-line2[1].x,2)+pow(line1[0].y-line2[1].y,2);
			ff3 = pow(line1[1].x-line2[0].x,2)+pow(line1[1].y-line2[0].y,2);
			ff4 = pow(line1[1].x-line2[1].x,2)+pow(line1[1].y-line2[1].y,2);
			if( max( min(ff1,ff2), min(ff3,ff4) ) < 100 )
				continue;
			if(ff1 < 100)
			{
				p1 = line1[1];
				p2 = line2[1];
			}
			else if(ff2 < 100)
			{
				p1 = line1[1];
				p2 = line2[0];
			}
			else if(ff3 < 100)
			{
				p1 = line1[0];
				p2 = line2[1];
			}
			else if(ff4 < 100)
			{
				p1 = line1[0];
				p2 = line2[0];
			}
			else
				continue;
			ff1 = pow(line1[0].x-line1[1].x,2)+pow(line1[0].y-line1[1].y,2); //The length of line1
			ff2 = pow(line2[0].x-line2[1].x,2)+pow(line2[0].y-line2[1].y,2); //The lenght of line2
			t = ((line1[0].x-line1[1].x)*(line2[0].x-line2[1].x)+(line1[0].y-line1[1].y)*(line2[0].y-line2[1].y))/pow(ff1*ff2,0.5);//The value of cos(angle)
			t = abs(t);
			if(t < 0.342 || t > 0.643)  //The range of angle is 50~70, if out of range, then unacceptable
				continue;
			for(k = 0; k < n_line; k++)
			{
				line3 = (CvPoint*)cvGetSeqElem(lines, k); //line3 is the third line of the triangle
				//Calculate the Euclidean distance from endpoints of line3 the other two unconnected endpoints
				ff1 = pow(p1.x-line3[0].x,2)+pow(p1.y-line3[0].y,2);
				ff2 = pow(p1.x-line3[1].x,2)+pow(p1.y-line3[1].y,2);
				ff3 = pow(p2.x-line3[0].x,2)+pow(p2.y-line3[0].y,2);
				ff4 = pow(p2.x-line3[1].x,2)+pow(p2.y-line3[1].y,2);
				ff5 = pow(line1[0].x-line1[1].x,2)+pow(line1[0].y-line1[1].y,2); //Length of line1
				ff6 = pow(line2[0].x-line2[1].x,2)+pow(line2[0].y-line2[1].y,2);//Length of line2
				ff7 = pow(line3[0].x-line3[1].x,2)+pow(line3[0].y-line3[1].y,2); //Length of line3
				dd = min(min(ff1,ff2), min(ff3,ff4));  //The minimum length from line3 to the other two lines
				if(dd > min(min(ff5,ff6), ff7)) //They cannot be too close
					continue;
				t = ((line1[0].x-line1[1].x)*(line3[0].x-line3[1].x)+(line1[0].y-line1[1].y)*(line3[0].y-line3[1].y))/pow(ff5*ff7,0.5);//cos(angle)
				c = ((line2[0].x-line2[1].x)*(line3[0].x-line3[1].x)+(line2[0].y-line2[1].y)*(line3[0].y-line3[1].y))/pow(ff6*ff7,0.5);//cos(angle)
				t = abs(t);
				c = abs(c);
				if(t < 0.342 || t > 0.643 || c < 0.342 || c > 0.643)  //the acceptable range is 50~70
					continue;
				//Define the ROI
				OverLap = false;
				yMin = min(min(line1[0].y, line2[0].y), min(line2[0].y, line3[0].y));
				yMax = max(max(line1[1].y, line2[1].y), max(line2[1].y, line3[1].y));
				xMin = min(min(line1[0].x, line2[0].x), min(line2[0].x, line3[0].x));
				xMax = max(max(line1[1].x, line2[1].x), max(line2[1].x, line3[1].x));
				dd = max(xMax-xMin,yMax-yMin);         
				inte_reg[TriNum].size = cvSize((int)(dd*1.3), (int)(dd*1.2));
				inte_reg[TriNum].strPoint.x = (int)((xMax+xMin-dd*1.2)/2);
				if(inte_reg[TriNum].strPoint.x < 0)
					inte_reg[TriNum].strPoint.x = 0;
				inte_reg[TriNum].strPoint.y = (int)((yMax+yMin-dd*1.2)/2);
				if(inte_reg[TriNum].strPoint.y < 0)
					inte_reg[TriNum].strPoint.y = 0;
				for(a = 0; a < TriNum; a++) //Define if the ROI is overlapped
				{
					dd = pow(inte_reg[TriNum].strPoint.x-inte_reg[a].strPoint.x,2)+pow(inte_reg[TriNum].strPoint.y-inte_reg[a].strPoint.y,2);
					if(dd < inte_reg[a].size.height)
					{
						OverLap = true;
						break;
					}
				}
				if(OverLap == true)
					break;
				++TriNum;
			}
		}
	}
	//Store the image
	for(i = 0; i < min(TriNum,4); i++)
	{
		ImageAddres = ImageBase + "tri" + (char)(i + 48) +".jpg";
		cvSetImageROI(srcD,cvRect(inte_reg[i].strPoint.x, inte_reg[i].strPoint.y, inte_reg[i].size.width, inte_reg[i].size.height));
		pDest = cvCreateImage(inte_reg[i].size, src->depth, src->nChannels);
		cvCopy(srcD, pDest);
		cvSaveImage(ImageAddres.data(), pDest);
		cvResetImageROI(pDest);
		cvReleaseImage(&pDest);
	}
	cvReleaseImage(&cannyImg);
	cvReleaseImage(&srcD);
}

void CROIDetect::RectangleDec(IplImage *bimg, IplImage *src) //Define if rectangle areas exist
{
	IplImage* srcD = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
    IplImage* cannyImg = cvCreateImage(cvGetSize(srcD),IPL_DEPTH_8U,1);  //canny contour image
	IplImage* pDest;
    CvMemStorage* stor;  
    CvSeq* lines=NULL;  
	ROI inte_reg[10];
	CvPoint* line1, * line2, * line3, * line4;
	CvPoint p1, p2; //The two unconnected endpoints after two lines are confirmed
	CvPoint p3, p4;  //The two unconnected endpoints after three lines are confirmed
	bool OverLap;
	int yMin, yMax, xMin, xMax;
	int n_line;
	int i, j, k, l, a;
	int n_co;  //n_co is the number of the line that is adjacent with line3
	double ff1, ff2, ff3, ff4, ff5, ff6, ff7, ff8, t, dd;
	string ImageBase ="..\\";
	string ImageAddres;

	RecNum = 0;
	cvCopy(src,srcD,NULL);
    stor = cvCreateMemStorage(0);
    lines = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor); 
    cvCanny (bimg, cannyImg, slider_pos, 200, 3); 
    lines = cvHoughLines2 (cannyImg, stor, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 30, 40, 20);
	n_line = lines->total;
	if(n_line < 4) //If the total amount of existing lines is less than 4, then the contour cannot be a rectangle
		n_line=0;
    for (i = 0; i < n_line; i++)  
    {  
        line1 = (CvPoint*)cvGetSeqElem(lines, i); 
		for(j = i+1; j < n_line; j++)
		{
			line2 = (CvPoint*)cvGetSeqElem(lines, j);
			ff1 = pow(line1[0].x-line2[0].x,2)+pow(line1[0].y-line2[0].y,2);
			ff2 = pow(line1[0].x-line2[1].x,2)+pow(line1[0].y-line2[1].y,2);
			ff3 = pow(line1[1].x-line2[0].x,2)+pow(line1[1].y-line2[0].y,2);
			ff4 = pow(line1[1].x-line2[1].x,2)+pow(line1[1].y-line2[1].y,2);
			if(ff1 < 100)
			{
				p1 = line1[1];
				p2 = line2[1];
			}
			else if(ff2 < 100)
			{
				p1 = line1[1];
				p2 = line2[0];
			}
			else if(ff3 < 100)
			{
				p1 = line1[0];
				p2 = line2[1];
			}
			else if(ff2 < 100)
			{
				p1 = line1[0];
				p2 = line2[0];
			}
			else  
				continue;
			ff1 = pow(line1[0].x-line1[1].x,2)+pow(line1[0].y-line1[1].y,2); //Length of line1
			ff2 = pow(line2[0].x-line2[1].x,2)+pow(line2[0].y-line2[1].y,2);//Length of line2
			t = ((line1[0].x-line1[1].x)*(line2[0].x-line2[1].x)+(line1[0].y-line1[1].y)*(line2[0].y-line2[1].y))/pow(ff1*ff2,0.5);//cos(angle)
			t = abs(t);
			if(t > 0.087)  //The acceptable range of angle is 85~95
				continue;
			for(k = 0; k < n_line; k++)
			{
				line3 = (CvPoint*)cvGetSeqElem(lines, k);
				//Calculate the Euclidean distance
				ff1 = pow(p1.x-line3[0].x,2)+pow(p1.y-line3[0].y,2);
				ff2 = pow(p1.x-line3[1].x,2)+pow(p1.y-line3[1].y,2);
				ff3 = pow(p2.x-line3[0].x,2)+pow(p2.y-line3[0].y,2);
				ff4 = pow(p2.x-line3[1].x,2)+pow(p2.y-line3[1].y,2);
				ff5 = pow(line1[0].x-line1[1].x,2)+pow(line1[0].y-line1[1].y,2);
				ff6 = pow(line2[0].x-line2[1].x,2)+pow(line2[0].y-line2[1].y,2);
				ff7 = pow(line3[0].x-line3[1].x,2)+pow(line3[0].y-line3[1].y,2);
				dd = min(min(ff1,ff2), min(ff3,ff4));//×îÐ¡Å·ÊÏ¾àÀë
				if(dd > min(min(ff5,ff6), ff7))
					continue;
				if((ff1 == dd || ff2 == dd) && min(ff3,ff4) > ff5*0.7) //In such case, line3 is adjacent with line1, while cannot be close with line2 at the same time
				{
					n_co = 1;
					p3 = p2;
					if(ff1 == dd)
						p4 = line3[1];
					else
						p4 = line3[0];
					t = ((line1[0].x-line1[1].x)*(line3[0].x-line3[1].x)+(line1[0].y-line1[1].y)*(line3[0].y-line3[1].y))/pow(ff5*ff7,0.5);//cos(angle)
				}
				else if((ff3 == dd || ff4 == dd) && min(ff1,ff2) > ff6*0.7)
				{
					n_co = 2;
					p3 = p1;
					if(ff3 == dd)
						p4 = line3[1];
					else
						p4 = line3[0];
					t = ((line2[0].x-line2[1].x)*(line3[0].x-line3[1].x)+(line2[0].y-line2[1].y)*(line3[0].y-line3[1].y))/pow(ff6*ff7,0.5);
				}
				else
					continue;
				t = abs(t);
				if(t > 0.087)  //The acceptable range of angle is 85~95
					continue;
				for(l = 0; l < n_line; l++)
				{
					line4 = (CvPoint*)cvGetSeqElem(lines, l);
					//Calculte the Euclidean distance of line4 to the two other unconnected points
					ff1 = pow(p3.x-line4[0].x,2)+pow(p3.y-line4[0].y,2);
					ff2 = pow(p3.x-line4[1].x,2)+pow(p3.y-line4[1].y,2);
					ff3 = pow(p4.x-line4[0].x,2)+pow(p4.y-line4[0].y,2);
					ff4 = pow(p4.x-line4[1].x,2)+pow(p4.y-line4[1].y,2);
					ff5 = pow(line1[0].x-line1[1].x,2)+pow(line1[0].y-line1[1].y,2); //Length of line1
					ff6 = pow(line2[0].x-line2[1].x,2)+pow(line2[0].y-line2[1].y,2); //Length of line2
					ff7 = pow(line3[0].x-line3[1].x,2)+pow(line3[0].y-line3[1].y,2); //Length of line3
					ff8 = pow(line4[0].x-line4[1].x,2)+pow(line4[0].y-line4[1].y,2); //Length of line4
					dd = max(min(ff1,ff2), min(ff3, ff4));  //The distance from line4 to the other parallel line
					if(dd > min(min(ff5,ff6), min(ff7, ff8)))
						continue;
					if(n_co == 1)
					{
						ff1 = pow(line1[0].x-line4[0].x,2)+pow(line1[0].y-line4[0].y,2); 
						ff2 = pow(line1[1].x-line4[1].x,2)+pow(line1[1].y-line4[1].y,2);
						if(min(ff1,ff2) < min(ff6,ff7)) //The distance between parallel lines must be larger than perpendicular lines
							continue;
						t = ((line1[0].x-line1[1].x)*(line4[0].x-line4[1].x)+(line1[0].y-line1[1].y)*(line4[0].y-line4[1].y))/pow(ff5*ff8,0.5);//cos(angle) between line1 and line4
					}
					else if(n_co == 2)
					{
						ff1 = pow(line2[0].x-line4[0].x,2)+pow(line2[0].y-line4[0].y,2);
						ff2 = pow(line2[1].x-line4[1].x,2)+pow(line2[1].y-line4[1].y,2);
						if(min(ff1,ff2) < min(ff5,ff7))
							continue;
						t = ((line2[0].x-line2[1].x)*(line4[0].x-line4[1].x)+(line2[0].y-line2[1].y)*(line4[0].y-line4[1].y))/pow(ff6*ff8,0.5);//cos(angle) between line2 and line4
					}
					t = abs(t);
					if(t < 0.98)  //the two line should be nearly parallel
						continue;
					OverLap = false;
					yMin = min(min(line1[0].y, line2[0].y), min(line3[0].y, line4[0].y));
					yMax = max(max(line1[1].y, line2[1].y), max(line3[1].y, line4[1].y));
					xMin = min(min(line1[0].x, line2[0].x), min(line3[0].x, line4[0].x));
					xMax = max(max(line1[1].x, line2[1].x), max(line3[1].x, line4[1].x));
					inte_reg[RecNum].size = cvSize((int)((xMax-xMin)*1.2), (int)((yMax-yMin)*1.2));
					inte_reg[RecNum].strPoint.x = (int)((xMin*2.2-xMax*0.2)/2);
					if(inte_reg[RecNum].strPoint.x < 0)
						inte_reg[RecNum].strPoint.x = 0;
					inte_reg[RecNum].strPoint.y = (int)((yMin*2.2-yMax*0.2)/2);
					if(inte_reg[RecNum].strPoint.y < 0)
						inte_reg[RecNum].strPoint.x = 0;
					for(a = 0; a < RecNum; a++) //Define if the ROI is overlapped
					{
						dd = pow(inte_reg[RecNum].strPoint.x-inte_reg[a].strPoint.x,2)+pow(inte_reg[RecNum].strPoint.y-inte_reg[a].strPoint.y,2);
						if(dd < (pow(inte_reg[a].size.height,2)+pow(inte_reg[a].size.width,2)))
						{
							OverLap = true;
							break;
						}
					}
					if(OverLap == true)
						break;
					++RecNum;
				}
			}
		}
	}
	//store the image
	for(i = 0; i < min(RecNum,4); i++)
	{
		ImageAddres = ImageBase + "rec" + (char)(i + 48) +".jpg";
		cvSetImageROI(srcD,cvRect(inte_reg[i].strPoint.x,inte_reg[i].strPoint.y,inte_reg[i].size.width,inte_reg[i].size.height));
		pDest = cvCreateImage(inte_reg[i].size,src->depth,src->nChannels);
		cvCopy(srcD,pDest);
		cvSaveImage(ImageAddres.data(), pDest);
		cvResetImageROI(pDest);
		cvReleaseImage(&pDest);
	}
	cvReleaseImage(&cannyImg);
	cvReleaseImage(&srcD);
}