#pragma once
#include <EuroScopePlugIn.h>
#include "CGCALabel.h"
// ReSharper disable once CppInconsistentNaming
class CGCAPlot
{
public:
	CGCAPlot(const CGCAPlot& other) = delete;
	CGCAPlot(CGCAPlot&& other) noexcept = delete;
	CGCAPlot& operator=(const CGCAPlot& other) = delete;
	CGCAPlot& operator=(CGCAPlot&& other) noexcept = delete;
	void draw_plot(CDC* dc, CPen* plot_pen, CPen* first_error_pen, CPen* second_error_pen) const;
private:
	int track_distance_to_x() const;
	int track_error_to_track_y() const;
	double get_max_track_error_at_distance() const;
public:

	CGCAPlot(const EuroScopePlugIn::CRadarTarget& target, const EuroScopePlugIn::CPosition& runway_threshold,
		const CRect& glideslope_area, const CRect& track_area, const CRect& deviation_area, unsigned max_range,
		unsigned max_alt, int max_track_deviation, int max_glideslope_deviation, int threshold_altitude, double glideslope, int heading);
	void plot_track(CDC* dc, CPen* pen) const;
	~CGCAPlot();

protected:
	EuroScopePlugIn::CRadarTarget target_;
	EuroScopePlugIn::CPosition runway_threshold_;
	CRect glideslope_area_;
	CRect track_area_;
	CRect deviation_area_;
	unsigned max_range_ = 20;
	unsigned max_alt_ = 16000;
	int max_track_deviation_ = 1000;
	int max_glideslope_deviation_ = 500;
	int threshold_altitude_ = 274;
	double glide_slope_ = 3.0;
	int heading_ = 294;

	CGCALabel* label_;
};

