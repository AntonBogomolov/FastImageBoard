#ifndef CRULESPAGE_H
#define CRULESPAGE_H

#include "CSitePage.h"

class CRulesPage : public CSitePage
{
	public:
		CRulesPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CRulesPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CRULESPAGE_H
