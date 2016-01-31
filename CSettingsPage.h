#ifndef CSETTINGSPAGE_H
#define CSETTINGSPAGE_H

#include "CSitePage.h"

class CSettingsPage : public CSitePage
{
	public:
		CSettingsPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CSettingsPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CSETTINGSPAGE_H
