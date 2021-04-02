#include "pch.h"
#include "CGCAScreen.h"
// ReSharper disable once CppInconsistentNaming
#define _USE_MATH_DEFINES
#include <math.h>  // NOLINT(modernize-deprecated-headers) because VS fails to include MATH DEFINES from cmath
#include <string>

CGCAScreen::CGCAScreen(void)
{
	Description = "EPKS PAR 29";
	Lat = "N052.19.36.15";
	Lon = "E016.58.57.79";
	Altitude = 274;
	ObstacleClearanceHeight = 280;
	Heading = 294;
	GlideSlope = 3.0;
	ScopeMaxHeight = 3000;
}

CGCAScreen::~CGCAScreen(void)
{
}

void CGCAScreen::OnAsrContentLoaded(bool loaded)
{
	const char* p_value;
	if ((p_value = GetDataFromAsr("Description")) != nullptr)
		Description = p_value;
	if ((p_value = GetDataFromAsr("Longitude")) != nullptr)
		Lon = p_value;
	if ((p_value = GetDataFromAsr("Latitude")) != nullptr)
		Lat = p_value;
	if ((p_value = GetDataFromAsr("Altitude")) != nullptr)
		Altitude = atoi(p_value);
	if ((p_value = GetDataFromAsr("Heading")) != nullptr)
		Heading = atoi(p_value);
	if ((p_value = GetDataFromAsr("Slope")) != nullptr)
		GlideSlope = atof(p_value);
	if (GlideSlope > 8.0)
		GlideSlope = 8.0;
	if (GlideSlope <= 0.0)
		GlideSlope = 1.0;
	if ((p_value = GetDataFromAsr("OCH")) != nullptr)
		ObstacleClearanceHeight = atoi(p_value);

	RunwayPosition.LoadFromStrings(Lon, Lat);
	ScopeMaxHeight = Altitude + static_cast<int>(20.0 * 6076.0 * sin(GlideSlope / 180.0 * M_PI));
}


void CGCAScreen::DrawGlideslopeAxes(CDC* dc, const CRect area, const unsigned maxRange = 20, const unsigned maxAlt = 16000) const
{
	// Store current dc
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();

	// Create red pen for axis
	CPen redPen(0, 2, RGB(172, 36, 51));
	dc->SelectObject(&redPen);

	// Draw altitude axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.left, area.top);

	const auto numVTicks = static_cast<int>(maxAlt / 4000);
	const auto altTick = area.Height() / numVTicks;
	dc->SetTextColor(RGB(172, 36, 51));
	dc->SetTextAlign(TA_RIGHT);
	for (auto i = 0; i <= numVTicks; i++)
	{
		dc->MoveTo(area.left - 10, area.bottom - i * altTick);
		dc->LineTo(area.left + 10, area.bottom - i * altTick);
		if (i == 0)
			continue;
		auto label = std::to_string(i * 4000);
		dc->TextOutA(area.left - 15, area.bottom - i * altTick, label.c_str());
	}
	// Draw track distance axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom);
	const auto numTicks = static_cast<int>(maxRange / 2.5);
	const auto rangeTick = area.Width() / numTicks;
	for (auto i = 0; i <= numTicks; i++)
	{
		dc->MoveTo(area.left + i * rangeTick, area.bottom - 10);
		dc->LineTo(area.left + i * rangeTick, area.bottom + 10);
		if (i == 0)
			continue;
		if (i % 2 != 0)
			continue;
		auto label = std::to_string(static_cast<int>(i * 2.5));
		dc->SetTextAlign(TA_TOP);
		dc->TextOutA(area.left + i * rangeTick, area.bottom + 15, label.c_str());
	}
	
	// Restore previous dc
	dc->RestoreDC(sDC);
}

void CGCAScreen::OnAsrContentToBeSaved(void)
{
	CString str;
	SaveDataToAsr("Description", "PAR description", Description);
	SaveDataToAsr("Longitude", "RWY threshold longitude", Lon);
	SaveDataToAsr("Latitude", "RWY threshold latitude", Lat);
	str.Format("%d", Altitude);
	SaveDataToAsr("Altitude", "RWY threshold altitude", str);
	str.Format("%d", ObstacleClearanceHeight);
	SaveDataToAsr("OCH", "Obstacle Clearance Height", str);
	str.Format("%d", Heading);
	SaveDataToAsr("Heading", "RWY heading", str);
	str.Format("%.1f", GlideSlope);
	SaveDataToAsr("Slope", "Glide slope angle", str);
}

void CGCAScreen::OnRefresh(HDC hDC, int Phase)
{
    // only in first phase
    if (Phase != EuroScopePlugIn::REFRESH_PHASE_BEFORE_TAGS)
        return;

    // I am using MFC, so load the DC in to MFC structure
    CDC     dc;
    dc.Attach(hDC);
	// Creating pens
	CPen redPen(0, 2, RGB(172, 36, 51));
	CPen bluPen(0, 2, RGB(19, 97, 232));
	CPen yelPen(0, 2, RGB(223, 212, 36));
	CPen grePen(0, 2, RGB(34, 85, 48));
	CPen* p_old_pen;
	p_old_pen = dc.SelectObject(&redPen);
	// Get drawing area
	CRect radarArea = GetRadarArea();
	CRect chatArea = GetChatArea();
	radarArea.bottom = chatArea.top;
	// Add margins
	radarArea.DeflateRect(50, 50);
    auto midPoint = radarArea.CenterPoint();
	// Get Glideslope, track and cross areas
    auto gsArea = CRect(radarArea.left, radarArea.top, radarArea.right, midPoint.y);
    auto tkArea = CRect(radarArea.left, midPoint.y, radarArea.right, radarArea.bottom);
	gsArea.DeflateRect(0, 20);
	tkArea.DeflateRect(0, 20);
    auto xsArea = CRect(gsArea.left, gsArea.top, gsArea.left + 250, gsArea.top + 250);
	xsArea.DeflateRect(20, 20);
	// ### GLIDESLOPE PORTION ###
	DrawGlideslopeAxes(&dc, gsArea, 20, 16000);
	// Draw cross
	dc.SelectObject(&redPen);
    auto crossMiddle = xsArea.CenterPoint();
	dc.MoveTo(xsArea.left, crossMiddle.y);
	dc.LineTo(xsArea.right, crossMiddle.y);
	dc.MoveTo(crossMiddle.x, xsArea.top);
	dc.LineTo(crossMiddle.x, xsArea.bottom);
	
	const unsigned int maxRange = 20;
	const unsigned int maxAlt = 16000;
	// Show Glideslope
    auto slopeRadians = GlideSlope * M_PI / 180;
    auto slopeMaxAlt = tan(slopeRadians) * maxRange * 6076;
	dc.SelectObject(&grePen);
	dc.MoveTo(gsArea.left, gsArea.bottom);
	dc.LineTo(gsArea.right, gsArea.bottom - (slopeMaxAlt/maxAlt * gsArea.Height()));
	// Show Obstacle Clearance Height
    auto obstacleClearanceHeightDistance = (1/tan(slopeRadians)) * (ObstacleClearanceHeight / 6076.0);
	auto obstacleClearanceHeightCenterX = gsArea.left + obstacleClearanceHeightDistance / maxRange * gsArea.Width();
	auto obstacleClearanceHeightCenterY = gsArea.bottom - static_cast<float>(ObstacleClearanceHeight) / maxAlt * gsArea.Height();
	dc.MoveTo(obstacleClearanceHeightCenterX - 100, obstacleClearanceHeightCenterY);
	dc.LineTo(obstacleClearanceHeightCenterX + 100, obstacleClearanceHeightCenterY);
	// Draw runway
	dc.SelectObject(&bluPen);
	dc.MoveTo(0, gsArea.bottom);
	dc.LineTo(gsArea.left + 50, gsArea.bottom);
	// ### MIDDLE TEXT ###
	// Draw info text in the middle
	dc.SetTextColor(RGB(170, 29, 93));
	dc.SetTextAlign(TA_LEFT);
	std::string topLabel = "  GS: ";
	topLabel.append(std::to_string(GlideSlope).substr(0,3));
	topLabel.append("�        RWY: ");
	topLabel.append(std::to_string(Heading));
	std::string windDir = "270"; // TODO: Add wind from METAR
	std::string windSpd = "15";  // TODO: Add wind from METAR
	std::string QNH = "1013";    // TODO: Add QNH from METAR
	std::string botLabel = "WIND: ";
	botLabel.append(windDir);
	botLabel.append("� / ");
	botLabel.append(windSpd);
	botLabel.append("KT ALT: ");
	botLabel.append(QNH);
	dc.TextOutA(radarArea.left + 20, midPoint.y - 10, topLabel.c_str());
	dc.TextOutA(radarArea.left + 20, midPoint.y     , botLabel.c_str());
	// ### TRACK PORTION ###
    // Show Track area
	dc.SelectObject(&yelPen);
	dc.MoveTo(tkArea.right, tkArea.top);
	dc.LineTo(tkArea.left, tkArea.top);
	dc.LineTo(tkArea.right, tkArea.bottom);
    // detach
    dc.Detach();
}

//bool OnCompileCommand(const char* sCommandLine)
//{
//	if (strncmp(sCommandLine, ".gca", 4))
//		return false;
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	return true;
//}