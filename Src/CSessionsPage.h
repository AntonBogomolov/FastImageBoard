#ifndef CSESSIONSPAGE_H
#define CSESSIONSPAGE_H

#include "CSitePage.h"

class CFCGIRequest;

class CSessionsPage : public CSitePage
{
	public:
		CSessionsPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CSessionsPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CSESSIONSPAGE_H
