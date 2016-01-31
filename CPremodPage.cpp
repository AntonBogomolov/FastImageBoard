#include "CPremodPage.h"

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
#include "CThread.h"
#include "CPost.h"


CPremodPage::CPremodPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	//ctor
}

CPremodPage::~CPremodPage()
{
	//dtor
}

std::string CPremodPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	std::map<std::string, std::string> params;
	tmpString = "";
	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	const CHTMLTemplate* contentTemplate  = templateManager->findTemplate("content");
	if(contentTemplate == NULL) return "Missing content template";

	if(!user->getIsValid() || user->getUserType(0) <= CUser::UT_USER) return "User not valid";

	std::vector<int> threadsId;
	std::vector<int> postsId;

	tmpString += "<input type='hidden' name='return_page' value='";
	tmpString += getPageName();
	tmpString += "' /> \n";
	tmpString += buildNewPostForm(getPageName(), user);
	tmpString += "\n";

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "Threads", "`threadIsNeedPremod` = 1");
	//const CDBRequestResult* result = dbRequest->createRequest("SELECT `id` FROM `Threads` WHERE `threadIsNeedPremod` = 1;");
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		for(int i = 0; i < result->getRowsCnt(); i++)
		{
			int threadId = 0;
			try{ threadId = result->getIntValue(i, 0); }
			catch(...){ threadId = 0; }
			if(threadId == 0) continue;

			threadsId.push_back(threadId);
		}
	}
	result = dbRequest->selectRequest(CDBValues("id"), "Posts", "`postIsNeedPremod` = 1");
	//result = DBRequest.createRequest("SELECT `id` FROM `Posts` WHERE `postIsNeedPremod` = 1;");
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		for(int i = 0; i < result->getRowsCnt(); i++)
		{
			int postId = 0;
			try{ postId = result->getIntValue(i, 0); }
			catch(...){ postId = 0; }
			if(postId == 0) continue;

			postsId.push_back(postId);
		}
	}

	if(threadsId.size() > 0)
	{
		tmpString += "<h2>Темы в премодерации</h2> \n";
		tmpString += "<br> \n";
		for(unsigned int i = 0; i < threadsId.size(); i++)
		{
			const CThread* currThread = new CThread(threadsId[i]);
			if(!currThread->getIsValid())
			{
				delete currThread;
				continue;
			}
			tmpString += buildThread(currThread, user);
			tmpString += "/n";
			delete currThread;
		}
		tmpString += "<hr> \n";
	}
	if(postsId.size() > 0)
	{
		tmpString += "<h2>Сообщения в премодерации</h2> \n";
		tmpString += "<br> \n";
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
	}

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpString;

	return contentTemplate->getHTMLData(&params);
}
