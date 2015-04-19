#include "UrlShorteningTask.h"

#include "Core/Utils/CoreUtils.h"
#include <assert.h>

UrlShorteningTask::UrlShorteningTask(const std::string& url, UploadTask* parentTask) : UploadTask(parentTask) {
	url_ = url;
	parentUrlType_ = None;
}


std::string UrlShorteningTask::getType() const {
	return "url";
}

std::string UrlShorteningTask::getMimeType() const {
	return "text/plain";
}

int64_t UrlShorteningTask::getDataLength() const {
	return url_.length();
}

std::string UrlShorteningTask::getUrl() const {
	return url_;
}

void UrlShorteningTask::setFinished(bool finished)
{
	if (finished && role_ == UrlShorteningRole && uploadSuccess())
	{
		assert(parentTask_);
		if (!parentTask_)
		{
			return;
		}
		//TODO: keep server thumbnail url
		if (parentUrlType_ == DirectUrl)
		{
			parentTask_->uploadResult()->directUrlShortened = uploadResult_.directUrl;
		}
		else if (parentUrlType_ == DownloadUrl)
		{
			parentTask_->uploadResult()->downloadUrlShortened = uploadResult_.directUrl;
		}
		
	}
	UploadTask::setFinished(finished);
}

void UrlShorteningTask::setParentUrlType(ParentUrlType type)
{
	parentUrlType_ = type;
}

UrlShorteningTask::ParentUrlType UrlShorteningTask::parentUrlType()
{
	return parentUrlType_;
}