#include "CMenu.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/managers/CDBManager.h"

#include "CThread.h"

CMenu* CMenu::instance = NULL;

CMenu::CMenu()
{
	readMenuFromDB();
}

CMenu::~CMenu()
{
	for(auto it = menuGroups.begin(); it != menuGroups.end(); it++)
	{
		CMenuGroup* currGroup = (*it);
		delete currGroup;
	}
}

const CMenu* CMenu::getInstance()
{
	if(instance == NULL)
	{
		instance = new CMenu();
	}
	return instance;
}

void CMenu::readMenuFromDB()
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("*"), "Groups");
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		for(int i = 0; i < result->getRowsCnt(); i++)
		{
			int groupId = 0;
			int groupAccess = 0;
			std::string groupName  = "";

			groupId = result->getIntValue(i, 0);
			groupName = result->getStringValue(i, 1);
			groupAccess = result->getIntValue(i, 3);

			if(groupId > 0 && (int)groupName.size() > 0 )menuGroups.push_back(new CMenuGroup(groupName, groupId, groupAccess));
		}
	}

	for(unsigned int i = 0; i < menuGroups.size(); i++)
	{
		CMenuGroup* currGroup = menuGroups[i];
		result = dbRequest->selectRequest(CDBValues("*"), "Category", "`CategoryGroupId` = "+ valueToString(currGroup->getId()) );
		if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
		{
			for(int j = 0; j < result->getRowsCnt(); j++)
			{
				int catId = 0;
				int catThreadCnt = 0;
				int catAccessLevel = 1;
				int catAgeRating = 1;
				int threadCreationLevel = 1;
				int postCreationLevel = 1;
				bool needPremod = false;
				std::string catName = "";
				std::string catDescription = "";
				std::string allowMIMES = "";

				catId = result->getIntValue(j, 0);
				catThreadCnt = result->getIntValue(j, 3);
				catAccessLevel = result->getIntValue(j, 6);
				catAgeRating = result->getIntValue(j, 7);

				threadCreationLevel = result->getIntValue(j, 9, 1);
				postCreationLevel = result->getIntValue(j, 10, 1);

				catName = result->getStringValue(j, 1);
				catDescription = result->getStringValue(j, 2);
				allowMIMES = result->getStringValue(j, 11);
				needPremod = result->getBoolValue(j, 8);

				if((int)catName.size() > 0 && catId > 0)
				{
					currGroup->addCategoryInGroup(new CMenuCategory(catName, catDescription, catId, catThreadCnt, catAccessLevel, catAgeRating, needPremod, threadCreationLevel, postCreationLevel, allowMIMES) );
				}
			}
		}
	}
}

const CMenuGroup* CMenu::findGroupById(int grId) const
{
	for(auto it = menuGroups.begin(); it != menuGroups.end(); it++)
	{
		CMenuGroup* currGroup = (*it);
		if(currGroup->getId() == grId) return currGroup;
	}
	return NULL;
}

const CMenuGroup* CMenu::findGroupByName(std::string grName) const
{
	for(auto it = menuGroups.begin(); it != menuGroups.end(); it++)
	{
		CMenuGroup* currGroup = (*it);
		if(currGroup->getName().compare(grName) == 0) return currGroup;
	}
	return NULL;
}

const CMenuCategory* CMenu::findCategoryById(int catId) const
{
	for(auto it = menuGroups.begin(); it != menuGroups.end(); it++)
	{
		CMenuGroup* currGroup = (*it);
		const CMenuCategory* currCat = currGroup->findCategoryById(catId);
		if(currCat != NULL) return currCat;
	}
	return NULL;
}

const CMenuCategory* CMenu::findCategoryByName(std::string catName) const
{
	for(auto it = menuGroups.begin(); it != menuGroups.end(); it++)
	{
		CMenuGroup* currGroup = (*it);
		const CMenuCategory* currCat = currGroup->findCategoryByName(catName);
		if(currCat != NULL) return currCat;
	}
	return NULL;
}

const std::vector<CMenuCategory*>* 	CMenu::getAllCategoriesInGroup(int grId) const
{
	for(auto it = menuGroups.begin(); it != menuGroups.end(); it++)
	{
		CMenuGroup* currGroup = (*it);
		if(currGroup->getId() == grId) return currGroup->getAllCategoriesInGroup();
	}
	return NULL;
}

const std::vector<CMenuGroup*>* 	CMenu::getAllGroups() const
{
	return &menuGroups;
}

/////////////////////////////////////////////////////////////////////////////////////

CMenuCategory::CMenuCategory(const std::string name,const std::string description,const int id, const int threadsCnt, const int accessLevel, const int ageRating, bool needPremod, int threadCreationLevel, int postCreationLevel, std::string allowMIMES)
{
	this->name = name;
	this->description = description;
	this->id = id;
	this->threadsCnt  = threadsCnt;
	this->accessLevel = accessLevel;
	this->ageRating = ageRating;
	this->needPremod = needPremod;
	this->threadCreationLevel = threadCreationLevel;
	this->postCreationLevel = postCreationLevel;
	this->allowMIMES = allowMIMES;

	initCategory();
}

CMenuCategory::~CMenuCategory()
{

}

void CMenuCategory::initCategory() const
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	threadsId.clear();

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "Threads", "`threadCatId` = "+ valueToString(id), "ORDER BY `threadPosition` DESC");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
    {
		threadsCnt = result->getRowsCnt();
        for(int j = 0; j < threadsCnt; j++)
        {
            threadsId.push_back(result->getIntValue(j,0));
        }
    }
}

int CMenuCategory::getIsNeedPremod() const
{
	return needPremod;
}

int CMenuCategory::getAgeRating() const
{
	return ageRating;
}

int CMenuCategory::getAccessLevel() const
{
	return accessLevel;
}

int CMenuCategory::getThreadCreationAccessLevel() const
{
	return threadCreationLevel;
}

int CMenuCategory::getPostCreationAccessLevel() const
{
	return postCreationLevel;
}

bool CMenuCategory::getIsAllowMIME(std::string MIME) const
{
	if(allowMIMES.find(MIME) != std::string::npos) return true;
	return false;
}

const std::string CMenuCategory::getName() const
{
	return name;
}

const std::string CMenuCategory::getDescription() const
{
	return description;
}

int CMenuCategory::getId() const
{
	return id;
}

int CMenuCategory::getAllThreadsCnt() const
{
	return threadsCnt;
}

const std::vector<int> CMenuCategory::getThreadsId(const int topThreadsCnt) const
{
//	initCategory();
//
//	if(topThreadsCnt >= threadsId.size()) return threadsId;
//
//	std::vector<int> topThreadsId;
//	topThreadsId.insert(topThreadsId.begin(), threadsId.begin(), threadsId.begin() + topThreadsCnt);
//
//	return topThreadsId;

	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	std::vector<int> threadsId;

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "Threads", "`threadCatId` = "+ valueToString(id), "ORDER BY `threadPosition` DESC LIMIT "+valueToString(topThreadsCnt) );

    if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
    {
		int threadsCnt = result->getRowsCnt();
		int topThCnt = threadsCnt;
		if(threadsCnt > topThreadsCnt) topThCnt = topThreadsCnt;
        for(int j = 0; j < topThCnt; j++)
        {
            threadsId.push_back(result->getIntValue(j, 0));
        }
    }
    return threadsId;
}

/////////////////////////////////////////////////////////////////////////////////////

CMenuGroup::CMenuGroup(std::string name, int id, int accessLevel)
{
	this->name = name;
	this->id = id;
	this->accessLevel = accessLevel;
}

CMenuGroup::~CMenuGroup()
{
	for(auto it = categories.begin(); it != categories.end(); it++)
	{
		CMenuCategory* currCat = (*it);
		delete currCat;
	}
}

int CMenuGroup::getAccessLevel() const
{
	return accessLevel;
}

const std::string CMenuGroup::getName() const
{
	return name;
}

int CMenuGroup::getId() const
{
	return id;
}

const std::vector<CMenuCategory*>* CMenuGroup::getAllCategoriesInGroup() const
{
	return &categories;
}

const CMenuCategory* CMenuGroup::findCategoryById(int catId) const
{
	for(auto it = categories.begin(); it != categories.end(); it++)
	{
		CMenuCategory* currCat = (*it);
		if(currCat->getId() == catId) return currCat;
	}
	return NULL;
}

const CMenuCategory* CMenuGroup::findCategoryByName(std::string catName) const
{
	for(auto it = categories.begin(); it != categories.end(); it++)
	{
		CMenuCategory* currCat = (*it);
		if(currCat->getName().compare(catName) == 0) return currCat;
	}
	return NULL;
}

void CMenuGroup::addCategoryInGroup(CMenuCategory* newCategory)
{
	categories.push_back(newCategory);
}
