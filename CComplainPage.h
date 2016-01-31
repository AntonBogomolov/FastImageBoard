#ifndef CCOMPLAINPAGE_H
#define CCOMPLAINPAGE_H

#include "CSitePage.h"

class CComplainPage : public CSitePage
{
	public:
		CComplainPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CComplainPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CCOMPLAINPAGE_H
