#pragma once
#include "EuroScopePlugIn.h"

class CGCAScreen :
	public EuroScopePlugIn::CRadarScreen
{
protected:
	CString m_Description; // Radar description
	CString m_Lat, m_Lon; // Threshold coordinates
	int m_Altitude; // Threshold altitude
	int m_OCH; // Obstacle Clearance Height
	int m_Heading; // Final Approach Heading
	double m_Slope; // Glidepath angle
	EuroScopePlugIn::CPosition    m_RwyPos; // Runway threshold
	int m_maxH; // max height of slope
public:
	CGCAScreen(void);
	virtual ~CGCAScreen(void);
	virtual void OnAsrContentLoaded(bool Loaded);
	virtual void OnAsrContentToBeSaved(void);
	virtual void OnRefresh(HDC hDC, int Phase);
	virtual bool OnCompileCommand(const char* sCommandLine);
	inline virtual void OnAsrContentToBeClosed(void)
	{
		delete this;
	};
};

