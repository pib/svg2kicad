/*
This file is part of SVG2Kicad.

    SVG2Kicad is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SVG2Kicad is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SVG2Kicad.  If not, see <http://www.gnu.org/licenses/>.

	Copyright © 2011 Robert Latham
*/

// svg2kicadDlg.h : header file
//

#if !defined(AFX_SVG2KICADDLG_H__E225B4CB_27DE_4280_BBB6_1F79E6DFE9C9__INCLUDED_)
#define AFX_SVG2KICADDLG_H__E225B4CB_27DE_4280_BBB6_1F79E6DFE9C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSvg2kicadDlg dialog

class CSvg2kicadDlg : public CDialog
{
// Construction
public:
	virtual void OnOK();
	CSvg2kicadDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSvg2kicadDlg)
	enum { IDD = IDD_SVG2KICAD_DIALOG };
	CEdit	m_editResults;
	CEdit	m_editScale;
	CButton	m_radioMM;
	CButton	m_radioIN;
	CEdit	m_editName;
	CButton	m_buttonOK;
	CString	m_strKicadPathname;
	CString	m_strSVGPathname;
	float	m_fScale;
	UINT	m_unSmoothness;
	CString	m_strName;
	int		m_nScaleType;
	int		m_nUnits;
	CString	m_strResults;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSvg2kicadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSvg2kicadDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSVG();
	afx_msg void OnButtonKicad();
	afx_msg void OnChangeEdit();
	afx_msg void OnChangeRadio();
	afx_msg void OnUpdateEditScale();
	afx_msg void OnUpdateEditName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strOutputType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVG2KICADDLG_H__E225B4CB_27DE_4280_BBB6_1F79E6DFE9C9__INCLUDED_)
