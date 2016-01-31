#include "CThreadPage.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/helpers/CTemplateHelper.h"
#include "novemberlib/helpers/CMessageParser.h"
#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/managers/CPageManager.h"
#include "novemberlib/CHTMLTemplate.h"

#include "CUser.h"
#include "CMenu.h"
#include "CThread.h"


CThreadPage::CThreadPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	std::string tidStr = currRequest->getRequest()->get.get("tid", "");
	try
	{
		this->threadId = std::stoi(tidStr);
	}
	catch(...)
	{
		this->threadId = 0;
	}
}

CThreadPage::~CThreadPage()
{
	//dtor
}

std::string CThreadPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	tmpString = "";

	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	const CHTMLTemplate* contentTemplate  = templateManager->findTemplate("content");
	if(contentTemplate == NULL) return "Missing content template";

	const CThread* currThread = new CThread(threadId, true);
	if(!currThread->getIsValid())
	{
		std::string pageContent;
		CManagers::getInstance()->getPageManager()->getErrorPageContent("404", "Sorry, resource not found :'(");
		delete currThread;
		return pageContent;
	}
	tmpString += "<div class='new_thread_form'></div> \n";
	tmpString += buildNewPostForm("thread&tid="+valueToString(threadId), user);
	tmpString += "\n";
	tmpString += buildThread(currThread, user);
	tmpString += "\n";
	delete currThread;

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpString;

	return contentTemplate->getHTMLData(&params);
}
