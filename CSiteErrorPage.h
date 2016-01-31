#ifndef CSITEERRORPAGE_H
#define CSITEERRORPAGE_H

#include "novemberlib/pages/CErrorPage.h"
#include "novemberlib/interfaces/IErrorMessage.h"

class CSiteErrorPage : public CErrorPage
{
	public:
		CSiteErrorPage(const std::string& name, const std::string& errorMessage, const std::string& errorCode);
		virtual ~CSiteErrorPage();
	protected:
		virtual std::string buildContent() const;
	private:
};

#endif // CSITEERRORPAGE_H
