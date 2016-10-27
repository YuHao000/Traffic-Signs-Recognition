#pragma once
class CROIPrepro
{
public:
	CROIPrepro(void);
	~CROIPrepro(void);

public:
	void TiltCorrect(IplImage *img);
	void bound(int x, int y, float ca, float sa, int *xmin, int *xmax, int *ymin, int *ymax);
	void ColorPro(IplImage *img);
	void ROISegment(CString ImgAddre, IplImage *img);
};

