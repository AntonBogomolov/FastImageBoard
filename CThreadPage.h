#ifndef CTHREADPAGE_H
#define CTHREADPAGE_H

#include "CSitePage.h"


class CThreadPage : public CSitePage
{
	public:
		CThreadPage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CThreadPage();
	protected:
		virtual std::string buildContent() const;
	private:
	int threadId;
};

#endif // CTHREADPAGE_H
