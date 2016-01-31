#include "CSettingsPage.h"

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


CSettingsPage::CSettingsPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{
	//ctor
}

CSettingsPage::~CSettingsPage()
{
	//dtor
}

std::string CSettingsPage::buildContent() const
{
	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	tmpString = "";

	const CHTMLTemplate* contentTemplate  = templateManager->findTemplate("content");
	const CHTMLTemplate* settingsTemplate = templateManager->findTemplate("settingsPage");
	if(contentTemplate == NULL || settingsTemplate == NULL) return "Missing content template";

	params["{USERID}"] 	= valueToString(user->getUserId());
	params["{USERKEY}"] = user->getUserKey();

	const std::vector<int>* hiddenPosts = user->getHiddenPosts();
	std::string hiddenPostsStr = "";
	for(auto it = (*hiddenPosts).begin(); it != (*hiddenPosts).end(); ++it)
	{
		const int currPost = (*it);
		hiddenPostsStr += "<span class='settings_links' oncontextmenu='hideClick(\"p"+valueToString(currPost)+"\", true)'><span class='replyToLink reply_to_link'>p" + valueToString(currPost) + "</span></span>  ; ";
	}
	params["{HIDDENPOSTS}"] = hiddenPostsStr;

	const std::vector<int>* hiddenThreads = user->getHiddenThreads();
	std::string hiddenThreadsStr = "";
	for(auto it = (*hiddenThreads).begin(); it != (*hiddenThreads).end(); ++it)
	{
		const int currThread = (*it);
		hiddenThreadsStr += "<span class='settings_links' oncontextmenu='hideClick(\"t"+valueToString(currThread)+"\", true)'><span class='replyToLink reply_to_link'>t" + valueToString(currThread) + "</span></span>  ; ";
	}
	params["{HIDDENTHREADS}"] 	= hiddenThreadsStr;

	const std::vector<int>* favoritePosts = user->getFavoritePosts();
	std::string favoritePostsStr = "";
	for(auto it = (*favoritePosts).begin(); it != (*favoritePosts).end(); ++it)
	{
		const int currPost = (*it);
		favoritePostsStr += "<span class='settings_links' oncontextmenu='addToFavorite(\"p"+valueToString(currPost)+"\", true)'><span class='replyToLink reply_to_link'>p" + valueToString(currPost) + "</span></span>  ; ";
	}
	params["{FAVORITEPOSTS}"] 	= favoritePostsStr;

	const std::vector<int>* favoriteThreads = user->getFavoriteThreads();
	std::string favoriteThreadsStr = "";
	for(auto it = (*favoriteThreads).begin(); it != (*favoriteThreads).end(); ++it)
	{
		const int currThread = (*it);
		favoriteThreadsStr += "<span class='settings_links' oncontextmenu='addToFavorite(\"t"+valueToString(currThread)+"\", true)'><span class='replyToLink reply_to_link'>t" + valueToString(currThread) + "</span></span>  ; ";
	}
	params["{FAVORITETHREADS}"] = favoriteThreadsStr;

	const std::vector<int>* fixedThreads = user->getFixedThreads();
	std::string fixedThreadsStr = "";
	for(auto it = (*fixedThreads).begin(); it != (*fixedThreads).end(); ++it)
	{
		const int currThread = (*it);
		fixedThreadsStr += "<span class='settings_links' oncontextmenu='fixThread(\"t"+valueToString(currThread)+"\", true)'><span class='replyToLink reply_to_link'>t" + valueToString(currThread) + "</span></span>  ; ";
	}
	params["{FIXEDTHREADS}"] 	= fixedThreadsStr;

	params["{RE}"] = user->getUserAgeRating() == 0 ? "selected" : "";
	params["{RT}"] = user->getUserAgeRating() == 1 ? "selected" : "";
	params["{RM}"] = user->getUserAgeRating() == 2 ? "selected" : "";
	params["{RA}"] = user->getUserAgeRating() == 3 ? "selected" : "";

	tmpString += settingsTemplate->getHTMLData(&params);
	params.clear();

	params["{LEFTPANEL}"] 	= buildLeftPanel(user);
	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpString;

	return contentTemplate->getHTMLData(&params);
}
