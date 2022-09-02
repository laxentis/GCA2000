#include "pch.h"
#include "CGCALabel.h"
// ReSharper disable once CppInconsistentNaming
#define _USE_MATH_DEFINES
#include <functional>
#include <math.h>  // NOLINT(modernize-deprecated-headers)

CGCALabel::CGCALabel(const EuroScopePlugIn::CRadarTarget track, const EuroScopePlugIn::CPosition threshold, const int threshold_altitude,
                     const int heading,
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
	auto track_angle = this->RunwayThreshold.DirectionTo(this->RadarTrackPosition);
	track_angle = track_angle * M_PI / 180;
	const auto straight_distance = this->RunwayThreshold.DistanceTo(this->RadarTrackPosition);
	this->TrackDistance = cos(track_angle) * straight_distance;
}

void CGCALabel::calculate_track_deviation()
{
	auto track_angle = this->RunwayThreshold.DirectionTo(this->RadarTrackPosition);
	track_angle = track_angle * M_PI / 180;
	const auto straight_distance = this->RunwayThreshold.DistanceTo(this->RadarTrackPosition);
	this->TrackDeviation = sin(track_angle) * straight_distance;
}

double CGCALabel::get_perfect_altitude_for_present_distance() const
{
	double altitude = ThresholdAltitude;
	const auto glide_slope_radians = this->GlideslopeAngle * M_PI / 180;
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
