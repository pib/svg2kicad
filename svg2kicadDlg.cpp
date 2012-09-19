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

// svg2kicadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "svg2kicad.h"
#include "svg2kicadDlg.h"
#include "svg_kicad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSvg2kicadDlg dialog

CSvg2kicadDlg::CSvg2kicadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSvg2kicadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSvg2kicadDlg)
	m_strKicadPathname = _T("SVGModule.emp");
	m_strSVGPathname = _T("");
	m_fScale = 50.0f;
	m_unSmoothness = 20;
	m_strName = _T("FROMSVG");
	m_nScaleType = 0;
	m_nUnits = 0;
	m_strResults = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSvg2kicadDlg::DoDataExchange(CDataExchange* pDX)
{
	int				nDot;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSvg2kicadDlg)
	DDX_Control(pDX, IDC_EDIT_RESULTS, m_editResults);
	DDX_Control(pDX, IDC_EDIT_SCALE, m_editScale);
	DDX_Control(pDX, IDC_RADIO_MM, m_radioMM);
	DDX_Control(pDX, IDC_RADIO_IN, m_radioIN);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDOK, m_buttonOK);
	DDX_Text(pDX, IDC_EDIT_KICAD, m_strKicadPathname);
	DDX_Text(pDX, IDC_EDIT_SVG, m_strSVGPathname);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_fScale);
	DDX_Text(pDX, IDC_EDIT_SMOOTH, m_unSmoothness);
	DDV_MinMaxUInt(pDX, m_unSmoothness, 1, 100000);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 16);
	DDX_Radio(pDX, IDC_RADIO_PC, m_nScaleType);
	DDX_Radio(pDX, IDC_RADIO_MM, m_nUnits);
	DDX_Text(pDX, IDC_EDIT_RESULTS, m_strResults);
	//}}AFX_DATA_MAP
	DDV_MinMaxFloat(pDX, m_fScale, 0.0f, 100000.0f);

	m_buttonOK.EnableWindow(!m_strSVGPathname.IsEmpty() && !m_strKicadPathname.IsEmpty());

	nDot = m_strKicadPathname.ReverseFind('.');
	if (nDot > -1)
	{
		m_strOutputType = m_strKicadPathname.Mid(nDot + 1);
		m_strOutputType.MakeLower();
	}

	m_radioMM.EnableWindow(m_nScaleType);
	m_radioIN.EnableWindow(m_nScaleType);
}

BEGIN_MESSAGE_MAP(CSvg2kicadDlg, CDialog)
	//{{AFX_MSG_MAP(CSvg2kicadDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SVG, OnButtonSVG)
	ON_BN_CLICKED(IDC_BUTTON_KICAD, OnButtonKicad)
	ON_EN_CHANGE(IDC_EDIT_SCALE, OnChangeEdit)
	ON_BN_CLICKED(IDC_RADIO_PC, OnChangeRadio)
	ON_EN_UPDATE(IDC_EDIT_SCALE, OnUpdateEditScale)
	ON_EN_CHANGE(IDC_EDIT_SVG, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_KICAD, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_SMOOTH, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEdit)
	ON_BN_CLICKED(IDC_RADIO_H, OnChangeRadio)
	ON_BN_CLICKED(IDC_RADIO_IN, OnChangeRadio)
	ON_BN_CLICKED(IDC_RADIO_MM, OnChangeRadio)
	ON_BN_CLICKED(IDC_RADIO_W, OnChangeRadio)
	ON_EN_UPDATE(IDC_EDIT_NAME, OnUpdateEditName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSvg2kicadDlg message handlers

BOOL CSvg2kicadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSvg2kicadDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSvg2kicadDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSvg2kicadDlg::OnButtonSVG() 
{
	CFileDialog			dlgFile(TRUE, 
							"svg", 
							(LPCTSTR) m_strSVGPathname, 
							OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
							"Scalable Vector Graphics Files (*.svg)|*.svg||", 
							this);

	if (dlgFile.DoModal() == IDOK)
	{
		m_strSVGPathname = dlgFile.GetPathName();
		UpdateData(FALSE);
	}
}

void CSvg2kicadDlg::OnButtonKicad() 
{
	CFileDialog			dlgFile(FALSE, 
							"emp", 
							(LPCTSTR) m_strKicadPathname, 
							OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
							"All supported files|*.emp;*.mod;*.lib|"
							"PCBNew module (*.emp)|*.emp|"
							"PCBNew library (*.mod)|*.mod|"
							"EESchema library (*.lib)|*.lib||", 
							this);

	if (dlgFile.DoModal() == IDOK)
	{
		m_strKicadPathname = dlgFile.GetPathName();
		UpdateData(FALSE);
	}
}

void CSvg2kicadDlg::OnOK()
{
	CWaitCursor			wc;
	char				cType = 'e';
	float				fScale;

	if (!m_strSVGPathname.IsEmpty() && !m_strKicadPathname.IsEmpty())
	{
		fScale = m_fScale;
		if (!m_nUnits)
		{
			// Metric - scale to inches

			fScale = fScale / 25.4f;
		}
		if ((m_strOutputType == "mod") || (m_strOutputType == "lib"))
		{
			cType = m_strOutputType[0];
		}
		m_strResults.Empty();
		UpdateData(FALSE);
		m_editResults.FmtLines(TRUE);
		SVG2Kicad((LPCTSTR) m_strSVGPathname, (LPCTSTR) m_strKicadPathname, (LPCTSTR) m_strName, cType, fScale, (char) m_nScaleType, m_unSmoothness);
		if (GetLog())
		{
			int			nLen;

			m_strResults = GetLog();
			UpdateData(FALSE);
			nLen = m_strResults.GetLength() - 1;
			m_editResults.SetSel(nLen, nLen);
		}
		MessageBeep(MB_OK);
		DestroyLog();
	}
}

void CSvg2kicadDlg::OnChangeEdit() 
{
	UpdateData();
}

void CSvg2kicadDlg::OnChangeRadio() 
{
	UpdateData();	
}

void CSvg2kicadDlg::OnUpdateEditScale() 
{
	CString			strScale, strNewScale;
	int				nChar, nLen, nStart, nEnd;

	m_editScale.GetWindowText(strScale);
	m_editScale.GetSel(nStart, nEnd);
	nLen = strScale.GetLength();
	for (nChar = 0; nChar < nLen; nChar++)
	{
		if (isdigit(strScale[nChar]) || (strScale[nChar] == '.'))
		{
			strNewScale += strScale[nChar];
		}
	}

	if (strScale != strNewScale)
	{
		MessageBeep(MB_ICONWARNING);
		m_editScale.SetWindowText((LPCTSTR) strNewScale);
		m_editScale.SetSel(--nStart, --nEnd);
	}
}

void CSvg2kicadDlg::OnUpdateEditName() 
{
	CString			strName, strNewName;
	int				nChar, nLen, nStart, nEnd;

	// Names containing whitespace seem to give kicad brainache

	m_editName.GetWindowText(strName);
	m_editName.GetSel(nStart, nEnd);
	nLen = strName.GetLength();
	for (nChar = 0; nChar < nLen; nChar++)
	{
		if (!isspace(strName[nChar]))
		{
			strNewName += strName[nChar];
		}
	}

	if (strName != strNewName)
	{
		MessageBeep(MB_ICONWARNING);
		m_editName.SetWindowText((LPCTSTR) strNewName);
		m_editName.SetSel(--nStart, --nEnd);
	}
}
