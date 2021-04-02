#include "pch.h"
#include "CGCALabel.h"
#define _USE_MATH_DEFINES
#include <functional>
#include <math.h>

CGCALabel::CGCALabel(const EuroScopePlugIn::CRadarTarget track, char wakeTurbulenceCategory, const EuroScopePlugIn::CPosition threshold, const int thresholdAltitude,
                     const int heading,
                     const double glideSlope)
{
	this->RadarTrackPosition = track.GetPosition().GetPosition();
	this->WakeTurbulenceCategory = track.GetCorrelatedFlightPlan().GetFlightPlanData().GetAircraftWtc();
	this->Callsign = track.GetCallsign();
	this->Altitude = track.GetPosition().GetPressureAltitude();
	this->RunwayThreshold = threshold;
	this->ThresholdAltitude = thresholdAltitude;
	this->FinalApproachHeading = heading;
	this->GlideslopeAngle = glideSlope;
	this->TrackDeviation = 0;
	this->TrackDistance = 0;
	this->GlidepathDeviation = 0;
	CalculateTrackDistance();
	CalculateTrackDeviation();
	CalculateGlidepathDeviation();
}

void CGCALabel::CalculateTrackDistance()
{
	auto trackAngle = this->RunwayThreshold.DirectionTo(this->RadarTrackPosition);
	trackAngle = trackAngle * M_PI / 180;
	const auto straightDistance = this->RunwayThreshold.DistanceTo(this->RadarTrackPosition);
	this->TrackDistance = cos(trackAngle) * straightDistance;
}

void CGCALabel::CalculateTrackDeviation()
{
	auto trackAngle = this->RunwayThreshold.DirectionTo(this->RadarTrackPosition);
	trackAngle = trackAngle * M_PI / 180;
	const auto straightDistance = this->RunwayThreshold.DistanceTo(this->RadarTrackPosition);
	this->TrackDeviation = sin(trackAngle) * straightDistance;
}

double CGCALabel::GetPerfectAltitudeForPresentDistance() const
{
	double altitude = ThresholdAltitude;
	const auto glideSlopeRadians = this->GlideslopeAngle * M_PI / 180;
	altitude += tan(glideSlopeRadians) * this->TrackDistance;
	return altitude;
}

void CGCALabel::CalculateGlidepathDeviation()
{
	const auto perfectAltitude = GetPerfectAltitudeForPresentDistance();
	this->GlidepathDeviation = this->Altitude - perfectAltitude;
}

double CGCALabel::GetTrackDistance() const
{
	return this->TrackDistance;
}

double CGCALabel::GetTrackDeviation() const
{
	return this->TrackDeviation;
}

double CGCALabel::GetGlidepathDeviation() const
{
	return this->GlidepathDeviation;
}
