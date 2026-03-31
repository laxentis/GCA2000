#include "pch.h"
#include "CGCALabel.h"
// ReSharper disable once CppInconsistentNaming
#define _USE_MATH_DEFINES
#include <functional>
#include <math.h>  // NOLINT (modernize-deprecated-headers)

CGCALabel::CGCALabel(const EuroScopePlugIn::CRadarTarget track, const EuroScopePlugIn::CPosition threshold, const double  threshold_altitude,
                     const double  heading,
                     const double glide_slope)
{
	this->RadarTrackPosition = track.GetPosition().GetPosition();
	this->WakeTurbulenceCategory = track.GetCorrelatedFlightPlan().GetFlightPlanData().GetAircraftWtc();
	this->Callsign = track.GetCallsign();
	this->Altitude = track.GetPosition().GetPressureAltitude();
	this->RunwayThreshold = threshold;
	this->ThresholdAltitude = threshold_altitude;
	this->FinalApproachHeading = heading;
	this->GlideslopeAngle = glide_slope;
	this->TrackDeviation = 0;
	this->TrackDistance = 0;
	this->GlidepathDeviation = 0;
	calculate_track_distance();
	calculate_track_deviation();
	calculate_glidepath_deviation();
}

void CGCALabel::calculate_track_distance()
{
	auto track_angle = this->RadarTrackPosition.DirectionTo(this->RunwayThreshold); //EDIT: corrected calculation of track angle to be relative to final approach heading by switching parameters
	track_angle = track_angle * M_PI / 180; 
	auto rwy_heading = this->FinalApproachHeading * M_PI / 180;
	const auto straight_distance = this->RunwayThreshold.DistanceTo(this->RadarTrackPosition);
	this->TrackDistance = cos(track_angle - rwy_heading) * straight_distance;
}

void CGCALabel::calculate_track_deviation()
{
	auto track_angle = this->RadarTrackPosition.DirectionTo(this->RunwayThreshold); //EDIT: corrected calculation of track angle to be relative to final approach heading by switching parameters
	track_angle = track_angle * M_PI / 180;
	auto rwy_heading = this->FinalApproachHeading * M_PI / 180;
	const auto straight_distance = this->RunwayThreshold.DistanceTo(this->RadarTrackPosition);
	this->TrackDeviation = sin(track_angle - rwy_heading) * straight_distance;
}

double CGCALabel::get_perfect_altitude_for_present_distance() const
{
	double altitude = ThresholdAltitude;
	const double glide_slope_radians = this->GlideslopeAngle * M_PI / 180;
	altitude += tan(glide_slope_radians) * this->TrackDistance;
	return altitude;
}

void CGCALabel::calculate_glidepath_deviation()
{
	const auto perfect_altitude = get_perfect_altitude_for_present_distance();
	this->GlidepathDeviation = this->Altitude - perfect_altitude;
}

double CGCALabel::get_track_distance() const
{
	return this->TrackDistance;
}

double CGCALabel::get_track_deviation() const
{
	return this->TrackDeviation;
}

double CGCALabel::get_glidepath_deviation() const
{
	return this->GlidepathDeviation;
}

double CGCALabel::get_altitude() const
{
	return this->Altitude;
}

std::string CGCALabel::get_callsign() const
{
	return this->Callsign;
}