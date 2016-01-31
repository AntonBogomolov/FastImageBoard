#ifndef CUSER_H
#define CUSER_H

#include "novemberlib/CDefaultUser.h"

#include <string>
#include <vector>

class CBan;
class CSurvey;

class CUser : public CDefaultUser
{
	public:
		int getUserType(const int catId) const;
		int getUserAgeRating() const;
		bool getIsModerInAnyCategories() const;

		bool getIsThreadHidden(const int threadId) const;
		bool getIsPostHidden(const int postId) const;
		bool getIsThreadFavorite(const int threadId) const;
		bool getIsPostFavorite(const int postId) const;
		bool getIsThreadFixed(const int threadId) const;

		void hidePost(const int postId);
		void hideThread(const int threadId);
		void addToFavoritePost(const int postId);
		void addToFavoriteThread(const int threadId);
		void addToFixedThread(const int threadId);

		void unhidePost(const int postId);
		void unhideThread(const int threadId);
		void removeFromFavoritePost(const int postId);
		void removeFromFavoriteThread(const int threadId);
		void removeFromFixedThread(const int threadId);

		const std::vector<int>* getHiddenThreads() const;
		const std::vector<int>* getHiddenPosts() const;
		const std::vector<int>* getFavoriteThreads() const;
		const std::vector<int>* getFavoritePosts() const;
		const std::vector<int>* getFixedThreads() const;

		const std::vector<CSurvey*>* getSurveys() const;
		const std::vector<int>* getAnsweredSurveysId() const;

		bool getIsUserSurveyMember(const int surveyId) const;
		bool getIsUserSurveyOwner(const int surveyId) const;

		std::string getHiddenThreadsStr() const;
		std::string getHiddenPostsStr() const;
		std::string getFavoriteThreadsStr() const;
		std::string getFavoritePostsStr() const;
		std::string getFixedThreadsStr() const;

		//static CUser* createNewUser(const std::string& cookieData, const std::string& remouteAddress, const std::string& userAgent);
		virtual void fillUserData(const std::string& cookieData);

		CUser();
		CUser(const int userId);
		virtual ~CUser();
	protected:
		int ageRating;

		std::vector<int> hiddenThreads;
		std::vector<int> hiddenPosts;
		std::vector<int> favoritesThreads;
		std::vector<int> favoritesPosts;
		std::vector<int> fixedThreads;

		std::vector<CSurvey*> surveys;
		std::vector<int> answeredSurveysId;
		std::vector<int> moderCats;
	private:
};

#endif // CUSER_H
