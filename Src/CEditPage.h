#ifndef CEDITPAGE_H
#define CEDITPAGE_H

#include "CSitePage.h"

class CFCGIRequest;

class CEditPage : public CSitePage
{
	public:
		CEditPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CEditPage();
	protected:
		virtual std::string buildContent() const;
	private:
		int id;
		bool isThreadId;
		bool isValidId;
		std::string idStr;
};

#endif // CEDITPAGE_H
