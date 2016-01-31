#include "CThread.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/managers/CManagers.h"

#include "CAttachment.h"
#include "CPost.h"
#include "CSurvey.h"

CThread::CThread(const int threadId, bool isExpand) : CBaseMessage(threadId)
{
	this->isExpand = isExpand;
	this->isThreadFixed = false;
	this->isNeedPremod = false;
	this->survey = NULL;
	this->isHidden = false;

	readThreadFromDB();
}

CThread::~CThread()
{
	for(unsigned int i = 0; i < posts.size(); i++)
	{
		delete posts[i];
	}
	delete survey;
}

int CThread::getPostsCnt() const
{
	return postsCnt;
}

int CThread::getCatId() const
{
	return catId;
}

int CThread::getPosition() const
{
	return position;
}

const std::string CThread::getCaption() const
{
	return caption;
}

int CThread::getType()
{
	return 0;
}

bool CThread::getIsFixed()  const
{
	return isThreadFixed;
}

bool CThread::getIsNeedPremod()  const
{
	return isNeedPremod;
}

bool CThread::getIsHidden() const
{
	return isHidden;
}

const CSurvey* CThread::getSuvrey() const
{
	return survey;
}

void CThread::readThreadFromDB()
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	CConfigHelper* gs = CConfigHelper::getInstance();

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("*"), "Threads", "`id` = " + valueToString(id));
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		caption 			= result->getStringValue(0, 1);
		postsCnt 			= result->getIntValue(0, 2);
		creationDate 		= result->getLongValue(0, 3);
		attachmentString 	= result->getStringValue(0, 4);
		text 				= result->getStringValue(0, 5);
		catId 				= result->getIntValue(0, 6);
		userId 				= result->getIntValue(0, 7);
		userName 			= result->getStringValue(0, 8);
		position 			= result->getIntValue(0, 9);
		isThreadFixed 		= result->getBoolValue(0, 10);
		answersIdsStr 		= result->getStringValue(0, 11);
		isNeedPremod 		= result->getBoolValue(0, 12);
		complainCnt 		= result->getIntValue(0, 13);
		modDate 			= result->getLongValue(0, 14);
		ageRating 			= result->getIntValue(0, 15);
		surveyId 			= result->getIntValue(0, 16);
		isHidden 			= result->getBoolValue(0, 17);

		if(isThreadFixed) position = INT_MAX;
		attachments = CAttachment::processAttachmentString(attachmentString);

		std::vector<std::string> IdStrs;
		split(IdStrs, answersIdsStr, ";");
		for(unsigned int i = 0; i < IdStrs.size(); i++)
		{
			int answerId = 0;
			try{ answerId = std::stoi(IdStrs[i]); }
			catch(...){ answerId = 0; }
			if(answerId == 0) continue;

			answersIds.push_back(answerId);
		}

		if(surveyId > 0)
		{
			survey = new CSurvey(surveyId);
		}
		if(postsCnt <= gs->getIntParamValue("defaultPostsCntInThread", 10)) isExpand = true;
		if(isExpand) readAllPosts();
		else readLastPosts();

		isValid = true;
	}
}

void CThread::readAllPosts()
{
	readPosts(postsCnt);
}

void CThread::readLastPosts()
{
	CConfigHelper* gs = CConfigHelper::getInstance();
	readPosts(gs->getIntParamValue("defaultPostsCntInThread", 10));
}

void CThread::readPosts(const int lastPostsCnt)
{
	std::string postText;
	std::string postUserName;
	long postCreationDate;
	long postModDate;
	std::string postAttachmentString;
	std::string postAnswersIdsStr;
	std::string postParentId;
	bool postIsNeedPremod;
	int postId;
	int postUserId;
	int postThreadId;
	int postComplainCnt;
	int postAgeRating;

	posts.clear();
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	int rowLimit = lastPostsCnt;
	if(rowLimit > postsCnt) rowLimit = postsCnt;

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("*"), "Posts", "`postThreadId` = " + valueToString(id), "ORDER BY `id` LIMIT  "+valueToString(rowLimit));
	//const CDBRequestResult* result = dbRequest->createRequest("SELECT * FROM `Posts` WHERE `postThreadId` = " + valueToString(id) + " ORDER BY `id` LIMIT  "+valueToString(rowLimit)+";");
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		int rowsCnt = result->getRowsCnt();
		int startRow = rowsCnt - lastPostsCnt;
		if(startRow < 0) startRow = 0;

		for(int i = startRow; i < rowsCnt; i++)
		{
			postId 					= result->getIntValue(i, 0);
			postCreationDate 		= result->getLongValue(i, 1);
			postAttachmentString 	= result->getStringValue(i, 2);
			postText 				= result->getStringValue(i, 3);
			postUserId 				= result->getIntValue(i, 4);
			postThreadId 			= result->getIntValue(i, 5);
			postUserName 			= result->getStringValue(i, 6);
			postAnswersIdsStr 		= result->getStringValue(i, 7);
			postParentId 			= result->getStringValue(i, 8, "");
			postIsNeedPremod 		= result->getBoolValue(i, 9);
			postComplainCnt 		= result->getIntValue(i, 10);
			postModDate 			= result->getLongValue(i, 11);
			postAgeRating 			= result->getIntValue(i, 12);

			posts.push_back(new CPost(postId, postUserId, postThreadId, postText, postUserName, postCreationDate, postAttachmentString, postAnswersIdsStr, postParentId, postIsNeedPremod,postComplainCnt, postModDate, postAgeRating, catId));
		}
	}
}

const std::vector<CPost*>* CThread::getPosts() const
{
	return &posts;
}

const CPost* CThread::findPostById(const int postId) const
{
	for(unsigned int i = 0; i < posts.size(); i++)
	{
		CPost* currPost = posts[i];
		if(currPost->getId() == postId) return currPost;
	}
	return NULL;
}
