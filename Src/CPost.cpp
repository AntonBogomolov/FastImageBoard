#include "CPost.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"
#include "novemberlib/FCGI/CFCGIApp.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/managers/CDBManager.h"

#include "CAttachment.h"

CPost::CPost(const int postId) : CBaseMessage(postId)
{
	isNeedPremod = false;

	readPostFromDB();
}

CPost::CPost(const int postId, const int userId, const int threadId, const std::string text, const std::string userName, const long creationDate, const std::string attachmentString, const std::string answersIdsStr, const std::string parentId, bool isNeedPremod, const int complainCnt, const long modDate, const int ageRating, const int catId) : CBaseMessage(postId, userId, text, userName, creationDate, attachmentString, answersIdsStr, complainCnt, modDate, ageRating)
{
    this->threadId = threadId;
    this->parentId = parentId;
    this->catId = catId;
    this->isNeedPremod = isNeedPremod;

    attachments = CAttachment::processAttachmentString(attachmentString);

    std::vector<std::string> IdStrs;
    split(IdStrs, answersIdsStr, ";");
    for(unsigned int i = 0; i < IdStrs.size(); i++)
    {
        int answerId = 0;
        try
        {
            answerId = std::stoi(IdStrs[i]);
        }
        catch(...)
        {
            answerId = 0;
        }
        if(answerId == 0) continue;

        answersIds.push_back(answerId);
    }

	isValid = true;
}

CPost::~CPost()
{

}

int CPost::getType()
{
	return 1;
}

int CPost::getThreadId() const
{
	return threadId;
}

int CPost::getCatId() const
{
	return catId;
}

const std::string CPost::getParentId() const
{
	return parentId;
}

bool CPost::getIsNeedPremod()  const
{
	return isNeedPremod;
}

void CPost::readPostFromDB()
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("*"), "Posts", "`id` = " + valueToString(id));
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		creationDate 		= result->getLongValue(0, 1);
		attachmentString 	= result->getStringValue(0, 2);
		text 				= result->getStringValue(0, 3);
		userId 				= result->getIntValue(0, 4);
		threadId 			= result->getIntValue(0, 5);
		userName 			= result->getStringValue(0, 6);
		answersIdsStr 		= result->getStringValue(0, 7);
		parentId 			= result->getStringValue(0, 8, "");
		isNeedPremod 		= result->getBoolValue(0, 9);
		complainCnt 		= result->getIntValue(0, 10);
		modDate 			= result->getLongValue(0, 11);
		ageRating 			= result->getIntValue(0, 12);

		attachments = CAttachment::processAttachmentString(attachmentString);

		std::vector<std::string> IdStrs;
		split(IdStrs, answersIdsStr, ";");
		for(unsigned int i = 0; i < IdStrs.size(); i++)
		{
			int answerId = 0;
			try
			{
				answerId = std::stoi(IdStrs[i]);
			}
			catch(...)
			{
				answerId = 0;
			}
			if(answerId == 0) continue;

			answersIds.push_back(answerId);
		}

		isValid = true;
	}

	catId = 0;
	result = dbRequest->selectRequest(CDBValues("threadCatId"), "Threads", "`id` = " + valueToString(threadId));
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		catId = result->getIntValue(0,0);
	}
}
