#pragma once
#include <EuroScopePlugIn.h>
#include "CGCALabel.h"
#include "CGCAScreen.h"
#include <map>

// ReSharper disable once CppInconsistentNaming
class CGCAPlot
{
public:
	CGCAPlot(const CGCAPlot& other) = delete;
	CGCAPlot(CGCAPlot&& other) noexcept = delete;
	CGCAPlot& operator=(const CGCAPlot& other) = delete;
	CGCAPlot& operator=(CGCAPlot&& other) noexcept = delete;
	void draw_plot(CDC* dc, CPen* plot_pen, CPen* first_error_pen, CPen* second_error_pen, CGCAScreen* screen) const;
private:
	double track_distance_to_x() const;
	double track_error_to_track_y() const;
	double get_max_track_error_at_distance() const;
	double gs_error_to_gs_y() const;
	double get_vertical_dev_angle() const;
public:

	CGCAPlot(const EuroScopePlugIn::CRadarTarget& target, const EuroScopePlugIn::CPosition& runway_threshold,
		const CRect& glideslope_area, const CRect& track_area, const CRect& deviation_area, double max_range,
		double max_alt, double max_track_deviation, double max_glideslope_deviation, double threshold_altitude, double glideslope, double heading);
	~CGCAPlot();

public:
	EuroScopePlugIn::CRadarTarget target_;
	EuroScopePlugIn::CPosition runway_threshold_;
	CRect glideslope_area_;
	CRect track_area_;
	CRect deviation_area_;
	double max_range_ = 20;
	double max_alt_ = 6000.0;
	double max_track_deviation_ = 3.0;
	double max_glideslope_deviation_ = 500;
	double threshold_altitude_;
	double glide_slope_ = 3.0;
	double heading_;

	CGCALabel* label_;
};

