#include "CSitePage.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/helpers/CTemplateHelper.h"
#include "novemberlib/helpers/CMessageParser.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/CHTMLTemplate.h"

#include "CMenu.h"
#include "CPost.h"
#include "CThread.h"
#include "CAttachment.h"
#include "CUser.h"

#include "CSurvey.h"


CSitePage::CSitePage(const std::string name, const CFCGIRequest* currRequest) : CPage(name, currRequest)
{

}

CSitePage::~CSitePage()
{
	//dtor
}

std::string CSitePage::buildLeftPanel(const CUser* user)
{
	const CMenu* menu = CMenu::getInstance();
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;

	const CHTMLTemplate* leftPanelTemplate = templateManager->findTemplate("leftPanel");
	if(leftPanelTemplate == NULL) return "Missing menu template";

	std::string tmpString = "";
	const std::vector<CMenuGroup*>* menuGroups = menu->getAllGroups();
	for(unsigned int i = 0; i < menuGroups->size(); i++)
	{
		CMenuGroup* currGroup = (*menuGroups)[i];
		if(!user->getIsValid() || user->getUserType(0) < currGroup->getAccessLevel() ) continue;
		tmpString += "<tr class='group'><td>";
		tmpString += currGroup->getName();
		tmpString += "</td></tr> \n";

		const std::vector<CMenuCategory*>* menuCats = currGroup->getAllCategoriesInGroup();
		for(unsigned int j = 0; j < menuCats->size(); j++)
		{
			const CMenuCategory* currCat = (*menuCats)[j];
			if(!user->getIsValid() || user->getUserType(0) < currCat->getAccessLevel() || user->getUserAgeRating() < currCat->getAgeRating()) continue;
			tmpString += "<tr class='cat'><td><a href='?page=";
			tmpString += currCat->getName();
			tmpString += "'>";
			tmpString += currCat->getDescription();
			tmpString += "</a></td></tr> \n";
		}
	}

	params["{MENU}"] = tmpString;
	return  leftPanelTemplate->getHTMLData(&params);
}

std::string CSitePage::buildUpperMenu(const CUser* user)
{
	const CMenu* menu = CMenu::getInstance();
	std::string tmpString;

	const std::vector<CMenuGroup*>* menuGroups = menu->getAllGroups();
	for(unsigned int i = 0; i < menuGroups->size(); i++)
	{
		CMenuGroup* currGroup = (*menuGroups)[i];
		if(!user->getIsValid() || user->getUserType(0) < currGroup->getAccessLevel() ) continue;
		tmpString += "<span>[</span>";

		const std::vector<CMenuCategory*>* menuCats = currGroup->getAllCategoriesInGroup();
		for(unsigned int j = 0; j < menuCats->size(); j++)
		{
			const CMenuCategory* currCat = (*menuCats)[j];
			if(!user->getIsValid() || user->getUserType(0) < currCat->getAccessLevel() || user->getUserAgeRating() < currCat->getAgeRating() ) continue;
			if(j > 0) tmpString += "<span> | </span>";
			tmpString += "<a href='?page=";
			tmpString += currCat->getName();
			tmpString += "'>";
			tmpString += currCat->getName();
			tmpString += "</a> \n";
		}
		tmpString += "<span>] </span>";
	}

	return  tmpString;
}

std::string CSitePage::buildPost(const int postId, const CUser* user, bool isPopup)
{
	std::string resultHTML;

	CPost* post = new CPost(postId);
	resultHTML = buildPost(post, user, isPopup);
	delete post;

	return resultHTML;
}

std::string CSitePage::buildPost(const CPost* post, const CUser* user, bool isPopup)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;

	const CHTMLTemplate* postTemplate = templateManager->findTemplate("post");
	if(postTemplate == NULL) return "Missing post template";

	if(!post->getIsValid()) return "Cant create post";
	bool isPostHidden = user->getIsPostHidden(post->getId());

	if(user->getUserType(post->getCatId()) <= CUser::UT_USER && post->getIsNeedPremod() && post->getUserId() != user->getUserId()) return "";

	params["{POSTID}"] 	 = valueToString(post->getId());
	params["{PARENTID}"] = post->getParentId();
	params["{USERNAME}"] = htmlspecialchars(post->getUserName());
	params["{USERID}"] 	 = valueToString(post->getUserId());
	params["{DATE}"] 	 = unixTimeToDate(post->getCreationDate());
	params["{EDITTIME}"] = unixTimeToDate(post->getModDate());
	params["{ID}"] 	 	 = valueToString(post->getId());
	params["{COMPLAINTS}"] = valueToString(post->getComplainCnt());
	params["{CONTROLS}"] = isPopup ? " " : buildPostControls(post, user);
	switch(post->getAgeRating())
	{
		case 0:
			params["{AGE}"] = " Для всех ";
		break;
		case 1:
			params["{AGE}"] = " После 13 лет ";
		break;
		case 2:
			params["{AGE}"] = " После 17 лет ";
		break;
		case 3:
			params["{AGE}"] = " После 18 лет ";
		break;
		default:
			params["{AGE}"] = valueToString(post->getAgeRating());
		break;
	}

	params["{isNEEDPREMOD}"] = post->getIsNeedPremod() ? "true" : "false";
	params["{isMODDER}"] = user->getUserType(post->getCatId()) >= CUser::UT_MODER ? "true" : "false";
	params["{isHIDDEN}"] = isPostHidden ? "true" : "false";
	params["{isEDITED}"] = post->getModDate() > 0 ? "true" : "false";
	params["{isOWNER}"]  = post->getUserId() == user->getUserId() ? "true" : "false";
	params["{isMANYATTACH}"] = (int)post->getAttachments()->size() > 2 ? "true" : "false";
	if(!isPostHidden)
	{
		if(post->getAgeRating() > user->getUserAgeRating())
		{
			params["{ATTACH}"] 	 = " ";
			params["{TEXT}"] 	 = "Возрастной рейтинг данного сообщения не позволяет нам показать его Вам ;)";
		}
		else
		{
			params["{ATTACH}"] 	 = buildAttachments(post->getAttachments());
			params["{TEXT}"] 	 = CMessageParser::getInstance()->removeTags(post->getText());
		}
	}

	return postTemplate->getHTMLData(&params);;
}

std::string CSitePage::buildThread(const int threadId, const CUser* user, bool isPopup)
{
	std::string resultHTML;

	CThread* thread = new CThread(threadId);
	resultHTML = buildThread(thread, user, isPopup);
	delete thread;

	return resultHTML;
}

std::string CSitePage::buildThread(const CThread* thread, const CUser* user, bool isPopup)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	//std::ostringstream tmpStringStream;
	std::string tmpStringStream = "";

	const CHTMLTemplate* threadTemplate = templateManager->findTemplate("thread");
	if(threadTemplate == NULL) return "Missing thread template";

	if(!thread->getIsValid()) return "Cant create thread";
	bool isThreadHidden = user->getIsThreadHidden(thread->getId());

	if(user->getUserType(thread->getCatId()) <= CUser::UT_USER && thread->getIsNeedPremod() && thread->getUserId() != user->getUserId()) return "";

	params["{THREADID}"] 	 = valueToString(thread->getId());
	params["{THREADNAME}"] 	 = htmlspecialchars(thread->getCaption());
	params["{USERNAME}"] 	 = htmlspecialchars(thread->getUserName());
	params["{USERID}"] 		 = valueToString(thread->getUserId());
	params["{DATE}"] 	 	 = unixTimeToDate(thread->getCreationDate());
	params["{EDITTIME}"]	 = unixTimeToDate(thread->getModDate());
	params["{ID}"] 	 	 	 = valueToString(thread->getId());
	params["{COMPLAINTS}"] 	 = valueToString(thread->getComplainCnt());
	params["{THREADPOS}"] 	 = valueToString(thread->getPosition());
	params["{CONTROLS}"] 	 = isPopup ? " " : buildThreadControls(thread, user);
	params["{POSTSINFO}"] 	 = isPopup ? " " : valueToString(thread->getPostsCnt());
	params["{SURVEY}"] 		 = " ";
	switch(thread->getAgeRating())
	{
		case 0:
			params["{AGE}"] = " Для всех ";
		break;
		case 1:
			params["{AGE}"] = " После 13 лет ";
		break;
		case 2:
			params["{AGE}"] = " После 17 лет ";
		break;
		case 3:
			params["{AGE}"] = " После 18 лет ";
		break;
		default:
			params["{AGE}"] = valueToString(thread->getAgeRating());
		break;
	}

	params["{isNEEDPREMOD}"] = thread->getIsNeedPremod() ? "true" : "false";
	params["{isMODDER}"]	 = user->getUserType(thread->getCatId()) >= CUser::UT_MODER ? "true" : "false";
	params["{isHIDDEN}"]	 = isThreadHidden ? "true" : "false";
	params["{isEDITED}"] 	 = thread->getModDate() > 0 ? "true" : "false";
	params["{isPOPUP}"] 	 = isPopup ? "true" : "false";
	params["{isOWNER}"] 	 = thread->getUserId() == user->getUserId() ? "true" : "false";
	params["{isMANYATTACH}"] = (int)thread->getAttachments()->size() > 2 ? "true" : "false";
	if(!isThreadHidden)
	{
		if(thread->getAgeRating() > user->getUserAgeRating())
		{
			params["{ATTACH}"] 	 = " ";
			params["{TEXT}"] 	 = "Возрастной рейтинг данной темы не позволяет нам показать её Вам ;)";
			params["{POSTS}"] 	 = " ";
		}
		else
		{
			params["{ATTACH}"] 	 = buildAttachments(thread->getAttachments());
			params["{TEXT}"] 	 = CMessageParser::getInstance()->removeTags(thread->getText());
			params["{POSTS}"] 	 = " ";

			if(!isPopup)
			{
				const CSurvey* survey = thread->getSuvrey();
				if(survey && survey->getIsValid() && survey->getQuestionsCnt() > 0)
				{
					if(user->getIsUserSurveyOwner(survey->getId()) || user->getIsUserSurveyMember(survey->getId()) || survey->getIsEnded() ) params["{SURVEY}"] = buildSurveyResult(survey, user);
					else params["{SURVEY}"] = buildSurvey(survey,user);
				}
			}

			if(!isPopup)
			{
				const std::vector<CPost*>* currThreardPosts = thread->getPosts();
				for(unsigned int i = 0; i < currThreardPosts->size(); i++)
				{
					const CPost* currPost = (*currThreardPosts)[i];
					if(!currPost->getIsValid()) continue;

					tmpStringStream += buildPost(currPost, user);
				}
				params["{POSTS}"] = tmpStringStream;
			}

		}
	}

	return threadTemplate->getHTMLData(&params);
}

std::string CSitePage::buildNews(const int threadId, const CUser* user)
{
	std::string resultHTML = "";
	CThread* thread = new CThread(threadId);
	resultHTML = buildNews(thread, user);
	delete thread;

	return resultHTML;
}

std::string CSitePage::buildNews(const CThread* thread, const CUser* user)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStringStream = "";

	const CHTMLTemplate* newsTemplate = templateManager->findTemplate("news");
	if(newsTemplate == NULL) return "Missing news template";

	if(!thread->getIsValid()) return "Cant create news";
	if(user->getUserType(thread->getCatId()) <= CUser::UT_USER && thread->getIsNeedPremod() && thread->getUserId() != user->getUserId()) return "";

	params["{ID}"] 	 	 = valueToString(thread->getId());
	params["{NEWSNAME}"] = htmlspecialchars(thread->getCaption());
	params["{DATE}"] 	 = unixTimeToDate(thread->getCreationDate());
	params["{ATTACH}"] 	 = buildAttachments(thread->getAttachments());
	params["{TEXT}"] 	 = CMessageParser::getInstance()->removeTags(thread->getText());
	params["{SURVEY}"]	 = "";
	params["{isMANYATTACH}"] = (int)thread->getAttachments()->size() > 2 ? "true" : "false";

	const CSurvey* survey = thread->getSuvrey();
    if(survey && survey->getIsValid() && survey->getQuestionsCnt() > 0)
    {
        if(user->getIsUserSurveyOwner(survey->getId()) || user->getIsUserSurveyMember(survey->getId()) || survey->getIsEnded() ) params["{SURVEY}"] = buildSurveyResult(survey, user);
        else params["{SURVEY}"] = buildSurvey(survey,user);
    }

    return newsTemplate->getHTMLData(&params);
}

std::string CSitePage::buildNewThreadForm(const std::string returnPage, const int catId, const CUser* user)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr = "";

	const CHTMLTemplate* threadFormTemplate = templateManager->findTemplate("newThreadForm");
	if(threadFormTemplate == NULL) return "Missing thread form template";

	if(!user->getIsUserGuest())
	{
		const std::vector<CSurvey*>* surveys = user->getSurveys();
		for(auto it = surveys->begin(); it != surveys->end(); ++it)
		{
			const CSurvey* currSurvey = (*it);
			if(!currSurvey->getIsValid()) continue;

			tmpStr += "<option value='"+valueToString(currSurvey->getId())+"'>" + htmlspecialchars(currSurvey->getTitle()) + "</option> \n";
		}
	}

	params["{SURVEYSLIST}"] = tmpStr;
	params["{RETURNPAGE}"] 	= returnPage;
	params["{USERID}"] 		= valueToString(user->getUserId());
	params["{CATID}"] 	 	= valueToString(catId);

	return threadFormTemplate->getHTMLData(&params);
}

std::string CSitePage::buildNewPostForm(const std::string returnPage, const CUser* user)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;

	const CHTMLTemplate* postFormTemplate = templateManager->findTemplate("newPostForm");
	if(postFormTemplate == NULL) return "Missing thread form template";

	params["{USERID}"] 		= valueToString(user->getUserId());
	params["{RETURNPAGE}"] 	= returnPage;

	return postFormTemplate->getHTMLData(&params);
}

std::string CSitePage::buildThreadControls(const CThread* thread, const CUser* user)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr;

	if(thread->getAgeRating() > user->getUserAgeRating()) return "";

	const CHTMLTemplate* controlsTemplate = NULL;
	controlsTemplate = templateManager->findTemplate("controls");
	if(controlsTemplate == NULL) return "Missing controls template";

	bool isModder = false;
	bool isOwner  = false;
	if(user->getUserType(thread->getCatId()) > CUser::UT_USER) isModder = true;
	if(user->getUserId() == thread->getUserId()) isOwner = true;
	tmpStr = "t" + valueToString(thread->getId());

	params["{THREADID}"] 	= valueToString(thread->getId());
	params["{CURRBLOCKID}"]	= tmpStr;
	params["{isTHREAD}"]	= "true";
	params["{isMODER}"]		= isModder ? "true" : "false";
	params["{isOWNER}"]		= (isModder || isOwner) ? "true" : "false";
	params["{isHIDDEN}"]	= user->getIsThreadHidden(thread->getId()) ? "true" : "false";
	params["{isFAVORITE}"]	= user->getIsThreadFavorite(thread->getId()) ? "true" : "false";
	params["{isFIXED}"]		= user->getIsThreadFixed(thread->getId()) ? "true" : "false";
	params["{isALLFIXED}"]	= thread->getIsFixed()	? "true" : "false";

	return controlsTemplate->getHTMLData(&params);
}

std::string CSitePage::buildPostControls(const CPost* post, const CUser* user)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr;

	if(post->getAgeRating() > user->getUserAgeRating()) return "";

	const CHTMLTemplate* controlsTemplate = NULL;
	controlsTemplate = templateManager->findTemplate("controls");
	if(controlsTemplate == NULL) return "Missing controls template";

	bool isModder = false;
	bool isOwner  = false;
	if(user->getUserType(post->getCatId()) > CUser::UT_USER) isModder = true;
	if(user->getUserId() == post->getUserId()) isOwner = true;
	tmpStr = "p" + valueToString(post->getId());

	params["{THREADID}"] 	= valueToString(post->getThreadId());
	params["{CURRBLOCKID}"]	= tmpStr;
	params["{isTHREAD}"]	= "false";
	params["{isMODER}"]		= isModder ? "true" : "false";
	params["{isOWNER}"]		= (isModder || isOwner) ? "true" : "false";
	params["{isHIDDEN}"]	= user->getIsPostHidden(post->getId()) 	? 	"true" : "false";
	params["{isFAVORITE}"]	= user->getIsPostFavorite(post->getId())? 	"true" : "false";
	params["{isFIXED}"]		= "false";

	return controlsTemplate->getHTMLData(&params);
}

std::string CSitePage::buildAttachments(const std::vector<CAttachment*>* attachments)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr = "";

	const CHTMLTemplate* attachTemplate = NULL;
	attachTemplate = templateManager->findTemplate("attach");

	if(attachTemplate == NULL) return "Missing attach template";

	for(unsigned int i = 0; i < attachments->size(); i++ )
	{
		CAttachment* currAttach = (*attachments)[i];
		bool isImagePrev = true;
		bool isVidPrev = false;
		bool isAudPrev = false;
		std::string MIME = currAttach->getFileMIMEType();

		if(currAttach->getFileType() == "video")
		{
			if(MIME == "video/webm" || MIME == "video/mp4" || MIME == "video/mpeg" )
			{
				params["{SOURCE}"]  = replaceAll(currAttach->getFilePath(), "//", "/");
				params["{VIDTYPE}"] = MIME;
				isImagePrev = false;
				isAudPrev = false;
				isVidPrev = true;
			}
		}
		if(currAttach->getFileType() == "audio")
		{
			//if(MIME == "video/webm" || MIME == "video/mp4" || MIME == "video/mpeg" )
			//{
				params["{SOURCE}"]  = replaceAll(currAttach->getFilePath(), "//", "/");
				params["{AUDTYPE}"] = MIME;
				isImagePrev = false;
				isVidPrev = false;
				isAudPrev = true;
			//}
		}

		params["{FILEPATH}"] 			= replaceAll(currAttach->getFilePath(), "//", "/");
		params["{FILETYPE}"] 			= currAttach->getFileType();
		params["{FILESIZE}"] 			= valueToString(currAttach->getFileSize() / 1024);
		params["{FILENAME}"] 			= currAttach->getFileName();
		params["{isIMAGE}"] 			= isImagePrev ? "true" : "false";
		params["{isVIDEO}"] 			= isVidPrev   ? "true" : "false";
		params["{isAUDIO}"] 			= isAudPrev   ? "true" : "false";
		params["{IMAGEPREVIEWPATH}"] 		= replaceAll(currAttach->getFilePreviewPath(), "//", "/");
		params["{IMAGEPREVIEWDESCRIPTION}"] = "img";
		params["{IMAGEPREVIEWWIDTH}"] 	= valueToString(currAttach->getFilePreviewWidth());
		params["{IMAGEPREVIEWHEIGHT}"] 	= valueToString(currAttach->getFilePreviewHeight());
		params["{IMAGEWIDTH}"] 			= valueToString(currAttach->getFileWidth());
		params["{IMAGEHEIGHT}"] 		= valueToString(currAttach->getFileHeight());

		tmpStr += attachTemplate->getHTMLData(&params);
		params.clear();
	}

	return tmpStr;
}

std::string CSitePage::buildSurvey(const CSurvey* survey, const CUser* user)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr = "";

	const CHTMLTemplate* surveyTemplate = NULL;
	surveyTemplate = templateManager->findTemplate("survey");

	if(surveyTemplate == NULL)  return "Missing survey template";
	if(!survey->getIsValid())   return "Survey is not valid";
	if(!user->getIsValid()) 	return "User not valid";

	params["{ID}"] 			= valueToString(survey->getId());
	params["{USERID}"] 		= valueToString(user->getUserId());
	params["{TITLE}"]	 	= htmlspecialchars(survey->getTitle());
	params["{DESCRIPTION}"] = htmlspecialchars(survey->getDescription());

	const std::vector<CSurveyQuestion*>* questions = survey->getQuestions();
	int questionNumber = 0;
	for(auto it = questions->begin(); it != questions->end(); ++it)
	{
        const CSurveyQuestion* currQuestion = (*it);
        if(!currQuestion->getIsValid()) continue;

		tmpStr += "<div class='survey_question' id='s"+params["{ID}"]+"q"+valueToString(questionNumber)+"'> \n";
		tmpStr += "<div class='survey_question_title'>"+htmlspecialchars(currQuestion->getText())+"</div> \n";
		const std::vector<CSurveyField*>* fields = currQuestion->getFields();
		int fieldNumber = 0;
		for(auto itFields = fields->begin(); itFields != fields->end(); ++itFields)
		{
			const CSurveyField* field = (*itFields);
			if(!field->getIsValid()) continue;
			const std::vector<std::string>* comboValues = field->getComboValues();
			int comboValNumber = 0;

			tmpStr += "<div class='survey_question_field'> \n";

			std::string elementId = "s" + params["{ID}"] + "q" + valueToString(questionNumber) + "f" + valueToString(fieldNumber);
			std::string radioGroup = "s" + params["{ID}"] + "q" + valueToString(questionNumber) + "group";
			switch(field->getType())
			{
				case CSurveyField::FIELD_CHECKBOX:
					tmpStr += "<input type='checkbox' name='"+elementId+"' id='"+elementId+"' value='"+htmlspecialchars(field->getText())+"'> \n";
					tmpStr += "<span>"+htmlspecialchars(field->getText())+"</span><br> \n";
				break;
				case CSurveyField::FIELD_RADIO:
					tmpStr += "<input type='radio' name='"+radioGroup+"' id='"+elementId+"' value='"+htmlspecialchars(field->getText())+"'> \n";
					tmpStr += "<span>"+htmlspecialchars(field->getText())+"</span><br> \n";
				break;
				case CSurveyField::FIELD_TEXT:
					tmpStr += "<input type='text' name='"+elementId+"' id='"+elementId+"' maxlength='20' pattern='[а-яА-Яa-zA-Z0-9_ ]+'> \n";
				break;
				case CSurveyField::FIELD_COMBO:
					tmpStr += "<select name='"+elementId+"' id='"+elementId+"'> \n";
					for(auto itCombo = comboValues->begin(); itCombo != comboValues->end(); ++itCombo)
					{
						const std::string currVal = (*itCombo);
						tmpStr += "<option value='"+ valueToString(comboValNumber) +"'>"+currVal+"</option> \n";
						comboValNumber++;
					}
					tmpStr += "</select> \n";
				break;

				default:
				break;
			}

			fieldNumber++;
			tmpStr += "</div> \n";
		}
		tmpStr += "</div> \n";

		questionNumber++;
	}

	params["{QUESTIONS}"] = tmpStr;

	return surveyTemplate->getHTMLData(&params);
}

std::string CSitePage::buildSurveyResult(const CSurvey* survey, const CUser* user)
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	std::map<std::string, std::string> params;
	std::string tmpStr = "";

	const CHTMLTemplate* surveyTemplate = NULL;
	surveyTemplate = templateManager->findTemplate("surveyResult");

	if(surveyTemplate == NULL)  return "Missing surveyResult template";
	if(!survey->getIsValid())   return "Survey is not valid";
	if(!user->getIsValid()) 	return "User not valid";

	survey->calcResults();
	//if(!user->getIsUserSurveyOwner(survey->getId()) && !user->getIsUserSurveyMember(survey->getId()) ) return "User not valid";

	params["{ID}"] 			= valueToString(survey->getId());
	params["{USERID}"] 		= valueToString(user->getUserId());
	params["{TITLE}"]	 	= htmlspecialchars(survey->getTitle());
	params["{DESCRIPTION}"] = htmlspecialchars(survey->getDescription());
	params["{INFO}"] 		= "Всего ответов: " + valueToString(survey->getAnswersCnt());

	const std::vector<CSurveyQuestion*>* questions = survey->getQuestions();
	int questionNumber = 0;
	for(auto it = questions->begin(); it != questions->end(); ++it)
	{
        const CSurveyQuestion* currQuestion = (*it);
        if(!currQuestion->getIsValid()) continue;

		tmpStr += "<div class='survey_question' id='s"+params["{ID}"]+"q"+valueToString(questionNumber)+"r'> \n";
		tmpStr += "<div class='survey_question_title'>"+htmlspecialchars(currQuestion->getText())+"</div> \n";
		const std::vector<CSurveyField*>* fields = currQuestion->getFields();
		int fieldNumber = 0;
		for(auto itFields = fields->begin(); itFields != fields->end(); ++itFields)
		{
			const CSurveyField* field = (*itFields);
			if(!field->getIsValid()) continue;

			const std::vector<std::string>* comboValues = field->getComboValues();
			std::vector<std::string> mostCommonTextAnswers = field->getResults()->getMostCommonTextAnswers();
			int answersCnt = survey->getAnswersCnt();
			if(answersCnt == 0) answersCnt = 1;
			int choicePersent = (int)((float)field->getResults()->answersCnt / answersCnt * 100);

			tmpStr += "<div class='survey_question_field'> \n";

			std::string elementId = "s" + params["{ID}"] + "q" + valueToString(questionNumber) + "f" + valueToString(fieldNumber) + "r";
			switch(field->getType())
			{
				case CSurveyField::FIELD_CHECKBOX:
				case CSurveyField::FIELD_RADIO:
					tmpStr += "<span>"+htmlspecialchars(field->getText())+"  : "+valueToString(choicePersent)+"% </span>\n";
					tmpStr += "<progress value='"+valueToString(choicePersent)+"' max='100' id='"+elementId+"'></progress>\n";
				break;

				case CSurveyField::FIELD_TEXT:
					for(auto itTextVars = mostCommonTextAnswers.begin(); itTextVars != mostCommonTextAnswers.end(); ++it)
					{
						tmpStr += "<span>"+htmlspecialchars((*itTextVars))+ " </span><br>\n";
					}
				break;
				case CSurveyField::FIELD_COMBO:
					for(auto itCombo = comboValues->begin(); itCombo != comboValues->end(); ++itCombo)
					{
						const std::string currVal = (*itCombo);
						const int comboValChoises = field->getResultsForModify()->comboAnswersCnt[itCombo - comboValues->begin()];
						const int comboValChoicePersent = (int)( (float)comboValChoises / answersCnt * 100 ) ;
						tmpStr += "<span>"+htmlspecialchars(currVal)+"  : "+valueToString(comboValChoicePersent)+"% </span>\n";
						tmpStr += "<progress value='"+valueToString(comboValChoicePersent)+"' max='100' id='"+elementId+"'></progress><br>\n";
					}
				break;

				default:
				break;
			}

			fieldNumber++;
			tmpStr += "</div> \n";
		}
		tmpStr += "</div> \n";

		questionNumber++;
	}
	params["{QUESTIONS}"] = tmpStr;

	return surveyTemplate->getHTMLData(&params);
}
