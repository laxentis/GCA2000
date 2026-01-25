#pragma once
#include "EuroScopePlugIn.h"
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

// ReSharper disable once CppInconsistentNaming
class CGCAScreen final :
	public EuroScopePlugIn::CRadarScreen
{
	void draw_glideslope_axes(CDC *dc, CRect area, CPen* pen, double max_range, double max_alt) const;
	void draw_deviation_cross(CDC* dc, CRect area, CPen* pen) const;
	void draw_glideslope(CDC* dc, CRect area, CPen* pen, double max_range, double max_alt) const;
	void draw_gs_radar_cursors(CDC* dc, const CRect area, CPen* pen, const double max_range, const double max_alt) const;
	void draw_track_radar_cursors(CDC* dc, const CRect area, CPen* pen, const double max_range, const double max_track_error) const;
	void draw_obstacle_clearance_height(CDC* dc, CRect area, CPen* pen, double max_range, double max_alt) const;
	static void draw_glideslope_runway(CDC* dc, CRect area, CPen* pen);
	void draw_middle_text(CDC* dc, const CRect area) const;
	void draw_track_axes(CDC* dc, CRect area, CPen* pen, double max_range, double max_track_error) const;
	static void draw_track_runway(CDC* dc, CRect area, CPen* pen);
	
public:
	CString description_; // Radar description
	CString lat_, lon_; // Threshold coordinates
	double altitude_; // Threshold altitude
	double obstacle_clearance_height_; // Obstacle Clearance Height
	double heading_; // Final Approach Heading
	double glide_slope_; // Glidepath angle (degrees)
	EuroScopePlugIn::CPosition    runway_position_; // Runway threshold
	double scope_max_height_; // max height of slope (ft)
	double max_range_; //max range of radar screen (NM)
	double max_track_error_; //max lateral dev from track (NM)
	std::map<std::tuple<double, double, double>, int> plot_map_; //map of plotted targets to store for history trails
public:
	CGCAScreen();
	virtual ~CGCAScreen();
	void OnAsrContentLoaded(bool loaded) override;
	void draw_history_trails(CDC* dc, CPen* plot_pen, CPen* first_error_pen, CPen* second_error_pen);
	void OnAsrContentToBeSaved() override;
	// ReSharper disable once CppInconsistentNaming
	void OnRefresh(const HDC hDC, int phase) override;
	//bool OnCompileCommand(const char* sCommandLine) override;
	void OnAsrContentToBeClosed() override
	{
		delete this;
	}
};

