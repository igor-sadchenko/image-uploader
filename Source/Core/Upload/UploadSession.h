﻿#ifndef IU_CORE_UPLOAD_UPLOADSESSION_H
#define IU_CORE_UPLOAD_UPLOADSESSION_H

#pragma once
#include "UploadTask.h"
#include <atomic>

class CHistorySession;
class UploadSession
{
    public:
        UploadSession();
        typedef fastdelegate::FastDelegate2<UploadSession*, UploadTask*> TaskAddedCallback;
        typedef fastdelegate::FastDelegate1<UploadSession*> SessionFinishedCallback;
        /**
        This function is NOT thread safe!
        */
        void addTask(std::shared_ptr<UploadTask> task);
        void removeTask(std::shared_ptr<UploadTask> task);
        int getNextTask(UploadTaskAcceptor *acceptor, std::shared_ptr<UploadTask>& outTask);
        bool isRunning();
        bool isFinished();
        int pendingTasksCount(UploadTaskAcceptor* acceptor);
        int taskCount();
        int finishedTaskCount(UploadTask::Status status);
        bool isStopped();
        std::shared_ptr<UploadTask> getTask(int index);
        void addSessionFinishedCallback(const SessionFinishedCallback& callback);
        void addTaskAddedCallback(const TaskAddedCallback& callback);
        void stop();
        bool isFatalErrorSet(const std::string& serverName, const std::string& profileName);
        void setFatalErrorForServer(const std::string& serverName, const std::string& profileName);
        void clearErrorsForServer(const std::string& serverName, const std::string& profileName);
        friend class UploadTask;
        friend class UploadManager;
    protected:
        std::vector<std::shared_ptr<UploadTask>> tasks_;
        bool finishedSignalSent_;
        bool isStopped_;
        std::atomic<int> finishedCount_;
        void taskFinished(UploadTask* task);
        void childTaskAdded(UploadTask* task);
        bool stopSignal();
        //std::recursive_mutex tasksMutex_;
        std::vector<TaskAddedCallback> taskAddedCallbacks_;
        std::vector<SessionFinishedCallback> sessionFinishedCallbacks_;
        void notifyTaskAdded(UploadTask* task);
        std::shared_ptr<CHistorySession> historySession_;
        std::map<std::pair<std::string, std::string>, bool> serverFatalErrors_;
        std::mutex serverFatalErrorsMutex_;
        std::mutex historySessionMutex_;
        std::mutex finishMutex_;
        std::atomic<bool> stopSignal_;
private:
    DISALLOW_COPY_AND_ASSIGN(UploadSession);
};

#endif
