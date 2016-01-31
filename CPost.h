#ifndef CPOST_H
#define CPOST_H

#include "CBaseMessage.h"

#include <string>
#include <vector>

class CAttachment;

class CPost : public CBaseMessage
{
	public:
		int getThreadId() const;
		int getCatId() const;
		const std::string getParentId() const;
		bool getIsNeedPremod()  const;

		virtual int getType();

		CPost(const int postId);
		CPost(const int postId, const int userId, const int threadId, const std::string text, const std::string userName, const long creationDate, const std::string attachmentString, const std::string answersIdsStr, const std::string parentId, bool isNeedPremod, const int complainCnt, const long modDate, const int ageRating, const int catId);
		virtual ~CPost();
	protected:
	private:
		bool isNeedPremod;
		int threadId;
		int catId;
		std::string parentId;

		void readPostFromDB();
};

#endif // CPOST_H
