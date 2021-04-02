#pragma once
#include <EuroScopePlugIn.h>
#define MY_PLUGIN_NAME      "PAR GCA-2000"
#define MY_PLUGIN_VERSION   "0.1a"
#define MY_PLUGIN_DEVELOPER "Dawid Reszel"
#define MY_PLUGIN_COPYRIGHT "GPL v3"
#define MY_PLUGIN_VIEW_AVISO  "GCA PAR radar display"
class CGCAPlugin :
	public EuroScopePlugIn::CPlugIn
{
public:
	CGCAPlugin(void);
	virtual ~CGCAPlugin(void);
	virtual EuroScopePlugIn::CRadarScreen* OnRadarScreenCreated(
		const char* sDisplayName,
		bool needRadarContent,
		bool geoReferenced,
		bool canBeSaved,
		bool canBeCreated);
	void OnNewMetarReceived(const char* sStation, const char* sFullMetar) override;
};

