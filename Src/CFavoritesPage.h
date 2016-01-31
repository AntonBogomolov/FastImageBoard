#ifndef CFAVORITESPAGE_H
#define CFAVORITESPAGE_H

#include "CSitePage.h"

#include <string>

class CFavoritesPage : public CSitePage
{
	public:
		CFavoritesPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CFavoritesPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CFAVORITESPAGE_H
