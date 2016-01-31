#include "CUser.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/CBan.h"

#include "CSurvey.h"

#include <cstdlib>

CUser::CUser() : CDefaultUser()
{
	this->ageRating = 0;
}

CUser::CUser(const int userId) : CDefaultUser(userId)
{
	this->ageRating = 0;
}

CUser::~CUser()
{
	for(auto it = surveys.begin(); it != surveys.end(); ++it)
	{
		CSurvey* currSurvey = (*it);
		delete currSurvey;
	}
}

int CUser::getUserType(const int catId) const
{
	if(userType >= UT_ADMIN) return UT_ADMIN;
	if(userType == UT_MODER) return UT_MODER;
	if(userType <= UT_NONE)	 return UT_NONE;

	bool isModder = false;
	for(auto it = moderCats.begin(); it != moderCats.end(); ++it)
	{
		int currModerCat = (*it);
		if(currModerCat == catId && currModerCat != 0)
		{
			isModder = true;
			break;
		}
	}
	if(isModder) return UT_MODER;
	else return UT_USER;
}

int CUser::getUserAgeRating() const
{
	return ageRating;
}

const std::vector<int>* CUser::getAnsweredSurveysId() const
{
	return &answeredSurveysId;
}

bool CUser::getIsUserSurveyMember(const int surveyId) const
{
	bool result = false;
	for(auto it = answeredSurveysId.begin(); it != answeredSurveysId.end(); ++it)
	{
		int currId = (*it);
		if(currId == surveyId)
		{
			result = true;
			break;
		}
	}

	return result;
}

bool CUser::getIsUserSurveyOwner(const int surveyId) const
{
	bool result = false;
	for(auto it = surveys.begin(); it != surveys.end(); ++it)
	{
		const CSurvey* currSurvey = (*it);
		if(currSurvey->getIsValid() && currSurvey->getId() == surveyId && currSurvey->getOwnerId() == userId)
		{
			result = true;
			break;
		}
	}

	return result;
}

bool CUser::getIsModerInAnyCategories() const
{
	if((int)moderCats.size() > 0 || userType > UT_USER) return true;
	return false;
}

//CUser* CUser::createNewUser(const std::string& cookieData, const std::string& remouteAddress, const std::string& userAgent)
//{
//	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
//	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
//	time_t currUnixTime;
//	time(&currUnixTime);
//
//	CUser* newUser = new CUser();
//	CLog::getInstance()->addInfo("New User");
//
//    CDBValues fields;
//    fields.addValue("remouteAddress", remouteAddress);
//    fields.addValue("type", 1);
//    fields.addValue("key", cookieData);
//    fields.addValue("ageRating", 1);
//    fields.addValue("lastAction", (long)currUnixTime);
//    fields.addValue("userAgent", userAgent);
//    dbRequest->insertRequest(fields, "Users");
//
//	newUser->fillUserData(cookieData);
//
//	return newUser;
//}

void CUser::fillUserData(const std::string& cookieData)
{
	int userId = 0;
	int userType = 0;

	std::string hiddenThreadsStr 	= "";
	std::string hiddenPostsStr 		= "";
	std::string favoriteThreadsStr 	= "";
	std::string favoritePostsStr	= "";
	std::string fixedThreadsStr 	= "";
	std::string moderCatsStr = "";

	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id,type,remouteAddress,favoriteThreads,favoritePosts,hiddenThreads,hiddenPosts,fixedThreads,ageRating,lastChange,lastAction,userAgent,moderCats"), "Users", "`key` = '" + dbManager->getEscapeString(cookieData) + "'", "LIMIT 1");
	if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
	{

		userId   = result->getIntValue(0, 0, 0);
		userType = result->getIntValue(0, 1, 0);

		key = cookieData;
		remouteAddress 		= result->getStringValue(0, 2, "127.0.0.1");
		favoriteThreadsStr 	= result->getStringValue(0, 3, "");
		favoritePostsStr 	= result->getStringValue(0, 4, "");
		hiddenThreadsStr 	= result->getStringValue(0, 5, "");
		hiddenPostsStr 		= result->getStringValue(0, 6, "");
		fixedThreadsStr 	= result->getStringValue(0, 7, "");
		ageRating 			= result->getIntValue(0, 8, 1);
		lastChange 			= result->getLongValue(0, 9, 0);
		lastAction 			= result->getLongValue(0, 10, 0);
		userAgent 			= result->getStringValue(0, 11, "");
		moderCatsStr 		= result->getStringValue(0, 12, "");
	}

	this->cookie = cookieData;
	this->userId = userId;
	this->userType = userType;

	isValid = true;
	if(cookie == "" || userId == 0 || userType < 0 || key == "") isValid = false;

	if(isValid)
	{
		const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("*"), "Bans", "`userId` = "+valueToString(userId));
		//const CDBRequestResult* result = dbRequest->createRequest("SELECT * FROM `Bans` WHERE `userId` = "+valueToString(userId)+";");
		if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
		{
			time_t currUnixTime;
			time(&currUnixTime);

			for(int i = 0; i < result->getRowsCnt(); i++)
			{
				int banId = 0;
				int banUserId = 0;
				long banCreationTime = 0;
				long banROTime = 0;
				bool banIsAccessClosed = false;

				banId 				= result->getIntValue(i, 0);
				banUserId 			= result->getIntValue(i, 1);
				banROTime 			= result->getLongValue(i, 2);
				banIsAccessClosed 	= result->getBoolValue(i, 3);
				banCreationTime 	= result->getLongValue(i, 4);

				if(banId != 0 && banUserId != 0 && (banIsAccessClosed || currUnixTime <= banCreationTime + banROTime ))
				{
					CBan* ban = new CBan(userId, banROTime, banCreationTime, banUserId, banIsAccessClosed);
					bans.push_back(ban);
				}
				else
				{
					// delete ban
				}
			}
		}

		CConfigHelper* gs = CConfigHelper::getInstance();
		if(cookie != gs->getStringParamValue("guestCookieValue", "GUEST"))
		{
			result = dbRequest->selectRequest(CDBValues("id"), "Surveys", "`userId` = "+valueToString(userId));
			//result = DBRequest.createRequest("SELECT `id` FROM `Surveys` WHERE `userId` = "+valueToString(userId)+";");
			if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
			{
				for(int i = 0; i < result->getRowsCnt(); i++)
				{
					int surveyId = result->getIntValue(i, 0);

					if(surveyId > 0)
					{
						CSurvey* survey = new CSurvey(surveyId);
						if(survey->getIsValid())
						{
							surveys.push_back(survey);
						}
					}
				}
			}

			result = dbRequest->selectRequest(CDBValues("surveyId"), "SurveysResults", "`userId` = " + valueToString(userId));
			//result = DBRequest.createRequest("SELECT `surveyId` FROM `SurveysResults` WHERE `userId` = " + valueToString(userId) + ";");
			if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
			{
				for(int i = 0; i < result->getRowsCnt(); i++)
				{
					int surveyId = result->getIntValue(i, 0);
					if(surveyId > 0) answeredSurveysId.push_back(surveyId);
				}
			}
		}

		std::vector<std::string> ids;
		// moderCats
		//boost::split(ids, moderCatsStr, boost::is_any_of(";"));
		split(ids, moderCatsStr, ";");
		for(unsigned int i = 0; i < ids.size(); i++)
		{
			int id = 0;
			try
			{
				id = std::stoi(ids[i]);
			}
			catch(...)
			{
				id = 0;
			}
			if(id != 0) moderCats.push_back(id);
		}
		// favoriteThreads
		//boost::split(ids, favoriteThreadsStr, boost::is_any_of(";"));
		split(ids, favoriteThreadsStr, ";");
		for(unsigned int i = 0; i < ids.size(); i++)
		{
			int id = 0;
			try
			{
				id = std::stoi(ids[i]);
			}
			catch(...)
			{
				id = 0;
			}
			if(id != 0) favoritesThreads.push_back(id);
		}
		// favoritePosts
		//boost::split(ids, favoritePostsStr, boost::is_any_of(";"));
		split(ids, favoritePostsStr, ";");
		for(unsigned int i = 0; i < ids.size(); i++)
		{
			int id = 0;
			try
			{
				id = std::stoi(ids[i]);
			}
			catch(...)
			{
				id = 0;
			}
			if(id != 0) favoritesPosts.push_back(id);
		}
		// hiddenThreads
		//boost::split(ids, hiddenThreadsStr, boost::is_any_of(";"));
		split(ids, hiddenThreadsStr, ";");
		for(unsigned int i = 0; i < ids.size(); i++)
		{
			int id = 0;
			try
			{
				id = std::stoi(ids[i]);
			}
			catch(...)
			{
				id = 0;
			}
			if(id != 0) hiddenThreads.push_back(id);
		}
		// hiddenPosts
		//boost::split(ids, hiddenPostsStr, boost::is_any_of(";"));
		split(ids, hiddenPostsStr, ";");
		for(unsigned int i = 0; i < ids.size(); i++)
		{
			int id = 0;
			try
			{
				id = std::stoi(ids[i]);
			}
			catch(...)
			{
				id = 0;
			}
			if(id != 0) hiddenPosts.push_back(id);
		}
		// fixedThreads
		//boost::split(ids, fixedThreadsStr, boost::is_any_of(";"));
		split(ids, fixedThreadsStr, ";");
		for(unsigned int i = 0; i < ids.size(); i++)
		{
			int id = 0;
			try
			{
				id = std::stoi(ids[i]);
			}
			catch(...)
			{
				id = 0;
			}
			if(id != 0) fixedThreads.push_back(id);
		}
	}
}

bool CUser::getIsThreadHidden(const int threadId) const
{
	for(unsigned int i = 0; i < hiddenThreads.size(); i++)
	{
		if(hiddenThreads[i] == threadId) return true;
	}
	return false;
}

bool CUser::getIsPostHidden(const int postId) const
{
	for(unsigned int i = 0; i < hiddenPosts.size(); i++)
	{
		if(hiddenPosts[i] == postId) return true;
	}
	return false;
}

bool CUser::getIsThreadFavorite(const int threadId) const
{
	for(unsigned int i = 0; i < favoritesThreads.size(); i++)
	{
		if(favoritesThreads[i] == threadId) return true;
	}
	return false;
}

bool CUser::getIsPostFavorite(const int postId) const
{
	for(unsigned int i = 0; i < favoritesPosts.size(); i++)
	{
		if(favoritesPosts[i] == postId) return true;
	}
	return false;
}

bool CUser::getIsThreadFixed(const int threadId) const
{
	for(unsigned int i = 0; i < fixedThreads.size(); i++)
	{
		if(fixedThreads[i] == threadId) return true;
	}
	return false;
}

void CUser::unhidePost(const int postId)
{
	for(auto it = hiddenPosts.begin(); it != hiddenPosts.end(); it++)
	{
        int currId = (*it);
        if(currId == postId)
        {
			hiddenPosts.erase(it);
			return;
        }
	}
}

void CUser::unhideThread(const int threadId)
{
	for(auto it = hiddenThreads.begin(); it != hiddenThreads.end(); it++)
	{
        int currId = (*it);
        if(currId == threadId)
        {
			hiddenThreads.erase(it);
			return;
        }
	}
}

void CUser::removeFromFavoritePost(const int postId)
{
	for(auto it = favoritesPosts.begin(); it != favoritesPosts.end(); it++)
	{
        int currId = (*it);
        if(currId == postId)
        {
			favoritesPosts.erase(it);
			return;
        }
	}
}

void CUser::removeFromFavoriteThread(const int threadId)
{
	for(auto it = favoritesThreads.begin(); it != favoritesThreads.end(); it++)
	{
        int currId = (*it);
        if(currId == threadId)
        {
			favoritesThreads.erase(it);
			return;
        }
	}
}

void CUser::removeFromFixedThread(const int threadId)
{
	for(auto it = fixedThreads.begin(); it != fixedThreads.end(); it++)
	{
        int currId = (*it);
        if(currId == threadId)
        {
			fixedThreads.erase(it);
			return;
        }
	}
}

void CUser::hidePost(const int postId)
{
	if(!getIsPostHidden(postId)) hiddenPosts.push_back(postId);
}

void CUser::hideThread(const int threadId)
{
	if(!getIsThreadHidden(threadId)) hiddenThreads.push_back(threadId);
}

void CUser::addToFavoritePost(const int postId)
{
	if(!getIsPostFavorite(postId)) favoritesPosts.push_back(postId);
}

void CUser::addToFavoriteThread(const int threadId)
{
	if(!getIsThreadFavorite(threadId)) favoritesThreads.push_back(threadId);
}

void CUser::addToFixedThread(const int threadId)
{
	if(!getIsThreadFixed(threadId)) fixedThreads.push_back(threadId);
}

const std::vector<int>* CUser::getHiddenThreads() const
{
	return &hiddenThreads;
}

const std::vector<int>* CUser::getHiddenPosts() const
{
	return &hiddenPosts;
}

const std::vector<int>* CUser::getFavoriteThreads() const
{
	return &favoritesThreads;
}

const std::vector<int>* CUser::getFavoritePosts() const
{
	return &favoritesPosts;
}

const std::vector<int>* CUser::getFixedThreads() const
{
	return &fixedThreads;
}

const std::vector<CSurvey*>* CUser::getSurveys() const
{
	return &surveys;
}

std::string CUser::getHiddenThreadsStr() const
{
	std::string resultStr = "";
	for(unsigned int i = 0; i < hiddenThreads.size(); i++)
	{
		resultStr += valueToString(hiddenThreads[i]);
		if(i < hiddenThreads.size()) resultStr += ";";
	}
	return resultStr;
}

std::string CUser::getHiddenPostsStr() const
{
	std::string resultStr = "";
	for(unsigned int i = 0; i < hiddenPosts.size(); i++)
	{
		resultStr += valueToString(hiddenPosts[i]);
		if(i < hiddenPosts.size()) resultStr += ";";
	}
	return resultStr;
}

std::string CUser::getFavoriteThreadsStr() const
{
	std::string resultStr = "";
	for(unsigned int i = 0; i < favoritesThreads.size(); i++)
	{
		resultStr += valueToString(favoritesThreads[i]);
		if(i < favoritesThreads.size()) resultStr += ";";
	}
	return resultStr;
}

std::string CUser::getFavoritePostsStr() const
{
	std::string resultStr = "";
	for(unsigned int i = 0; i < favoritesPosts.size(); i++)
	{
		resultStr += valueToString(favoritesPosts[i]);
		if(i < favoritesPosts.size()) resultStr += ";";
	}
	return resultStr;
}

std::string CUser::getFixedThreadsStr() const
{
	std::string resultStr = "";
	for(unsigned int i = 0; i < fixedThreads.size(); i++)
	{
		resultStr += valueToString(fixedThreads[i]);
		if(i < fixedThreads.size()) resultStr += ";";
	}
	return resultStr;
}
