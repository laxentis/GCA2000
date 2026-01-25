#include "pch.h"
#include "CGCAPlot.h"
#include "CGCAScreen.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <map>

CGCAPlot::CGCAPlot(const EuroScopePlugIn::CRadarTarget& target, const EuroScopePlugIn::CPosition& runway_threshold,
                   const CRect& glideslope_area, const CRect& track_area, const CRect& deviation_area, const double max_range = 20,
                   const double max_alt = 6000, const double max_track_deviation = 3.0, const double max_glideslope_deviation = 500,
                   const double threshold_altitude = 0.0, const double glideslope = 3.0, const double heading = 0.0)
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
	this->threshold_altitude_ = threshold_altitude;
	this->heading_ = heading;

	this->label_ = new CGCALabel(target, runway_threshold, threshold_altitude, heading, glideslope);
}


CGCAPlot::~CGCAPlot()
{
	delete this->label_;
}

double CGCAPlot::track_distance_to_x() const
{
	const auto distance = this->label_->get_track_distance();
	return (this->track_area_.left + distance / this->max_range_ * this->track_area_.Width());

}


double CGCAPlot::get_max_track_error_at_distance() const
{
	constexpr auto track_angle = 15.0 * M_PI / 180.0;
	const auto straight_distance = this->runway_threshold_.DistanceTo(this->target_.GetPosition().GetPosition());
	return sin(track_angle) * straight_distance;
}

double CGCAPlot::track_error_to_track_y() const
{
	const auto track_error = this->label_->get_track_deviation();
	const auto middle = this->track_area_.CenterPoint().y;

	auto deflection = track_error * this->track_area_.Height() / 2.0 / max_track_deviation_; // TO TEST
	return middle + deflection;
}

double CGCAPlot::gs_error_to_gs_y() const
{
	return this->glideslope_area_.bottom - (this->label_->get_altitude() - this->threshold_altitude_) * glideslope_area_.Height() / this->max_alt_;
}

void CGCAPlot::draw_plot(CDC* dc, CPen* plot_pen, CPen* first_error_pen, CPen* second_error_pen, CGCAScreen* screen) const
{

    auto const s_dc = dc->SaveDC();
	dc->SelectObject(plot_pen);
	const double x = this->track_distance_to_x();
	const double y_track = this->track_error_to_track_y();
	const double y_gs = + this->gs_error_to_gs_y();
	std::string callsign = this->label_->get_callsign();
	double altitude = this->label_->get_altitude();
	std::string altitude_str = std::to_string(static_cast<int>(round(altitude/10.0)*10.0));
	altitude_str.append(" ft");
	double height = altitude - threshold_altitude_;
	std::string height_str = std::to_string(static_cast<int>(round(height / 10.0) * 10.0));
	height_str.append(" AGL");
	//Check if inside correct area before drawing
	if (this->track_area_.PtInRect(CPoint(x, y_track)) && this->glideslope_area_.PtInRect(CPoint(x, y_gs)))
	{
		//Store coordinates in map for history trails
		screen->plot_map_[std::make_tuple(x, y_track, y_gs)] = 1;

		
		//Draw on track display ----------------------
		// Draw a circle
		dc->Arc(x - 10.0, y_track - 10.0, x + 10.0, y_track + 10.0, x, y_track, x, y_track);

		// Draw a '+' cross:
		dc->MoveTo(x - 11.0, y_track);
		dc->LineTo(x + 10.0, y_track);
		dc->MoveTo(x, y_track - 10.0);
		dc->LineTo(x, y_track + 10.0);

		//Draw tag
		dc->SetTextColor(RGB(255, 255, 255));
		dc->SetTextAlign(TA_BOTTOM);
		dc->TextOutA(x, y_track - 15, callsign.c_str());

		// Draw on glideslope display ---------------
		// Draw a circle
		dc->Arc(x - 10.0, y_gs - 10.0, x + 10.0, y_gs + 10.0, x, y_gs, x, y_gs);

        // Draw a '+' cross:
        dc->MoveTo(x - 11.0, y_gs);
        dc->LineTo(x + 10.0, y_gs);
        dc->MoveTo(x, y_gs - 10.0);
        dc->LineTo(x, y_gs + 10.0);

		// Draw tag
		dc->SetTextAlign(TA_LEFT);
		dc->TextOutA(x, y_gs + 15, callsign.c_str());
		//dc->TextOutA(x, y_gs + 30, height_str.c_str()); //show height above threshold
		//dc->TextOutA(x, y_gs + 45, altitude_str.c_str()); //show altitude


		std::string distance_str = std::to_string(static_cast<double>(this->label_->get_track_distance()));
		//dc->TextOutA(x, y_gs + 60, distance_str.c_str()); //show track miles to THR
		


		
	} 
	
	
	dc->RestoreDC(s_dc);
}
