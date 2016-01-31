#ifndef CCATPAGE_H
#define CCATPAGE_H

#include "CSitePage.h"

class CCatPage : public CSitePage
{
	public:
		CCatPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CCatPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CCATPAGE_H
