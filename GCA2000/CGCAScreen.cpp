#include "pch.h"
#include "CGCAScreen.h"
// ReSharper disable once CppInconsistentNaming
#define _USE_MATH_DEFINES  // NOLINT(clang-diagnostic-reserved-id-macro)
#include <math.h>  // NOLINT(modernize-deprecated-headers) because VS fails to include MATH DEFINES from cmath
#include <string>

#include "CGCAPlot.h"

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


void CGCAScreen::DrawGlideslopeAxes(CDC* dc, const CRect area, CPen* pen, const unsigned maxRange = 20, const unsigned maxAlt = 16000) const
{
	// Store current dc
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();


	dc->SelectObject(pen);

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


void CGCAScreen::DrawDeviationCross(CDC* dc, const CRect area, CPen* pen) const
{
	// Store current dc
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();
	dc->SelectObject(pen);
	const auto crossMiddle = area.CenterPoint();
	dc->MoveTo(area.left, crossMiddle.y);
	dc->LineTo(area.right, crossMiddle.y);
	dc->MoveTo(crossMiddle.x, area.top);
	dc->LineTo(crossMiddle.x, area.bottom);
	// Restore previous dc
	dc->RestoreDC(sDC);
}


void CGCAScreen::DrawGlideslope(CDC* dc, const CRect area, CPen* pen, const unsigned maxRange, const unsigned maxAlt) const
{
	// Store current dc
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();
	dc->SelectObject(pen);
	const auto slopeRadians = GlideSlope * M_PI / 180;
	const auto slopeMaxAlt = tan(slopeRadians) * maxRange * 6076;
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom - (slopeMaxAlt / maxAlt * area.Height()));
	// Restore previous dc
	dc->RestoreDC(sDC);
}

void CGCAScreen::DrawRadarCursors(CDC* dc, const CRect area, CPen* pen, const unsigned maxRange, const unsigned maxAlt) const
{
	const auto sDC = dc->SaveDC();
	dc->SelectObject(pen);
	const auto upSlopeRadians = (GlideSlope + 6) * M_PI / 180;
	const auto downSlopeRadians = (GlideSlope - 2) * M_PI / 180;
	const auto upMaxAlt = tan(upSlopeRadians) * maxRange * 6076;
	const auto downMaxAlt = tan(downSlopeRadians) * maxRange * 6076;
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom - (upMaxAlt / maxAlt * area.Height()));
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom - (downMaxAlt / maxAlt * area.Height()));
	dc->RestoreDC(sDC);
}


void CGCAScreen::DrawObstacleClearanceHeight(CDC* dc, const CRect area, CPen* pen, const unsigned maxRange, const unsigned maxAlt) const
{
	// Store current dc
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();
	dc->SelectObject(pen);
	const auto slopeRadians = GlideSlope * M_PI / 180;
	// Show Obstacle Clearance Height
	const auto obstacleClearanceHeightDistance = (1 / tan(slopeRadians)) * (ObstacleClearanceHeight / 6076.0);
	const auto obstacleClearanceHeightCenterX = area.left + obstacleClearanceHeightDistance / maxRange * area.Width();
	const auto obstacleClearanceHeightCenterY = area.bottom - static_cast<double>(ObstacleClearanceHeight) / maxAlt * area.Height();
	dc->MoveTo(obstacleClearanceHeightCenterX - 100, obstacleClearanceHeightCenterY);
	dc->LineTo(obstacleClearanceHeightCenterX + 100, obstacleClearanceHeightCenterY);
	// Restore previous dc
	dc->RestoreDC(sDC);
}


void CGCAScreen::DrawGlideslopeRunway(CDC* dc, const CRect area, CPen* pen)
{
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();
	dc->SelectObject(pen);
	dc->MoveTo(0, area.bottom);
	dc->LineTo(area.left + 50, area.bottom);
	dc->RestoreDC(sDC);
}


void CGCAScreen::DrawMiddleText(CDC* dc, const CRect area) const
{
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();
	const auto midPoint = area.CenterPoint();
	CFont arial;
	arial.CreatePointFont(100, _T("Arial"), dc);
	dc->SetTextColor(RGB(170, 29, 93));
	dc->SetTextAlign(TA_LEFT);
	auto* defFont = dc->SelectObject(&arial);
	std::string topLabel = "GS: ";
	topLabel.append(std::to_string(GlideSlope).substr(0, 3));
	topLabel.append("°        RWY: ");
	topLabel.append(std::to_string(Heading));
	std::string windDir = "270"; // TODO: Add wind from METAR
	std::string windSpd = "15";  // TODO: Add wind from METAR
	std::string QNH = "1013";    // TODO: Add QNH from METAR
	std::string botLabel = "WIND: ";
	botLabel.append(windDir);
	botLabel.append("° / ");
	botLabel.append(windSpd);
	botLabel.append("KT ALT: ");
	botLabel.append(QNH);
	dc->TextOutA(area.left + 20, midPoint.y - 15, topLabel.c_str());
	dc->TextOutA(area.left + 20, midPoint.y, botLabel.c_str());
	dc->SelectObject(defFont);
	arial.DeleteObject();
	dc->RestoreDC(sDC);
}


void CGCAScreen::DrawTrackAxes(CDC* dc, const CRect area, CPen* pen, const unsigned maxRange, const int maxTrackError) const
{
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();
	dc->SelectObject(pen);
	dc->SetTextColor(RGB(172, 36, 51));
	const auto midPoint = area.CenterPoint();
	// Draw vertical axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.left, area.top);
	// Draw ticks
	const auto numVTicks = static_cast<int>(maxTrackError / 8000);
	const auto tickHeight = area.Height() / (2*numVTicks);
	for (auto i = 0; i <= numVTicks; i++)
	{
		dc->MoveTo(area.left - 10, area.top + i * tickHeight);
		dc->LineTo(area.left + 10, area.top + i * tickHeight);
		dc->MoveTo(area.left - 10, area.bottom - i * tickHeight);
		dc->LineTo(area.left + 10, area.bottom - i * tickHeight);
		if(i==0)
			continue;
		const auto tickValue = i * maxTrackError / numVTicks;
		auto topLabel = std::to_string(tickValue);
		auto bottomLabel = std::to_string(-tickValue);
		dc->SetTextAlign(TA_RIGHT);
		dc->TextOutA(area.left - 15, midPoint.y - i * tickHeight, topLabel.c_str());
		dc->TextOutA(area.left - 15, midPoint.y + i * tickHeight, bottomLabel.c_str());
	}
	// Draw middle axis
	dc->MoveTo(area.left, midPoint.y);
	dc->LineTo(area.right, midPoint.y);
	// Draw ticks
	const auto numTicks = static_cast<int>(maxRange / 2.5);
	const auto tickWidth = area.Width() / numTicks;
	for (auto i = 0; i <= numTicks; i++)
	{
		dc->MoveTo(area.left + i * tickWidth, midPoint.y - 10);
		dc->LineTo(area.left + i * tickWidth, midPoint.y + 10);
		if (i == 0)
			continue;
		if (i % 2 != 0)
			continue;
		auto label = std::to_string(static_cast<int>(i * 2.5));
		dc->SetTextAlign(TA_TOP);
		dc->TextOutA(area.left + i * tickWidth, midPoint.y + 15, label.c_str());
	}
	dc->RestoreDC(sDC);
}


void CGCAScreen::DrawTrackRunway(CDC* dc, CRect area, CPen* pen)
{
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();
	const auto midPoint = area.CenterPoint();
	dc->SelectObject(pen);
	dc->MoveTo(0, midPoint.y);
	dc->LineTo(area.left + 50, midPoint.y);
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

// ReSharper disable once CppInconsistentNaming
void CGCAScreen::OnRefresh(HDC hDC, const int phase)
{
    // only in first phase
    if (phase != EuroScopePlugIn::REFRESH_PHASE_BEFORE_TAGS)
        return;

    // I am using MFC, so load the DC in to MFC structure
    CDC     dc;
    dc.Attach(hDC);
	// Creating pens
	CPen redPen(0, 2, RGB(172, 36, 51));
	CPen bluPen(0, 2, RGB(19, 97, 232));
	CPen yelPen(0, 2, RGB(223, 212, 36));
	CPen grePen(0, 2, RGB(34, 85, 48));
	CPen grePenDashed(PS_DASH, 1, RGB(34, 85, 48));
	CPen whtPen(0, 2, RGB(255, 255, 255));
	CPen orgPen(0, 2, RGB(255, 127, 80));
    CPen* p_old_pen = dc.SelectObject(&redPen);
	// Get drawing area
	CRect radarArea = GetRadarArea();
    const CRect chatArea = GetChatArea();
	radarArea.bottom = chatArea.top;
	// Add margins
	radarArea.DeflateRect(75, 50);
    const auto midPoint = radarArea.CenterPoint();
	// Get Glideslope, track and cross areas
    auto gsArea = CRect(radarArea.left, radarArea.top, radarArea.right, midPoint.y);
    auto tkArea = CRect(radarArea.left, midPoint.y, radarArea.right, radarArea.bottom);
	gsArea.DeflateRect(0, 20);
	tkArea.DeflateRect(0, 20);
    auto xsArea = CRect(gsArea.left, gsArea.top, gsArea.left + 250, gsArea.top + 250);
	xsArea.DeflateRect(20, 20);

	// Set maximum values
	const unsigned maxRange = 20;
	const unsigned maxAlt = 16000;
	const auto maxTrackError = maxAlt * 2;
	// ### GLIDESLOPE PORTION ###
	DrawGlideslopeAxes(&dc, gsArea, &redPen , maxRange, maxAlt);
	// Draw cross
	DrawDeviationCross(&dc, xsArea, &redPen);
    // Show Glideslope
	DrawGlideslope(&dc, gsArea, &grePen, maxRange, maxAlt);
	DrawRadarCursors(&dc, gsArea, &grePenDashed, maxRange, maxAlt);
	DrawObstacleClearanceHeight(&dc, gsArea, &grePen, maxRange, maxAlt);
	// Draw runway
	DrawGlideslopeRunway(&dc, gsArea, &bluPen);
	// ### MIDDLE TEXT ###
	// Draw info text in the middle
	DrawMiddleText(&dc, radarArea);
	// ### TRACK PORTION ###
	DrawTrackAxes(&dc, tkArea, &redPen, maxRange, maxTrackError);
	DrawTrackRunway(&dc, tkArea, &bluPen);
    for (auto radarTarget = GetPlugIn()->RadarTargetSelectFirst(); radarTarget.IsValid(); radarTarget = GetPlugIn()->RadarTargetSelectNext(radarTarget))
	{
		auto position = radarTarget.GetPosition();
		const auto distance = position.GetPosition().DirectionTo(RunwayPosition);
    	// Skip tracks outside of maximum range
		if (position.GetPosition().DistanceTo(RunwayPosition) > maxRange)
			continue;
		// Get angle to runway
		const auto angle = position.GetPosition().DirectionTo(RunwayPosition);
		const auto angleDiff = angle - Heading;

		// Skip tracks outside of bounds
		if (fabs(angleDiff) > 15)
			continue;

		//CGCAPlot plot {radarTarget, RunwayPosition, Altitude, Heading, GlideSlope };
		auto *plot = new CGCAPlot(radarTarget, RunwayPosition, gsArea, tkArea, xsArea, maxRange, maxAlt, 1000, 500,
		                         Altitude, GlideSlope, Heading);
		plot->DrawPlot(&dc, &whtPen, &yelPen, &orgPen);
	}
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