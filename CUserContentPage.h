#ifndef CUSERCONTENTPAGE_H
#define CUSERCONTENTPAGE_H

#include "CSitePage.h"

class CUserContentPage : public CSitePage
{
	public:
		CUserContentPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CUserContentPage();
	protected:
		virtual std::string buildContent() const;
	private:
		mutable int userId;
};

#endif // CUSERCONTENTPAGE_H
