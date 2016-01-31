#include "CSiteCommandManager.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/md5.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/managers/CManagers.h"
#include "novemberlib/managers/CPageManager.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/helpers/CConfigHelper.h"

#include "CMenu.h"
#include "CThread.h"
#include "CPost.h"
#include "CSitePage.h"
#include "CUser.h"
#include "CSurvey.h"

#include "CSiteResourceManager.h"

CSiteCommandManager::CSiteCommandManager()
{
	//ctor
}

CSiteCommandManager::~CSiteCommandManager()
{
	//dtor
}


CCommandResult CSiteCommandManager::processCommand(CFCGIRequest* currRequest)
{
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	const std::string command = request->get.get("command","");
	CCommandResult result;
	result.setData("Not valid user");
	if(command.size() == (size_t)0)
	{
        result.setIsValid(false);
        result.setData("Not valid command");
        return result;
	}

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
	if(!user->getIsValid() || user->getUserType(0) == CUser::UT_NONE) return result;

    if(command == "newthread")		return newThreadCommand(currRequest);
    if(command == "newpost")		return newPostCommand(currRequest);
    if(command == "getthreadposts")	return getThreadPostsCommand(currRequest);
    if(command == "getthreads")		return getThreadsCommand(currRequest);
	if(command == "del")			return delCommand(currRequest);
	if(command == "fixtoall")		return fixToAllCommand(currRequest);
	if(command == "hidetoall")      return hideToAllCommand(currRequest);
    if(command == "fix")			return fixCommand(currRequest);
	if(command == "favorite")		return favoriteCommand(currRequest);
	if(command == "hide")			return hideCommand(currRequest);
	if(command == "allowpremod")	return allowPremodCommand(currRequest);
    if(command == "complain")		return complainCommand(currRequest);
    if(command == "edit")			return editCommand(currRequest);
    if(command == "login")			return loginCommand(currRequest);
    if(command == "setuseragerating")return setUserAgeRatingCommand(currRequest);
    if(command == "ban")			return banCommand(currRequest);
    if(command == "getmessage")     return getMessage(currRequest);
    if(command == "answertosurvey") return answerToSurveyCommand(currRequest);
    if(command == "createsurvey")   return createSurveyCommand(currRequest);
    if(command == "endsurvey")      return endSurveyCommand(currRequest);
    if(command == "delsurvey")      return delSurveyCommand(currRequest);

    result.setIsValid(false);
    result.setData("Not valid command");
	return result;
}

CCommandResult CSiteCommandManager::getMessage(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 	= request->post.get("id","");
    std::string returnPage  = request->post.get("retpage","");

    time_t currUnixTime;
    time(&currUnixTime);

    bool isThreadId = false;
    bool isPostId = false;

    if(id.size() == 0) return commandResult;
    if(id[0] == 't') isThreadId = true;
    if(id[0] == 'p') isPostId = true;
    if(!isThreadId && !isPostId) return commandResult;
    id[0] = '0';

    int threadId = 0;
    int postId = 0;

    try
    {
        if(isThreadId) 	threadId = std::stoi(id);
        if(isPostId) 	postId 	 = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

    if(isThreadId && threadId > 0)
    {
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadPosition"), "Threads", "`id` = "+valueToString(threadId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            commandResult.setIsSuccess(true);
            commandResult.setData(CSitePage::buildThread(threadId, user, true));
        }
        else
        {
            commandResult.setData("thread not exist");
        }
    }
    if(isPostId && postId > 0)
    {
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("postThreadId"), "Posts", "`id` = "+valueToString(postId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            commandResult.setIsSuccess(true);
            commandResult.setData(CSitePage::buildPost(postId, user, true));
        }
        else
        {
            commandResult.setData("post not exist");
        }
    }

    return commandResult;
}

CCommandResult CSiteCommandManager::newThreadCommand(CFCGIRequest* currRequest) const
{
    CConfigHelper* gs = CConfigHelper::getInstance();

	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string returnPage 	= request->post.get("ret_page","");
    std::string userName 	= dbManager->getEscapeString(request->post.get("name",""));
    std::string subject 	= dbManager->getEscapeString(request->post.get("subject",""));
    std::string message 	= dbManager->getEscapeString(request->post.get("message",""));
    std::string strCatId 	= request->post.get("cat_id","");
    std::string ageRatingStr= request->post.get("age_rating","");
    std::string surveyIdStr = request->post.get("surveys_ids","");
    std::string fakeLoginStr= request->post.get("login","fake");
    std::string hiddenStr 	= request->post.get("hide","off");

    time_t currUnixTime;
    time(&currUnixTime);

    if(returnPage.size() == 0 || strCatId.size() == 0 || subject.size() == 0 ) return commandResult;
    if((int)message.size() > gs->getIntParamValue("maxMessageLen", 10240) || (int)userName.size() >  gs->getIntParamValue("maxCaptionLen", 256) || (int)subject.size() > gs->getIntParamValue("maxCaptionLen", 256))return commandResult;
    if(fakeLoginStr.length() > 0) return commandResult;

    int catId = 0;
    int ageRating = 0;
    int surveyId = 0;
    int isHidden = hiddenStr == "on" ? 1 : 0;
    try
    {
        catId = std::stoi(strCatId);
        ageRating = std::stoi(ageRatingStr);
        surveyId = std::stoi(surveyIdStr);
    }
    catch(...)
    {
        return commandResult;
    }

    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    guestUserCheck(currRequest);
    if(isUserReadOnly(currRequest))
    {
        commandResult.setData("User in read only mode");
        return commandResult;
    }
    if(!checkLastChangeTime(currRequest))
    {
        commandResult.setData("Too many requests");
        return commandResult;
    }

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

    int threadPosition = 0;
    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadPosition"), "Threads", "", "ORDER BY `threadPosition` DESC LIMIT 1");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
        threadPosition = result->getIntValue(0, 0) + 1;
    }

    const std::string attachStr = handleFiles(currRequest, true);

    int isNeedPremod = gs->getBoolParamValue("isNeedPremod", false) ? 1 : 0;
    const CMenuCategory* menuCat = CMenu::getInstance()->findCategoryByName(returnPage);
    if(menuCat && menuCat->getIsNeedPremod()) isNeedPremod = 1;
    if(user->getUserType(catId) > CUser::UT_USER) isNeedPremod = 0;

    if(surveyId == 0) surveyIdStr = "NULL";
    else
    {
        surveyIdStr = valueToString(surveyId);
        result = dbRequest->selectRequest(CDBValues("id"), "Threads", "`threadSurveyId`="+ valueToString(surveyId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            surveyIdStr = "NULL";
        }
    }

    CDBValues fields;
    fields.addValue("threadCaption", subject);
    fields.addValue("threadPostsCnt", 0);
    fields.addValue("threadCreationDate", (long)currUnixTime);
    fields.addValue("threadText", message);
    fields.addValue("threadCatId", catId);
    fields.addValue("threadUserId", user->getUserId());
    fields.addValue("threadUserName", userName);
    fields.addValue("threadPosition", threadPosition);
    fields.addValue("threadAttachment", attachStr);
    fields.addValue("threadAnswersIds", "");
    fields.addValue("isFixed", 0);
    fields.addValue("threadIsNeedPremod", isNeedPremod);
    fields.addValue("threadComplainCnt", 0);
    fields.addValue("threadModTime", 0);
    fields.addValue("threadAgeRating", ageRating);
    fields.addValue("threadSurveyId", surveyIdStr, false);
    fields.addValue("threadIsHidden", isHidden);
    dbRequest->insertRequest(fields, "Threads");

    if(!dbRequest->getIsLastQuerySuccess()) return commandResult;

    dbRequest->updateRequest("Category", "`CategoryTheardsCnt` = `CategoryTheardsCnt` + 1", "`id`=" + valueToString(catId), "LIMIT 1" );
    dbRequest->updateRequest("Users", "`lastChange` = "+valueToString(currUnixTime), "`id`=" + valueToString(currRequest->getUser()->getUserId()), "LIMIT 1");

    commandResult.setIsSuccess(true);
    commandResult.setData("<meta http-equiv='refresh' content='0; url=?page="+returnPage+"' />");

    return commandResult;
}

CCommandResult CSiteCommandManager::newPostCommand(CFCGIRequest* currRequest) const
{
    CConfigHelper* gs = CConfigHelper::getInstance();
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string returnPage 	= request->post.get("ret_page","");
    std::string userName 	= dbManager->getEscapeString(request->post.get("name",""));
    std::string message 	= dbManager->getEscapeString(request->post.get("message",""));
    std::string strThreadId = request->post.get("thread_id","");
    std::string parentIdStr = request->post.get("parent_id","");
    std::string sageStr 	= request->post.get("sage","");
    std::string ageRatingStr= request->post.get("age_rating","");
    std::string fakeLoginStr= request->post.get("login","fake");

    time_t currUnixTime;
    time(&currUnixTime);

    if(returnPage.size() == 0 || strThreadId.size() == 0 || parentIdStr.size() == 0 ) return commandResult;
    if((int)message.size() > gs->getIntParamValue("maxMessageLen", 10240) || (int)userName.size() >  gs->getIntParamValue("maxCaptionLen", 256)) return commandResult;
    if(fakeLoginStr.length() > 0 ) return commandResult;

    int threadId = 0;
    int ageRating = 0;
    bool isSage = sageStr == "on" ? true : false;

    bool isParentThread = false;
    if(parentIdStr[0] == 't') isParentThread = true;
    parentIdStr[0] = '0';

    try
    {
        threadId = std::stoi(strThreadId);
        ageRating = std::stoi(ageRatingStr);
    }
    catch(...)
    {
        return commandResult;
    }

    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    guestUserCheck(currRequest);
    if(isUserReadOnly(currRequest))
    {
        commandResult.setData("User in read only mode");
        return commandResult;
    }
    if(!checkLastChangeTime(currRequest))
    {
        commandResult.setData("Too many requests");
        return commandResult;
    }

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

    if(isParentThread) parentIdStr[0] = 't';
    else  parentIdStr[0] = 'p';

    const std::string attachStr = handleFiles(currRequest, false);

    int postsCnt = -1;
    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadPostsCnt"), "Threads", "`id` = "+valueToString(threadId), "LIMIT 1");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
        postsCnt = result->getIntValue(0,0, -1);
    }
    if(postsCnt == -1 || postsCnt >= gs->getIntParamValue("maxPostsInThread", 500))
    {
        commandResult.setData("");
        return commandResult;
    }

    int isNeedPremod = gs->getBoolParamValue("isNeedPremod", false) ? 1 : 0;
    const CMenuCategory* menuCat = CMenu::getInstance()->findCategoryByName(returnPage);
    if(menuCat && menuCat->getIsNeedPremod()) isNeedPremod = 1;
    int catId = 0;
    if(menuCat) catId = menuCat->getId();
    if(user->getUserType(catId) > CUser::UT_USER) isNeedPremod = 0;

    CDBValues fields;
    fields.addValue("postCreationDate", (long)currUnixTime);
    fields.addValue("postText", message);
    fields.addValue("postThreadId", threadId);
    fields.addValue("postUserId", user->getUserId());
    fields.addValue("postUserName", userName);
    fields.addValue("postAttachment", attachStr);
    fields.addValue("postAnswersIds", "");
    fields.addValue("postParentId", parentIdStr);
    fields.addValue("postIsNeedPremod", isNeedPremod);
    fields.addValue("postComplainCnt", 0);
    fields.addValue("postModTime", 0);
    fields.addValue("postAgeRating", ageRating);
    dbRequest->insertRequest(fields, "Posts");

    if(!dbRequest->getIsLastQuerySuccess())
    {
        commandResult.setData("DB error");
        return commandResult;
    }

    int newPostId = 0;
    result = dbRequest->selectRequest(CDBValues("id"), "Posts", "", "ORDER BY `id` DESC LIMIT 1");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
        newPostId = result->getIntValue(0,0);
    }
    if(newPostId == 0) return commandResult;
    dbRequest->updateRequest("Users", "`lastChange` = "+valueToString(currUnixTime), "`id`=" + valueToString(currRequest->getUser()->getUserId()), "LIMIT 1");

    if(isSage)dbRequest->updateRequest("Threads", "`threadPostsCnt` = `threadPostsCnt` + 1", "`id`=" + valueToString(threadId), "LIMIT 1");
    else dbRequest->updateRequest("Threads", "`threadPostsCnt` = `threadPostsCnt` + 1, `threadPosition` = `threadPosition` + 1", "`id`=" + valueToString(threadId), "LIMIT 1");

    if(isParentThread)
    {
        parentIdStr[0] = '0';
        dbRequest->updateRequest("Threads", "`threadAnswersIds` = CONCAT(`threadAnswersIds`,';"+valueToString(newPostId)+"')", "`id`=" + valueToString(parentIdStr), "LIMIT 1");
    }
    else
    {
        parentIdStr[0] = '0';
        dbRequest->updateRequest("Posts", "`postAnswersIds` = CONCAT(`postAnswersIds`,';"+valueToString(newPostId)+"')", "`id`=" + valueToString(parentIdStr), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("<meta http-equiv='refresh' content='0; url=?page="+returnPage+"' />");
    return commandResult;
}

CCommandResult CSiteCommandManager::getThreadsCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
    CFCGIRequestHandler* request  = currRequest->getRequestForModify();
    CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string catIdStr		 = request->post.get("catid","");
    std::string lastThreadPosStr = request->post.get("lastthread","");
    std::string threadsToLoadStr = request->post.get("thcnt","");
    std::string returnPage 		 = request->post.get("retpage","");
    std::string exsistThreadsIds = request->post.get("exsistids","");

    time_t currUnixTime;
    time(&currUnixTime);

    int catId = 0;
    int lastThreadPos = 0;
    if(catIdStr.size() == 0) return commandResult;

    try
    {
        catId = std::stoi(catIdStr);
        lastThreadPos = std::stoi(lastThreadPosStr);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }

    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "Threads", "`threadCatId`="+valueToString(catId)+" AND `threadPosition` <= " + valueToString(lastThreadPos), "ORDER BY `threadPosition` DESC LIMIT 5");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
        for(int j = 0; j < result->getRowsCnt(); j++)
        {
            int threadId = 0;
            threadId  = result->getIntValue(j, 0);

            if(threadId == 0) continue;
            std::string idStr = "t" + valueToString(threadId);
            std::size_t found = exsistThreadsIds.find(idStr);
            if(found != std::string::npos) continue;

            CThread newThread(threadId, false);
            if(!newThread.getIsValid())
            {
                continue;
            }
            commandResult.setData(CSitePage::buildThread(&newThread, dynamic_cast<const CUser*>(currRequest->getUser())));
        }
    }

    commandResult.setIsSuccess(true);
    return commandResult;
}

CCommandResult CSiteCommandManager::getThreadPostsCommand(CFCGIRequest* currRequest) const
{
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 	= request->post.get("id","");

    time_t currUnixTime;
    time(&currUnixTime);

    int threadId = 0;
    if(id.size() == 0) return commandResult;

    try
    {
        threadId = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }

    const CThread currThread(threadId, true);
    if(!currThread.getIsValid())
    {
        commandResult.setData("Thread not valid");
        return commandResult;
    }

    commandResult.setData("");
    const std::vector<CPost*>* currThreardPosts = currThread.getPosts();
    for(unsigned int i = 0; i < currThreardPosts->size(); i++)
    {
        const CPost* currPost = (*currThreardPosts)[i];
        if(!currPost->getIsValid()) continue;

        commandResult.appendData(CSitePage::buildPost(currPost, dynamic_cast<const CUser*>(currRequest->getUser())));

    }
    commandResult.setIsSuccess(true);
    return commandResult;
}

CCommandResult CSiteCommandManager::editCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CConfigHelper* gs = CConfigHelper::getInstance();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 	= request->post.get("id","");
    std::string returnPage 	= request->post.get("ret_page","");
    std::string message 	= request->post.get("message","");
    std::string delFilesStr = request->post.get("delfiles","");
    std::string hiddenStr 	= request->post.get("hide","off");
    bool isThreadId = false;
    bool isPostId = false;
    bool isDelAttach = delFilesStr == "del" ? true : false;
    int  isHidden = hiddenStr == "on" ? 1 : 0;

    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    if(id[0] == 't') isThreadId = true;
    if(id[0] == 'p') isPostId = true;
    if(!isThreadId && !isPostId) return commandResult;
    id[0] = '0';

    int threadId = 0;
    int postId = 0;
    try
    {
        if(isThreadId) 	threadId = std::stoi(id);
        if(isPostId) 	postId 	 = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }

    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    guestUserCheck(currRequest);
     if(isUserReadOnly(currRequest))
    {
        commandResult.setData("User in read only mode");
        return commandResult;
    }

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

    if(isThreadId && threadId > 0 )
    {
        int userId = 0;
        long creationDate = 0;
        int catId = 0;
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadUserId,threadCreationDate,threadCatId"), "Threads", "`id`="+valueToString(threadId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            userId  = result->getIntValue(0, 0);
            catId   = result->getIntValue(0, 2);
            creationDate  = result->getLongValue(0, 1);
        }

        if(user->getUserType(catId) < CUser::UT_MODER && user->getUserId() != userId)
        {
            commandResult.setData("Access denied");
            return commandResult;
        }
        if(user->getUserType(catId) < CUser::UT_MODER && currUnixTime - creationDate > gs->getIntParamValue("maxTimeToEdit", 1000))
        {
            commandResult.setData("Too late...");
            return commandResult;
        }
        CUser ownerUser(userId);
        ownerUser.fillUserDataById(userId);
        if(user->getUserType(catId) < ownerUser.getUserType(catId))
        {
            commandResult.setData("Access denied");
            return commandResult;
        }

        dbRequest->updateRequest("Threads", "`threadText` = '"+dbManager->getEscapeString(message)+"', `threadModTime` = "+valueToString(currUnixTime)+", `threadIsHidden` = "+valueToString(isHidden), "`id`=" + valueToString(threadId), "LIMIT 1");
        if(isDelAttach) dbRequest->updateRequest("Threads", "`threadAttachment` = ''", "`id`=" + valueToString(threadId), "LIMIT 1");
    }
    if(isPostId && postId > 0 )
    {
        int userId = 0;
        long creationDate = 0;
        int postThreadId = 0;
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("postUserId,postCreationDate,postThreadId"), "Posts", "`id`="+valueToString(postId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            userId  = result->getIntValue(0, 0);
            postThreadId = result->getIntValue(0, 2);
            creationDate  = result->getLongValue(0, 1);
        }

        int catId = 0;
        result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id`="+ valueToString(postThreadId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
           catId = result->getIntValue(0,0);
        }

        if(user->getUserType(catId) < CUser::UT_MODER && user->getUserId() != userId)
        {
            commandResult.setData("Access denied");
            return commandResult;
        }
        if(user->getUserType(catId) < CUser::UT_MODER && currUnixTime - creationDate > gs->getIntParamValue("maxTimeToEdit", 1000))
        {
            commandResult.setData("Too late...");
            return commandResult;
        }
        CUser ownerUser(userId);
        ownerUser.fillUserDataById(userId);
        if(user->getUserType(catId) < ownerUser.getUserType(catId))
        {
            commandResult.setData("Access denied");
            return commandResult;
        }

        dbRequest->updateRequest("Posts", "`postText` = '"+dbManager->getEscapeString(message)+"', `postModTime` = "+valueToString(currUnixTime),"`id`=" + valueToString(postId), "LIMIT 1" );
        if(isDelAttach) dbRequest->updateRequest("Posts", "`postAttachment` = ''", "`id`=" + valueToString(postId), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("<meta http-equiv='refresh' content='0; url=?page="+returnPage+"' />");
    return commandResult;
}

CCommandResult CSiteCommandManager::delCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");
	CConfigHelper* gs = CConfigHelper::getInstance();

    std::string id 	= request->post.get("id","");
    bool isThreadId = false;
    bool isPostId = false;

    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    if(id[0] == 't') isThreadId = true;
    if(id[0] == 'p') isPostId = true;
    if(!isThreadId && !isPostId) return commandResult;
    id[0] = '0';

    int threadId = 0;
    int postId = 0;
    try
    {
        if(isThreadId) 	threadId = std::stoi(id);
        if(isPostId) 	postId 	 = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    guestUserCheck(currRequest);
    if(isUserReadOnly(currRequest))
    {
        commandResult.setData("User in read only mode");
        return commandResult;
    }

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

    int userId = 0;
    if(isThreadId && threadId > 0)
    {
        int catId = 0;
        int postsCnt = 0;
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadCatId,threadUserId,threadPostsCnt"), "Threads", "`id`="+valueToString(threadId), "LIMIT 1" );
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            catId  = result->getIntValue(0,0);
            userId  = result->getIntValue(0,1);
            postsCnt  = result->getIntValue(0,2);
        }
        if(catId == 0) return commandResult;
        if(user->getUserType(catId) < CUser::UT_MODER && user->getUserId() != userId)
        {
            commandResult.setData("Access denied");
            return commandResult;
        }
        if(user->getUserType(catId) < CUser::UT_MODER && postsCnt > gs->getIntParamValue("maxPostsInThreadForDel", 30))
        {
            commandResult.setData("Too many posts in thread");
            return commandResult;
        }
        CUser ownerUser(userId);
        ownerUser.fillUserDataById(userId);
        if(user->getUserType(catId) < ownerUser.getUserType(catId))
        {
            commandResult.setData("Access denied");
            return commandResult;
        }

        dbRequest->deleteRequest("Threads", "`id`="+valueToString(threadId));
        if(!dbRequest->getIsLastQuerySuccess())
        {
            commandResult.setData("DB error");
            return commandResult;
        }
        dbRequest->updateRequest("Category", "`CategoryTheardsCnt` = CategoryTheardsCnt - 1", "`id`=" + valueToString(catId), "LIMIT 1" );
    }
    if(isPostId && postId > 0)
    {
        int threadId = 0;
        long creationDate = 0;
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("postThreadId,postUserId,postCreationDate"), "Posts", "`id`="+valueToString(postId), "LIMIT 1" );
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            threadId  = result->getIntValue(0,0);
            userId  = result->getIntValue(0,1);
            creationDate  = result->getLongValue(0,2);
        }
        if(threadId == 0) return commandResult;

        int catId = 0;
        result = dbRequest->selectRequest(CDBValues("threadCatId"),"Threads" ,"`id`="+ valueToString(threadId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            catId = result->getIntValue(0,0);
        }

        if(user->getUserType(catId) < CUser::UT_MODER && user->getUserId() != userId)
        {
            commandResult.setData("Access denied");
            return commandResult;
        }
        if(user->getUserType(catId) < CUser::UT_MODER && currUnixTime - creationDate > gs->getIntParamValue("maxTimeToEdit", 1000))
        {
            commandResult.setData("Too late");
            return commandResult;
        }
        CUser ownerUser(userId);
        ownerUser.fillUserDataById(userId);
        if(user->getUserType(catId) < ownerUser.getUserType(catId))
        {
            commandResult.setData("Access denied");
            return commandResult;
        }

        dbRequest->deleteRequest("Posts", "`id`="+valueToString(postId), "LIMIT 1");
        if(!dbRequest->getIsLastQuerySuccess())
        {
            commandResult.setData("DB error");
            return commandResult;
        }
        dbRequest->updateRequest("Threads", "`threadPostsCnt` = threadPostsCnt - 1", "`id`=" + valueToString(threadId), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::fixToAllCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 	  	  = request->post.get("id","");
    std::string metod 	  = request->post.get("metod","");
    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    id[0] = '0';

    int threadId = 0;
    try
    {
        threadId = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }

    int catId = 0;
    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id`="+ valueToString(threadId), "LIMIT 1");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
       catId = result->getIntValue(0,0);
    }

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    if(user->getUserType(catId) < CUser::UT_MODER)
    {
        commandResult.setData("Access denied");
        return commandResult;
    }

    if(threadId > 0 )
    {
        if(metod == "remove") dbRequest->updateRequest("Threads", "`isFixed` = 0",  "`id`=" + valueToString(threadId), "LIMIT 1");
        else dbRequest->updateRequest("Threads", "`isFixed` = 1",  "`id`=" + valueToString(threadId), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::hideToAllCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 	  	  = request->post.get("id","");
    std::string metod 	  = request->post.get("metod","");
    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    id[0] = '0';

    int threadId = 0;
    try
    {
        threadId = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

    int catId = 0;
    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id`="+ valueToString(threadId), "LIMIT 1");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
        catId = result->getIntValue(0,0);
    }
    if(user->getUserType(catId) < CUser::UT_MODER)
    {
        commandResult.setData("Access denied");
        return commandResult;
    }

    if(threadId > 0 )
    {
        if(metod == "remove") dbRequest->updateRequest("Threads", "`threadIsHidden` = 0",  "`id`=" + valueToString(threadId), "LIMIT 1");
        else dbRequest->updateRequest("Threads", "`threadIsHidden` = 1",  "`id`=" + valueToString(threadId), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::fixCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 	  	  = request->post.get("id","");
    std::string metod 	  = request->post.get("metod","");
    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    id[0] = '0';

    int threadId = 0;
    try
    {
        threadId = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    guestUserCheck(currRequest);
    CUser* user = dynamic_cast<CUser*>(currRequest->getUserForModify());

    if(threadId > 0 )
    {
        if(metod == "remove") user->removeFromFixedThread(threadId);
        else user->addToFixedThread(threadId);

        const std::string fixThreadsString = user->getFixedThreadsStr();
        dbRequest->updateRequest("Users", "`fixedThreads` = '" + fixThreadsString + "'",  "`id`=" + valueToString(user->getUserId()), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::favoriteCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 		= request->post.get("id","");
    std::string metod 	= request->post.get("metod","");
    bool isThreadId = false;
    bool isPostId = false;

    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    if(id[0] == 't') isThreadId = true;
    if(id[0] == 'p') isPostId = true;
    if(!isThreadId && !isPostId) return commandResult;
    id[0] = '0';

    int threadId = 0;
    int postId = 0;
    try
    {
        if(isThreadId) 	threadId = std::stoi(id);
        if(isPostId) 	postId 	 = std::stoi(id);

    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    guestUserCheck(currRequest);
    CUser* user = dynamic_cast<CUser*>(currRequest->getUserForModify());

    if(isThreadId && threadId > 0 )
    {
        if(metod == "remove") user->removeFromFavoriteThread(threadId);
        else user->addToFavoriteThread(threadId);

        const std::string favoriteThreadsString = user->getFavoriteThreadsStr();
        dbRequest->updateRequest("Users", "`favoriteThreads` = '" + favoriteThreadsString + "'",  "`id`=" + valueToString(user->getUserId()), "LIMIT 1");
    }
    if(isPostId && postId > 0 )
    {
        if(metod == "remove") user->removeFromFavoritePost(postId);
        else user->addToFavoritePost(postId);

        const std::string favoritePostString = user->getFavoritePostsStr();
        dbRequest->updateRequest("Users", "`favoritePosts` = '" + favoritePostString + "'",  "`id`=" + valueToString(user->getUserId()), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::hideCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 		= request->post.get("id","");
    std::string metod 	= request->post.get("metod","");
    bool isThreadId = false;
    bool isPostId = false;

    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    if(id[0] == 't') isThreadId = true;
    if(id[0] == 'p') isPostId = true;
    if(!isThreadId && !isPostId) return commandResult;
    id[0] = '0';

    int threadId = 0;
    int postId = 0;
    try
    {
        if(isThreadId) 	threadId = std::stoi(id);
        if(isPostId) 	postId 	 = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    guestUserCheck(currRequest);
    CUser* user = dynamic_cast<CUser*>(currRequest->getUserForModify());

    if(isThreadId && threadId > 0 )
    {
        if(metod == "remove") user->unhideThread(threadId);
        else user->hideThread(threadId);

        const std::string hiddenThreadsString = user->getHiddenThreadsStr();
        dbRequest->updateRequest("Users", "`hiddenThreads` = '" + hiddenThreadsString + "'",  "`id`=" + valueToString(user->getUserId()), "LIMIT 1");
    }
    if(isPostId && postId > 0 )
    {
        if(metod == "remove") user->unhidePost(postId);
        else user->hidePost(postId);

        const std::string hiddenPostString = user->getHiddenPostsStr();
        dbRequest->updateRequest("Users", "`hiddenPosts` = '" + hiddenPostString + "'",  "`id`=" + valueToString(user->getUserId()), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::allowPremodCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 	= request->post.get("id","");
    bool isThreadId = false;
    bool isPostId = false;

    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    if(id[0] == 't') isThreadId = true;
    if(id[0] == 'p') isPostId = true;
    if(!isThreadId && !isPostId) return commandResult;
    id[0] = '0';

    int threadId = 0;
    int postId = 0;
    try
    {
        if(isThreadId) 	threadId = std::stoi(id);
        if(isPostId) 	postId 	 = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

    int catId = 0;
    if(isThreadId)
    {
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id`="+ valueToString(threadId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            catId = result->getIntValue(0,0);
        }
    }
    if(isPostId)
    {
        int postThreadId = 0;
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("postThreadId"), "Posts", "`id`="+ valueToString(postId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            postThreadId = result->getIntValue(0,0);
        }
        result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id`="+ valueToString(postThreadId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            catId = result->getIntValue(0,0);
        }
    }

    if(isThreadId && threadId > 0 && user->getUserType(catId) >= CUser::UT_MODER)
    {
        dbRequest->updateRequest("Threads", "`threadIsNeedPremod` = 0", "`id`=" + valueToString(threadId), "LIMIT 1");
    }
    if(isPostId && postId > 0 && user->getUserType(catId) >= CUser::UT_MODER)
    {
        dbRequest->updateRequest("Posts", "`postIsNeedPremod` = 0", "`id`=" + valueToString(postId), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::complainCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 		= request->post.get("id","");
    std::string metod 	= request->post.get("metod","");
    bool isThreadId = false;
    bool isPostId = false;

    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    if(id[0] == 't') isThreadId = true;
    if(id[0] == 'p') isPostId = true;
    if(!isThreadId && !isPostId) return commandResult;
    id[0] = '0';

    int threadId = 0;
    int postId = 0;
    try
    {
        if(isThreadId) 	threadId = std::stoi(id);
        if(isPostId) 	postId 	 = std::stoi(id);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

    int catId = 0;
    if(isThreadId)
    {
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id`="+ valueToString(threadId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            catId = result->getIntValue(0,0);
        }
    }
    if(isPostId)
    {
        int postThreadId = 0;
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("postThreadId"), "Posts", "`id`="+ valueToString(postId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            postThreadId = result->getIntValue(0,0);
        }
        result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id`="+ valueToString(postThreadId), "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            catId = result->getIntValue(0,0);
        }
    }

    if(isThreadId && threadId > 0 )
    {
        if(metod == "remove" && user->getUserType(catId) >= CUser::UT_MODER )
        {
            dbRequest->updateRequest("Threads", "`threadComplainCnt` = 0", "`id`=" + valueToString(threadId), "LIMIT 1");
        }
        else
        {
            dbRequest->updateRequest("Threads", "`threadComplainCnt` = `threadComplainCnt` + 1", "`id`=" + valueToString(threadId), "LIMIT 1");
        }
    }
    if(isPostId && postId > 0 )
    {
        if(metod == "remove" && user->getUserType(catId) >= CUser::UT_MODER )
        {
            dbRequest->updateRequest("Posts", "`postComplainCnt` = 0", "`id`=" + valueToString(postId), "LIMIT 1");
        }
        else
        {
            dbRequest->updateRequest("Posts", "`postComplainCnt` = `postComplainCnt` + 1", "`id`=" + valueToString(postId), "LIMIT 1");
        }
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::banCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string id 		= request->post.get("id","");
    std::string metod 	= request->post.get("metod","");
    std::string closeAccessStr 	= request->post.get("closeaccess","");
    std::string roTimeStr 	    = request->post.get("rotime","0");

    int isAccessClosed = closeAccessStr == "true" ? 1 : 0;
    long roTime = 0;
    int bannedUserId = 0;

    time_t currUnixTime;
    time(&currUnixTime);

    if(id.size() == 0) return commandResult;
    try
    {
        bannedUserId = std::stoi(id);
        roTime = std::stoi(roTimeStr);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
    if(!user->getIsModerInAnyCategories())
    {
        commandResult.setData("User is moderator");
        return commandResult;
    }

    if(roTime < 0) return commandResult;
    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("type"), "Users", "`id`="+ valueToString(bannedUserId));
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
        int bannedUserType = 0;
        bannedUserType = result->getIntValue(0,0);

        if(bannedUserType >= user->getUserType(0))
        {
            commandResult.setData("Access denie");
            return commandResult;
        }
    }

    if(metod == "remove")
    {
        dbRequest->deleteRequest("Bans", "`userId`=" + valueToString(bannedUserId));
    }
    else
    {
        CDBValues fields;
        fields.addValue("userId", bannedUserId);
        fields.addValue("roTime", roTime);
        fields.addValue("banCreationTime", (long)currUnixTime);
        fields.addValue("closeAccess", isAccessClosed);
        dbRequest->insertRequest(fields, "Bans");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::loginCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CConfigHelper* gs = CConfigHelper::getInstance();
    CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string key = request->post.get("userKey","");
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }

    time_t currUnixTime;
    time(&currUnixTime);

    if(key.length() == 0 ) return commandResult;

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
    if(user->getIsValid())
    {
        int userId = 0;
        const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "Users", "`key`='"+ dbManager->getEscapeString(key) +"'", "LIMIT 1");
        if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
        {
            userId = result->getIntValue(0,0);
        }
        CLog::getInstance()->addInfo(userId);
        if(userId == 0)
        {
            commandResult.setData("User load error");
            return commandResult;
        }
        else
        {
            time_t currUnixTime;
            time(&currUnixTime);
            std::string endSessionDateStr = unixTimeToDate((long)currUnixTime + gs->getLongParamValue("sessionLifeTimeInMs", 5184000l), "%a, %d-%b-%Y %H:%M:%S GMT");
            request->createCookie(gs->getStringParamValue("cookieName", "SESSIONID"), key, gs->getLongParamValue("sessionLifeTimeInMs", 5184000l));
        }
    }
    else
    {
        commandResult.setData("User load error");
        return commandResult;
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::setUserAgeRatingCommand(CFCGIRequest* currRequest) const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string ageRatingStr = request->post.get("agerating","");
    int ageRating = 0;

    time_t currUnixTime;
    time(&currUnixTime);

    try
    {
        ageRating = std::stoi(ageRatingStr);
    }
    catch(...)
    {
        ageRating = 0;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    guestUserCheck(currRequest);

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
    if(user->getIsValid())
    {
        dbRequest->updateRequest("Users", "`ageRating`=" + valueToString(ageRating), "LIMIT 1");
    }

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::answerToSurveyCommand(CFCGIRequest* currRequest) const
{
    CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string surveyIdStr		= request->post.get("id","");
    std::string surveyContent   = request->post.get("content","");

    int surveyId = 0;

    time_t currUnixTime;
    time(&currUnixTime);

    try
    {
        surveyId = std::stoi(surveyIdStr);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    if(isUserReadOnly(currRequest))
    {
        commandResult.setData("User in readonly mode");
        return commandResult;
    }
    if(currRequest->getUser()->getIsUserGuest())
    {
        commandResult.setData("User is GUEST");
        return commandResult;
    }

    CLog::getInstance()->addInfo(surveyContent);

    if(surveyId <= 0) return commandResult;
    bool validCheck = true;

    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "SurveysResults", "`surveyId`="+ valueToString(surveyId) +" AND `userId`="+ valueToString(currRequest->getUser()->getUserId()), "LIMIT 1");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
       validCheck = false;
    }
    if(!validCheck) return commandResult;
    if((int)surveyContent.length() > 10000) return commandResult;

    CSurvey* currSurvey = new CSurvey(surveyId);
    CSurveyAnswer* surveyAnswer = new CSurveyAnswer(1, surveyId, currRequest->getUser()->getUserId(), surveyContent, (long)currUnixTime);

    if(!validCheck || !currSurvey->getIsValid()) validCheck = false;
    if(!validCheck || !surveyAnswer->getIsValid()) validCheck = false;
    if(!validCheck || !currSurvey->getIsAnswerCorrect(surveyAnswer)) validCheck = false;
	if (currSurvey->getEndDate() < currUnixTime) validCheck = false;

    delete surveyAnswer;
    delete currSurvey;

    if(!validCheck) return commandResult;

    CLog::getInstance()->addInfo("Valid answer");
    CDBValues fields;
    fields.addValue("userId", currRequest->getUser()->getUserId());
    fields.addValue("surveyId", surveyId);
    fields.addValue("content", surveyContent);
    fields.addValue("creationDate", (long)currUnixTime);
    dbRequest->insertRequest(fields, "SurveysResults");

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::createSurveyCommand(CFCGIRequest* currRequest) const
{
    CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");
	CConfigHelper* gs = CConfigHelper::getInstance();

    std::string surveyTitle	        = dbManager->getEscapeString(request->post.get("title",""));
    std::string surveyDescription   = dbManager->getEscapeString(request->post.get("description",""));
    std::string surveyContent       = dbManager->getEscapeString(request->post.get("content",""));
    std::string surveyLifeTimeStr   = dbManager->getEscapeString(request->post.get("lifetime",""));

    int surveyLifeTime = 0;

    time_t currUnixTime;
    time(&currUnixTime);

    try
    {
        surveyLifeTime = std::stoi(surveyLifeTimeStr);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    if(isUserReadOnly(currRequest))
    {
        commandResult.setData("User in readonly mode");
        return commandResult;
    }

    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
    if(user->getIsUserGuest())
    {
        commandResult.setData("User is GUEST");
        return commandResult;
    }
    if((int)user->getSurveys()->size() > gs->getIntParamValue("maxSurveysCnt", 10))
    {
        commandResult.setData("User have maximum surveys");
        return commandResult;
    }

    CLog::getInstance()->addInfo(surveyContent);
    bool validCheck = true;
    if((int)surveyContent.length() > gs->getIntParamValue("maxSurveyContentLen", 10240))
    {
        commandResult.setData("Survey content length more than maximum");
        return commandResult;
    }

    CSurvey* currSurvey = new CSurvey(0);
    currSurvey->contentFromString(surveyContent);

    if(!validCheck || !currSurvey->getIsValid()) validCheck = false;
    delete currSurvey;

    CLog::getInstance()->addInfo("Valid survey");
    CDBValues fields;
    fields.addValue("userId", user->getUserId());
    fields.addValue("content", surveyContent);
    fields.addValue("creationDate", (long)currUnixTime);
    fields.addValue("endDate", (long)(currUnixTime+surveyLifeTime));
    fields.addValue("ended", 0);
    fields.addValue("title", surveyTitle);
    fields.addValue("description", surveyDescription);
    dbRequest->insertRequest(fields, "Surveys");

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::delSurveyCommand(CFCGIRequest* currRequest) const
{
    CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string surveyIdStr = dbManager->getEscapeString(request->post.get("survey_id","0"));
    int surveyId = 0;

    time_t currUnixTime;
    time(&currUnixTime);

    try
    {
        surveyId = std::stoi(surveyIdStr);
    }
    catch(...)
    {
        return commandResult;
    }

    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    if(isUserReadOnly(currRequest))
    {
        commandResult.setData("User in readonly mode");
        return commandResult;
    }
    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
    if(user->getIsUserGuest())
    {
        commandResult.setData("User is GUEST");
        return commandResult;
    }

    int userId = 0;
    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("userId"), "Surveys", "`id`='"+ valueToString(surveyId), "LIMIT 1");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
        userId = result->getIntValue(0,0);
    }
    if(user->getUserId() != userId && !user->getIsModerInAnyCategories())
    {
        commandResult.setData("User is moderator");
        return commandResult;
    }

    dbRequest->deleteRequest("Surveys", "`id`="+valueToString(surveyId));

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

CCommandResult CSiteCommandManager::endSurveyCommand(CFCGIRequest* currRequest) const
{
    CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CFCGIRequestHandler* request  = currRequest->getRequestForModify();
	CCommandResult commandResult;
	commandResult.setData("Not valid input data");

    std::string surveyIdStr = dbManager->getEscapeString(request->post.get("survey_id","0"));
    int surveyId = 0;

    time_t currUnixTime;
    time(&currUnixTime);

    try
    {
        surveyId = std::stoi(surveyIdStr);
    }
    catch(...)
    {
        return commandResult;
    }
    if(!isUserIdentity(currRequest))
    {
        commandResult.setData("Not valid user");
        return commandResult;
    }
    if(isUserReadOnly(currRequest))
    {
        commandResult.setData("User in readonly mode");
        return commandResult;
    }
    const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());
    if(user->getIsUserGuest())
    {
        commandResult.setData("User is GUEST");
        return commandResult;
    }

    int userId = 0;
    const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("userId"), "Surveys", "`id`='"+ valueToString(surveyId), "LIMIT 1");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
    {
        userId = result->getIntValue(0,0);
    }
    if(user->getUserId() != userId && !user->getIsModerInAnyCategories())
    {
        commandResult.setData("User is moderator");
        return commandResult;
    }

    dbRequest->updateRequest("Surveys", "`ended`=1", "`id`="+valueToString(surveyId));

    commandResult.setIsSuccess(true);
    commandResult.setData("success");
    return commandResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string CSiteCommandManager::handleFiles(CFCGIRequest* currRequest, bool isThreadAttach) const
{
    int filesCnt = 0;
    std::string filesCntStr = "";
    std::string filePostParamName = "";
    CConfigHelper* gs = CConfigHelper::getInstance();

    if(isThreadAttach)
    {
        filesCntStr = currRequest->getRequest()->post.get("thread_files_cnt","0");
        filePostParamName = "threadfile";
    }
    else
    {
        filesCntStr = currRequest->getRequest()->post.get("post_files_cnt","0");
        filePostParamName = "postfile";
    }

    CLog::getInstance()->addInfo("Files count: " + filesCntStr);

    try
    {
        filesCnt = std::stoi(filesCntStr);
    }
    catch(...)
    {
        filesCnt = 0;
    }

    std::string attachStr = "";
    for(int i = 0; i < filesCnt; i++)
    {
        std::string postParamName = filePostParamName + valueToString(i);
        const CFileDescriptor* file = currRequest->getRequestForModify()->files->getFile(postParamName);
        if(file == NULL)
        {
            CLog::getInstance()->addError("NULL file: " + postParamName);
            continue;
        }

        const std::string fileName = file->getFullFileName();
        const std::string fileType = file->getFileMIME();
        long fileSize  = file->getFileSize();
        const char* fileData = file->getFileData();

        if(fileSize == 0 || fileName == "" || fileData == 0 || (long)fileSize > gs->getLongParamValue("maxFileSize", 10485760l)) continue;
        CLog::getInstance()->addInfo(fileSize);

        std::string newFileName = md5(fileData, fileSize);
        std::string filePath = CManagers::getInstance()->getResourceManager()->saveFile(newFileName, fileType, fileSize, fileData);
        if(filePath.length() == 0) continue;
        if(fileType == "image/jpeg") CSiteResourceManager::createJPEGPreview(filePath);

        // Attach string gen;
        int imgWidth  = 0;
        int imgHeight = 0;
       // bool isCorrectImgSize = CResourceManager::getImageDimensions(filePath, imgWidth, imgHeight);
       CSiteResourceManager::getImageDimensions(filePath, imgWidth, imgHeight);

        attachStr += filePath + ";" + valueToString(fileSize) + ";" + valueToString(imgWidth) + ";" + valueToString(imgHeight);
        if(i < filesCnt - 1) attachStr += ";";
    }

    return attachStr;
}

void CSiteCommandManager::guestUserCheck(CFCGIRequest* currRequest) const
{
    if(!currRequest->getUser()->getIsValid()) return;

    CConfigHelper* gs = CConfigHelper::getInstance();
    if(currRequest->getUser()->getCookie() == gs->getStringParamValue("guestCookieValue", "GUEST"))
    {
        std::string userKey = CUser::genUserKey();
        currRequest->getRequestForModify()->createCookie(gs->getStringParamValue("cookieName", "SESSIONID"), userKey, gs->getLongParamValue("sessionLifeTimeInMs", 5184000l));
        currRequest->setUser(CUser::createNewUser(userKey,"", userKey, valueToString(currRequest->getRequest()->param.get("REMOTE_ADDR","")), valueToString(currRequest->getRequest()->param.get("HTTP_USER_AGENT",""))));
    }
}
