#include "pch.h"
#include "CGCAPlot.h"
#define _USE_MATH_DEFINES
#include <math.h>

CGCAPlot::CGCAPlot(const EuroScopePlugIn::CRadarTarget& target, const EuroScopePlugIn::CPosition& runway_threshold,
                   const CRect& glideslope_area, const CRect& track_area, const CRect& deviation_area, const unsigned max_range = 20,
                   const unsigned max_alt = 16000, const int max_track_deviation = 1000, const int max_glideslope_deviation = 500,
                   const int threshold_altitude = 274, const double glideslope = 3.0, const int heading = 294)
{
	this->Target = target;
	this->RunwayThreshold = runway_threshold;
	this->GlideslopeArea = glideslope_area;
	this->TrackArea = track_area;
	this->DeviationArea = deviation_area;
	this->MaxRange = max_range;
	this->MaxAlt = max_alt;
	this->MaxTrackDeviation = max_track_deviation;
	this->MaxGlideslopeDeviation = max_glideslope_deviation;

	this->Label = new CGCALabel(target, runway_threshold, threshold_altitude, heading, glideslope);
}


CGCAPlot::~CGCAPlot()
{
	delete this->Label;
}

int CGCAPlot::track_distance_to_x() const
{
	const auto distance = this->Label->get_track_distance();
	return this->TrackArea.left + this->MaxRange / this->TrackArea.Width() * distance;
}


double CGCAPlot::get_max_track_error_at_distance() const
{
	const auto distance = this->Label->get_track_distance();
	constexpr auto track_angle = 15.0 * M_PI / 180;
	const auto straight_distance = this->RunwayThreshold.DistanceTo(this->Target.GetPosition().GetPosition());
	return sin(track_angle) * straight_distance;
}

int CGCAPlot::track_error_to_track_y() const
{
	const auto track_error = this->Label->get_track_deviation();
	const auto middle = this->TrackArea.CenterPoint().y;
	auto deflection = get_max_track_error_at_distance() / this->TrackArea.Height() * track_error;
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
