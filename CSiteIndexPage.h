#ifndef CSITEINDEXPAGE_H
#define CSITEINDEXPAGE_H

#include "CSitePage.h"


class CSiteIndexPage : public CSitePage
{
	public:
		CSiteIndexPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CSiteIndexPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CSITEINDEXPAGE_H
