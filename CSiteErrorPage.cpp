#include "CSiteErrorPage.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/helpers/CTemplateHelper.h"
#include "novemberlib/helpers/CMessageParser.h"
#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/CHTMLTemplate.h"

#include "CUser.h"


CSiteErrorPage::CSiteErrorPage(const std::string& name, const std::string& errorMessage, const std::string& errorCode) : CErrorPage(name, errorMessage, errorCode)
{

}

CSiteErrorPage::~CSiteErrorPage()
{
	//dtor
}

std::string CSiteErrorPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr;

	const CHTMLTemplate* contentTemplate  = templateManager->findTemplate("content");
	if(contentTemplate == NULL) return "Missing content template";

	CLog::getInstance()->addInfo(errorCode);

	tmpStr = "<h1>";
	tmpStr += errorCode;
	tmpStr += "</h1> \n";
	tmpStr += "<br> \n";
	tmpStr += "<h3>";
	tmpStr += errorMessage;
	tmpStr += "</h3> \n";

	params["{LEFTPANEL}"] 	= "";
	params["{UPMENU}"] 		= "";
	params["{CONTENT}"] 	= tmpStr;

	return contentTemplate->getHTMLData(&params);
}
