#include "UploadManager.h"
#include "Func/Common.h"
#include "Func/HistoryManager.h"
#include "Func/Base.h"
#include "Func/LocalFileCache.h"
#include "Core/Upload/FileUploadTask.h"

UploadManager::UploadManager(UploadEngineManager* uploadEngineManager) : CFileQueueUploader(uploadEngineManager)
{
	addUploadFilter(&imageConverterFilter);
	addUploadFilter(&urlShorteningFilter);
	OnConfigureNetworkClient.bind(this, &UploadManager::configureNetwork);
}

bool UploadManager::shortenLinksInSession(std::shared_ptr<UploadSession> session)
{
	int taskCount = session->taskCount();

	bool res = false;
	for (int i = 0; i < taskCount; i++)
	{
		std::shared_ptr<UploadTask> task = session->getTask(i);
		if (task->shorteningStarted())
		{
			continue;
		}
		task->serverProfile().setShortenLinks(true);
		res = urlShorteningFilter.PostUpload(task.get()) || res;
	}
	return res;
}

void UploadManager::configureNetwork(CFileQueueUploader* uploader, NetworkClient* networkClient)
{
	IU_ConfigureProxy(*networkClient);
}

void UploadManager::sessionAdded(UploadSession* session)
{
	session->addSessionFinishedCallback(UploadSession::SessionFinishedCallback(this, &UploadManager::onSessionFinished));
}

void UploadManager::onSessionFinished(UploadSession* uploadSession)
{
	
}

void UploadManager::onTaskFinished(UploadTask* task, bool ok)
{
	UploadSession* uploadSession = task->session();
	CHistoryManager * mgr = ZBase::get()->historyManager();
	std::lock_guard<std::mutex> lock(uploadSession->historySessionMutex_);
	std_tr::shared_ptr<CHistorySession> session = uploadSession->historySession_;
	if (!session) {
		session = mgr->newSession();
		uploadSession->historySession_ = session;
	}

	FileUploadTask* fileTask = dynamic_cast<FileUploadTask*>(task);
	if (!fileTask)
	{
		return ;
	}

	TempFileDeleter* tempFileDeleter = fileTask->tempFileDeleter(false);
	if (tempFileDeleter)
	{
		tempFileDeleter->cleanup();
	}
	if (!fileTask->uploadSuccess())
	{
		return ;
	}
	HistoryItem hi;
	hi.localFilePath = fileTask->originalFileName();
	hi.serverName = fileTask->serverProfile().serverName();
	UploadResult* uploadResult = fileTask->uploadResult();
	hi.directUrl = uploadResult->directUrl;
	hi.directUrlShortened = uploadResult->directUrlShortened;
	hi.thumbUrl = uploadResult->thumbUrl;
	hi.viewUrl = uploadResult->downloadUrl;
	hi.viewUrlShortened = uploadResult->downloadUrlShortened;
	hi.uploadFileSize = fileTask->getDataLength();
	if (!hi.directUrl.empty())
	{
		LocalFileCache::instance().addFile(hi.directUrl, hi.localFilePath);
	}
	session->AddItem(hi);
}

void UploadManager::taskAdded(UploadTask* task)
{
	FileUploadTask* fileTask = dynamic_cast<FileUploadTask*>(task);
	if (!fileTask)
	{
		return;
	}

	if (task->parentTask())
	{
		return ;
	}
	task->addTaskFinishedCallback(UploadTask::TaskFinishedCallback(this, &UploadManager::onTaskFinished));
}