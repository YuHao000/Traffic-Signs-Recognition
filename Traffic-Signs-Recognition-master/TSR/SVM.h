#pragma once
class CSVM
{
public:
	CSVM(void);
	~CSVM(void);

public:
	bool RedTrain, BlueTrain, YellTrain;
	CString ImgPath, SVMPath;
	CString CirSVM(IplImage *img);
	CString TriSVM(IplImage *img);
	CString RecSVM(IplImage *img);
	void SVMLearn();
	int SVMDetect(IplImage *img);
};

