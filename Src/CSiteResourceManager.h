#ifndef CSITERESOURCEMANAGER_H
#define CSITERESOURCEMANAGER_H

#include "novemberlib/managers/CResourceManager.h"

class CSiteResourceManager : public CResourceManager
{
	public:
		static void createJPEGPreview(const std::string fullFileName);
		static bool getImageDimensions(const std::string& fullFileName, int &width, int &height);
		static void calcPreviewDimension(const int imgWidth, const int imgHeight, int &prevWidth, int &prevHeight);

		CSiteResourceManager();
		virtual ~CSiteResourceManager();
	protected:
		static bool getPNGOrGIFImageDimensions(const std::string& fullFileName, int &width, int &height);
		static bool getJPEGImageDimensions(const std::string& fullFileName, int &width, int &height);
	private:
};

#endif // CSITERESOURCEMANAGER_H
