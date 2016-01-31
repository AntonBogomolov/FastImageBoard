#include "CRulesPage.h"

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


CRulesPage::CRulesPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{

}

CRulesPage::~CRulesPage()
{
	//dtor
}

std::string CRulesPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr;
	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	const CHTMLTemplate* contentTemplate 	= templateManager->findTemplate("content");
	const CHTMLTemplate* rulesTemplate 		= templateManager->findTemplate("rulesPage");
	if(contentTemplate == NULL || rulesTemplate == NULL) return "Missing content template";

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= rulesTemplate->getHTMLData();

	return contentTemplate->getHTMLData(&params);
}
