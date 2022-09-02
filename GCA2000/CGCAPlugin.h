#pragma once
#include <EuroScopePlugIn.h>
#define MY_PLUGIN_NAME      "PAR GCA-2000"
#define MY_PLUGIN_VERSION   "0.1a"
#define MY_PLUGIN_DEVELOPER "Dawid 984547"
#define MY_PLUGIN_COPYRIGHT "GPL v3"
#define MY_PLUGIN_VIEW_AVISO  "GCA PAR radar display"
class CGCAPlugin final :
	public EuroScopePlugIn::CPlugIn
{
public:
	CGCAPlugin();
	~CGCAPlugin() override;
	EuroScopePlugIn::CRadarScreen* OnRadarScreenCreated(
		const char* s_display_name,
		bool need_radar_content,
		bool geo_referenced,
		bool can_be_saved,
		bool can_be_created) override;
	void OnNewMetarReceived(const char* s_station, const char* s_full_metar) override;
};

