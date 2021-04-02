// GCA2000.h: główny plik nagłówka dla biblioteki DLL GCA2000
//

#pragma once
#include "EuroScopePlugIn.h"
#include "CGCAPlugin.h"
#ifndef __AFXWIN_H__
#error "dołącz nagłówek „pch.h” przed dołączeniem tego pliku na potrzeby optymalizacji PCH"
#endif

#include "resource.h"		// główne symbole


// CGCA2000App
// Aby uzyskać implementację klasy, zobacz GCA2000.cpp
//

class CGCA2000App : public CWinApp
{
public:
	CGCA2000App();

	// Przesłania
public:
	BOOL InitInstance() override;
	DECLARE_MESSAGE_MAP()
};
