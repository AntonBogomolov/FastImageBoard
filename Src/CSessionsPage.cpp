#include "CSessionsPage.h"

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


CSessionsPage::CSessionsPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{

}

CSessionsPage::~CSessionsPage()
{
	//dtor
}

std::string CSessionsPage::buildContent() const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	tmpString = "";

	const CHTMLTemplate* contentTemplate  = templateManager->findTemplate("content");
	const CHTMLTemplate* sessionsTemplate = templateManager->findTemplate("sessionPage");
	if(contentTemplate == NULL || sessionsTemplate == NULL) return "Missing content template";

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("*"), "Users");
	//const CDBRequestResult* result = dbRequest->createRequest("SELECT * FROM `Users`;");
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		for(int i = 0; i < result->getRowsCnt(); i++)
		{
			tmpString += "<tr><td>" + valueToString(result->getIntValue(i,0)) + "</td><td>" + "std::string(row[2])" + "</td><td> " + "unixTimeToDate(std::stol(row[8]))" + " </td></tr>";
		}
	}

	params["{SESSION_LIST}"] = tmpString;
	tmpString = "";
	tmpString += sessionsTemplate->getHTMLData(&params);
	params.clear();

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpString;

	return contentTemplate->getHTMLData(&params);
}
