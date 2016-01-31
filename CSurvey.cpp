#include "CSurvey.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/managers/CDBManager.h"

#include <algorithm>

CSurvey::CSurvey(const int surveyId)
{
	this->id = surveyId;

	this->title = "";
	this->description = "";
	this->contentStr = "";
	this->ownerId = 0;
	this->isValid = false;
	this->creationDate = 0;
	this->endDate = 0;
	this->isEnded = false;

	isValid = false;

	readFromDB();
}

CSurvey::CSurvey()
{
	this->id = 0;

	this->title = "";
	this->description = "";
	this->contentStr = "";
	this->ownerId = 0;
	this->isValid = false;
	this->creationDate = 0;
	this->endDate = 0;
	this->isEnded = false;

	isValid = false;
}

CSurvey::~CSurvey()
{
	for(auto it = answers.begin(); it != answers.end(); ++it)
	{
		CSurveyAnswer* currAnsw = (*it);
		if(currAnsw) delete currAnsw;
	}
	for(auto it = questions.begin(); it != questions.end(); ++it)
	{
		CSurveyQuestion* currQuestion = (*it);
		if(currQuestion) delete currQuestion;
	}
}

void CSurvey::readFromDB()
{
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("*"), "Surveys", "`id` = " + valueToString(id));
	//const CDBRequestResult* result = dbRequest->createRequest("SELECT * FROM `Surveys` WHERE `id` = " + valueToString(id) + ";");
	if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
	{
		ownerId 		= result->getIntValue(0, 1);
		contentStr 		= result->getStringValue(0, 2);
		creationDate 	= result->getLongValue(0, 3);
		endDate 		= result->getLongValue(0, 4);
		isEnded 		= result->getBoolValue(0, 5);
		title 			= result->getStringValue(0, 6);
		description 	= result->getStringValue(0, 7);

		time_t currUnixTime;
		time(&currUnixTime);
		if(endDate < currUnixTime) isEnded = true;

		contentFromString(contentStr);

		isValid = true;
	}
}

int CSurvey::getId() const
{
	return id;
}

int CSurvey::getOwnerId() const
{
	return ownerId;
}

const std::string CSurvey::getContent() const
{
	return contentStr;
}

const std::string CSurvey::getTitle() const
{
	return title;
}

const std::string CSurvey::getDescription() const
{
	return description;
}

const std::vector<CSurveyAnswer*>* CSurvey::getAnswers() const
{
	return &answers;
}

const std::vector<CSurveyQuestion*>* CSurvey::getQuestions() const
{
	return &questions;
}

int CSurvey::getAnswersCnt() const
{
	return answers.size();
}

int CSurvey::getQuestionsCnt() const
{
	return questions.size();
}

long CSurvey::getCreationDate() const
{
	return creationDate;
}

long CSurvey::getEndDate() const
{
	return endDate;
}

bool  CSurvey::getIsEnded() const
{
	return isEnded;
}

bool CSurvey::getIsValid() const
{
	return isValid;
}

bool CSurvey::getIsUserInMembers(const int userId) const
{
	bool result = false;
	for(auto it = answers.begin(); it != answers.end(); ++it)
	{
		CSurveyAnswer* currAnsw = (*it);
		if(currAnsw->getUserId() == userId)
		{
			result = true;
			break;
		}
	}
	return result;
}

CSurveyAnswer* CSurvey::getUserAnswer(const int userId) const
{
	CSurveyAnswer* result = NULL;
	for(auto it = answers.begin(); it != answers.end(); ++it)
	{
		CSurveyAnswer* currAnsw = (*it);
		if(currAnsw->getUserId() == userId)
		{
			result = currAnsw;
			break;
		}
	}
	return result;
}

const std::vector<CSurveyField*> CSurvey::getAllFields() const
{
	std::vector<CSurveyField*> fields;
	if(!getIsValid()) return fields;

	for(auto it = questions.begin(); it != questions.end(); ++it)
    {
		CSurveyQuestion* currQuestion = (*it);
		if(!currQuestion->getIsValid()) continue;

		const std::vector<CSurveyField*>* questFields = currQuestion->getFields();
		fields.insert(fields.end(), questFields->begin(), questFields->end());
    }

    return fields;
}

void CSurvey::calcResults() const
{
	if(!getIsValid()) return;

	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());

	const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("*"), "SurveysResults", "`surveyId` = " + valueToString(id));
    //const CDBRequestResult* result = dbRequest->createRequest("SELECT * FROM `SurveysResults` WHERE `surveyId` = " + valueToString(id) + ";");
    if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
    {
        int answerId = 0;
        int userAnswerId = 0;
        long answerCreationDate = 0;
        std::string answerContent = "";

        for(int i = 0; i < (int)result->getRowsCnt(); i++)
        {
			answerId 			= result->getIntValue(i, 0);
			userAnswerId		= result->getIntValue(i, 1);
			answerContent 		= result->getStringValue(i, 3);
			answerCreationDate 	= result->getLongValue(i, 4);

			CSurveyAnswer* answer = new CSurveyAnswer(answerId, id, userAnswerId, answerContent, answerCreationDate);
			if(answer->getIsValid() && getIsAnswerCorrect(answer)) answers.push_back(answer);
			else delete answer;
        }
    }

    CLog::getInstance()->addInfo("answers cnt: "+ valueToString(answers.size()));

    for(auto itAnswer = answers.begin(); itAnswer != answers.end(); ++itAnswer)
    {
		const CSurveyAnswer* currAnswer = (*itAnswer);
		if(!currAnswer->getIsValid()) continue;
		const std::vector<CSurveyQuestionAnswer*>* answerQuestions = currAnswer->getQuestionsAnswers();

		for(auto itQuestion = answerQuestions->begin(); itQuestion != answerQuestions->end(); ++itQuestion)
		{
			const CSurveyQuestionAnswer* currQuestionAnswer = (*itQuestion);
			if(!currQuestionAnswer->getIsValid()) continue;
			if( currQuestionAnswer->getFieldsCnt() == 0) continue;

			int questionNumber = itQuestion - answerQuestions->begin();
			const std::map<int, std::string>* answerQuestionField = currQuestionAnswer->getFields();
			for(auto itFields = answerQuestionField->begin(); itFields != answerQuestionField->end(); ++itFields )
			{
				const int number = itFields->first;
				const std::string fieldStrValue = itFields->second;
				int fieldValue = 0;

				const CSurveyField* surveyField = NULL;
				try
				{
					surveyField = questions.at(questionNumber)->getFields()->at(number);
					if(surveyField && surveyField->getType() != CSurveyField::FIELD_TEXT) fieldValue = std::stoi(fieldStrValue);
				}
				catch(...)
				{
					surveyField = NULL;
				}
				if(surveyField == NULL) continue;

				std::string tmpStr = "";
				int comboValCnt = 0;
				CSurveyFieldResult* result = surveyField->getResultsForModify();
				switch(surveyField->getType())
				{
					case CSurveyField::FIELD_CHECKBOX:
					case CSurveyField::FIELD_RADIO:
						if(fieldValue == 1) result->answersCnt++;
					break;
					case CSurveyField::FIELD_COMBO:
						comboValCnt = surveyField->getComboValuesCnt();
						if(fieldValue < comboValCnt) result->comboAnswersCnt[fieldValue] = result->comboAnswersCnt[fieldValue] + 1;
					break;
					case CSurveyField::FIELD_TEXT:
						tmpStr = fieldStrValue;
						//boost::algorithm::to_lower(tmpStr);
						//boost::algorithm::trim(tmpStr);

						//result->textAnswers[tmpStr] = 1;
					break;
					default:
					break;
				}
			}
		}
    }
}

bool CSurvey::getIsAnswerCorrect(const CSurveyAnswer* answer) const
{
	if(!getIsValid()) return false;
	if(!answer->getIsValid()) return false;
	if( answer->getSurveyId() != id) return false;

	const std::vector<CSurveyQuestionAnswer*>* answerQuestions = answer->getQuestionsAnswers();
	if((int)answerQuestions->size() != getQuestionsCnt()) return false;
	for(auto itQuestion = answerQuestions->begin(); itQuestion != answerQuestions->end(); ++itQuestion)
	{
		const CSurveyQuestionAnswer* currQuestionAnswer = (*itQuestion);
		if(!currQuestionAnswer->getIsValid()) return false;
		if( currQuestionAnswer->getFieldsCnt() == 0) return false;

		int questionNumber = itQuestion - answerQuestions->begin();
		const std::map<int, std::string>* answerQuestionField = currQuestionAnswer->getFields();
		for(auto itFields = answerQuestionField->begin(); itFields != answerQuestionField->end(); ++itFields )
		{
			const int number = itFields->first;
			const std::string fieldValue = itFields->second;

			const CSurveyField* surveyField = NULL;
			try
			{
				surveyField = questions.at(questionNumber)->getFields()->at(number);
			}
			catch(...)
			{
				return false;
			}
			if(!surveyField || !surveyField->getIsValid()) return false;

			if(surveyField->getType() == CSurveyField::FIELD_CHECKBOX || surveyField->getType() == CSurveyField::FIELD_RADIO)
			{
				int valueCheck = -1;
				try
				{
					valueCheck = std::stoi(fieldValue);
				}
				catch(...)
				{
					return false;
				}
				if(valueCheck < 0) return false;
			}
		}
	}

	return true;
}

std::string CSurvey::contentToString() const
{
	std::string result = "";

	if(!getIsValid()) return "Not valid!";

	result += valueToString(questions.size()) + ";";
	for(unsigned int i = 0; i < questions.size(); i++)
	{
		std::string questionsStr = questions[i]->toString();
		std::vector<std::string> parts;
		//boost::split(parts, questionsStr, boost::is_any_of(";"));
		split(parts, questionsStr, ";");
		result += valueToString(parts.size() - 1) + ";" + questionsStr;
	}

	return result;
}

void CSurvey::contentFromString(std::string& str)
{
	std::vector<std::string> parts;
    setIsValid(false);
    questions.clear();
   // boost::split(parts, str, boost::is_any_of(";"));
	split(parts, str, ";");
    if((int)parts.size() > 2000 ) return;

    if(parts.size() < 1) return;
    int questionsCnt = 0;
    try
    {
		questionsCnt = std::stoi(parts[0]);
	}
	catch(...)
	{
		return;
	}
	if(questionsCnt > MAX_QUESTIONS) return;

	int currPartPos = 1;
    for(int i = 0; i < questionsCnt; i++)
    {
		if(currPartPos >= (int)parts.size()) return;

		int questionPartsCnt = 0;
		std::string questionStr = "";
		try
		{
			questionPartsCnt = std::stoi(parts[currPartPos]);
		}
		catch(...)
		{
			return;
		}
		currPartPos++;

		for(int j = 0; j < questionPartsCnt; j++)
		{
			if(currPartPos >= (int)parts.size()) return;

			questionStr += parts[currPartPos] + ";";
			currPartPos++;
		}
		CSurveyQuestion* question = new CSurveyQuestion();
		question->fromString(questionStr);
		if(question->getIsValid() && question->getFieldsCnt() > 0) questions.push_back(question);
		else delete question;
    }

    setIsValid(true);
}

/////////////////////////////////////////////////////////////////////////////////////

CSurveyAnswer::CSurveyAnswer(const int id, const int surveyId, const int userId, const std::string& contentStr, const long creationDate)
{
	this->id = id;
	this->surveyId = surveyId;
	this->userId = userId;
	this->contentStr = contentStr;
	this->creationDate = creationDate;

	contentFromString(this->contentStr);
}

CSurveyAnswer::~CSurveyAnswer()
{
	for(auto it = questionsAnswers.begin(); it != questionsAnswers.end(); ++it)
	{
		delete (*it);
	}
}

int CSurveyAnswer::getId() const
{
	return id;
}

int CSurveyAnswer::getSurveyId() const
{
	return surveyId;
}

int CSurveyAnswer::getUserId() const
{
	return userId;
}

long CSurveyAnswer::getCreationDate() const
{
	return creationDate;
}

const std::string CSurveyAnswer::getContent() const
{
	return contentStr;
}

const std::vector<CSurveyQuestionAnswer*>* CSurveyAnswer::getQuestionsAnswers() const
{
	return &questionsAnswers;
}

int CSurveyAnswer::getQuestionsAnswersCnt() const
{
	return (int)questionsAnswers.size();
}

std::string CSurveyAnswer::contentToString() const
{
	std::string result = "";

	if(!getIsValid()) return "Not valid!";

	result += valueToString(questionsAnswers.size()) + ";";
	for(int i = 0; i < (int)questionsAnswers.size(); i++ )
	{
		if(questionsAnswers[i]->getIsValid()) result += questionsAnswers[i]->toString();
	}

	return result;
}

void CSurveyAnswer::contentFromString(std::string& str)
{
	std::vector<std::string> parts;
    setIsValid(false);
    questionsAnswers.clear();
    //boost::split(parts, str, boost::is_any_of(";"));
    split(parts, str, ";");
    if((int)parts.size() > 2000) return;

    if(parts.size() < 1) return;
    int questionsCnt = 0;
    try
    {
		questionsCnt = std::stoi(parts[0]);
	}
	catch(...)
	{
		return;
	}
	if(questionsCnt == 0 || questionsCnt > CSurvey::MAX_QUESTIONS) return;

	int currPartPos = 1;
    for(int i = 0; i < questionsCnt; i++)
    {
		if(currPartPos >= (int)parts.size()) return;

		int questionPartsCnt = 0;
		std::string questionStr = parts[currPartPos] + ";";
		try
		{
			questionPartsCnt = std::stoi(parts[currPartPos]);
		}
		catch(...)
		{
			return;
		}
		currPartPos++;

		for(int j = 0; j < questionPartsCnt * 2; j++)
		{
			if(currPartPos >= (int)parts.size()) return;

			questionStr += parts[currPartPos] + ";";
			currPartPos++;
		}

		CSurveyQuestionAnswer* question = new CSurveyQuestionAnswer();
		question->fromString(questionStr);
		if(question->getIsValid() && question->getFieldsCnt() > 0)	questionsAnswers.push_back(question);
		else delete question;
    }
    setIsValid(true);
}

/////////////////////////////////////////////////////////////////////////////////////

CSurveyQuestionAnswer::CSurveyQuestionAnswer()
{
	isValid = false;
}

CSurveyQuestionAnswer::~CSurveyQuestionAnswer()
{

}

int CSurveyQuestionAnswer::getFieldsCnt() const
{
	return fieldValues.size();
}

const std::string CSurveyQuestionAnswer::getFieldValue(const int fieldId) const
{
	auto it = fieldValues.find(fieldId);
	if(it != fieldValues.end()) return it->second;

	return "";
}

const std::map<int, std::string>* CSurveyQuestionAnswer::getFields() const
{
	return &fieldValues;
}

const std::string CSurveyQuestionAnswer::getContent() const
{
	return contentStr;
}

std::string CSurveyQuestionAnswer::toString() const
{
	std::string result = "";

	if(!getIsValid()) return "Not valid!";

	result += valueToString(fieldValues.size()) + ";";
	for(auto it = fieldValues.begin(); it != fieldValues.end(); ++it)
	{
		std::string fieldText = it->second;
		if(fieldText.find(";") != std::string::npos) fieldText = replaceAll(fieldText, ";", " ");
		result += valueToString(it->first) + ";" + fieldText + ";";
	}
	return result;
}

void CSurveyQuestionAnswer::fromString(std::string& str)
{
	fieldValues.clear();

	std::vector<std::string> parts;
    setIsValid(false);
    //boost::split(parts, str, boost::is_any_of(";"));
    split(parts, str, ";");
    if((int)parts.size() > 2000) return;

    if(parts.size() < 1) return;

    int fieldsCnt = 0;
    try
    {
		fieldsCnt = std::stoi(parts[0]);
    }
    catch(...)
    {
		return;
    }
    if(fieldsCnt > CSurveyQuestion::MAX_FIELDS) return;

    int currPartNumber = 1;
    int fieldNumber = 0;
    for(int i = 0; i < fieldsCnt; i++)
    {
		int fieldType = 0;
		try
		{
			fieldType = std::stoi(parts[currPartNumber]);
		}
		catch(...)
		{
			return;
		}
		currPartNumber++;

		if( fieldType >= 0 && fieldType <= 4 && fieldNumber < CSurveyQuestion::MAX_FIELDS )
		{
			fieldValues[fieldNumber] = parts[currPartNumber];
			fieldNumber++;
		}
		currPartNumber++;
    }

	bool oneValChecked = false;
	if((int)fieldValues.size() == 1) oneValChecked = true;
	else
	{
		for(auto it = fieldValues.begin(); it != fieldValues.end(); ++it)
		{
			std::string fieldValue = it->second;

			int fieldCheckState = 0;
			try
			{
				fieldCheckState = std::stoi(fieldValue);
			}
			catch(...)
			{
				return;
			}
			if(fieldCheckState == 1) oneValChecked = true;
		}
    }
    if(oneValChecked) setIsValid(true);
}

/////////////////////////////////////////////////////////////////////////////////////

CSurveyField::CSurveyField(const int type, const std::string& text, const std::vector<std::string>& comboValues)
{
	this->type = type;
	this->text = text;

	this->comboValues = comboValues;

	if(this->type >= 0 && this->type <= 3) isValid = true;
}

CSurveyField::CSurveyField(const int type, const std::string& text)
{
	this->type = type;
	this->text = text;

	if(this->type >= 0 && this->type <= 2) isValid = true;
}

CSurveyField::CSurveyField()
{
	this->type = -1;
	this->text = "";
}

CSurveyField::~CSurveyField()
{

}

int CSurveyField::getType() const
{
	return type;
}

const std::string CSurveyField::getText() const
{
	return text;
}

const std::vector<std::string>* CSurveyField::getComboValues() const
{
	return &comboValues;
}

int CSurveyField::getComboValuesCnt() const
{
	return (int)comboValues.size();
}

const CSurveyFieldResult* CSurveyField::getResults() const
{
	return &results;
}

CSurveyFieldResult* CSurveyField::getResultsForModify() const
{
	return &results;
}

std::string CSurveyField::toString() const
{
	std::string result = "";

	if(!getIsValid()) return "Not valid!";

	std::string fieldText = text;
	if(fieldText.find(";") != std::string::npos) fieldText = replaceAll(fieldText, ";", " ");

	result += valueToString(type) + ";" + fieldText + ";" + valueToString(comboValues.size()) + ";";
	for(auto it = comboValues.begin(); it != comboValues.end(); ++it)
	{
		std::string comboText = (*it);
		if(comboText.find(";") != std::string::npos) comboText = replaceAll(comboText, ";", " ");
		result += comboText + ";";
	}

	return result;
}

void CSurveyField::fromString(std::string& str)
{
    std::vector<std::string> parts;
    setIsValid(false);
    comboValues.clear();
    //boost::split(parts, str, boost::is_any_of(";"));
    split(parts, str, ";");
    if((int)parts.size() > 2000) return;

    if(parts.size() < 3) return;
    int comboValuesSize = 0;
    text = parts[1];
    try
    {
		type = std::stoi(parts[0]);
		comboValuesSize = std::stoi(parts[2]);
	}
	catch(...)
	{
		return;
	}
	if(comboValuesSize > CSurveyField::MAX_COMBO_VALUES) return;

	if((int)parts.size() < 3 + comboValuesSize) return;
    for(int i = 0; i < comboValuesSize; i++)
    {
        comboValues.push_back(parts[3 + i]);
    }

    setIsValid(true);
}

/////////////////////////////////////////////////////////////////////////////////////

CSurveyQuestion::CSurveyQuestion(const std::string& text,  const std::vector<CSurveyField*>& fields)
{
	this->text = text;
	this->fields = fields;

	isValid = true;
}

CSurveyQuestion::CSurveyQuestion(const std::string& text)
{
	this->text = text;

	isValid = true;
}

CSurveyQuestion::CSurveyQuestion()
{
	this->text = "";
}

CSurveyQuestion::~CSurveyQuestion()
{
	for(auto it = fields.begin(); it != fields.end(); ++it)
	{
		CSurveyField* currField = (*it);
		delete currField;
	}
}

int CSurveyQuestion::getFieldsCnt() const
{
	return fields.size();
}

const std::string CSurveyQuestion::getText() const
{
	return text;
}

const std::vector<CSurveyField*>* CSurveyQuestion::getFields() const
{
	return &fields;
}

std::string CSurveyQuestion::toString() const
{
	std::string result = "";

	if(!getIsValid()) return "Not valid!";

	std::string fieldText = text;
	if(fieldText.find(";") != std::string::npos) fieldText = replaceAll(fieldText, ";", " ");

	result += fieldText + ";" + valueToString(fields.size()) + ";";
	for(unsigned int i = 0; i < fields.size(); i++)
	{
		std::string fielsStr = fields[i]->toString();
		std::vector<std::string> parts;
		//boost::split(parts, fielsStr, boost::is_any_of(";"));
		split(parts, fielsStr, ";");
		result += valueToString(parts.size() - 1) + ";" + fielsStr;
	}

	return result;
}

void CSurveyQuestion::fromString(std::string& str)
{
	std::vector<std::string> parts;
    setIsValid(false);
    fields.clear();
    //boost::split(parts, str, boost::is_any_of(";"));
    split(parts, str, ";");
    if((int)parts.size() > 2000) return;

    if(parts.size() < 2) return;
    int fieldsCnt = 0;
    text = parts[0];
    try
    {
		fieldsCnt = std::stoi(parts[1]);
	}
	catch(...)
	{
		return;
	}
	if(fieldsCnt == 0 || fieldsCnt > CSurveyQuestion::MAX_FIELDS) return;

	int currPartPos = 2;
    for(int i = 0; i < fieldsCnt; i++)
    {
		if(currPartPos >= (int)parts.size()) return;

		int fieldPartsCnt = 0;
		std::string fieldStr = "";
		try
		{
			fieldPartsCnt = std::stoi(parts[currPartPos]);
		}
		catch(...)
		{
			return;
		}
		currPartPos++;

		for(int j = 0; j < fieldPartsCnt; j++)
		{
			if(currPartPos >= (int)parts.size()) return;

			fieldStr += parts[currPartPos] + ";";
			currPartPos++;
		}
		CSurveyField* field = new CSurveyField();
		field->fromString(fieldStr);
		if(field->getIsValid())fields.push_back(field);
		else delete field;
    }

    setIsValid(true);
}

/////////////////////////////////////////////////////////////////////////////////////

CSurveyFieldResult::CSurveyFieldResult()
{
	answersCnt = 0;
	for(int i = 0; i < CSurveyField::MAX_COMBO_VALUES; i ++)
	{
		comboAnswersCnt[i] = 0;
	}
}

CSurveyFieldResult::~CSurveyFieldResult()
{

}

std::vector<std::string> CSurveyFieldResult::getMostCommonTextAnswers(const int cnt) const
{
	std::vector<std::string> result;

	std::vector<std::pair<std::string, int>> items;
	for(auto it = textAnswers.begin(); it != textAnswers.end(); ++it)
	{
		items.push_back(std::make_pair(it->first, it->second));
	}

	auto cmp = [](std::pair<std::string, int> const & a, std::pair<std::string, int> const & b)
	{
		 return a.second != b.second?  a.second < b.second : a.first < b.first;
	};
	std::sort(items.begin(), items.end(), cmp);

	int currCnt = 0;
	for(auto it = items.begin(); it != items.end(); ++it)
	{
		if(currCnt >= cnt) break;
		result.push_back(it->first);
		currCnt++;
	}

	return result;
}
