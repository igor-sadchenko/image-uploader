﻿#ifndef IU_CORE_UPLOAD_SERVERPROFILE_H
#define IU_CORE_UPLOAD_SERVERPROFILE_H

#pragma once
#include "Core/Images/ImageConverter.h"

class CUploadEngineData;
class ServerSettingsStruct;
class SettingsNode;

struct ImageUploadParams {
    ImageUploadParams() {
        UseServerThumbs = false;
        CreateThumbs = false;
        ProcessImages = false;
        UseDefaultThumbSettings = true;
        ImageProfileName = "Default";
        Thumb.Size = 180;
        Thumb.ResizeMode = ThumbCreatingParams::trByWidth;
        Thumb.AddImageSize = true;
        Thumb.Format = ThumbCreatingParams::tfPNG;
        Thumb.TemplateName = "default";
        Thumb.BackgroundColor = 0xffffff;
        Thumb.Quality = 85;
        Thumb.Text = "%width%x%height% (%size%)";
    }
#ifndef IU_SERVERLISTTOOL

    void bind(SettingsNode& n);
#endif

    bool UseServerThumbs;
    bool CreateThumbs;
    bool ProcessImages;
    bool ThumbAddImageSize;

    std::string ImageProfileName;

    bool UseDefaultThumbSettings;
    ThumbCreatingParams getThumb();
    ThumbCreatingParams& getThumbRef();
    void setThumb(ThumbCreatingParams tcp);
protected:
    ThumbCreatingParams Thumb;
};
class ServerProfile {

public:

    ServerProfile();

    ServerProfile(const std::string& serverName);
    ServerSettingsStruct& serverSettings();
    CUploadEngineData* uploadEngineData() const;

    void setProfileName(const std::string& newProfileName);
    std::string profileName() const;

    void setServerName(const std::string& newProfileName);
    std::string serverName() const;

    std::string folderTitle() const;
    void setFolderTitle(const std::string& newTitle);
    std::string folderId() const;
    void setFolderId(const std::string& newId);
    std::string folderUrl() const;
    void setFolderUrl(const std::string& newUrl);

    bool shortenLinks() const;
    void setShortenLinks(bool shorten);
    bool isNull();
    bool UseDefaultSettings;
    void clearFolderInfo();
    ServerProfile deepCopy();
#ifndef IU_SERVERLISTTOOL
    void bind(SettingsNode& n);
#endif
#ifdef IU_WTL
    ImageUploadParams getImageUploadParams();
    ImageUploadParams& getImageUploadParamsRef();

    void setImageUploadParams(ImageUploadParams iup);
    friend struct ImageUploadParams;
#endif
protected:
    std::string serverName_;
    std::string profileName_;
    ImageUploadParams imageUploadParams;
    std::string folderTitle_;
    std::string folderId_;
    std::string folderUrl_;
    bool shortenLinks_;

    friend class CommonGuiSettings;
};

#endif
