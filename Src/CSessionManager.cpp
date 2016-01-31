#include "CSessionManager.h"

#include "CManagers.h"
#include "CDBManager.h"
#include "utils.h"

#include "CDefaultUser.h"
#include "CConfigHelper.h"
#include "CManagers.h"
#include "CFCGIRequest.h"

CSessionManager::CSessionManager()
{
	isNeedSessionCheck = true;
}

CSessionManager::~CSessionManager()
{
	//dtor
}

bool CSessionManager::getIsNeedSessionCheck() const
{
	return isNeedSessionCheck;
}

void CSessionManager::setIsNeedSessionCheck(const bool value)
{
	isNeedSessionCheck = value;
}

bool CSessionManager::checkSession(CFCGIRequest* currRequest)
{
	if(!isNeedSessionCheck) return true;

	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CConfigHelper* gs = CConfigHelper::getInstance();

	time_t currUnixTime;
	time(&currUnixTime);

	std::string cookieData = dbManager->getEscapeString(currRequest->getRequest()->cookie.get(gs->getStringParamValue("cookieName", "SESSIONID"),""));

	if(cookieData.length() == 0)							// new user
	{
		currRequest->getRequestForModify()->createCookie(gs->getStringParamValue("cookieName", "SESSIONID"), gs->getStringParamValue("guestCookieValue", "GUEST"), gs->getLongParamValue("sessionLifeTimeInMs", 5184000l));
		cookieData = gs->getStringParamValue("guestCookieValue", "GUEST");
	}
	else													// old user has cookie
	{
		bool isHasDB = gs->getStringParamValue("isHasDB", "false") == "true";
		if(!isHasDB)
		{
			cookieData = gs->getStringParamValue("guestCookieValue", "GUEST");
		}

		const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "Users", "`key` = '" + cookieData + "'", "LIMIT 1");
		if(dbRequest->getIsLastQuerySuccess() && result != NULL  && result->getRowsCnt() > 0)
		{
			currRequest->getRequestForModify()->createCookie(gs->getStringParamValue("cookieName", "SESSIONID"), cookieData, gs->getLongParamValue("sessionLifeTimeInMs", 5184000l));
		}
		else
		{
			currRequest->getRequestForModify()->createCookie(gs->getStringParamValue("cookieName", "SESSIONID"), gs->getStringParamValue("guestCookieValue", "GUEST"), gs->getLongParamValue("sessionLifeTimeInMs", 5184000l));
			cookieData = gs->getStringParamValue("guestCookieValue", "GUEST");
		}
	}

	CDefaultUser* currUser = new CDefaultUser();
	currUser->fillUserData(cookieData);
	currRequest->setUser(currUser);

	if(!currRequest->getUser()->getIsValid() || currRequest->getUser()->getIsAccessClosed()) return false;
	return true;
}

void CSessionManager::loginUser(CFCGIRequest* currRequest, const std::string& login, const std::string& pass)
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CConfigHelper* gs = CConfigHelper::getInstance();

	time_t currUnixTime;
	time(&currUnixTime);

	std::string newSessionStr = dbManager->getEscapeString(CDefaultUser::genUserKey());
	currRequest->getRequestForModify()->createCookie(gs->getStringParamValue("cookieName", "SESSIONID"), newSessionStr, gs->getLongParamValue("sessionLifeTimeInMs", 5184000l));
	std::string cookieData = newSessionStr;

	CDefaultUser* currUser = CDefaultUser::loginUser(login, pass);
    if(currUser && currUser->getIsValid()) currRequest->setUser(currUser);
}

void CSessionManager::registerUser(CFCGIRequest* currRequest, const std::string& login, const std::string& pass)
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CConfigHelper* gs = CConfigHelper::getInstance();

	time_t currUnixTime;
	time(&currUnixTime);

	std::string newSessionStr = dbManager->getEscapeString(CDefaultUser::genUserKey());
	currRequest->getRequestForModify()->createCookie(gs->getStringParamValue("cookieName", "SESSIONID"), newSessionStr, gs->getLongParamValue("sessionLifeTimeInMs", 5184000l));
	std::string cookieData = newSessionStr;

	CDefaultUser* currUser = CDefaultUser::createNewUser(login, pass, cookieData, currRequest->getRequest()->param.get("REMOTE_ADDR",""), currRequest->getRequest()->param.get("HTTP_USER_AGENT",""));
	if(currUser->getIsValid()) currRequest->setUser(currUser);
}
