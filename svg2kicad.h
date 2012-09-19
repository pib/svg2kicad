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

// svg2kicad.h : main header file for the SVG2KICAD application
//

#if !defined(AFX_SVG2KICAD_H__56D6C186_A1FA_43E8_8195_9180FFD59D8D__INCLUDED_)
#define AFX_SVG2KICAD_H__56D6C186_A1FA_43E8_8195_9180FFD59D8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSvg2kicadApp:
// See svg2kicad.cpp for the implementation of this class
//

class CSvg2kicadApp : public CWinApp
{
public:
	CSvg2kicadApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSvg2kicadApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSvg2kicadApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVG2KICAD_H__56D6C186_A1FA_43E8_8195_9180FFD59D8D__INCLUDED_)
