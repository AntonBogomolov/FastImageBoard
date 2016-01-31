#include "CSiteIndexPage.h"

#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/CLog.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/helpers/CTemplateHelper.h"
#include "novemberlib/helpers/CMessageParser.h"
#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CDBManager.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/CHTMLTemplate.h"

#include "CMenu.h"
#include "CThread.h"
#include "CUser.h"

CSiteIndexPage::CSiteIndexPage(const std::string name, const CFCGIRequest* currRequest) : CSitePage(name, currRequest)
{

}

CSiteIndexPage::~CSiteIndexPage()
{
	//dtor
}

std::string CSiteIndexPage::buildContent() const
{
	CTemplateHelper* templateManager = CTemplateHelper::getInstance();
	CDBManager* dbManager = CManagers::getInstance()->getDBManager();
	std::shared_ptr<CDBRequest> dbRequest(dbManager->createDBRequest());
	std::map<std::string, std::string> params;
	std::string tmpStr = "";
	const CUser* user = dynamic_cast<const CUser*>(currRequest->getUser());

	const CHTMLTemplate* contentTemplate = templateManager->findTemplate("content");
	const CHTMLTemplate* indexTemplate	 = templateManager->findTemplate("indexPage");
	if(contentTemplate == NULL) return "Missing content template";

	params["{TITLE}"] = "Advice Project";
	params["{DESCRIPTION}"] = "";

	tmpStr = "<ul> \n";
	tmpStr += "<li>Постинг без регистрации, анонимность</li> \n";
	tmpStr += "<li>Поддержка нескольких тематических разделов</li> \n";
	tmpStr += "<li>Возможность ограничения/разрешения постинга для определённых групп пользователей</li> \n";
	tmpStr += "<li>Создание и проведение опросов</li> \n";
	tmpStr += "<li>Премодерация сообщений</li> \n";
	tmpStr += "<li>Закрытие тем</li> \n";
	tmpStr += "<li>Прикрепление тем</li> \n";
	tmpStr += "<li>Встроенная возможность скрытия тем и сообщений</li> \n";
	tmpStr += "<li>Просмотр сообщений при наведении на ссылку</li> \n";
	tmpStr += "<li>Разворачивание картинок прямо в теме</li> \n";
	tmpStr += "<li>Разворачивание тем</li> \n";
	tmpStr += "<li>Возможность развернуть слишком длинное сообщение без посещения темы</li> \n";
	tmpStr += "<li>Встроенные ролики YouTube</li> \n";
	tmpStr += "<li>Контроль NSFW-контента</li> \n";
	tmpStr += "<li>Добавление тем в «избранное»</li> \n";
	tmpStr += "<li>Возможность приложить больше одного файла к сообщению</li> \n";
	tmpStr += "<li>Возможность прикладывать не только картинки, но и другие типы файлов</li> \n";
	tmpStr += "<li>Удаление сообщений пользователем</li> \n";
	tmpStr += "<li>Жалобы на сообщения</li> \n";
	tmpStr += "</ul> \n";
	params["{FEATURES}"] = tmpStr;

	tmpStr = "<ul> \n";
	tmpStr += "<li>Трипкоды</li> \n";
	tmpStr += "<li>Отображение причины бана для пользователя</li> \n";
	tmpStr += "<li>Перенос тем между разделами</li> \n";
	tmpStr += "<li>Возможность сохранения пользователем всей темы одним файлом</li> \n";
	tmpStr += "<li>Пользовательские настройки интерфейса</li> \n";
	tmpStr += "<li>Версия для мобильных устройств</li> \n";
	tmpStr += "<li>Уведомления о новых ответах в избранные темы</li> \n";
	tmpStr += "<li>Автоподгрузка сообщений</li> \n";
	tmpStr += "<li>Поиск по разделам</li> \n";
	tmpStr += "<li>Список тем (режим каталога)</li> \n";
	tmpStr += "<li>Встроенный проигрыватель для музыкальных файлов</li> \n";
	tmpStr += "<li>Оэкаки</li> \n";
	tmpStr += "<li>API</li> \n";
	tmpStr += "<li>Доступность исходных кодов</li> \n";
	tmpStr += "</ul> \n";
	params["{FUTUREFEATURES}"] = tmpStr;

	tmpStr = "";
	const CMenuCategory* dCat = CMenu::getInstance()->findCategoryByName("d");
	if(dCat)
	{
		const CDBRequestResult* result = dbRequest->selectRequest(CDBValues("id"), "Threads", "`threadCatID` = "+valueToString(dCat->getId()), "LIMIT 1");
		//const CDBRequestResult* result = dbRequest->createRequest("SELECT `id` FROM `Threads` WHERE `threadCatID` = "+valueToString(dCat->getId())+" LIMIT 1;");
		if(dbRequest->getIsLastQuerySuccess() && result != NULL && result->getRowsCnt() > 0)
		{
			for(int i = 0; i < result->getRowsCnt(); i++)
			{
				int threadId = 0;
				try{ threadId = result->getIntValue(i, 0); }
				catch(...){ threadId = 0; }

				const CThread* currThread = new CThread(threadId);
				if(!currThread->getIsValid())
				{
					delete currThread;
					continue;
				}
				tmpStr += buildNews(currThread, user);
				tmpStr += '\n';
				delete currThread;
			}
		}
	}
	params["{NEWS}"] = tmpStr;

	tmpStr = indexTemplate->getHTMLData(&params);
	params.clear();

	params["{UPMENU}"] 		= buildUpperMenu(user);
	params["{CONTENT}"] 	= tmpStr;
	params["{LEFTPANEL}"] 	= buildLeftPanel(user);

	return contentTemplate->getHTMLData(&params);
}
