#pragma once
#include "EuroScopePlugIn.h"

// ReSharper disable once CppInconsistentNaming
class CGCAScreen final :
	public EuroScopePlugIn::CRadarScreen
{
private:
	void DrawGlideslopeAxes(CDC *dc, CRect area, const unsigned maxRange, const unsigned maxAlt) const;
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
	void OnRefresh(HDC hDC, int Phase) override;
	//virtual bool OnCompileCommand(const char* sCommandLine);
	inline void OnAsrContentToBeClosed(void) override
	{
		delete this;
	}
};

