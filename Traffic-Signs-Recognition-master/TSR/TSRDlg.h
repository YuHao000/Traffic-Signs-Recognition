#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "opencv2/opencv.hpp"
#include "ROIDetect.h"
#include "ROIPrepro.h"
#include "SVM.h"

// TSRDlg.h : 头文件
//

#pragma once


// CTSRDlg 对话框
class CTSRDlg : public CDialogEx
{
// 构造
public:
	CTSRDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TSR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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