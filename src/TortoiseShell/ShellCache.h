// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2004 - Tim Kemp and Stefan Kueng

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

#pragma once
#include "atltrace.h"
#include "globals.h"
#include <tchar.h>
#include <string>
#include <vector>
#include "registry.h"

#define REGISTRYTIMEOUT 2000
#define EXCLUDELISTTIMEOUT 5000
#define DRIVETYPETIMEOUT 300000		// 5 min
#define NUMBERFMTTIMEOUT 300000
class ShellCache
{
public:
	ShellCache()
	{
		showrecursive = CRegStdWORD(_T("Software\\TortoiseSVN\\RecursiveOverlay"));
		folderoverlay = CRegStdWORD(_T("Software\\TortoiseSVN\\FolderOverlay"), TRUE);
		driveremote = CRegStdWORD(_T("Software\\TortoiseSVN\\DriveMaskRemote"));
		drivefixed = CRegStdWORD(_T("Software\\TortoiseSVN\\DriveMaskFixed"), TRUE);
		drivecdrom = CRegStdWORD(_T("Software\\TortoiseSVN\\DriveMaskCDROM"));
		driveremove = CRegStdWORD(_T("Software\\TortoiseSVN\\DriveMaskRemovable"));
		driveram = CRegStdWORD(_T("Software\\TortoiseSVN\\DriveMaskRAM"));
		driveunknown = CRegStdWORD(_T("Software\\TortoiseSVN\\DriveMaskUnknown"));
		excludelist = CRegStdString(_T("Software\\TortoiseSVN\\OverlayExcludeList"));
		recursiveticker = GetTickCount();
		folderoverlayticker = GetTickCount();
		driveticker = recursiveticker;
		drivetypeticker = recursiveticker;
		langticker = recursiveticker;
		columnrevformatticker = recursiveticker;
		excludelistticker = recursiveticker;
		menulayout = CRegStdWORD(_T("Software\\TortoiseSVN\\ContextMenuEntries"), MENUCHECKOUT | MENUUPDATE | MENUCOMMIT);
		langid = CRegStdWORD(_T("Software\\TortoiseSVN\\LanguageID"), 1033);
		blockstatus = CRegStdWORD(_T("Software\\TortoiseSVN\\BlockStatus"), 0);
		for (int i=0; i<27; i++)
		{
			drivetypecache[i] = -1;
		}
		TCHAR szBuffer[5];
		columnrevformatticker = GetTickCount();
		ZeroMemory(&columnrevformat, sizeof(NUMBERFMT));
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, &szDecSep[0], sizeof(szDecSep));
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, &szThousandsSep[0], sizeof(szThousandsSep));
		columnrevformat.lpDecimalSep = szDecSep;
		columnrevformat.lpThousandSep = szThousandsSep;
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, &szBuffer[0], sizeof(szBuffer));
		columnrevformat.Grouping = _ttoi(szBuffer);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, &szBuffer[0], sizeof(szBuffer));
		columnrevformat.NegativeOrder = _ttoi(szBuffer);
	}
	DWORD BlockStatus()
	{
		if ((GetTickCount() - REGISTRYTIMEOUT) > blockstatusticker)
		{
			blockstatusticker = GetTickCount();
			blockstatus.read();
		} // if ((GetTickCount() - REGISTRYTIMEOUT) > blockstatusticker)
		return (blockstatus);
	}
	DWORD GetMenuLayout()
	{
		if ((GetTickCount() - REGISTRYTIMEOUT) > layoutticker)
		{
			layoutticker = GetTickCount();
			menulayout.read();
		} // if ((GetTickCount() - REGISTRYTIMEOUT) > layoutticker)
		return (menulayout);
	}
	BOOL IsRecursive()
	{
		if ((GetTickCount() - REGISTRYTIMEOUT)>recursiveticker)
		{
			recursiveticker = GetTickCount();
			showrecursive.read();
		} // if ((GetTickCount() - REGISTRYTIMEOUT)>recursiveticker)
		return (showrecursive);
	}
	BOOL IsFolderOverlay()
	{
		if ((GetTickCount() - REGISTRYTIMEOUT)>folderoverlayticker)
		{
			folderoverlayticker = GetTickCount();
			folderoverlay.read();
		} // if ((GetTickCount() - REGISTRYTIMEOUT)>recursiveticker) 
		return (folderoverlay);
	}
	BOOL IsRemote()
	{
		DriveValid();
		return (driveremote);
	}
	BOOL IsFixed()
	{
		DriveValid();
		return (drivefixed);
	}
	BOOL IsCDRom()
	{
		DriveValid();
		return (drivecdrom);
	}
	BOOL IsRemovable()
	{
		DriveValid();
		return (driveremove);
	}
	BOOL IsRAM()
	{
		DriveValid();
		return (driveram);
	}
	BOOL IsUnknown()
	{
		DriveValid();
		return (driveunknown);
	}
	BOOL IsPathAllowed(LPCTSTR path)
	{
		UINT drivetype = 0;
		int drivenumber = PathGetDriveNumber(path);
		if ((drivenumber >=0)&&(drivenumber < 25))
		{
			drivetype = drivetypecache[drivenumber];
			if ((drivetype == -1)||((GetTickCount() - DRIVETYPETIMEOUT)>drivetypeticker))
			{
				drivetypeticker = GetTickCount();
				TCHAR pathbuf[MAX_PATH+4];
				_tcscpy(pathbuf, path);
				PathRemoveFileSpec(pathbuf);
				PathAddBackslash(pathbuf);
				ATLTRACE2(_T("GetDriveType for %s, Drive %d\n"), pathbuf, drivenumber);
				drivetype = GetDriveType(pathbuf);
				drivetypecache[drivenumber] = drivetype;
			} // if (drivetype == -1)
		} // if ((drivenumber >=0)&&(drivenumber < 25)) 
		else
		{
			TCHAR pathbuf[MAX_PATH+4];
			_tcscpy(pathbuf, path);
			PathRemoveFileSpec(pathbuf);
			PathAddBackslash(pathbuf);
			if (_tcsncmp(pathbuf, drivetypepathcache, MAX_PATH-1)==0)
				drivetype = drivetypecache[26];
			else
			{
				ATLTRACE2(_T("GetDriveType for %s\n"), pathbuf);
				drivetype = GetDriveType(pathbuf);
				drivetypecache[26] = drivetype;
				_tcsncpy(drivetypepathcache, pathbuf, MAX_PATH);
			} 
		}
		if ((drivetype == DRIVE_REMOVABLE)&&(!IsRemovable()))
			return FALSE;
		if ((drivetype == DRIVE_FIXED)&&(!IsFixed()))
			return FALSE;
		if ((drivetype == DRIVE_REMOTE)&&(!IsRemote()))
			return FALSE;
		if ((drivetype == DRIVE_CDROM)&&(!IsCDRom()))
			return FALSE;
		if ((drivetype == DRIVE_RAMDISK)&&(!IsRAM()))
			return FALSE;
		if ((drivetype == DRIVE_UNKNOWN)&&(IsUnknown()))
			return FALSE;

		ExcludeListValid();
		for (std::vector<stdstring>::iterator I = exvector.begin(); I != exvector.end(); ++I)
		{
			if (I->empty())
				continue;
			if (I->at(I->size()-1)=='*')
			{
				stdstring str = I->substr(0, I->size()-1);
				if (_tcsnicmp(str.c_str(), path, str.size())==0)
					return FALSE;
			}
			else if (_tcsicmp(I->c_str(), path)==0)
				return FALSE;
		}
		return TRUE;
	}
	DWORD GetLangID()
	{
		if ((GetTickCount() - REGISTRYTIMEOUT) > langticker)
		{
			langticker = GetTickCount();
			langid.read();
		} // if ((GetTickCount() - REGISTRYTIMEOUT) > layoutticker)
		return (langid);
	}
	NUMBERFMT * GetNumberFmt()
	{
		if ((GetTickCount() - NUMBERFMTTIMEOUT) > columnrevformatticker)
		{
			TCHAR szBuffer[5];
			columnrevformatticker = GetTickCount();
			ZeroMemory(&columnrevformat, sizeof(NUMBERFMT));
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, &szDecSep[0], sizeof(szDecSep));
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, &szThousandsSep[0], sizeof(szThousandsSep));
			columnrevformat.lpDecimalSep = szDecSep;
			columnrevformat.lpThousandSep = szThousandsSep;
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, &szBuffer[0], sizeof(szBuffer));
			columnrevformat.Grouping = _ttoi(szBuffer);
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, &szBuffer[0], sizeof(szBuffer));
			columnrevformat.NegativeOrder = _ttoi(szBuffer);
		} // if ((GetTickCount() - NUMBERFMTTIMEOUT) > columnrevformatticker)
		return &columnrevformat;
	}
private:
	void DriveValid()
	{
		if ((GetTickCount() - REGISTRYTIMEOUT)>driveticker)
		{
			driveticker = GetTickCount();
			driveremote.read();
			drivefixed.read();
			drivecdrom.read();
			driveremove.read();
		}
	}
	void ExcludeListValid()
	{
		if ((GetTickCount() - EXCLUDELISTTIMEOUT)>excludelistticker)
		{
			excludelistticker = GetTickCount();
			excludelist.read();
			if (excludeliststr.compare((stdstring)excludelist)==0)
				return;
			excludeliststr = (stdstring)excludelist;
			exvector.clear();
			int pos = 0, pos_ant = 0;
			pos = excludeliststr.find(_T("\n"), pos_ant);
			while (pos != stdstring::npos)
			{
				stdstring token = excludeliststr.substr(pos_ant, pos-pos_ant);
				exvector.push_back(token);
				pos_ant = pos+1;
				pos = excludeliststr.find(_T("\n"), pos_ant);
			}
			if (!excludeliststr.empty())
			{
				exvector.push_back(excludeliststr.substr(pos_ant, excludeliststr.size()-1));
			}
			excludeliststr = (stdstring)excludelist;
		}
	}
	CRegStdWORD blockstatus;
	CRegStdWORD langid;
	CRegStdWORD showrecursive;
	CRegStdWORD folderoverlay;
	CRegStdWORD driveremote;
	CRegStdWORD drivefixed;
	CRegStdWORD drivecdrom;
	CRegStdWORD driveremove;
	CRegStdWORD driveram;
	CRegStdWORD driveunknown;
	CRegStdWORD menulayout;
	CRegStdString excludelist;
	stdstring excludeliststr;
	std::vector<stdstring> exvector;
	DWORD recursiveticker;
	DWORD folderoverlayticker;
	DWORD driveticker;
	DWORD drivetypeticker;
	DWORD layoutticker;
	DWORD langticker;
	DWORD blockstatusticker;
	DWORD columnrevformatticker;
	DWORD excludelistticker;
	UINT  drivetypecache[27];
	TCHAR drivetypepathcache[MAX_PATH];
	NUMBERFMT columnrevformat;
	TCHAR szDecSep[5];
	TCHAR szThousandsSep[5];
};