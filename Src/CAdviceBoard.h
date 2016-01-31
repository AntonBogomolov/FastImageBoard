#ifndef CADVICEBOARD_H
#define CADVICEBOARD_H

#include "novemberlib/FCGI/CFCGIApp.h"


class CAdviceBoard : public CFCGIApp
{
	public:
		CAdviceBoard();
		virtual ~CAdviceBoard();
	protected:
		virtual void init();
	private:
		void initDB();
};

#endif // CADVICEBOARD_H
