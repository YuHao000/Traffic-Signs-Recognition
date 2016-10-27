#pragma once

static int slider_pos = 100; 

typedef struct Circal
{
	CvPoint center;
	double r;
	int num; 
}Circal;

typedef struct ROI
{
	CvSize size; 
	CvPoint strPoint;
}ROI;

class CROIDetect
{

public:
	CROIDetect(void);
	~CROIDetect(void);

public:
	int CirNum;
	int TriNum;
	int RecNum;
	void RGBMark(IplImage *img);
	void SizeDeJug(IplImage *img);
	void CircleDec(IplImage *bimg, IplImage *src);
	void TriangleDec(IplImage *bimg, IplImage *src);
	void RectangleDec(IplImage *bimg, IplImage *src);
};

