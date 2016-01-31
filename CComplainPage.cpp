#include "CComplainPage.h"

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


CComplainPage::CComplainPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	//ctor
}

CComplainPage::~CComplainPage()
{
	//dtor
}

std::string CComplainPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();

	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	std::map<std::string, std::string> params;

	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	const CHTMLTemplate* contentTemplate  = templateManager->findTemplate("content");
	if(contentTemplate == NULL) return "Missing content template";

	if(!user->getIsValid()) return "User not valid";

	std::vector<int> threadsId;
	std::vector<int> postsId;

	tmpString += "<input type='hidden' name='return_page' value='";
	tmpString += getPageName();
	tmpString += "' /> \n";

	tmpString += buildNewPostForm(getPageName(), user);
	tmpString += "\n";

	const CDBRequestResult* resultThreads = dbRequest->selectRequest(CDBValues("id"), "Threads", "`threadComplainCnt` > 0", "ORDER BY `threadComplainCnt`");
	//const CDBRequestResult* result = dbRequest->createRequest("SELECT `id` FROM `Threads` WHERE `threadComplainCnt` > 0 ORDER BY `threadComplainCnt`;");
	if(dbRequest->getIsLastQuerySuccess() && resultThreads != NULL && resultThreads->getRowsCnt() > 0)
	{
		for(int i = 0; i < resultThreads->getRowsCnt(); i++)
		{
			int threadId = 0;
			threadId = resultThreads->getIntValue(0,0);
			if(threadId == 0) continue;

			threadsId.push_back(threadId);
		}
	}
	const CDBRequestResult* resultPosts = dbRequest->selectRequest(CDBValues("id"), "Posts", "`postComplainCnt` > 0", "ORDER BY `postComplainCnt`");
	//result = DBRequest.createRequest("SELECT `id` FROM `Posts` WHERE `postComplainCnt` > 0 ORDER BY `postComplainCnt`;");
	if(dbRequest->getIsLastQuerySuccess() && resultPosts != NULL && resultPosts->getRowsCnt() > 0)
	{
		for(int i = 0; i < resultPosts->getRowsCnt(); i++)
		{
			int postId = 0;
			postId = resultPosts->getIntValue(0,0);
			if(postId == 0) continue;

			postsId.push_back(postId);
		}
	}

	if(threadsId.size() > 0)
	{
		tmpString += "<h2>Темы с жалобами</h2> \n";
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
			tmpString += "\n";
			delete currThread;
		}
		tmpString += "<hr> \n";
	}
	if(postsId.size() > 0)
	{
		tmpString += "<h2>Сообщения с жалобами</h2> \n";
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
