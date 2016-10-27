#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "opencv2/opencv.hpp"
#include "ROIDetect.h"
#include "ROIPrepro.h"
#include "SVM.h"

// TSRDlg.h : ͷ�ļ�
//

#pragma once


// CTSRDlg �Ի���
class CTSRDlg : public CDialogEx
{
// ����
public:
	CTSRDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TSR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenImg();
	CString m_strPath;
	CString cirImgName; 
	CString triImgName; 
	CString recImgName; 
	void DrawPicToHDC(IplImage *img, UINT ID);
	afx_msg void OnTSDetect();
	afx_msg void OnImgProcess();
	afx_msg void OnImgRecg();
};