#include "pch.h"
#include "CGCAScreen.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

CGCAScreen::CGCAScreen(void)
{
	m_Description = "EPKS PAR 29";
	m_Lat = "N052.19.36.15";
	m_Lon = "E016.58.57.79";
	m_Altitude = 274;
	m_OCH = 280;
	m_Heading = 294;
	m_Slope = 3.0;
}

CGCAScreen::~CGCAScreen(void)
{
}

void CGCAScreen::OnAsrContentLoaded(bool Loaded)
{
	const char* p_value;
	if ((p_value = GetDataFromAsr("Description")) != NULL)
		m_Description = p_value;
	if ((p_value = GetDataFromAsr("Longitude")) != NULL)
		m_Lon = p_value;
	if ((p_value = GetDataFromAsr("Latitude")) != NULL)
		m_Lat = p_value;
	if ((p_value = GetDataFromAsr("Altitude")) != NULL)
		m_Altitude = atoi(p_value);
	if ((p_value = GetDataFromAsr("Heading")) != NULL)
		m_Heading = atoi(p_value);
	if ((p_value = GetDataFromAsr("Slope")) != NULL)
		m_Slope = atof(p_value);
	if (m_Slope > 8.0)
		m_Slope = 8.0;
	if (m_Slope <= 0.0)
		m_Slope = 1.0;
	if ((p_value = GetDataFromAsr("OCH")) != NULL)
		m_OCH = atoi(p_value);

	m_RwyPos.LoadFromStrings(m_Lon, m_Lat);
	m_maxH = m_Altitude + (int)(20.0 * 6076.0 * sin(m_Slope / 180.0 * M_PI));
}

void CGCAScreen::OnAsrContentToBeSaved(void)
{
	CString str;
	SaveDataToAsr("Description", "PAR description", m_Description);
	SaveDataToAsr("Longitude", "RWY treshold longitude", m_Lon);
	SaveDataToAsr("Latitude", "RWY treshold latitude", m_Lat);
	str.Format("%d", m_Altitude);
	SaveDataToAsr("Altitude", "RWY treshold altitude", str);
	str.Format("%d", m_OCH);
	SaveDataToAsr("OCH", "Obstacle Clearance Height", str);
	str.Format("%d", m_Heading);
	SaveDataToAsr("Heading", "RWY heading", str);
	str.Format("%.1f", m_Slope);
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
	CPoint midPoint = radarArea.CenterPoint();
	// Get Glideslope, track and cross areas
	CRect gsArea = CRect(radarArea.left, radarArea.top, radarArea.right, midPoint.y);
	CRect tkArea = CRect(radarArea.left, midPoint.y, radarArea.right, radarArea.bottom);
	gsArea.DeflateRect(0, 20);
	tkArea.DeflateRect(0, 20);
	CRect xsArea = CRect(gsArea.left, gsArea.top, gsArea.left + 250, gsArea.top + 250);
	xsArea.DeflateRect(20, 20);
	// ### GLIDESLOPE PORTION ###
	// Draw cross
	dc.SelectObject(&redPen);
	CPoint crossMiddle = xsArea.CenterPoint();
	dc.MoveTo(xsArea.left, crossMiddle.y);
	dc.LineTo(xsArea.right, crossMiddle.y);
	dc.MoveTo(crossMiddle.x, xsArea.top);
	dc.LineTo(crossMiddle.x, xsArea.bottom);
	// Draw altitude axis
	dc.MoveTo(gsArea.left, gsArea.bottom);
	dc.LineTo(gsArea.left, gsArea.top);
	const unsigned int maxAlt = 16000;
	int numVTicks = int(maxAlt / 4000);
	int altTick = gsArea.Height() / numVTicks;
	dc.SetTextColor(RGB(172, 36, 51));
	dc.SetTextAlign(TA_RIGHT);
	for (int i = 0; i <= numVTicks; i++)
	{
		dc.MoveTo(gsArea.left - 10, gsArea.bottom - i * altTick);
		dc.LineTo(gsArea.left + 10, gsArea.bottom - i * altTick);
		if (i == 0)
			continue;
		std::string label = std::to_string(i * 4000);
		dc.TextOutA(gsArea.left - 15, gsArea.bottom - i * altTick, label.c_str());
	}
	// Draw track distance axis
	dc.MoveTo(gsArea.left, gsArea.bottom);
	dc.LineTo(gsArea.right, gsArea.bottom);
	const unsigned int maxRange = 20;
	int numTicks = int(maxRange / 2.5);
	int rangeTick = gsArea.Width() / numTicks;
	for (int i = 0; i <= numTicks; i++)
	{
		dc.MoveTo(gsArea.left + i * rangeTick, gsArea.bottom - 10);
		dc.LineTo(gsArea.left + i * rangeTick, gsArea.bottom + 10);
		if (i == 0)
			continue;
		if (i % 2 != 0)
			continue;
		std::string label = std::to_string(int(i * 2.5));
		dc.SetTextAlign(TA_TOP);
		dc.TextOutA(gsArea.left + i * rangeTick, gsArea.bottom + 15, label.c_str());
	}
	// Show Glideslope
	float slopeMaxAlt = tan(m_Slope) * maxRange * 6076;
	dc.SelectObject(&grePen);
	dc.MoveTo(gsArea.left, gsArea.bottom);
	dc.LineTo(gsArea.right, gsArea.bottom - (slopeMaxAlt/maxAlt * gsArea.Height()));
	// Draw runway
	dc.SelectObject(&bluPen);
	dc.MoveTo(0, gsArea.bottom);
	dc.LineTo(gsArea.left + 50, gsArea.bottom);
	// ### MIDDLE TEXT ###
	// Draw info text in the middle
	dc.SetTextColor(RGB(170, 29, 93));
	dc.SetTextAlign(TA_LEFT);
	std::string topLabel = "  GS: ";
	topLabel.append(std::to_string(m_Slope).substr(0,3));
	topLabel.append("°        RWY: ");
	topLabel.append(std::to_string(m_Heading));
	std::string windDir = "270"; // TODO: Add wind from METAR
	std::string windSpd = "15";  // TODO: Add wind from METAR
	std::string QNH = "1013";    // TODO: Add QNH from METAR
	std::string botLabel = "WIND: ";
	botLabel.append(windDir);
	botLabel.append("° / ");
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