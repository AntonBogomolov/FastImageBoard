#ifndef CSITEPAGE_H
#define CSITEPAGE_H

#include "novemberlib/pages/CPage.h"

class CFCGIRequest;

class CPost;
class CThread;
class CUser;
class CAttachment;
class CSurvey;
class CSurveyAnswer;

class CSitePage : public CPage
{
	public:
		static std::string buildLeftPanel(const CUser* user);
		static std::string buildUpperMenu(const CUser* user);
		static std::string buildPost(const int postId, const CUser* user, bool isPopup = false);
		static std::string buildPost(const CPost* post, const CUser* user, bool isPopup = false);
		static std::string buildThread(const int threadId, const CUser* user, bool isPopup = false);
		static std::string buildThread(const CThread* thread, const CUser* user, bool isPopup = false);
		static std::string buildNews(const int threadId, const CUser* user);
		static std::string buildNews(const CThread* thread, const CUser* user);
		static std::string buildNewThreadForm(const std::string returnPage, const int catId, const CUser* user);
		static std::string buildNewPostForm(const std::string returnPage, const CUser* user);
		static std::string buildThreadControls(const CThread* thread, const CUser* user);
		static std::string buildPostControls(const CPost* post, const CUser* user);
		static std::string buildAttachments(const std::vector<CAttachment*>* attachments);
		static std::string buildSurvey(const CSurvey* survey, const CUser* user);
		static std::string buildSurveyResult(const CSurvey* survey, const CUser* user);

		CSitePage(const std::string name, const CFCGIRequest* currRequest);
		virtual ~CSitePage();
	protected:
		virtual std::string buildContent() const = 0;
	private:
};

#endif // CSITEPAGE_H
