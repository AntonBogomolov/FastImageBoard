#include "CUserContentPage.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/helpers/CTemplateHelper.h"
#include "novemberlib/helpers/CMessageParser.h"
#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/CHTMLTemplate.h"
#include "novemberlib/CBan.h"

#include "CUser.h"
#include "CThread.h"
#include "CPost.h"
#include "CSurvey.h"


CUserContentPage::CUserContentPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	std::string idStr = currRequest->getRequest()->get.get("id", "");
	try
	{
		this->userId = std::stoi(idStr);
	}
	catch(...)
	{
		this->userId = 0;
	}
}

CUserContentPage::~CUserContentPage()
{
	//dtor
}


std::string CUserContentPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	std::map<std::string, std::string> params;
	tmpString = "";
	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	const CHTMLTemplate* contentTemplate  	= templateManager->findTemplate("content");
	const CHTMLTemplate* userTemplate 		= templateManager->findTemplate("userPage");
	if(contentTemplate == NULL || userTemplate == NULL) return "Missing content template";

	if(!user->getIsValid()) return "User not valid";
	if(userId == 0) userId = user->getUserId();
	if(user->getUserId() != userId && !user->getIsModerInAnyCategories()) return "Access denied";

	std::vector<int> threadsId;
	std::vector<int> postsId;

	params["{isMODER}"]  = "false";

	if(user->getIsModerInAnyCategories() &&  user->getUserId() != userId)
	{
        CUser* currUser = new CUser(userId);
		currUser->fillUserDataById(userId);
        if(currUser->getIsValid())
        {
			params["{isMODER}"] = "true";
			params["{USERID}"]  = valueToString(currUser->getUserId());
			params["{MODID}"]   = valueToString(user->getUserId());
			params["{USERKEY}"] = currUser->getUserKey();

			params["{RE}"] = currUser->getUserAgeRating() == 0 ? "selected" : "";
			params["{RT}"] = currUser->getUserAgeRating() == 1 ? "selected" : "";
			params["{RM}"] = currUser->getUserAgeRating() == 2 ? "selected" : "";
			params["{RA}"] = currUser->getUserAgeRating() == 3 ? "selected" : "";

			const std::vector<CBan*>* userBans = currUser->getUserBans();
			for(auto it = userBans->begin(); it != userBans->end(); ++it)
			{
				CBan* currBan = (*it);
				tmpString += "Время создания: ";
				tmpString += unixTimeToDate(currBan->getBanCreadtionTime());
				tmpString += "; ";

				tmpString += "ReadOnly: ";
				tmpString += valueToString(currBan->getROTime()/(60*60*24));
				tmpString += " дней; ";

				if(currBan->getIsAccessClosed()) tmpString += " Доступ на сайт закрыт";
				tmpString += "<br> \n";
			}
			params["{USERBANS}"] = tmpString;
			tmpString = "";

			std::string tmpStr = "";
			const std::vector<CSurvey*>* surveys = currUser->getSurveys();
			for(auto it = surveys->begin(); it != surveys->end(); ++it)
			{
				const CSurvey* currSurvey = (*it);
				if(!currSurvey->getIsValid()) continue;

				tmpStr += CSitePage::buildSurveyResult(currSurvey, currUser);
				tmpStr += "<br> \n";
				tmpStr += "<input type='button' value='Удалить опрос' onclick='delSurvey("+valueToString(currSurvey->getId())+", "+valueToString(user->getUserId())+")'> \n";
				tmpStr += "<span>  </span> \n";
				tmpStr += "<input type='button' value='Завершить опрос' onclick='endSurvey("+valueToString(currSurvey->getId())+", "+valueToString(user->getUserId())+")'> \n";
				tmpStr += "<br> \n";
			}
			params["{USERSURVEYS}"] = tmpStr;
        }
        delete currUser;
	}
	if(user->getUserId() == userId)
	{
		std::string tmpStr = "";
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
	}

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "Threads", "`threadUserId` = "+valueToString(userId));
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		for(int i = 0; i < result->getRowsCnt(); i++)
		{
			int threadId = 0;
			threadId = result->getIntValue(i,0);
			if(threadId == 0) continue;

			threadsId.push_back(threadId);
		}
	}
	result = dbRequest->selectRequest(CDBValues("id"), "Posts", "`postUserId` = "+valueToString(userId));
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		for(int i = 0; i < result->getRowsCnt(); i++)
		{
			int postId = 0;
			postId = result->getIntValue(i,0);
			if(postId == 0) continue;

			postsId.push_back(postId);
		}
	}

	params["{USERTHEMES}"] = "";
	if(threadsId.size() > 0)
	{
		for(unsigned int i = 0; i < threadsId.size(); i++)
		{
			const CThread* currThread = new CThread(threadsId[i]);
			if(!currThread->getIsValid())
			{
				delete currThread;
				continue;
			}
			tmpString += buildThread(currThread, user);
			tmpString += "\n";
			delete currThread;
		}
		params["{USERTHEMES}"] = tmpString;
		tmpString = "";
	}

	params["{USERPOSTS}"] = "";
	if(postsId.size() > 0)
	{
		for(unsigned int i = 0; i < postsId.size(); i++)
		{
			const CPost* currPost = new CPost(postsId[i]);
			if(!currPost->getIsValid())
			{
				delete currPost;
				continue;
			}
			tmpString += buildPost(currPost, user);
			tmpString += "\n";
			delete currPost;
		}
		params["{USERPOSTS}"] = tmpString;
		tmpString = "";
	}

	tmpString = "";
	tmpString += userTemplate->getHTMLData(&params);
	params.clear();

	tmpString += "<input type='hidden' name='return_page' value='"+getPageName()+"' /> \n";
	tmpString += buildNewPostForm(getPageName(), user);
	tmpString += "\n";

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpString;

	return contentTemplate->getHTMLData(&params);
}
