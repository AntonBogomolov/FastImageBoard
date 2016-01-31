#include "CSurveyPage.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/helpers/CTemplateHelper.h"
#include "novemberlib/helpers/CMessageParser.h"
#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/CHTMLTemplate.h"

#include "CMenu.h"
#include "CThread.h"
#include "CPost.h"
#include "CUser.h"
#include "CSurvey.h"


CSurveyPage::CSurveyPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	//ctor
}

CSurveyPage::~CSurveyPage()
{
	//dtor
}

std::string CSurveyPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr = "";
	tmpString = "";

	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	if(!user->getIsValid()) return "";

	const CHTMLTemplate* contentTemplate      = templateManager->findTemplate("content");
	const CHTMLTemplate* surveysPageTemplate  = templateManager->findTemplate("surveysPage");
	if(contentTemplate == NULL || surveysPageTemplate == NULL) return "Missing content template";

	params["{USERID}"] = valueToString(user->getUserId());

	const std::vector<CSurvey*>* surveys = user->getSurveys();
	for(auto it = surveys->begin(); it != surveys->end(); ++it)
	{
		const CSurvey* currSurvey = (*it);
		if(!currSurvey->getIsValid()) continue;

		tmpStr += CSitePage::buildSurveyResult(currSurvey, user);
		tmpStr += "<br> \n";
		tmpStr += "<input type='button' value='Удалить опрос' onclick='delSurvey("+valueToString(currSurvey->getId())+", "+valueToString(user->getUserId())+")'> \n";
		tmpStr += "<span>  </span> \n";
		tmpStr += "<input type='button' value='Завершить опрос' onclick='endSurvey("+valueToString(currSurvey->getId())+", "+valueToString(user->getUserId())+")'> \n";
		tmpStr += "<br> \n";
	}
	params["{USERSURVEYS}"] = tmpStr;

	tmpStr = "";
	const std::vector<int>* surveysIds = user->getAnsweredSurveysId();
	for(auto it = surveysIds->begin(); it != surveysIds->end(); ++it)
	{
		const int currSurveyId = (*it);
		if(currSurveyId <= 0) continue;
		CSurvey* currSurvey = new CSurvey(currSurveyId);
		if(currSurvey->getIsValid())
		{
			tmpStr += CSitePage::buildSurveyResult(currSurvey, user);
		}
		delete currSurvey;
	}
	params["{USERSURVEYSANSWERS}"] = tmpStr;
	tmpStr = surveysPageTemplate->getHTMLData(&params);

	params.clear();

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpStr;

	return contentTemplate->getHTMLData(&params);
}
