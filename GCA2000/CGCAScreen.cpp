#include "pch.h"
#include "CGCAScreen.h"
// ReSharper disable once CppInconsistentNaming
#define _USE_MATH_DEFINES  // NOLINT(clang-diagnostic-reserved-id-macro)
#include <math.h>  // NOLINT(modernize-deprecated-headers) because VS fails to include MATH DEFINES from cmath
#include <string>

#include "CGCAPlot.h"

CGCAScreen::CGCAScreen(void)
{
	description_ = "EPKS PAR 29";
	lat_ = "N052.19.36.15";
	lon_ = "E016.58.57.79";
	altitude_ = 274;
	obstacle_clearance_height_ = 280;
	heading_ = 294;
	glide_slope_ = 3.0;
	scope_max_height_ = 3000;
}

CGCAScreen::~CGCAScreen(void)
= default;

void CGCAScreen::OnAsrContentLoaded(bool loaded)
{
	const char* p_value;
	if ((p_value = GetDataFromAsr("Description")) != nullptr)
		description_ = p_value;
	if ((p_value = GetDataFromAsr("Longitude")) != nullptr)
		lon_ = p_value;
	if ((p_value = GetDataFromAsr("Latitude")) != nullptr)
		lat_ = p_value;
	if ((p_value = GetDataFromAsr("Altitude")) != nullptr)
		altitude_ = atoi(p_value);
	if ((p_value = GetDataFromAsr("Heading")) != nullptr)
		heading_ = atoi(p_value);
	if ((p_value = GetDataFromAsr("Slope")) != nullptr)
		glide_slope_ = atof(p_value);
	if (glide_slope_ > 8.0)
		glide_slope_ = 8.0;
	if (glide_slope_ <= 0.0)
		glide_slope_ = 1.0;
	if ((p_value = GetDataFromAsr("OCH")) != nullptr)
		obstacle_clearance_height_ = atoi(p_value);

	runway_position_.LoadFromStrings(lon_, lat_);
	scope_max_height_ = altitude_ + static_cast<int>(20.0 * 6076.0 * sin(glide_slope_ / 180.0 * M_PI));
}


void CGCAScreen::draw_glideslope_axes(CDC* dc, const CRect area, CPen* pen, const unsigned max_range = 20, const unsigned max_alt = 16000) const
{
	// Store current dc
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();


	dc->SelectObject(pen);

	// Draw altitude axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.left, area.top);

	const auto num_v_ticks = static_cast<int>(max_alt / 4000);
	const auto alt_tick = area.Height() / num_v_ticks;
	dc->SetTextColor(RGB(172, 36, 51));
	dc->SetTextAlign(TA_RIGHT);
	for (auto i = 0; i <= num_v_ticks; i++)
	{
		dc->MoveTo(area.left - 10, area.bottom - i * alt_tick);
		dc->LineTo(area.left + 10, area.bottom - i * alt_tick);
		if (i == 0)
			continue;
		auto label = std::to_string(i * 4000);
		dc->TextOutA(area.left - 15, area.bottom - i * alt_tick, label.c_str());
	}
	// Draw track distance axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom);
	const auto num_ticks = static_cast<int>(max_range / 2.5);
	const auto range_tick = area.Width() / num_ticks;
	for (auto i = 0; i <= num_ticks; i++)
	{
		dc->MoveTo(area.left + i * range_tick, area.bottom - 10);
		dc->LineTo(area.left + i * range_tick, area.bottom + 10);
		if (i == 0)
			continue;
		if (i % 2 != 0)
			continue;
		auto label = std::to_string(static_cast<int>(i * 2.5));
		dc->SetTextAlign(TA_TOP);
		dc->TextOutA(area.left + i * range_tick, area.bottom + 15, label.c_str());
	}
	
	// Restore previous dc
	dc->RestoreDC(sDC);
}


void CGCAScreen::draw_deviation_cross(CDC* dc, const CRect area, CPen* pen) const
{
	// Store current dc
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const auto cross_middle = area.CenterPoint();
	dc->MoveTo(area.left, cross_middle.y);
	dc->LineTo(area.right, cross_middle.y);
	dc->MoveTo(cross_middle.x, area.top);
	dc->LineTo(cross_middle.x, area.bottom);
	// Restore previous dc
	dc->RestoreDC(s_dc);
}


void CGCAScreen::draw_glideslope(CDC* dc, const CRect area, CPen* pen, const unsigned maxRange, const unsigned maxAlt) const
{
	// Store current dc
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const auto slope_radians = glide_slope_ * M_PI / 180;
	const auto slope_max_alt = tan(slope_radians) * maxRange * 6076;
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom - (slope_max_alt / maxAlt * area.Height()));
	// Restore previous dc
	dc->RestoreDC(s_dc);
}

void CGCAScreen::draw_radar_cursors(CDC* dc, const CRect area, CPen* pen, const unsigned max_range, const unsigned max_alt) const
{
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const auto up_slope_radians = (glide_slope_ + 6) * M_PI / 180;
	const auto down_slope_radians = (glide_slope_ - 2) * M_PI / 180;
	const auto up_max_alt = tan(up_slope_radians) * max_range * 6076;
	const auto down_max_alt = tan(down_slope_radians) * max_range * 6076;
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom - (up_max_alt / max_alt * area.Height()));
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom - (down_max_alt / max_alt * area.Height()));
	dc->RestoreDC(s_dc);
}


void CGCAScreen::draw_obstacle_clearance_height(CDC* dc, const CRect area, CPen* pen, const unsigned max_range, const unsigned max_alt) const
{
	// Store current dc
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const auto slope_radians = glide_slope_ * M_PI / 180;
	// Show Obstacle Clearance Height
	const auto obstacle_clearance_height_distance = (1 / tan(slope_radians)) * (obstacle_clearance_height_ / 6076.0);
	const auto obstacle_clearance_height_center_x = area.left + obstacle_clearance_height_distance / max_range * area.Width();
	const auto obstacle_clearance_height_center_y = area.bottom - static_cast<double>(obstacle_clearance_height_) / max_alt * area.Height();
	dc->MoveTo(obstacle_clearance_height_center_x - 100, obstacle_clearance_height_center_y);
	dc->LineTo(obstacle_clearance_height_center_x + 100, obstacle_clearance_height_center_y);
	// Restore previous dc
	dc->RestoreDC(s_dc);
}


void CGCAScreen::draw_glideslope_runway(CDC* dc, const CRect area, CPen* pen)
{
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	dc->MoveTo(0, area.bottom);
	dc->LineTo(area.left + 50, area.bottom);
	dc->RestoreDC(s_dc);
}


void CGCAScreen::draw_middle_text(CDC* dc, const CRect area) const
{
	const auto s_dc = dc->SaveDC();
	const auto mid_point = area.CenterPoint();
	CFont arial;
	arial.CreatePointFont(100, _T("Arial"), dc);
	dc->SetTextColor(RGB(170, 29, 93));
	dc->SetTextAlign(TA_LEFT);
	auto* def_font = dc->SelectObject(&arial);
	std::string top_label = "GS: ";
	top_label.append(std::to_string(glide_slope_).substr(0, 3));
	top_label.append("°        RWY: ");
	top_label.append(std::to_string(heading_));
	std::string wind_dir = "270"; // TODO: Add wind from METAR
	std::string wind_spd = "15";  // TODO: Add wind from METAR
	std::string qnh = "1013";    // TODO: Add QNH from METAR
	std::string bot_label = "WIND: ";
	bot_label.append(wind_dir);
	bot_label.append("° / ");
	bot_label.append(wind_spd);
	bot_label.append("KT ALT: ");
	bot_label.append(qnh);
	dc->TextOutA(area.left + 20, mid_point.y - 15, top_label.c_str());
	dc->TextOutA(area.left + 20, mid_point.y, bot_label.c_str());
	dc->SelectObject(def_font);
	arial.DeleteObject();
	dc->RestoreDC(s_dc);
}


void CGCAScreen::draw_track_axes(CDC* dc, const CRect area, CPen* pen, const unsigned max_range, const int max_track_error) const
{
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	dc->SetTextColor(RGB(172, 36, 51));
	const auto mid_point = area.CenterPoint();
	// Draw vertical axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.left, area.top);
	// Draw ticks
	const auto numVTicks = static_cast<int>(max_track_error / 8000);
	const auto tickHeight = area.Height() / (2*numVTicks);
	for (auto i = 0; i <= numVTicks; i++)
	{
		dc->MoveTo(area.left - 10, area.top + i * tickHeight);
		dc->LineTo(area.left + 10, area.top + i * tickHeight);
		dc->MoveTo(area.left - 10, area.bottom - i * tickHeight);
		dc->LineTo(area.left + 10, area.bottom - i * tickHeight);
		if(i==0)
			continue;
		const auto tick_value = i * max_track_error / numVTicks;
		auto top_label = std::to_string(tick_value);
		auto bottom_label = std::to_string(-tick_value);
		dc->SetTextAlign(TA_RIGHT);
		dc->TextOutA(area.left - 15, mid_point.y - i * tickHeight, top_label.c_str());
		dc->TextOutA(area.left - 15, mid_point.y + i * tickHeight, bottom_label.c_str());
	}
	// Draw middle axis
	dc->MoveTo(area.left, mid_point.y);
	dc->LineTo(area.right, mid_point.y);
	// Draw ticks
	const auto num_ticks = static_cast<int>(max_range / 2.5);
	const auto tick_width = area.Width() / num_ticks;
	for (auto i = 0; i <= num_ticks; i++)
	{
		dc->MoveTo(area.left + i * tick_width, mid_point.y - 10);
		dc->LineTo(area.left + i * tick_width, mid_point.y + 10);
		if (i == 0)
			continue;
		if (i % 2 != 0)
			continue;
		auto label = std::to_string(static_cast<int>(i * 2.5));
		dc->SetTextAlign(TA_TOP);
		dc->TextOutA(area.left + i * tick_width, mid_point.y + 15, label.c_str());
	}
	dc->RestoreDC(s_dc);
}


void CGCAScreen::draw_track_runway(CDC* dc, const CRect area, CPen* pen)
{
	const auto s_dc = dc->SaveDC();
	const auto mid_point = area.CenterPoint();
	dc->SelectObject(pen);
	dc->MoveTo(0, mid_point.y);
	dc->LineTo(area.left + 50, mid_point.y);
	dc->RestoreDC(s_dc);
}

void CGCAScreen::OnAsrContentToBeSaved(void)
{
	CString str;
	SaveDataToAsr("Description", "PAR description", description_);
	SaveDataToAsr("Longitude", "RWY threshold longitude", lon_);
	SaveDataToAsr("Latitude", "RWY threshold latitude", lat_);
	str.Format("%d", altitude_);
	SaveDataToAsr("Altitude", "RWY threshold altitude", str);
	str.Format("%d", obstacle_clearance_height_);
	SaveDataToAsr("OCH", "Obstacle Clearance Height", str);
	str.Format("%d", heading_);
	SaveDataToAsr("Heading", "RWY heading", str);
	str.Format("%.1f", glide_slope_);
	SaveDataToAsr("Slope", "Glide slope angle", str);
}

// ReSharper disable once CppInconsistentNaming
void CGCAScreen::OnRefresh(const HDC hDC, const int phase)
{
    // only in first phase
    if (phase != EuroScopePlugIn::REFRESH_PHASE_BEFORE_TAGS)
        return;

    // I am using MFC, so load the DC in to MFC structure
    CDC     dc;
    dc.Attach(hDC);
	// Creating pens
	CPen red_pen(0, 2, RGB(172, 36, 51));
	CPen blu_pen(0, 2, RGB(19, 97, 232));
	CPen yel_pen(0, 2, RGB(223, 212, 36));
	CPen gre_pen(0, 2, RGB(34, 85, 48));
	CPen gre_pen_dashed(PS_DASH, 1, RGB(34, 85, 48));
	CPen wht_pen(0, 2, RGB(255, 255, 255));
	CPen org_pen(0, 2, RGB(255, 127, 80));
    CPen* p_old_pen = dc.SelectObject(&red_pen);
	// Get drawing area
	CRect radar_area = GetRadarArea();
    const CRect chat_area = GetChatArea();
	radar_area.bottom = chat_area.top;
	// Add margins
	radar_area.DeflateRect(75, 50);
    const auto mid_point = radar_area.CenterPoint();
	// Get Glideslope, track and cross areas
    auto gs_area = CRect(radar_area.left, radar_area.top, radar_area.right, mid_point.y);
    auto tk_area = CRect(radar_area.left, mid_point.y, radar_area.right, radar_area.bottom);
	gs_area.DeflateRect(0, 20);
	tk_area.DeflateRect(0, 20);
    auto xs_area = CRect(gs_area.left, gs_area.top, gs_area.left + 250, gs_area.top + 250);
	xs_area.DeflateRect(20, 20);

	// Set maximum values
	constexpr unsigned max_range = 20;
	constexpr unsigned max_alt = 16000;
	constexpr auto max_track_error = max_alt * 2;
	// ### GLIDESLOPE PORTION ###
	draw_glideslope_axes(&dc, gs_area, &red_pen , max_range, max_alt);
	// Draw cross
	draw_deviation_cross(&dc, xs_area, &red_pen);
    // Show Glideslope
	draw_glideslope(&dc, gs_area, &gre_pen, max_range, max_alt);
	draw_radar_cursors(&dc, gs_area, &gre_pen_dashed, max_range, max_alt);
	draw_obstacle_clearance_height(&dc, gs_area, &gre_pen, max_range, max_alt);
	// Draw runway
	draw_glideslope_runway(&dc, gs_area, &blu_pen);
	// ### MIDDLE TEXT ###
	// Draw info text in the middle
	draw_middle_text(&dc, radar_area);
	// ### TRACK PORTION ###
	draw_track_axes(&dc, tk_area, &red_pen, max_range, max_track_error);
	draw_track_runway(&dc, tk_area, &blu_pen);
    for (auto radar_target = GetPlugIn()->RadarTargetSelectFirst(); radar_target.IsValid(); radar_target = GetPlugIn()->RadarTargetSelectNext(radar_target))
	{
		auto position = radar_target.GetPosition();
		const auto distance = position.GetPosition().DirectionTo(runway_position_);
    	// Skip tracks outside of maximum range
		if (position.GetPosition().DistanceTo(runway_position_) > max_range)
			continue;
		// Get angle to runway
		const auto angle = position.GetPosition().DirectionTo(runway_position_);
		const auto angleDiff = angle - heading_;

		// Skip tracks outside of bounds
		if (fabs(angleDiff) > 15)
			continue;

		//CGCAPlot plot {radarTarget, RunwayPosition, Altitude, Heading, GlideSlope };
		const auto *plot = new CGCAPlot(radar_target, runway_position_, gs_area, tk_area, xs_area, max_range, max_alt, 1000, 500,
		                                altitude_, glide_slope_, heading_);
		plot->draw_plot(&dc, &wht_pen, &yel_pen, &org_pen);
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