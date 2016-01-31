#include "CBaseMessage.h"

#include "CAttachment.h"

CBaseMessage::CBaseMessage(const int id)
{
	this->id = id;

	this->isValid = false;
	this->attachments = NULL;
}

CBaseMessage::CBaseMessage(const int id, const int userId, const std::string text, const std::string userName, const long creationDate, const std::string attachmentString, const std::string answersIdsStr, const int complainCnt, const long modDate, const int ageRating)
{
	this->id = id;
	this->userId = userId;
	this->text = text;
	this->userName = userName;
	this->creationDate = creationDate;
	this->modDate = modDate;
	this->attachmentString = attachmentString;
	this->answersIdsStr = answersIdsStr;
	this->complainCnt = complainCnt;
	this->ageRating = ageRating;

	this->isValid = false;
	this->attachments = NULL;
}

CBaseMessage::~CBaseMessage()
{
	if(attachments)
	{
        for(unsigned int i = 0; i < attachments->size(); i++)
        {
            delete (*attachments)[i];
        }
	}
	delete attachments;
}

int CBaseMessage::getId() const
{
	return id;
}

int CBaseMessage::getUserId() const
{
	return userId;
}

int CBaseMessage::getComplainCnt() const
{
	return complainCnt;
}

int CBaseMessage::getAgeRating() const
{
	return ageRating;
}

const std::string CBaseMessage::getText() const
{
	return text;
}

const std::string CBaseMessage::getUserName() const
{
	return userName;
}

long CBaseMessage::getCreationDate() const
{
	return creationDate;
}

long CBaseMessage::getModDate() const
{
	return modDate;
}

const std::string CBaseMessage::getAttachmentString() const
{
	return attachmentString;
}

const std::vector<CAttachment*>* CBaseMessage::getAttachments() const
{
	return attachments;
}

const std::vector<int>* CBaseMessage::getAnswersId() const
{
	return &answersIds;
}

bool CBaseMessage::getIsValid() const
{
	return isValid;
}
