#include "CAttachment.h"

#include "novemberlib/utils/CLog.h"
#include "novemberlib/utils/utils.h"
#include "novemberlib/utils/MIMEType.h"

#include "CSiteResourceManager.h"

CAttachment::CAttachment(const std::string filePath, const long fileSize, const int width, const int height)
{
	this->filePath = filePath;
	this->width 	= width;
	this->height 	= height;
	this->fileSize  = fileSize;

	previewWidth 	= 128;
	previewHeight 	= 128;
	if(this->width  == 0) this->width  = 128;
	if(this->height == 0) this->height = 128;


	fileType = "none";
	filePreviewPath = "sitecontent//img//previewicons//file.png";

	size_t fileNameStartPos = filePath.find_last_of("//");
	if(fileNameStartPos != std::string::npos)
	{
		fileName = filePath.substr(fileNameStartPos + 1,filePath.length() - 1);
	}

	size_t dotPos = filePath.find_last_of(".");
	if(dotPos == std::string::npos)
	{
		fileExtention = "none";
	}
	else
	{
		fileExtention = filePath.substr(dotPos + 1,filePath.length() - 1);
	}
	fileMIME = MIMEType::getInstance()->get(fileExtention);

	initPreviewImage(filePath);
}

CAttachment::~CAttachment()
{
	//dtor
}

std::vector<CAttachment*>* CAttachment::processAttachmentString(const std::string attachmentString)
{
	std::vector<CAttachment*>* attachments = new std::vector<CAttachment*>();
	std::vector<std::string> strs;

	//boost::split(strs, attachmentString, boost::is_any_of(";"));
	split(strs, attachmentString, ";");
	for(unsigned int i = 0; i < strs.size(); i += 4)
	{
		if(strs[i].length() > 0 && strs[i+1].length() > 0 && strs[i+2].length() > 0 && strs[i+3].length() > 0)
		{
			std::string filePath = strs[i];
			long fileSize = 0;
			int width = 0;
			int height = 0;
			try
			{
				fileSize = std::stoi(strs[i+1]);
				width  = std::stoi(strs[i+2]);
				height = std::stoi(strs[i+3]);
			}
			catch(...)
			{
				continue;
			}

			attachments->push_back(new CAttachment(strs[i],fileSize, width, height));
		}
	}

	return attachments;
}

std::string CAttachment::createAttachmentString(const std::vector<CAttachment*>* attVector)
{
	std::string attachmentString = "";
	const unsigned int vectorSize = attVector->size();
	for(unsigned int i = 0; i < vectorSize; i++)
	{
		CAttachment* currAttach = (*attVector)[i];
		attachmentString += currAttach->getFilePath() + ";" + valueToString(currAttach->getFileSize()) + ";" + valueToString(currAttach->getFileWidth()) + ";" + valueToString(currAttach->getFileHeight());
		if(i < vectorSize - 1) attachmentString += ";";
	}
	return attachmentString;
}

std::string CAttachment::getFilePath() const
{
	return filePath;
}

std::string CAttachment::getFilePreviewPath() const
{
	return filePreviewPath;
}

std::string CAttachment::getFileType() const
{
	return fileType;
}

std::string CAttachment::getFileName() const
{
	return fileName;
}

std::string CAttachment::getFileExtention() const
{
	return fileExtention;
}

std::string CAttachment::getFileMIMEType() const
{
	return fileMIME;
}

int CAttachment::getFilePreviewWidth() const
{
	return previewWidth;
}

int CAttachment::getFilePreviewHeight() const
{
	return previewHeight;
}

int CAttachment::getFileWidth() const
{
	return width;
}

int CAttachment::getFileHeight() const
{
	return height;
}

long CAttachment::getFileSize() const
{
	return fileSize;
}

void CAttachment::initPreviewImage(const std::string filePath)
{
	std::string fileExtention =  "none";
	std::string fileName =  "";

	size_t dotPos = filePath.find_last_of(".");
	if(dotPos != std::string::npos)
	{
		fileExtention = filePath.substr(dotPos + 1,filePath.length() - 1);
	}
	size_t fileNameStartPos = filePath.find_last_of("//");
	if(fileNameStartPos != std::string::npos)
	{
		fileName = filePath.substr(fileNameStartPos + 1,filePath.length() - 1);
	}

	if(fileExtention == "none")
	{
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//file.png";

		return;
	}

    if(fileExtention == "jpg" || fileExtention == "jpeg")
    {
		CSiteResourceManager::calcPreviewDimension(width, height, previewWidth, previewHeight);
		filePreviewPath = "usercontent//img//preview//" + fileName;
		fileType = "image";
		return;
    }
	if(fileExtention == "png")
    {
		CSiteResourceManager::calcPreviewDimension(width, height, previewWidth, previewHeight);
		filePreviewPath = filePath;
		fileType = "image";
		return;
    }
	if(fileExtention == "bmp")
    {
		CSiteResourceManager::calcPreviewDimension(width, height, previewWidth, previewHeight);
		filePreviewPath = filePath;
		fileType = "image";
		return;
    }
	if(fileExtention == "gif")
    {
		CSiteResourceManager::calcPreviewDimension(width, height, previewWidth, previewHeight);
		filePreviewPath = filePath;
		fileType = "image";
		return;
    }
	if(fileExtention == "svg")
    {
		CSiteResourceManager::calcPreviewDimension(width, height, previewWidth, previewHeight);
		filePreviewPath = filePath;
		fileType = "image";
		return;
    }

	if(fileExtention == "avi")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//avi.png";

		fileType = "video";
		return;
    }
	if(fileExtention == "flv")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//flv.png";

		fileType = "video";
		return;
    }
	if(fileExtention == "mp4")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//mp4.png";

		fileType = "video";
		return;
    }
	if(fileExtention == "mpeg")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//mpg.png";

		fileType = "video";
		return;
    }
    if(fileExtention == "mpg")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//mpg.png";

		fileType = "video";
		return;
    }
    if(fileExtention == "webm")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//mpg.png";

		fileType = "video";
		return;
    }

	if(fileExtention == "mp3")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//mp3.png";

		fileType = "audio";
		return;
    }
    if(fileExtention == "ogg")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//ogg.png";

		fileType = "audio";
		return;
    }
	if(fileExtention == "wav")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//wav.png";

		fileType = "audio";
		return;
    }
    if(fileExtention == "wma")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//wma.png";

		fileType = "audio";
		return;
    }

	if(fileExtention == "doc")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//doc.png";

		fileType = "text";
		return;
    }
	if(fileExtention == "xls")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//xls.png";

		fileType = "text";
		return;
    }
	if(fileExtention == "txt")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//txt.png";

		fileType = "text";
		return;
    }
    if(fileExtention == "pdf")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//pdf.png";

		fileType = "text";
		return;
    }

	if(fileExtention == "rar")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//rar.png";

		fileType = "archive";
		return;
    }
	if(fileExtention == "zip")
    {
		previewWidth 	= 128;
		previewHeight 	= 128;
		filePreviewPath = "sitecontent//img//previewicons//zip.png";

		fileType = "archive";
		return;
    }

    previewWidth 	= 128;
	previewHeight 	= 128;
	filePreviewPath = "sitecontent//img//previewicons//file.png";
}
