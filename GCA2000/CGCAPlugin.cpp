#include "pch.h"
#include "CGCAPlugin.h"
#include "CGCAScreen.h"

CGCAPlugin::CGCAPlugin(void) : CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, MY_PLUGIN_NAME, MY_PLUGIN_VERSION, MY_PLUGIN_DEVELOPER, MY_PLUGIN_COPYRIGHT)
{
	RegisterDisplayType(MY_PLUGIN_VIEW_AVISO, false, false, true, true);
}

CGCAPlugin::~CGCAPlugin(void)
= default;

EuroScopePlugIn::CRadarScreen* CGCAPlugin::OnRadarScreenCreated(const char* s_display_name, bool need_radar_content, bool geo_referenced, bool can_be_saved, bool can_be_created)
{
	if (strcmp(s_display_name, MY_PLUGIN_VIEW_AVISO) != 0)
		return nullptr;
	else
		return new CGCAScreen;
}

void CGCAPlugin::OnNewMetarReceived(const char* s_station, const char* s_full_metar)
{
}
