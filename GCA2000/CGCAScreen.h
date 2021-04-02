#pragma once
#include "EuroScopePlugIn.h"

// ReSharper disable once CppInconsistentNaming
class CGCAScreen final :
	public EuroScopePlugIn::CRadarScreen
{
private:
	void DrawGlideslopeAxes(CDC *dc, CRect area, CPen* pen, unsigned maxRange, unsigned maxAlt) const;
	void DrawDeviationCross(CDC* dc, CRect area, CPen* pen) const;
	void DrawGlideslope(CDC* dc, CRect area, CPen* pen, unsigned maxRange, unsigned maxAlt) const;
	void DrawObstacleClearanceHeight(CDC* dc, CRect area, CPen* pen, unsigned maxRange, unsigned maxAlt) const;
protected:
	CString Description; // Radar description
	CString Lat, Lon; // Threshold coordinates
	int Altitude; // Threshold altitude
	int ObstacleClearanceHeight; // Obstacle Clearance Height
	int Heading; // Final Approach Heading
	double GlideSlope; // Glidepath angle
	EuroScopePlugIn::CPosition    RunwayPosition; // Runway threshold
	int ScopeMaxHeight; // max height of slope
public:
	CGCAScreen(void);
	virtual ~CGCAScreen(void);
	void OnAsrContentLoaded(bool loaded) override;
	void OnAsrContentToBeSaved(void) override;
	// ReSharper disable once CppInconsistentNaming
	void OnRefresh(HDC hDC, int phase) override;
	//virtual bool OnCompileCommand(const char* sCommandLine);
	inline void OnAsrContentToBeClosed(void) override
	{
		delete this;
	}
};

