#include "pch.h"
#include "CGCAPlugin.h"
#include "CGCAScreen.h"

CGCAPlugin::CGCAPlugin(void) : CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, MY_PLUGIN_NAME, MY_PLUGIN_VERSION, MY_PLUGIN_DEVELOPER, MY_PLUGIN_COPYRIGHT)
{
	RegisterDisplayType(MY_PLUGIN_VIEW_AVISO, false, false, true, true);
}

CGCAPlugin::~CGCAPlugin(void)
= default;

EuroScopePlugIn::CRadarScreen* CGCAPlugin::OnRadarScreenCreated(const char* sDisplayName, bool needRadarContent, bool geoReferenced, bool canBeSaved, bool canBeCreated)
{
	if (strcmp(sDisplayName, MY_PLUGIN_VIEW_AVISO) != 0)
		return nullptr;
	else
		return new CGCAScreen;
}

void CGCAPlugin::OnNewMetarReceived(const char* sStation, const char* sFullMetar)
{
}
