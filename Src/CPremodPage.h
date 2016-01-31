#ifndef CPREMODPAGE_H
#define CPREMODPAGE_H

#include "CSitePage.h"

class CPremodPage : public CSitePage
{
	public:
		CPremodPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CPremodPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CPREMODPAGE_H
