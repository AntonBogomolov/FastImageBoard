#include "CHelpPage.h"

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

CHelpPage::CHelpPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	//ctor
}

CHelpPage::~CHelpPage()
{
	//dtor
}

std::string CHelpPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr;
	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	const CHTMLTemplate* contentTemplate 	= templateManager->findTemplate("content");
	const CHTMLTemplate* helpTemplate 		= templateManager->findTemplate("helpPage");
	if(contentTemplate == NULL || helpTemplate == NULL) return "Missing content template";

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= helpTemplate->getHTMLData();

	return contentTemplate->getHTMLData(&params);
}
