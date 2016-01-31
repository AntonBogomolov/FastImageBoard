#ifndef CHELPPAGE_H
#define CHELPPAGE_H

#include "CSitePage.h"

class CHelpPage : public CSitePage
{
	public:
		CHelpPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CHelpPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CHELPPAGE_H
