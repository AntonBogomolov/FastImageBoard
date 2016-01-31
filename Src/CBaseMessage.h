#ifndef CBASEMESSAGE_H
#define CBASEMESSAGE_H

#include <string>
#include <vector>

class CAttachment;

class CBaseMessage
{
	public:
		int getId() const;
		int getUserId() const;
		int getComplainCnt() const;
		int getAgeRating() const;

		const std::string getText() const;
		const std::string getUserName() const;

		long getCreationDate() const;
		long getModDate() const;
		const std::string getAttachmentString() const;

		const std::vector<CAttachment*>* getAttachments() const;
		const std::vector<int>* getAnswersId() const;
		bool getIsValid() const;

		virtual int getType() = 0;

		CBaseMessage(const int id);
		CBaseMessage(const int id, const int userId, const std::string text, const std::string userName, const long creationDate, const std::string attachmentString, const std::string answersIdsStr, const int complainCnt, const long modDate, const int ageRating);
		virtual ~CBaseMessage();
	protected:
		bool isValid;
		int id;
		int userId;
		int complainCnt;
		int ageRating;
		std::string text;
		std::string userName;
		long creationDate;
		long modDate;
		std::string attachmentString;
		std::string answersIdsStr;
		std::vector<int> answersIds;
		std::vector<CAttachment*>* attachments;

	private:
};

#endif // CBASEMESSAGE_H
