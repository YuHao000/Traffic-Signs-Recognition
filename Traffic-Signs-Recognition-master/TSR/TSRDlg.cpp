
// TSRDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TSR.h"
#include "TSRDlg.h"

using namespace std;  
using namespace cv;  
CROIDetect de;
CROIPrepro pe;
CSVM se;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTSRDlg �Ի���



CTSRDlg::CTSRDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTSRDlg::IDD, pParent)
	, m_strPath(_T(""))
	, cirImgName(_T(""))
	, triImgName(_T(""))
	, recImgName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTSRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strPath);
	DDX_Text(pDX, IDC_EDIT2, cirImgName);
	DDX_Text(pDX, IDC_EDIT3, triImgName);
	DDX_Text(pDX, IDC_EDIT4, recImgName);
}

BEGIN_MESSAGE_MAP(CTSRDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTSRDlg::OnOpenImg)
	ON_BN_CLICKED(IDC_BUTTON2, &CTSRDlg::OnTSDetect)
	ON_BN_CLICKED(IDC_BUTTON3, &CTSRDlg::OnImgRecg)
	ON_BN_CLICKED(IDC_BUTTON4, &CTSRDlg::OnImgProcess)
END_MESSAGE_MAP()


// CTSRDlg ��Ϣ�������

BOOL CTSRDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTSRDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTSRDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTSRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTSRDlg::OnOpenImg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	 
	CString strFile = _T("");  
    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"), NULL);  
    if (dlg.DoModal())  
	{  
		strFile = dlg.GetPathName();  
	} 
	m_strPath = strFile;   //Ϊ�Ի�������һ�༭���Ӧ��CString�ͱ��������沢��ʾѡ�е�·��
	UpdateData(FALSE);        //����ˢ��m_strPath����Ӧ��CEdit�ؼ���ʾѡ�е��ļ���·��
	 
	//Refresh controls once loading a new image
	remove("..\\ellip0.jpg");
	remove("..\\ellip1.jpg");
	remove("..\\ellip2.jpg");
	remove("..\\ellip3.jpg");
	remove("..\\tri0.jpg");
	remove("..\\tri1.jpg");
	remove("..\\tri2.jpg");
	remove("..\\tri3.jpg");
	remove("..\\rec0.jpg");
	remove("..\\rec1.jpg");
	remove("..\rec2.jpg");
	remove("..\\rec3.jpg");
	cirImgName = "";
	UpdateData(FALSE); 
	triImgName = "";
	UpdateData(FALSE); 
	recImgName = "";
	UpdateData(FALSE); 
	RedrawWindow(); //Remove images in Picture control

	IplImage *image=NULL; //Original image
	if(image) 
		cvReleaseImage(&image);
	image = cvLoadImage( strFile, CV_LOAD_IMAGE_COLOR );
	DrawPicToHDC(image, IDC_STATIC);
}

void CTSRDlg::DrawPicToHDC(IplImage *img, UINT ID) //Display image in Picture control
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf( img );
	cimg.DrawToHDC( hDC, &rect ); 
	ReleaseDC( pDC );
}

void CTSRDlg::OnTSDetect() //Detect ROI and display the results
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	IplImage *img = cvLoadImage( m_strPath, CV_LOAD_IMAGE_COLOR );
	IplImage *src = cvLoadImage( m_strPath, CV_LOAD_IMAGE_COLOR ); 
	IplImage *pDest;  //Segmented ROI
	IplImage *bimg;   //Binarized image
	int ellipNum = 0, triNum = 0, recNum = 0;
	string ImageBase ="..\\";
	string ImageAddres;

	de.RGBMark( img );
	DrawPicToHDC( img, IDC_STATIC );

	de.SizeDeJug( img );
	DrawPicToHDC( img, IDC_STATIC );

	bimg = cvCreateImage( cvGetSize(img), img->depth, 1 );
	cvCvtColor( img, bimg, CV_BGR2GRAY );
	cvThreshold( bimg, bimg, slider_pos,255, CV_THRESH_BINARY );
	cvErode( bimg, bimg );  //Erode
	DrawPicToHDC( bimg, IDC_STATIC );
	de.CircleDec( bimg, src ); //Detect possible circle areas
	de.TriangleDec( bimg, src ); //Detect possible triangle areas
	de.RectangleDec( bimg, src ); //Detect possible rectangle areas
	for( ; ellipNum < de.CirNum; ellipNum++)  //Display images in controls
	{
		ImageAddres = ImageBase + "ellip" + (char)(ellipNum + 48) +".jpg";
		pDest = cvLoadImage( ImageAddres.data(), 1 );
		if(ellipNum == 0)
			DrawPicToHDC( pDest, IDC_STATIC1 );
		if(ellipNum == 1)
			DrawPicToHDC( pDest, IDC_STATIC2 );
		if(ellipNum == 2)
			DrawPicToHDC( pDest, IDC_STATIC3 );
		if(ellipNum == 3)
			DrawPicToHDC( pDest, IDC_STATIC4 );
	}
	for( ; triNum < de.TriNum; triNum++)
	{
		ImageAddres = ImageBase + "tri" + (char)(triNum + 48) +".jpg";
		pDest = cvLoadImage( ImageAddres.data(), 1 );
		if(triNum == 0)
			DrawPicToHDC( pDest, IDC_STATIC5 );
		if(triNum == 1)
			DrawPicToHDC( pDest, IDC_STATIC6 );
		if(triNum == 2)
			DrawPicToHDC( pDest, IDC_STATIC7 );
		if(triNum == 3)
			DrawPicToHDC( pDest, IDC_STATIC8 );
	}
	for( ; recNum < de.RecNum; recNum++)
	{
		ImageAddres = ImageBase + "rec" + (char)(recNum + 48) +".jpg";
		pDest = cvLoadImage( ImageAddres.data(), 1);
		if(recNum == 0)
			DrawPicToHDC( pDest, IDC_STATIC9 );
		if(recNum == 1)
			DrawPicToHDC( pDest, IDC_STATIC10 );
		if(recNum == 2)
			DrawPicToHDC( pDest, IDC_STATIC11 );
		if(recNum == 3)
			DrawPicToHDC( pDest, IDC_STATIC12 );
	}
}

void CTSRDlg::OnImgProcess() //Correct tile and resegment ROI
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	IplImage *pDest = NULL;
	string ImageBase ="..\\";
	string ImageAddres;
	int ellipNum=0, triNum=0, recNum=0;

	if(de.CirNum == 0 && de.TriNum == 0 && de.RecNum == 0)
		MessageBox("�����ڣ�","��ʾ",MB_ICONWARNING); 
	else
	{
		while(ellipNum < de.CirNum || triNum < de.TriNum || recNum < de.RecNum)
		{
			if(ellipNum < de.CirNum)
			{
				ImageAddres = ImageBase + "ellip" + (char)(ellipNum + 48) +".jpg";
				pDest = cvLoadImage(ImageAddres.data(),1);
				++ellipNum;
			}
			else if(triNum < de.TriNum)
			{
				ImageAddres = ImageBase + "tri" + (char)(triNum + 48) +".jpg";
				pDest = cvLoadImage(ImageAddres.data(),1);
				++triNum;
				pe.TiltCorrect(pDest);
			}
			else if(recNum < de.RecNum)
			{
				ImageAddres = ImageBase + "rec" + (char)(recNum + 48) +".jpg";
				pDest = cvLoadImage(ImageAddres.data(),1);
				++recNum;
				pe.TiltCorrect(pDest);
			}
		pe.ROISegment(ImageAddres.data(), pDest);
		}
	}

	ellipNum=0, triNum=0, recNum=0;
	for( ; ellipNum < de.CirNum; ellipNum++)  //Display results in controls
	{
		ImageAddres = ImageBase + "ellip" + (char)(ellipNum + 48) +".jpg";
		pDest = cvLoadImage( ImageAddres.data(), 1 );
		if(ellipNum == 0)
			DrawPicToHDC( pDest, IDC_STATIC1 );
		if(ellipNum == 1)
			DrawPicToHDC( pDest, IDC_STATIC2 );
		if(ellipNum == 2)
			DrawPicToHDC( pDest, IDC_STATIC3 );
		if(ellipNum == 3)
			DrawPicToHDC( pDest, IDC_STATIC4 );
	}
	for( ; triNum < de.TriNum; triNum++)
	{
		ImageAddres = ImageBase + "tri" + (char)(triNum + 48) +".jpg";
		pDest = cvLoadImage( ImageAddres.data(), 1 );
		if(triNum == 0)
			DrawPicToHDC( pDest, IDC_STATIC5 );
		if(triNum == 1)
			DrawPicToHDC( pDest, IDC_STATIC6 );
		if(triNum == 2)
			DrawPicToHDC( pDest, IDC_STATIC7 );
		if(triNum == 3)
			DrawPicToHDC( pDest, IDC_STATIC8 );
	}
	for( ; recNum < de.RecNum; recNum++)
	{
		ImageAddres = ImageBase + "rec" + (char)(recNum + 48) +".jpg";
		pDest = cvLoadImage( ImageAddres.data(), 1);
		if(recNum == 0)
			DrawPicToHDC( pDest, IDC_STATIC9 );
		if(recNum == 1)
			DrawPicToHDC( pDest, IDC_STATIC10 );
		if(recNum == 2)
			DrawPicToHDC( pDest, IDC_STATIC11 );
		if(recNum == 3)
			DrawPicToHDC( pDest, IDC_STATIC12 );
	}
}


void CTSRDlg::OnImgRecg() //Recognize traffic signs
{
	IplImage *pDest;
	string ImageBase ="..\\";
	string ImageAddres;
	int ellipNum=0, triNum=0, recNum=0;

	if(de.CirNum == 0 && de.TriNum == 0 && de.RecNum == 0)
		MessageBox("�����ڣ�","��ʾ",MB_ICONWARNING);  
	else //Load images and recognize them based on SVM
	{
		se.RedTrain = false, se.BlueTrain = false;
		for( ; ellipNum < de.CirNum; ellipNum++)
		{
			ImageAddres = ImageBase + "ellip" + (char)(ellipNum + 48) +".jpg";
			pDest = cvLoadImage(ImageAddres.data(),1);
			cirImgName = cirImgName + " " + se.CirSVM(pDest);
		}
		UpdateData(FALSE); 

		se.RedTrain = false, se.YellTrain = false;
		for( ; triNum < de.TriNum; triNum++)
		{
			ImageAddres = ImageBase + "tri" + (char)(triNum + 48) +".jpg";
			pDest = cvLoadImage(ImageAddres.data(),1);
			triImgName = triImgName + " " + se.TriSVM(pDest);
		}
		UpdateData(FALSE); 
		
		se.BlueTrain = false;
		for( ; recNum < de.RecNum; recNum++)
		{
			ImageAddres = ImageBase + "rec" + (char)(recNum + 48) +".jpg";
			pDest = cvLoadImage(ImageAddres.data(),1);
			recImgName = recImgName + " " + se.RecSVM(pDest);
		}
		UpdateData(FALSE); 
	}
}