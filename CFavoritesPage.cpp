#include "CFavoritesPage.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/helpers/CTemplateHelper.h"
#include "novemberlib/helpers/CMessageParser.h"
#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/CHTMLTemplate.h"

#include "CThread.h"
#include "CPost.h"
#include "CUser.h"


CFavoritesPage::CFavoritesPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	//ctor
}

CFavoritesPage::~CFavoritesPage()
{
	//dtor
}


std::string CFavoritesPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;

	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	const CHTMLTemplate* contentTemplate  = templateManager->findTemplate("content");
	if(contentTemplate == NULL) return "Missing content template";

	if(!user->getIsValid())return "User not valid";

	const std::vector<int>* threadsId = user->getFavoriteThreads();
	const std::vector<int>* postsId	  = user->getFavoritePosts();

	tmpString += "<input type='hidden' name='return_page' value='";
	tmpString += getPageName();
	tmpString += "' /> \n";
	tmpString += buildNewPostForm(getPageName(), user);
	tmpString += "\n";

	if(threadsId->size() > 0)
	{
		tmpString += "<h2>Избранные темы</h2> \n";
		tmpString += "<br> \n";
		for(unsigned int i = 0; i < threadsId->size(); i++)
		{
			const CThread* currThread = new CThread((*threadsId)[i]);
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
	if(postsId->size() > 0)
	{
		tmpString += "<h2>Избранные сообщения</h2> \n";
		tmpString += "<br> \n";
		for(unsigned int i = 0; i < postsId->size(); i++)
		{
			const CPost* currPost = new CPost((*postsId)[i]);
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
