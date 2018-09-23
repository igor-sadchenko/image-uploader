/*

Image Uploader -  free application for uploading images/files to the Internet

Copyright 2007-2018 Sergey Svistunov (zenden2k@yandex.ru)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include "CommonGuiSettings.h"

#include "Func/WinUtils.h"

const TCHAR CommonGuiSettings::VideoEngineDirectshow[] = _T("Directshow");
const TCHAR CommonGuiSettings::VideoEngineFFmpeg[] = _T("FFmpeg");
const TCHAR CommonGuiSettings::VideoEngineAuto[] = _T("Auto");

CommonGuiSettings::CommonGuiSettings() : BasicSettings()
{
    // Default values of settings

    ShowTrayIcon = false;
    ShowTrayIcon_changed = false;
    MaxThreads = 3;
    DeveloperMode = false;
    temporaryServer.UseDefaultSettings = false;
}

CommonGuiSettings::~CommonGuiSettings() {
}

bool CommonGuiSettings::IsFFmpegAvailable() {
    CString appFolder = WinUtils::GetAppFolder();
    return WinUtils::FileExists(appFolder + "avcodec-56.dll") != FALSE;
}
