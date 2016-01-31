#include "CAdviceBoard.h"

#include "novemberlib/helpers/CMessageParser.h"
#include "novemberlib/helpers/CTemplateHelper.h"
#include "novemberlib/helpers/CPathsHelper.h"
#include "novemberlib/helpers/CTextHelper.h"

#include "CSiteCommandManager.h"
#include "CSiteResourceManager.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/managers/CMySQLDBManager.h"
#include "novemberlib/managers/CPageManager.h"
#include "novemberlib/managers/CSessionManager.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "CMenu.h"
#include "CSiteErrorPage.h"
#include "CUser.h"

#include "CSiteIndexPage.h"
#include "CCatPage.h"
#include "CPremodPage.h"
#include "CComplainPage.h"
#include "CEditPage.h"
#include "CFavoritesPage.h"
#include "CHelpPage.h"
#include "CRulesPage.h"
#include "CSessionsPage.h"
#include "CSettingsPage.h"
#include "CSurveyPage.h"
#include "CThreadPage.h"
#include "CUserContentPage.h"

#include <vector>
#include <string>
#include <memory>

CAdviceBoard::CAdviceBoard()
{
	CManagers::getInstance()->setResourceManager(new CSiteResourceManager());
	CManagers::getInstance()->setCommandManager(new CSiteCommandManager());
}

CAdviceBoard::~CAdviceBoard()
{

}

void CAdviceBoard::init()
{
	CFCGIApp::init();
	initDB();

	CManagers::getInstance()->getSessionManager()->setUserType<CUser>();

	// Paths INIT
	CPathsHelper* paths = CPathsHelper::getInstance();
	// User content paths
	paths->addPath("app", "application/*");
	paths->addPath("txt", "text/*");
	paths->addPath("aud", "audio/*");
	paths->addPath("img", "image/*");
	paths->addPath("img/preview", "image/*");
	paths->addPath("vid", "video/*");
	// Site content paths (readonly)
	paths->addPath("", "image/vnd.microsoft.icon", true);
	paths->addPath("www", "application/*, text/*", true);
	paths->addPath("stl", "text/css", true);
	paths->addPath("scr", "text/javascript, application/x-shockwave-flash", true);
	paths->addPath("scr/jquery", "text/javascript", true);
	paths->addPath("img/previewicons", "image/*", true);
	paths->addPath("img/controls", "image/*", true);

	// Temlates INIT
	CTemplateHelper* templates = CTemplateHelper::getInstance();
	const CPath* templatesDir = paths->getPath("www");
	templates->loadTemplate(templatesDir, "header_t.html", 	"header");
	templates->loadTemplate(templatesDir, "footer_t.html", 	"footer");
	templates->loadTemplate(templatesDir, "content.html", 	"content");
	templates->loadTemplate(templatesDir, "indexPage.html", "indexPage");
    templates->loadTemplate(templatesDir, "post.html", 		"post");
    templates->loadTemplate(templatesDir, "thread.html", 	"thread");
    templates->loadTemplate(templatesDir, "controls.html", 	"controls");
    templates->loadTemplate(templatesDir, "attach.html", 	"attach");
    templates->loadTemplate(templatesDir, "survey.html", 	"survey");
    templates->loadTemplate(templatesDir, "newsTemplate.html", "news");
    templates->loadTemplate(templatesDir, "surveyResult.html", "surveyResult");
    templates->loadTemplate(templatesDir, "surveyCreationForm.html", "surveysPage");
    templates->loadTemplate(templatesDir, "leftPanel.html", 	"leftPanel");
    templates->loadTemplate(templatesDir, "newThreadForm.html", "newThreadForm");
    templates->loadTemplate(templatesDir, "newPostForm.html", 	"newPostForm");
    templates->loadTemplate(templatesDir, "sessionPage.html", 	"sessionPage");
    templates->loadTemplate(templatesDir, "settingsPage.html", "settingsPage");
    templates->loadTemplate(templatesDir, "userPage.html", 		"userPage");
    templates->loadTemplate(templatesDir, "editPage.html", 		"editPage");
    templates->loadTemplate(templatesDir, "rulesPage.html", 	"rulesPage");
    templates->loadTemplate(templatesDir, "helpPage.html", 		"helpPage");

	// Parser INIT
	CMessageParser* messageParser = CMessageParser::getInstance();
	messageParser->addAllowedTag("p");
	messageParser->addAllowedTag("b");
	messageParser->addAllowedTag("i");
	messageParser->addAllowedTag("u");
	messageParser->addAllowedTag("li");
	messageParser->addAllowedTag("ol");
	messageParser->addAllowedTag("ul");
	messageParser->addAllowedTag("h1");
	messageParser->addAllowedTag("h2");
	messageParser->addAllowedTag("h3");
	messageParser->addAllowedTag("br", false);

	// Page manager INIT
    CPageManager* pageManager = CManagers::getInstance()->getPageManager();
    pageManager->setErrorPage(new CSiteErrorPage("errorPage","",""));
    pageManager->addPageType<CSiteIndexPage>("index");
    pageManager->addPageType<CCatPage>("b");
    pageManager->addPageType<CCatPage>("mu");
    pageManager->addPageType<CCatPage>("tv");
    pageManager->addPageType<CCatPage>("lor");
    pageManager->addPageType<CCatPage>("bo");
    pageManager->addPageType<CCatPage>("vg");
    pageManager->addPageType<CCatPage>("a");
    pageManager->addPageType<CCatPage>("sb");
    pageManager->addPageType<CCatPage>("d");
    pageManager->addPageType<CThreadPage>("thread");
    pageManager->addPageType<CSessionsPage>("sessions");
    pageManager->addPageType<CSettingsPage>("settings");
    pageManager->addPageType<CFavoritesPage>("favorites");
    pageManager->addPageType<CPremodPage>("premod");
    pageManager->addPageType<CUserContentPage>("usercontent");
    pageManager->addPageType<CComplainPage>("complains");
    pageManager->addPageType<CEditPage>("edit");
    pageManager->addPageType<CSurveyPage>("surveys");
	pageManager->addPageType<CRulesPage>("ruuru");
    pageManager->addPageType<CHelpPage>("help");

    // Text helper INIT
    CTextHelper* textHelper = CTextHelper::getInstance();
}

void CAdviceBoard::initDB()
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequestThreads(dbManager->createDBRequest());
	std::shared_ptr<CDBRequest> dbRequestPosts(dbManager->createDBRequest());

	if (!dbManager->getIsConnInit())
	{
		CLog::getInstance()->addError("DB connect is not inited");
		return;
	}

	const CDBRequestResult* resultThreads = dbRequestThreads->selectRequest(CDBValues("id"), "Threads");
    if(dbRequestThreads->getIsLastQuerySuccess() && resultThreads != NULL && resultThreads->getRowsCnt() > 0)
    {
		int threadsCnt = resultThreads->getRowsCnt();
        for(int j = 0; j < threadsCnt; j++)
        {
            int threadId = 0;
            threadId = resultThreads->getIntValue(j,0);
            if(threadId == 0) continue;

			const CDBRequestResult* resultPosts = dbRequestPosts->selectRequest(CDBValues("id"), "Posts", "`postThreadId` = " + valueToString(threadId));
			if(dbRequestPosts->getIsLastQuerySuccess() && resultPosts != NULL && resultPosts->getRowsCnt() > 0)
			{
				int postsCnt = resultPosts->getRowsCnt();
				dbRequestPosts->updateRequest("Threads", "`threadPostsCnt` = "+valueToString(postsCnt), "`id` = "+valueToString(threadId));
			}
        }
    }
}
