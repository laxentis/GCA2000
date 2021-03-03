#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Wyklucz rzadko używane rzeczy z nagłówków systemu Windows
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // niektóre konstruktory CString będą jawne

#include <afxwin.h>         // standardowe i rdzenne składniki MFC
#include <afxext.h>         // Rozszerzenia MFC

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // Klasy OLE MFC
#include <afxodlgs.h>       // Klasy okien dialogowych OLE MFC
#include <afxdisp.h>        // Klasy automatyzacji MFC
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // klasy bazy danych ODBC MFC
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // klasy bazy danych DAO MFC
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Obsługa MFC dla Formantów standardowych programu Internet Explorer 4
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Obsługa MFC dla Formantów standardowych systemu Windows
#endif // _AFX_NO_AFXCMN_SUPPORT


