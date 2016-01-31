#ifndef CATTACHMENT_H
#define CATTACHMENT_H

#include <string>
#include <vector>

class CAttachment
{
	public:
		CAttachment(const std::string filePath, const long fileSize, const int width, const int height);

		std::string getFilePath() const;
		std::string getFilePreviewPath() const;
		std::string getFileType() const;
		std::string getFileName() const;
		std::string getFileExtention() const;
		std::string getFileMIMEType() const;
		int getFilePreviewWidth() const;
		int getFilePreviewHeight() const;
		int getFileWidth() const;
		int getFileHeight() const;
		long getFileSize() const;

		static std::vector<CAttachment*>* processAttachmentString(const std::string attachmentString);
		static std::string createAttachmentString(const std::vector<CAttachment*>* attVector);

		~CAttachment();
	protected:
	private:
		void initPreviewImage(const std::string filePath);

		std::string filePath;
		std::string fileName;
		std::string fileType;
		std::string fileExtention;
		std::string fileMIME;

		std::string filePreviewPath;
		int width;
		int height;
		int previewWidth;
		int previewHeight;
		long fileSize;
};

#endif // CATTACHMENT_H
