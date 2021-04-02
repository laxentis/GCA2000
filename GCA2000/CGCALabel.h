#pragma once
#include <string>
#include <EuroScopePlugIn.h>
class CGCALabel
{
public:
	CGCALabel(EuroScopePlugIn::CRadarTarget track, char wakeTurbulenceCategory, EuroScopePlugIn::CPosition threshold, int thresholdAltitude, int heading, double glideSlope);
	double GetTrackDistance() const;
	double GetTrackDeviation() const;
	auto GetGlidepathDeviation() const -> double;

private:
	// Calculated or retrieved parameters
	std::string Callsign;
	char WakeTurbulenceCategory;
	int Altitude;
	double TrackDeviation;
	double GlidepathDeviation;
	double TrackDistance;

	// Passed and stored parameters
	int FinalApproachHeading;
	double GlideslopeAngle;
	EuroScopePlugIn::CPosition RadarTrackPosition;
	EuroScopePlugIn::CPosition RunwayThreshold;
	int ThresholdAltitude;

	// Calculation of label elements
	void CalculateTrackDistance();
	void CalculateTrackDeviation();
	void CalculateGlidepathDeviation();

	// Helper calculators
	double GetPerfectAltitudeForPresentDistance() const;
};

