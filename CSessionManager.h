#ifndef CSESSIONMANAGER_H
#define CSESSIONMANAGER_H

#include "IUncopyable.h"

#include <string>

class CFCGIRequest;
class CDefaultUser;

class CSessionManager : public IUncopyable
{
	public:
		virtual bool checkSession(CFCGIRequest* currRequest);

		virtual void loginUser(CFCGIRequest* currRequest, const std::string& login, const std::string& pass);
		virtual void registerUser(CFCGIRequest* currRequest, const std::string& login, const std::string& pass);

		bool getIsNeedSessionCheck() const;
		void setIsNeedSessionCheck(const bool value);

		CSessionManager();
		virtual ~CSessionManager();
	protected:
		bool isNeedSessionCheck;
	private:
};

#endif // CSESSIONMANAGER_H
