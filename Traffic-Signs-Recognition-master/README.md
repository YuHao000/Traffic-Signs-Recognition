# Traffic-Signs-Recognition
Image processing, with OpenCV

CvvImage: Downloaded class, belongs to previous version of OpneCV
	void  CImage::DrawToHDC(HDC hDCDst, RECT* pDstRect)
	//Copy ROI(Region of Interest) of the image to Picture Control area

In the detection phase of this system, this paper presented a method based on color and size segmentation and local Hough transform. Firstly, segmented the image based on the color and size characteristics of traffic signs, which could remove a large number of independent interference region effectively. Then I did a contour tracking on the binary image and used the Hough transform to detect the shapes of contours. Finally, the circular, rectangular and triangular regions could be located in this image.
ROIDetect:
	void RGBMark(IplImage *img); //RGB color segmentation
	void SizeDeJug(IplImage *img); //Filter acceptable ROI based on the size of their contours
	void CircleDec(IplImage *bimg, IplImage *src); //Define if ellipse areas exist
	void TriangleDec(IplImage *bimg, IplImage *src); //Define if triangle areas exist
	void RectangleDec(IplImage *bimg, IplImage *src); //Define if rectangle areas exist

In order to improve the recognition rate in the traffic signs recognition stage, this paper carried out the preprocessing of detected regions-tilt correction and re-segmentation. When rectangular and triangular traffic signs tilted, it would be hard to extract features using inaccurate gradient information and then led to the interference of classification results. By calculating the minimum tilt angle of candidate regions, the system obtained the tilt angle of the whole image. According to the contours tracking of titled image, I re-segmented the image in which the distractions were reduced.
ROIPrepro:
	void TiltCorrect(IplImage *img); //Correct tilt of ROI 
	void bound(int x, int y, float ca, float sa, int *xmin, int *xmax, int *ymin, int *ymax);
	void ColorPro(IplImage *img); //RGB color segmentation
	void ROISegment(CString ImgAddre, IplImage *img); //Resegmentation of ROI
SVM:
	CString CirSVM(IplImage *img); //Circle Signs have blue and red colors in China, send circle signs to SVM based on color 
	CString TriSVM(IplImage *img); //Triangle signs have yellow and red colors in China, send triangle signs to SVM based on color 
	CString RecSVM(IplImage *img); //Rectangle signs have blue colors in China, send rectangle signs to SVM based on color 
	void SVMLearn(); //Train SVM
	int SVMDetect(IplImage *img); //Sign detection using SVM

TSRDlg:
	void OnTSDetect(); //Detect ROIs of the image
	void OnImgProcess(); //Tilt correction and resegmentation
	void OnImgRecg(); //Recognize detected signs
