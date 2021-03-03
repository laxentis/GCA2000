// GCA2000.cpp: Definiuje procedury inicjowania biblioteki DLL.
//

#include "pch.h"
#include "framework.h"
#include "GCA2000.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: Jeśli ta biblioteka DLL jest połączona dynamicznie z bibliotekami DLL MFC,
//		wszystkie funkcje wyeksportowane z tej biblioteki DLL, które wywołują
//		muszą mieć makro AFX_MANAGE_STATE dodane w
//		na samym początku funkcji.
//
//		Na przykład:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// zwykłe ciało funkcji
//		}
//
//		Jest bardzo ważnym, żeby to makro pojawiło się w każdym
//		funkcję, przed dowolnymi wywołaniami MFC.  Oznacza to, że
//		musi wystąpić jako pierwsza instrukcja w obrębie
//		funkcję, nawet przed dowolnymi deklaracjami zmiennych obiektu
//		tak, jak ich konstruktory mogę generować wywołania MFC
//		biblioteka DLL.
//
//		Zobacz uwagi techniczne MFC nr 33 i 58 dla dodatkowych
//		szczegóły.
//

// CGCA2000App

BEGIN_MESSAGE_MAP(CGCA2000App, CWinApp)
END_MESSAGE_MAP()


// Konstrukcja klasy CGCA2000App

CGCA2000App::CGCA2000App()
{
	// TODO: W tym miejscu dodaj kod konstruktora,
	// Umieść wszystkie znaczące inicjacje w InitInstance
}


// Jedyny obiekt CGCA2000App

CGCA2000App theApp;


// Inicjowanie klasy CGCA2000App

BOOL CGCA2000App::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//--- ES Plugin Init

CGCAPlugin* gpMyPlugin = NULL;

//---EuroScopePlugInInit-----------------------------------------------

void    __declspec (dllexport)    EuroScopePlugInInit(EuroScopePlugIn::CPlugIn** ppPlugInInstance)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

        // create the instance
        * ppPlugInInstance = gpMyPlugin = new CGCAPlugin();
}


//---EuroScopePlugInExit-----------------------------------------------

void    __declspec (dllexport)    EuroScopePlugInExit(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

        // delete the instance
        delete gpMyPlugin;
}