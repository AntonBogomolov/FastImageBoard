#ifndef CMENU_H
#define CMENU_H

#include <vector>
#include <string>

class CMenuCategory
{
	public:
		const std::string getName() const;
		const std::string getDescription() const;
		int getAllThreadsCnt() const;
		int getId() const;
		int getAccessLevel() const;
		int getAgeRating() const;
		int getIsNeedPremod() const;
		int getThreadCreationAccessLevel() const;
		int getPostCreationAccessLevel() const;
		bool getIsAllowMIME(std::string MIME) const;

		const std::vector<int> getThreadsId(const int topThreadsCnt) const;

		CMenuCategory(const std::string name,const std::string description,const int id, const int threadsCnt, const int accessLevel, const int ageRating, bool isNeedPremod, int threadCreationLevel, int postCreationLevel, std::string allowMIMES);
		~CMenuCategory();
	private:
		void initCategory() const;

		std::string name;
		std::string description;
		mutable std::vector<int> threadsId;
		int id;
		mutable int threadsCnt;
		int accessLevel;
		int threadCreationLevel;
		int postCreationLevel;
		int ageRating;
		bool needPremod;

		std::string allowMIMES;
};

class CMenuGroup
{
	public:
		const std::string getName() const;
		int getId() const;
		int getAccessLevel() const;

		const std::vector<CMenuCategory*>* getAllCategoriesInGroup() const;
		const CMenuCategory* findCategoryById(int catId) const;
		const CMenuCategory* findCategoryByName(std::string catName) const;
		void addCategoryInGroup(CMenuCategory* newCategory);

		CMenuGroup(std::string name, int id, int accessLevel);
		~CMenuGroup();
	private:
		std::string name;
		int id;
		int accessLevel;

		std::vector<CMenuCategory*> categories;
};

class CMenu
{
	public:
		static const CMenu* getInstance();

		const CMenuGroup* findGroupById(int grId) const;
		const CMenuGroup* findGroupByName(std::string grName) const;
		const CMenuCategory* findCategoryById(int catId) const;
		const CMenuCategory* findCategoryByName(std::string catName) const;

		const std::vector<CMenuCategory*>* 	getAllCategoriesInGroup(int grId) const;
		const std::vector<CMenuGroup*>* 	getAllGroups() const;

		~CMenu();
	protected:
	private:
		CMenu();
		void readMenuFromDB();

		std::vector<CMenuGroup*> menuGroups;
		static CMenu* instance;
};

#endif // CMENU_H
