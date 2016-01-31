#ifndef CSURVEY_H
#define CSURVEY_H

#include "novemberlib/interfaces/IStringSerializer.h"
#include "novemberlib/interfaces/IValidTester.h"

#include <string>
#include <vector>
#include <map>

class CSurveyFieldResult
{
	public:
		int answersCnt;
		std::map<std::string, int> textAnswers;
		std::map<int, int> comboAnswersCnt;

		std::vector<std::string> getMostCommonTextAnswers(const int cnt = 5) const;

		CSurveyFieldResult();
		~CSurveyFieldResult();
};

class CSurveyField : public IStringSerializer, public IValidTester
{
	public:
		static const int MAX_COMBO_VALUES = 20;

		static const int FIELD_CHECKBOX = 0;
		static const int FIELD_RADIO	= 1;
		static const int FIELD_TEXT		= 2;
		static const int FIELD_COMBO	= 3;

		int getType() const;
		const std::string getText() const;
		const std::vector<std::string>* getComboValues() const;
		const CSurveyFieldResult* getResults() const;
		CSurveyFieldResult* getResultsForModify() const;
		int getComboValuesCnt() const;

		virtual std::string toString() const;
		virtual void fromString(std::string& str);

		CSurveyField(const int type, const std::string& text, const std::vector<std::string>& comboValues);
		CSurveyField(const int type, const std::string& text);
		CSurveyField();
		virtual ~CSurveyField();
	private:
		int type;
		std::string text;
		std::vector<std::string> comboValues;

		mutable CSurveyFieldResult results;
};

class CSurveyQuestion : public IStringSerializer, public IValidTester
{
	public:
		static const int MAX_FIELDS = 20;

		int getFieldsCnt() const;
		const std::string getText() const;
		const std::vector<CSurveyField*>* getFields() const;

		virtual std::string toString() const;
		virtual void fromString(std::string& str);

		CSurveyQuestion(const std::string& text, const std::vector<CSurveyField*>& fields);
		CSurveyQuestion(const std::string& text);
		CSurveyQuestion();
		virtual ~CSurveyQuestion();
	private:
		std::string text;
		std::vector<CSurveyField*> fields;
};

class CSurveyQuestionAnswer : public IStringSerializer, public IValidTester
{
	public:
		int getFieldsCnt() const;
		const std::string getFieldValue(const int fieldId) const;
		const std::map<int, std::string>* getFields() const;
		const std::string getContent() const;

		virtual std::string toString() const;
		virtual void fromString(std::string& str);

		CSurveyQuestionAnswer();
		~CSurveyQuestionAnswer();
	private:
		std::map<int, std::string> fieldValues;
		std::string contentStr;
};

class CSurveyAnswer : public IValidTester
{
	public:
		int getId() const;
		int getSurveyId() const;
		int getUserId() const;
		long getCreationDate() const;
		const std::vector<CSurveyQuestionAnswer*>* getQuestionsAnswers() const;
		int getQuestionsAnswersCnt() const;

		const std::string getContent() const;

		std::string contentToString() const;
		void contentFromString(std::string& str);

		CSurveyAnswer(const int id, const int surveyId, const int userId, const std::string& contentStr, const long creationDate);
		~CSurveyAnswer();
	private:
		int id;
		int surveyId;
		int userId;
		long creationDate;
		std::vector<CSurveyQuestionAnswer*> questionsAnswers;

		std::string contentStr;
};

class CSurvey : public IValidTester
{
	public:
		static const int MAX_QUESTIONS = 30;

		int getId() const;
		int getOwnerId() const;
		const std::string getContent() const;
		const std::string getTitle() const;
		const std::string getDescription() const;
		const std::vector<CSurveyAnswer*>* getAnswers() const;
		const std::vector<CSurveyQuestion*>* getQuestions() const;
		const std::vector<CSurveyField*> getAllFields() const;

		long getCreationDate() const;
		long getEndDate() const;
		bool getIsEnded() const;

		int getAnswersCnt() const;
		int getQuestionsCnt() const;
		bool getIsValid() const;
		bool getIsUserInMembers(const int userId) const;
		bool getIsAnswerCorrect(const CSurveyAnswer* answer) const;
		CSurveyAnswer* getUserAnswer(const int userId) const;

		void calcResults() const;

		std::string contentToString() const;
		void contentFromString(std::string& str);

		CSurvey(const int surveyId);
		CSurvey();
		~CSurvey();
	protected:
	private:
		int id;
		int ownerId;
		std::string contentStr;
		std::string title;
		std::string description;
		bool isValid;

		long creationDate;
		long endDate;
		bool isEnded;

		mutable std::vector<CSurveyAnswer*> answers;
		std::vector<CSurveyQuestion*> questions;

		void readFromDB();
};

#endif // CSURVEY_H
