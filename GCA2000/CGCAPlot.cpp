#include "pch.h"
#include "CGCAPlot.h"
#define _USE_MATH_DEFINES
#include <math.h>

CGCAPlot::CGCAPlot(const EuroScopePlugIn::CRadarTarget& target, const EuroScopePlugIn::CPosition& runway_threshold,
                   const CRect& glideslope_area, const CRect& track_area, const CRect& deviation_area, const unsigned max_range = 20,
                   const unsigned max_alt = 16000, const int max_track_deviation = 1000, const int max_glideslope_deviation = 500,
                   const int threshold_altitude = 274, const double glideslope = 3.0, const int heading = 294)
{
	this->target_ = target;
	this->runway_threshold_ = runway_threshold;
	this->glideslope_area_ = glideslope_area;
	this->track_area_ = track_area;
	this->deviation_area_ = deviation_area;
	this->max_range_ = max_range;
	this->max_alt_ = max_alt;
	this->max_track_deviation_ = max_track_deviation;
	this->max_glideslope_deviation_ = max_glideslope_deviation;

	this->label_ = new CGCALabel(target, runway_threshold, threshold_altitude, heading, glideslope);
}


CGCAPlot::~CGCAPlot()
{
	delete this->label_;
}

int CGCAPlot::track_distance_to_x() const
{
	const auto distance = this->label_->get_track_distance();
	return this->track_area_.left + this->max_range_ / this->track_area_.Width() * distance;
}


double CGCAPlot::get_max_track_error_at_distance() const
{
	const auto distance = this->label_->get_track_distance();
	constexpr auto track_angle = 15.0 * M_PI / 180;
	const auto straight_distance = this->runway_threshold_.DistanceTo(this->target_.GetPosition().GetPosition());
	return sin(track_angle) * straight_distance;
}

int CGCAPlot::track_error_to_track_y() const
{
	const auto track_error = this->label_->get_track_deviation();
	const auto middle = this->track_area_.CenterPoint().y;
	auto deflection = get_max_track_error_at_distance() / this->track_area_.Height() * track_error;
	if (track_error > 0)
		deflection = -deflection;
	return middle + deflection;
}

void CGCAPlot::plot_track(CDC* dc, CPen* pen) const
{
	auto const s_dc = dc->SaveDC();
	dc->SelectObject(pen);
	const auto x = this->track_distance_to_x();
	const auto y = this->track_error_to_track_y();
	dc->Ellipse(x - 5, y - 5, x + 5, y + 5);
	dc->RestoreDC(s_dc);
}

void CGCAPlot::draw_plot(CDC* dc, CPen* plot_pen, CPen* first_error_pen, CPen* second_error_pen) const
{
	auto const s_dc = dc->SaveDC();
	plot_track(dc, plot_pen);
	dc->RestoreDC(s_dc);
}
