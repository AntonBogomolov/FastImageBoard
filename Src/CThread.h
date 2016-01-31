#ifndef CTHREAD_H
#define CTHREAD_H

#include "CBaseMessage.h"

#include <string>
#include <vector>

class CAttachment;
class CPost;
class CSurvey;

class CThread : public CBaseMessage
{
	public:
        int getPostsCnt() const;
        int getCatId() const;
        int getPosition() const;
        const std::string getCaption() const;

		bool getIsExpand() const;
		bool getIsFixed()  const;
		bool getIsNeedPremod()  const;
		bool getIsHidden() const;

		void readAllPosts();
		void readLastPosts();
		const std::vector<CPost*>* getPosts() const;
		const CPost* findPostById(const int postId) const;
		const CSurvey* getSuvrey() const;

		virtual int getType();

		CThread(const int threadId, bool isExpand = false);
		virtual ~CThread();
	protected:
	private:
		bool isExpand;
		bool isThreadFixed;
		bool isNeedPremod;
		bool isHidden;

		int postsCnt;
		int catId;
		int surveyId;
		int position;
		std::string caption;
		std::vector<CPost*> posts;

		CSurvey* survey;

		void readPosts(const int lastPostsCnt);
		void readThreadFromDB();
};

#endif // CTHREAD_H
