#ifndef CSITECOMMANDMANAGER_H
#define CSITECOMMANDMANAGER_H

#include "novemberlib/managers/CCommandsManager.h"

class CFCGIRequest;

class CSiteCommandManager : public CCommandsManager
{
	public:
		virtual CCommandResult processCommand(CFCGIRequest* currRequest);

		CCommandResult newThreadCommand(CFCGIRequest* currRequest) const;
		CCommandResult newPostCommand(CFCGIRequest* currRequest) const;
		CCommandResult getThreadsCommand(CFCGIRequest* currRequest) const;
		CCommandResult getThreadPostsCommand(CFCGIRequest* currRequest) const;
		CCommandResult editCommand(CFCGIRequest* currRequest) const;
		CCommandResult delCommand(CFCGIRequest* currRequest) const;
		CCommandResult fixCommand(CFCGIRequest* currRequest) const;
		CCommandResult favoriteCommand(CFCGIRequest* currRequest) const;
		CCommandResult hideCommand(CFCGIRequest* currRequest) const;
		CCommandResult allowPremodCommand(CFCGIRequest* currRequest) const;
		CCommandResult complainCommand(CFCGIRequest* currRequest) const;
		CCommandResult loginCommand(CFCGIRequest* currRequest) const;
		CCommandResult setUserAgeRatingCommand(CFCGIRequest* currRequest) const;
		CCommandResult getMessage(CFCGIRequest* currRequest) const;
		CCommandResult banCommand(CFCGIRequest* currRequest) const;
		CCommandResult answerToSurveyCommand(CFCGIRequest* currRequest) const;
		CCommandResult createSurveyCommand(CFCGIRequest* currRequest) const;
		CCommandResult delSurveyCommand(CFCGIRequest* currRequest) const;
		CCommandResult endSurveyCommand(CFCGIRequest* currRequest) const;
		CCommandResult fixToAllCommand(CFCGIRequest* currRequest) const;
		CCommandResult hideToAllCommand(CFCGIRequest* currRequest) const;

		const std::string handleFiles(CFCGIRequest* currRequest, bool isThreadAttach) const;
		virtual void guestUserCheck(CFCGIRequest* currRequest) const;

		CSiteCommandManager();
		virtual ~CSiteCommandManager();
	protected:
	private:
};

#endif // CSITECOMMANDMANAGER_H
