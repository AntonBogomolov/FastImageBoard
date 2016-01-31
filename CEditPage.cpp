#include "CEditPage.h"

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


CEditPage::CEditPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	isValidId = true;
	idStr = currRequest->getRequest()->get.get("id","");
	std::string tmpStr = idStr;

	if(tmpStr.size() == 0) isValidId = false;
	if(tmpStr[0] == 't') isThreadId = true;
	else
	if(tmpStr[0] == 'p') isThreadId = false;
	else isValidId = false;
	tmpStr[0] = '0';

	try
	{
		id = std::stoi(tmpStr);
	}
	catch(...)
	{
		id = 0;
		isValidId = false;
	}
}

CEditPage::~CEditPage()
{
	//dtor
}

std::string CEditPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();

	std::map<std::string, std::string> params;
	std::string tmpStr = "";

	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	if(!user->getIsValid() || !isValidId) return "";

	const CHTMLTemplate* contentTemplate   = templateManager->findTemplate("content");
	const CHTMLTemplate* editPageTemplate  = templateManager->findTemplate("editPage");
	if(contentTemplate == NULL || editPageTemplate == NULL) return "Missing content template";

	if(isThreadId)
	{
		const CThread* currThread = new CThread(id, false);
		if(!currThread->getIsValid() || (user->getUserId() != currThread->getUserId() && !user->getIsModerInAnyCategories()))
		{
			delete currThread;
			return "";
		}
		params["{RETURNPAGE}"] 	= CMenu::getInstance()->findCategoryById(currThread->getCatId())->getName();
		params["{EDITTYPE}"] 	= isThreadId ? "thread" : "post";
		params["{ID}"] 			= idStr;
		params["{USERID}"] 		= valueToString(user->getUserId());
		params["{TEXT}"] 		= currThread->getText();
		params["{isHIDDEN}"] 	= currThread->getIsHidden() ? "true" : "false";
		params["{isTHREAD}"] 	= "true";
		//params["{ATTACHMENTS}"] = currThread->getAttachmentString();

		tmpStr = editPageTemplate->getHTMLData(&params);

		delete currThread;
	}
	else
	{
		const CPost* currPost = new CPost(id);
		if(!currPost->getIsValid() || (user->getUserId() != currPost->getUserId() && !user->getIsModerInAnyCategories()))
		{
			delete currPost;
			return "";
		}

		int catId = 0;

		CDBManager* dbManager = CManagers::getInstance()->getDBManager();
		std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
		const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id`="+valueToString(currPost->getThreadId()), "LIMIT 1");
		//MYSQL_RES* result = DBRequest.createRequest("SELECT `threadCatId` FROM `Threads` WHERE `id`="+valueToString(currPost->getThreadId())+" LIMIT 1;");
		if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
		{
            try{ catId  = result->getIntValue(0,0); }
            catch(...){ catId  = 0; }
        }
        const CMenuCategory* menuCat = CMenu::getInstance()->findCategoryById(catId);
        std::string returnPage = "index";
        if(menuCat != NULL) returnPage = menuCat->getName();

		params["{RETURNPAGE}"] 	= returnPage;
		params["{EDITTYPE}"] 	= isThreadId ? "thread" : "post";
		params["{ID}"] 			= idStr;
		params["{USERID}"] 		= valueToString(user->getUserId());
		params["{TEXT}"] 		= currPost->getText();
		params["{isHIDDEN}"] 	= "false";
		params["{isTHREAD}"] 	= "false";
		//params["{ATTACHMENTS}"] = currPost->getAttachmentString();

		tmpStr = editPageTemplate->getHTMLData(&params);

		delete currPost;
	}

	params.clear();

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpStr;

	return contentTemplate->getHTMLData(&params);
}
