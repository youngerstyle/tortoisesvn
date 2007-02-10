// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2006 - Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#pragma once

#include "StandAloneDlg.h"
#include "Watereffect.h"
#include "Dib.h"
#include "HyperLink.h"
#include "StandAloneDlg.h"

#define ID_EFFECTTIMER 1111
#define ID_DROPTIMER 1112

/**
 * \ingroup TortoiseProc
 * Class for showing an About box of TortoiseSVN. Contains a Picture
 * with the TortoiseSVN logo with a nice water effect. See CWaterEffect
 * for the implementation.
 */
class CAboutDlg : public CStandAloneDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAboutDlg();

	enum { IDD = IDD_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedUpdate();

	DECLARE_MESSAGE_MAP()

private:
	CWaterEffect m_waterEffect;
	CDib m_renderSrc;
	CDib m_renderDest;
	CHyperLink m_cWebLink;
	CHyperLink m_cSupportLink;
};
