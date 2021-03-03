#include "pch.h"
#include "CGCAScreen.h"
#define _USE_MATH_DEFINES
#include <math.h>

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
	if (Phase != EuroScopePlugIn::REFRESH_PHASE_BEFORE_TAGS)
		return;
	CDC dc;
	dc.Attach(hDC);

	//pens
	CPen redNPen(0, 1, RGB(213, 17, 27));
	CPen redWPen(0, 5, RGB(213, 17, 27));
	CPen bluPen(0, 1, RGB(89, 99, 210));
	CPen yelPen(0, 1, RGB(244, 233, 0));
	CPen whtPen(0, 1, RGB(255, 255, 255));
	CPen prpPen(0, 1, RGB(177, 99, 175));
	CPen grnPen(0, 1, RGB(69, 220, 115));
	CPen* p_old_pen;

	CRect   rect1 = GetRadarArea();
	CRect   rect2 = GetChatArea();
	rect1.bottom = rect2.top;
	delete rect2;
	int middleY = rect1.CenterPoint().y;
	int lowerMiddleY = (middleY + rect1.bottom) / 2;
	int width = rect1.Width() - 10;
	int height = middleY - 20 - (rect1.top - 5);

	p_old_pen = dc.SelectObject(&redPen);
	// Draw track axis
	dc.MoveTo(rect1.left + 5, middleY);
	dc.LineTo(rect1.left + 5, rect1.top - 20);
}

bool OnCompileCommand(const char* sCommandLine)
{
	if (strncmp(sCommandLine, ".gca", 4))
		return false;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return true;
}