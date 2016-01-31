#ifndef CSURVEYPAGE_H
#define CSURVEYPAGE_H

#include "CSitePage.h"

class CSurveyPage : public CSitePage
{
	public:
		CSurveyPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CSurveyPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CSURVEYPAGE_H
