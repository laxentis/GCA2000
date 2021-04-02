#include "pch.h"
#include "CGCAPlot.h"
#define _USE_MATH_DEFINES
#include <math.h>

CGCAPlot::CGCAPlot(const EuroScopePlugIn::CRadarTarget& target, const EuroScopePlugIn::CPosition& runwayThreshold,
                   const CRect& glideslopeArea, const CRect& trackArea, const CRect& deviationArea, const unsigned maxRange = 20,
                   const unsigned maxAlt = 16000, const int maxTrackDeviation = 1000, const int maxGlideslopeDeviation = 500,
                   const int thresholdAltitude = 274, const double glideslope = 3.0, const int heading = 294)
{
	this->Target = target;
	this->RunwayThreshold = runwayThreshold;
	this->GlideslopeArea = glideslopeArea;
	this->TrackArea = trackArea;
	this->DeviationArea = deviationArea;
	this->MaxRange = maxRange;
	this->MaxAlt = maxAlt;
	this->MaxTrackDeviation = maxTrackDeviation;
	this->MaxGlideslopeDeviation = maxGlideslopeDeviation;

	this->Label = new CGCALabel(target, runwayThreshold, thresholdAltitude, heading, glideslope);
}


CGCAPlot::~CGCAPlot()
{
	delete this->Label;
}

int CGCAPlot::TrackDistanceToX() const
{
	const auto distance = this->Label->GetTrackDistance();
	return this->TrackArea.left + this->MaxRange / this->TrackArea.Width() * distance;
}


double CGCAPlot::GetMaxTrackErrorAtDistance() const
{
	const auto distance = this->Label->GetTrackDistance();
	const auto trackAngle = 15.0 * M_PI / 180;
	const auto straightDistance = this->RunwayThreshold.DistanceTo(this->Target.GetPosition().GetPosition());
	return sin(trackAngle) * straightDistance;
}

int CGCAPlot::TrackErrorToTrackY() const
{
	const auto trackError = this->Label->GetTrackDeviation();
	const auto middle = this->TrackArea.CenterPoint().y;
	const auto deflection = GetMaxTrackErrorAtDistance() / this->TrackArea.Height() * trackError;
}

void CGCAPlot::PlotTrack(CDC* dc, CPen* pen) const
{
	auto const sDC = dc->SaveDC();
	dc->SelectObject(pen);
	const auto x = this->TrackDistanceToX();
	const auto y = this->TrackErrorToTrackY();
	dc->Ellipse(x - 5, y - 5, x + 5, y + 5);
	dc->RestoreDC(sDC);
}
