#include "pch.h"
#include "CGCAScreen.h"
// ReSharper disable once CppInconsistentNaming
#define _USE_MATH_DEFINES  // NOLINT(clang-diagnostic-reserved-id-macro)
#include <math.h>  // NOLINT(modernize-deprecated-headers) because VS fails to include MATH DEFINES from cmath
#include <string>
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "CGCAPlot.h"

CGCAScreen::CGCAScreen(void)
{
	description_ = "ICAO PAR 00";
	lat_ = "N000.00.00.00";
	lon_ = "E000.00.00.00";
	altitude_ = 0;
	obstacle_clearance_height_ = 0;
	heading_ = 0;
	glide_slope_ = 3.0;
	scope_max_height_ = 6000;
	max_range_ = 20;
	max_track_error_ = 3;
	plot_map_ = {};
}

CGCAScreen::~CGCAScreen(void)
= default;

void CGCAScreen::OnAsrContentLoaded(bool loaded)
{
	const char* p_value;
	if ((p_value = GetDataFromAsr("Description")) != nullptr)
		this->description_ = p_value;
	if ((p_value = GetDataFromAsr("Longitude")) != nullptr)
		this->lon_ = p_value;
	if ((p_value = GetDataFromAsr("Latitude")) != nullptr)
		this->lat_ = p_value;
	if ((p_value = GetDataFromAsr("Altitude")) != nullptr)
		this->altitude_ = atof(p_value);
	if ((p_value = GetDataFromAsr("Heading")) != nullptr)
		this->heading_ = atof(p_value);
	if ((p_value = GetDataFromAsr("Slope")) != nullptr)
		this->glide_slope_ = atof(p_value);
	if (this->glide_slope_ > 8.0)
		this->glide_slope_ = 8.0;
	if (this->glide_slope_ <= 0.0)
		this->glide_slope_ = 1.0;
	if ((p_value = GetDataFromAsr("OCH")) != nullptr)
		this->obstacle_clearance_height_ = atof(p_value);
	if ((p_value = GetDataFromAsr("Maximum range")) != nullptr)
		this->max_range_ = atof(p_value);
	if ((p_value = GetDataFromAsr("Maximum height")) != nullptr)
		this->scope_max_height_ = atof(p_value);
	if ((p_value = GetDataFromAsr("Maximum track deviation")) != nullptr)
		this->max_track_error_ = atof(p_value);

	runway_position_.LoadFromStrings(lon_, lat_);
}


void CGCAScreen::draw_glideslope_axes(CDC* dc, const CRect area, CPen* pen, const double max_range = 20, const double max_alt = 6000) const
{
	// Store current dc
	// ReSharper disable once CppInconsistentNaming
	const auto sDC = dc->SaveDC();


	dc->SelectObject(pen);

	// Draw altitude axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.left, area.top);

	const double alt_tick_height = 1000.0;

	const double num_v_ticks = static_cast<double>(max_alt / alt_tick_height);
	const double alt_tick = area.Height() / num_v_ticks;
	dc->SetTextColor(RGB(172, 36, 51));
	dc->SetTextAlign(TA_RIGHT);
	for (auto i = 0; i <= num_v_ticks; i++)
	{
		dc->MoveTo(area.left - 10, area.bottom - i * alt_tick);
		dc->LineTo(area.left + 10, area.bottom - i * alt_tick);

		if (i == 0)
			continue;
		auto label = std::to_string(static_cast<int>(i * alt_tick_height));
		dc->TextOutA(area.left - 15, area.bottom - i * alt_tick, label.c_str());
	}
	// Draw track distance axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.right, area.bottom);
	const double num_ticks = static_cast<double>(max_range / 1) * 2; // max_range / 1: 1NM between ticks | *2 to include half-ticks every 0.5nm
	const double range_tick = area.Width() / num_ticks;
	for (auto i = 0; i <= num_ticks; i++)
	{
		if (i % 2 != 0)
		{
			// Half-tick
			dc->MoveTo(area.left + i * range_tick, area.bottom - 5);
			dc->LineTo(area.left + i * range_tick, area.bottom + 5);
			continue;
		}
		dc->MoveTo(area.left + i * range_tick, area.bottom - 10);
		dc->LineTo(area.left + i * range_tick, area.bottom + 10);
		if (i == 0 || i == 2) //No label for the threshold tick and the 1NM tick
			continue;
		if (i % 2 != 0) // Label every 2 half-ticks = 1NM
			continue;
		auto label = std::to_string(static_cast<int>(i * 1 / 2)); // i * 1: 1NM between ticks | / 2 to convert half-ticks to NM
		dc->SetTextAlign(TA_CENTER);
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


void CGCAScreen::draw_glideslope(CDC* dc, const CRect area, CPen* pen, const double max_range, const double max_alt) const
{
	// Store current dc
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const double slope_radians = glide_slope_ * M_PI / 180.0;
	const double x_end_gs_nm = (max_alt - 50) / 6076.11 / tan(slope_radians); //6076.11: ft => NM and 50ft above threshold
	const double x_end_gs = area.left + x_end_gs_nm / max_range * area.Width();
	dc->MoveTo(area.left, area.bottom - 50/max_alt * area.Height()); //50ft above threshold on usual approach
	dc->LineTo(x_end_gs, area.top);

	////Draw ILS intercept test bars
	//dc->MoveTo(area.left - 20, area.bottom - 4663 / max_alt * area.Height());
	//dc->LineTo(area.right + 20, area.bottom - 4663 / max_alt * area.Height());
	//dc->MoveTo(area.left - 20, area.bottom - 5000 / max_alt * area.Height());
	//dc->LineTo(area.right + 20, area.bottom - 5000 / max_alt * area.Height());
	//dc->MoveTo(area.left + 14.5 / max_range * area.Width(), area.bottom);
	//dc->LineTo(area.left + 14.5 / max_range * area.Width(), area.top);
	// Restore previous dc
	dc->RestoreDC(s_dc);
}

void CGCAScreen::draw_gs_radar_cursors(CDC* dc, const CRect area, CPen* pen, const double max_range, const double max_alt) const
{
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const auto up_slope_radians = (glide_slope_ + 2) * M_PI / 180.0;
	const auto down_slope_radians = (glide_slope_ - 2) * M_PI / 180.0;
	const auto up_max_alt = tan(up_slope_radians) * max_range * 6076.11;
	const auto down_max_alt = tan(down_slope_radians) * max_range * 6076.11;
	dc->MoveTo(area.left, area.bottom - 50 / this->scope_max_height_ * area.Height());  //50ft above threshold on usual approach
	dc->LineTo(area.right, area.bottom - (up_max_alt / max_alt * area.Height()));
	dc->MoveTo(area.left, area.bottom - 50 / this->scope_max_height_ * area.Height());  //50ft above threshold on usual approach
	dc->LineTo(area.right, area.bottom - (down_max_alt / max_alt * area.Height()));
	dc->RestoreDC(s_dc);
}

void CGCAScreen::draw_track_radar_cursors(CDC* dc, const CRect area, CPen* pen, const double max_range, const double max_track_error) const
{
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const auto max_dev_radians = 15.0 * M_PI / 180.0;
	const auto track_max_dev = max_track_error / 2 / tan(max_dev_radians);
	dc->MoveTo(area.left, area.CenterPoint().y);
	dc->LineTo(area.left + track_max_dev * area.Width() / max_range, area.CenterPoint().y - area.Height() / 2);
	dc->MoveTo(area.left, area.CenterPoint().y);  //50ft above threshold on usual approach
	dc->LineTo(area.left + track_max_dev * area.Width() / max_range, area.CenterPoint().y + area.Height() / 2);
	dc->RestoreDC(s_dc);
}


void CGCAScreen::draw_obstacle_clearance_height(CDC* dc, const CRect area, CPen* pen, const double max_range, const double max_alt) const
{
	// Store current dc
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const auto slope_radians = glide_slope_ * M_PI / 180.0;
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
	dc->LineTo(area.left, area.bottom);
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
	top_label.append("°     OCH: ");
	top_label.append(std::to_string(static_cast<int>(obstacle_clearance_height_))); // GS: 3.0°  OCH: 250
	std::string wind_dir = "---"; // TODO: Add wind from METAR						// QNH1013   WIND: 270°/15G25KT
	std::string wind_spd = "--";  // TODO: Add wind from METAR
	std::string qnh = "----";    // TODO: Add QNH from METAR
	std::string bot_label = "QNH";
	bot_label.append(qnh); // HIDDEN BECAUSE OF LACK OF METAR SUPPORT
	bot_label.append("      WIND: ");
	bot_label.append(wind_dir);
	bot_label.append("° / ");
	bot_label.append(wind_spd);
	bot_label.append("KT");
	
	dc->TextOutA(area.left + 17, mid_point.y - 7, top_label.c_str());
	//dc->TextOutA(area.left + 17, mid_point.y + 8, bot_label.c_str());
	dc->SelectObject(def_font);
	arial.DeleteObject();
	dc->RestoreDC(s_dc);
}


void CGCAScreen::draw_track_axes(CDC* dc, const CRect area, CPen* pen, const double max_range, const double max_track_error) const
{
	const auto s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	dc->SetTextColor(RGB(172, 36, 51));
	const auto mid_point = area.CenterPoint();
	// Draw vertical axis
	dc->MoveTo(area.left, area.bottom);
	dc->LineTo(area.left, area.top);
	// Draw ticks
	const double numVTicks = static_cast<double>(max_track_error); // max_track_error * 2: to include half-ticks every 0.5nm
	const double tickHeight = area.Height() / (2*numVTicks);
	for (auto i = 0; i <= numVTicks; i++)
	{
		dc->MoveTo(area.left - 10, area.top + i * tickHeight);
		dc->LineTo(area.left + 10, area.top + i * tickHeight);
		dc->MoveTo(area.left - 10, area.bottom - i * tickHeight);
		dc->LineTo(area.left + 10, area.bottom - i * tickHeight);
		if(i==0)
			continue;
		const double tick_value = i * max_track_error / numVTicks;
		auto top_label = std::to_string(tick_value);
		auto bottom_label = std::to_string(-tick_value);

		if (tick_value == static_cast<int>(tick_value)) {
			top_label = top_label.substr(0, top_label.find('.')); //Show as integer
			bottom_label = bottom_label.substr(0, bottom_label.find('.')); //Show as integer
		}
		else {
			top_label = top_label.substr(0, top_label.find('.') + 2); //Limit to 1 decimal place
			bottom_label = bottom_label.substr(0, bottom_label.find('.') + 2); // Limit to 1 decimal place
		}
		
		dc->SetTextAlign(TA_TOP);
		dc->SetTextAlign(TA_RIGHT);
		dc->TextOutA(area.left - 15, mid_point.y - i * tickHeight - 6, top_label.c_str()); // -6 to be vertically centered
		dc->TextOutA(area.left - 15, mid_point.y + i * tickHeight - 6, bottom_label.c_str());
	}
	// Draw middle horizontal axis
	dc->MoveTo(area.left, mid_point.y);
	dc->LineTo(area.right, mid_point.y);
	// Draw ticks
	const double num_ticks = static_cast<int>(max_range / 1) * 2; // max_range / 1: 1NM between ticks | *2 to include half-ticks every 0.5nm
	const double tick_width = area.Width() / num_ticks ;
	for (auto i = 0; i <= num_ticks; i++)
	{
		if (i % 2 != 0)
		{
			// Half-tick
			dc->MoveTo(area.left + i * tick_width, mid_point.y - 5);
			dc->LineTo(area.left + i * tick_width, mid_point.y + 5);
			continue;
		}

		dc->MoveTo(area.left + i * tick_width, mid_point.y - 10);
		dc->LineTo(area.left + i * tick_width, mid_point.y + 10);
		if (i == 0)
			continue;
		if (i % 2 != 0)
			continue;
		auto label = std::to_string(static_cast<int>(i * 1 / 2));  // i * 1: 1NM between ticks | / 2 to convert half-ticks to NM
		dc->SetTextAlign(TA_CENTER);
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
	dc->LineTo(area.left, mid_point.y);
	dc->RestoreDC(s_dc);
}

void CGCAScreen::draw_history_trails(CDC* dc, CPen* plot_pen, CPen* first_error_pen, CPen* second_error_pen) {

	
	for (auto it = plot_map_.begin(); it != plot_map_.end(); /* no increment here */) {

		const auto& coords = it->first;
		int val = it->second;

		if (val/5 <= 4) { //5s position update delay | 4 trails
			
			
			double x = std::get<0>(coords);
			double y_track = std::get<1>(coords);
			double y_gs = std::get<2>(coords);

			auto const s_dc = dc->SaveDC();
			dc->SelectObject(plot_pen);

			// Draw on track display ----------------------
			//dc->Arc(x - 10.0, y_track - 10.0, x + 10.0, y_track + 10.0, x, y_track, x, y_track);
			dc->MoveTo(x - 7.0, y_track);
			dc->LineTo(x + 6.0, y_track);
			dc->MoveTo(x, y_track - 6.0); //size of trail reduction with val
			dc->LineTo(x, y_track + 6.0);

			// Draw on glideslope display ---------------
			//dc->Arc(x - 10.0, y_gs - 10.0, x + 10.0, y_gs + 10.0, x, y_gs, x, y_gs);
			dc->MoveTo(x - 7.0, y_gs);
			dc->LineTo(x + 6.0, y_gs);
			dc->MoveTo(x, y_gs - 6.0);
			dc->LineTo(x, y_gs + 6.0);

			dc->RestoreDC(s_dc);

			//Increment the value for next time
			it->second = val + 1;
			// Move to the next element
			++it;
		}
		else {
			// Safe Erase: erase() returns the iterator to the NEXT valid element
			it = plot_map_.erase(it);
		}
	}
}

void CGCAScreen::OnAsrContentToBeSaved(void)
{
	CString str;
	SaveDataToAsr("Description", "PAR description", description_);
	SaveDataToAsr("Longitude", "RWY threshold longitude", lon_);
	SaveDataToAsr("Latitude", "RWY threshold latitude", lat_);
	str.Format("%.0f", altitude_);
	SaveDataToAsr("Altitude", "RWY threshold altitude", str);
	str.Format("%.0f", obstacle_clearance_height_);
	SaveDataToAsr("OCH", "Obstacle Clearance Height", str);
	str.Format("%.1f", heading_);
	SaveDataToAsr("Heading", "RWY heading", str);
	str.Format("%.1f", glide_slope_);
	SaveDataToAsr("Slope", "Glide slope angle", str);
	str.Format("%.1f", max_range_);
	SaveDataToAsr("Maximum range", "Maximum range", str);
	str.Format("%.0f", scope_max_height_);
	SaveDataToAsr("Maximum height", "Maximum height", str);
	str.Format("%.1f", max_track_error_);
	SaveDataToAsr("Maximum track deviation", "Maximum track deviation", str);
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
	CPen blk_pen(0, 2, RGB(0, 0, 0));
    CPen* p_old_pen = dc.SelectObject(&red_pen);
	// Get drawing area
	CRect full_radar_area = GetRadarArea();
    const CRect chat_area = GetChatArea();
	// Add margins
	CRect radar_area = GetRadarArea();
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
	double max_range = this->max_range_;
	double max_alt = this->scope_max_height_;
	double max_track_error = this->max_track_error_;
	//Draw black rectangle background for radar area
	dc.SelectObject(&blk_pen);
	dc.SelectStockObject(BLACK_BRUSH);
	dc.Rectangle(full_radar_area);
	dc.SelectStockObject(NULL_BRUSH);

	// ### GLIDESLOPE PORTION ###
	draw_glideslope_axes(&dc, gs_area, &red_pen , max_range, max_alt);
	// Draw cross
	draw_deviation_cross(&dc, xs_area, &red_pen);
    // Show Glideslope
	draw_glideslope(&dc, gs_area, &gre_pen, max_range, max_alt);
	draw_gs_radar_cursors(&dc, gs_area, &gre_pen_dashed, max_range, max_alt);
	draw_track_radar_cursors(&dc, tk_area, &gre_pen_dashed, max_range, max_track_error);
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
		auto speed = radar_target.GetGS();
		const auto distance = position.GetPosition().DistanceTo(runway_position_);
		
    	// Skip tracks outside of maximum range
		if (distance > max_range || speed < 45)
			continue;
		
		// Get angle to runway
		const auto angle = position.GetPosition().DirectionTo(runway_position_);
		const auto angle_diff = angle - heading_;

		
		// Skip tracks outside of bounds
		if (fabs(angle_diff) > 45)
			continue;
		
		//CGCAPlot plot {radarTarget, RunwayPosition, Altitude, Heading, GlideSlope };
		const auto *plot = new CGCAPlot(radar_target, runway_position_, gs_area, tk_area, xs_area, max_range, max_alt, max_track_error, 500, //500ft of max dev for X deviation display
		                                altitude_, glide_slope_, heading_);
		plot->draw_plot(&dc, &wht_pen, &yel_pen, &org_pen, this);
		
	}
	draw_history_trails(&dc, &wht_pen, &yel_pen, &org_pen);
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