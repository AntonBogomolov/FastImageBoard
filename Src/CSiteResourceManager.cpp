#include "CSiteResourceManager.h"

#include "novemberlib/FCGI/CFCGIRequest.h"
#include "novemberlib/helpers/CSyncHelper.h"
#include "novemberlib/helpers/CPathsHelper.h"
#include "novemberlib/helpers/CConfigHelper.h"
#include "novemberlib/managers/CManagers.h"
#include "novemberlib/utils/MIMEType.h"
#include "novemberlib/utils/CLog.h"
#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/md5.h"

#include "Epeg.h"

#include <string>
#include <fstream>
#include <sstream>

CSiteResourceManager::CSiteResourceManager()
{
	//ctor
}

CSiteResourceManager::~CSiteResourceManager()
{
	//dtor
}


bool CSiteResourceManager::getImageDimensions(const std::string& fullFileName, int &width, int &height)
{
	std::string fileExtention = "";
	size_t dotPos = fullFileName.find_last_of(".");
	if(dotPos != std::string::npos)
	{
		fileExtention = fullFileName.substr(dotPos + 1, std::string::npos);
	}

	if(fileExtention == "gif"  || fileExtention == "png") return getPNGOrGIFImageDimensions(fullFileName,width,height);
	if(fileExtention == "jpeg" || fileExtention == "jpg") return getJPEGImageDimensions(fullFileName,width,height);

	return false;
}

bool CSiteResourceManager::getPNGOrGIFImageDimensions(const std::string& fullFileName, int &width, int &height)
{
	std::ifstream file;
	width  = 0;
	height = 0;

	CSyncHelper::getInstance()->getFileMutex()->lock();

	struct stat fileStat;
	if(stat(fullFileName.c_str(), &fileStat) == -1) return false;
	const long fileSize = fileStat.st_size;

	if(fileSize < 24)
	{
		CSyncHelper::getInstance()->getFileMutex()->unlock();
		return false;
	}

	try
	{
		file.open(fullFileName);
	}
	catch(std::exception& e)
	{
		CLog::getInstance()->addError(e.what());
		file.close();
		CSyncHelper::getInstance()->getFileMutex()->unlock();
		return false;
	}

	char buf[24];
	file.read(buf, 24);

	if(!file)
	{
		CLog::getInstance()->addError("image dim read error");
		file.close();
		CSyncHelper::getInstance()->getFileMutex()->unlock();
		return false;
	}

	CSyncHelper::getInstance()->getFileMutex()->unlock();
    file.close();

    // GIF: first three bytes say "GIF", next three give version number. Then dimensions
    if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F')
    {
        width  = (unsigned char)(buf[6]) + ((unsigned char)(buf[7])<<8);
        height = (unsigned char)(buf[8]) + ((unsigned char)(buf[9])<<8);

        if(width > 1000 || height > 1000 || width < 0 || height < 0)
        {
			width = 0;
			height = 0;
			return false;
        }

        return true;
    }

    // PNG: the first frame is by definition an IHDR frame, which gives dimensions
    if ( buf[1]=='P' && buf[2]=='N' && buf[3]=='G')
    {
        width  = ((unsigned char)(buf[16])<<24) + ((unsigned char)(buf[17])<<16) + ((unsigned char)(buf[18])<<8) + ((unsigned char)(buf[19])<<0);
        height = ((unsigned char)(buf[20])<<24) + ((unsigned char)(buf[21])<<16) + ((unsigned char)(buf[22])<<8) + ((unsigned char)(buf[23])<<0);

		if(width > 10000 || height > 10000 || width < 0 || height < 0)
        {
			width = 0;
			height = 0;
			return false;
        }

        return true;
    }

    return false;
}

bool CSiteResourceManager::getJPEGImageDimensions(const std::string& fullFileName, int &width, int &height)
{
	Epeg_Image * image;
	image = epeg_file_open(fullFileName.c_str());

    if (!image)
    {
        CLog::getInstance()->addError("Cant open " + fullFileName);
        return false;
    }

	epeg_size_get(image, &width, &height);
    epeg_close(image);

	if(width > 10000 || height > 10000 || width < 0 || height < 0)
	{
		width = 0;
		height = 0;
		return false;
	}

    return true;
}

void CSiteResourceManager::createJPEGPreview(const std::string fullFileName)
{
    std::string fileName = "";
    std::string prevPath = "";
    size_t fileNameStartPos = fullFileName.find_last_of("/");
	if(fileNameStartPos != std::string::npos)
	{
		fileName = fullFileName.substr(fileNameStartPos + 1,fullFileName.length() - 1);
	}
	std::string userDir = CPathsHelper::getInstance()->getUserContentDir();
	std::string previewDir = CConfigHelper::getInstance()->getStringParamValue("imgPreviewDir", "img/preview/");
	prevPath = userDir + previewDir + fileName;
	CLog::getInstance()->addInfo(prevPath);
	if(isFileExists(prevPath)) return;

	CSyncHelper::getInstance()->getResourceMutex()->lock();

	Epeg_Image * image;
	image = epeg_file_open(fullFileName.c_str());

    if (!image)
    {
        CLog::getInstance()->addError("Cant open " + fullFileName);
        CSyncHelper::getInstance()->getResourceMutex()->unlock();
        return;
    }

    int imgWidth  = 0;
	int imgHeight = 0;
	int imgPrevWidth 	= 0;
	int imgPrevHeight	= 0;

    epeg_size_get(image, &imgWidth, &imgHeight);
    calcPreviewDimension(imgWidth, imgHeight, imgPrevWidth, imgPrevHeight);

	epeg_decode_size_set           (image, imgPrevWidth, imgPrevHeight);
	epeg_quality_set               (image, 80);
	epeg_thumbnail_comments_enable (image, 0);
	epeg_file_output_set           (image, prevPath.c_str());
	epeg_encode                    (image);
	epeg_close                     (image);

	CSyncHelper::getInstance()->getResourceMutex()->unlock();
}

void CSiteResourceManager::calcPreviewDimension(const int imgWidth, const int imgHeight, int &prevWidth, int &prevHeight)
{
	CConfigHelper* settingsManager = CConfigHelper::getInstance();

	int maxImgDim = imgWidth;
	if(imgHeight > maxImgDim) maxImgDim = imgHeight;

	if(maxImgDim == 0) return;
	if(maxImgDim < settingsManager->getIntParamValue("maxPreviewDimension", 200))
	{
		prevWidth  = imgWidth;
		prevHeight = imgHeight;
		return;
	}

	prevWidth  = (int)((float)imgWidth  / maxImgDim * settingsManager->getIntParamValue("maxPreviewDimension", 200));
	prevHeight = (int)((float)imgHeight / maxImgDim * settingsManager->getIntParamValue("maxPreviewDimension", 200));
}
