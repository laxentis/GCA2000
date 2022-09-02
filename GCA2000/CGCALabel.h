#pragma once
#include <string>
#include <EuroScopePlugIn.h>
// ReSharper disable once CppInconsistentNaming
class CGCALabel
{
public:
	CGCALabel(EuroScopePlugIn::CRadarTarget track, EuroScopePlugIn::CPosition threshold, int threshold_altitude, int heading, double glide_slope);
	double get_track_distance() const;
	double get_track_deviation() const;
	double get_glidepath_deviation() const;

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
	void calculate_track_distance();
	void calculate_track_deviation();
	void calculate_glidepath_deviation();

	// Helper calculators
	double get_perfect_altitude_for_present_distance() const;
};

