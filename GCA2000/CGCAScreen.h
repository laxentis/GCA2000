#pragma once
#include "EuroScopePlugIn.h"

// ReSharper disable once CppInconsistentNaming
class CGCAScreen final :
	public EuroScopePlugIn::CRadarScreen
{
	void draw_glideslope_axes(CDC *dc, CRect area, CPen* pen, unsigned max_range, unsigned max_alt) const;
	void draw_deviation_cross(CDC* dc, CRect area, CPen* pen) const;
	void draw_glideslope(CDC* dc, CRect area, CPen* pen, unsigned maxRange, unsigned maxAlt) const;
	void draw_radar_cursors(CDC* dc, const CRect area, CPen* pen, const unsigned max_range, const unsigned max_alt) const;
	void draw_obstacle_clearance_height(CDC* dc, CRect area, CPen* pen, unsigned max_range, unsigned max_alt) const;
	static void draw_glideslope_runway(CDC* dc, CRect area, CPen* pen);
	void draw_middle_text(CDC* dc, CRect area) const;
	void draw_track_axes(CDC* dc, CRect area, CPen* pen, unsigned max_range, int max_track_error) const;
	static void draw_track_runway(CDC* dc, CRect area, CPen* pen);
protected:
	CString description_; // Radar description
	CString lat_, lon_; // Threshold coordinates
	int altitude_; // Threshold altitude
	int obstacle_clearance_height_; // Obstacle Clearance Height
	int heading_; // Final Approach Heading
	double glide_slope_; // Glidepath angle
	EuroScopePlugIn::CPosition    runway_position_; // Runway threshold
	int scope_max_height_; // max height of slope
public:
	CGCAScreen();
	virtual ~CGCAScreen();
	void OnAsrContentLoaded(bool loaded) override;
	void OnAsrContentToBeSaved() override;
	// ReSharper disable once CppInconsistentNaming
	void OnRefresh(const HDC hDC, int phase) override;
	//bool OnCompileCommand(const char* sCommandLine) override;
	void OnAsrContentToBeClosed() override
	{
		delete this;
	}
};

