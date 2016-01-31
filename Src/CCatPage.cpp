#include "CCatPage.h"

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
#include "CUser.h"


CCatPage::CCatPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	//ctor
}

CCatPage::~CCatPage()
{
	//dtor
}

std::string CCatPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();

	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	std::map<std::string, std::string> params;
	tmpString = "";

	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
	CConfigHelper* settingsManager = CConfigHelper::getInstance();

	const CHTMLTemplate* contentTemplate  = templateManager->findTemplate("content");
	if(contentTemplate == NULL) return "Missing content template";

	const CMenuCategory* currCategory = CMenu::getInstance()->findCategoryByName(getPageName());
	if(currCategory == NULL) return "Cant find this category";

	if(!user->getIsValid())return "User not valid";

	const std::vector<int> threadsId       = currCategory->getThreadsId(settingsManager->getIntParamValue("defaulThreadsCnt", 10));
	const std::vector<int>* fixedThreadsId = user->getFixedThreads();

	tmpString += "<input type='hidden' name='category_id' value='";
	tmpString += valueToString(currCategory->getId());
	tmpString += "' /> \n";

	tmpString += "<input type='hidden' name='return_page' value='";
	tmpString += getPageName();
	tmpString += "' /> \n" ;

	tmpString += "<h1>/";
	tmpString += htmlspecialchars(currCategory->getName());
	tmpString += "/ &mdash; ";
	tmpString += htmlspecialchars(currCategory->getDescription());
	tmpString += "</h1> \n";

	if(currCategory->getThreadCreationAccessLevel() <= user->getUserType(currCategory->getId()))
	{
		tmpString += buildNewThreadForm(getPageName(), currCategory->getId(), user);
		tmpString += "\n";
	}
	if(currCategory->getPostCreationAccessLevel() <= user->getUserType(currCategory->getId()))
	{
		tmpString += buildNewPostForm(getPageName(), user);
		tmpString += "\n";
	}

	// GLOBAL FIXED THREADS
	int threadsCnt = 0;
	int hiddenthreadsCnt = 0;

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"),"Threads", "`threadCatId` = "+valueToString(currCategory->getId())+" AND `isFixed` = 1");
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		for(int i = 0; i < result->getRowsCnt(); i++)
		{
			int threadId = 0;
			threadId = result->getIntValue(i, 0);

			const CThread* currThread = new CThread(threadId);
			if(!currThread->getIsValid() || currThread->getIsHidden())
			{
				if(currThread->getIsHidden()) hiddenthreadsCnt++;
				delete currThread;
				continue;
			}
			threadsCnt++;
			tmpString += buildThread(currThread, user);
			tmpString += "\n";
			delete currThread;
		}
	}

	// USER FIXED THREADS
	for(unsigned int i = 0; i < fixedThreadsId->size(); i++)
	{
		const CThread* currThread = new CThread((*fixedThreadsId)[i]);
		if(!currThread->getIsValid() || currThread->getIsFixed() || currThread->getCatId() != currCategory->getId() || currThread->getIsHidden())
		{
			delete currThread;
			continue;
		}
		threadsCnt++;
		tmpString += buildThread(currThread, user);
		tmpString += "\n";
		delete currThread;
	}
	tmpString += "<br> \n";

	// ALL OTHER THREADS
	int minThreadPosition = INT_MAX;
	for(unsigned int i = 0; i < threadsId.size(); i++)
	{
		const CThread* currThread = new CThread(threadsId[i]);
		if(currThread->getIsValid() && currThread->getIsHidden() && !currThread->getIsFixed()) hiddenthreadsCnt++;
		if(!currThread->getIsValid() || user->getIsThreadFixed(threadsId[i]) || currThread->getIsFixed() || currThread->getIsHidden())
		{
			delete currThread;
			continue;
		}

		threadsCnt++;
		if(currThread->getPosition() < minThreadPosition) minThreadPosition = currThread->getPosition();
		tmpString += buildThread(currThread, user);
		tmpString += "\n";
		delete currThread;
	}

	tmpString += "<input type='hidden' name='all_threads_cnt' value='";
	tmpString += valueToString(currCategory->getAllThreadsCnt() - hiddenthreadsCnt);
	tmpString += "' /> \n";

	tmpString += "<input type='hidden' name='min_thread_pos' value='";
	tmpString += valueToString(minThreadPosition);
	tmpString += "' /> \n";

	tmpString += "<input type='hidden' id='thCnt' name='curr_threads_cnt' value='";
	tmpString += valueToString(threadsCnt);
	tmpString += "' /> \n";

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpString;

	return contentTemplate->getHTMLData(&params);
}
