#pragma once
#include <EuroScopePlugIn.h>
#include "CGCALabel.h"
// ReSharper disable once CppInconsistentNaming
class CGCAPlot
{
private:
	int TrackDistanceToX() const;
	int TrackErrorToTrackY() const;
	double GetMaxTrackErrorAtDistance() const;
public:

	CGCAPlot(const EuroScopePlugIn::CRadarTarget& target, const EuroScopePlugIn::CPosition& runwayThreshold,
		const CRect& glideslopeArea, const CRect& trackArea, const CRect& deviationArea, unsigned maxRange,
		unsigned maxAlt, int maxTrackDeviation, int maxGlideslopeDeviation, int thresholdAltitude, double glideslope, int heading);
	void PlotTrack(CDC* dc, CPen* pen) const;
	~CGCAPlot();

protected:
	EuroScopePlugIn::CRadarTarget Target;
	EuroScopePlugIn::CPosition RunwayThreshold;
	CRect GlideslopeArea;
	CRect TrackArea;
	CRect DeviationArea;
	unsigned MaxRange = 20;
	unsigned MaxAlt = 16000;
	int MaxTrackDeviation = 1000;
	int MaxGlideslopeDeviation = 500;
	int ThresholdAltitude = 0;
	double GlideSlope = 3.0;
	int Heading = 0;

	CGCALabel* Label;
};

