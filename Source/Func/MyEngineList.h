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

#ifndef IU_FUNC_MY_ENGINE_LIST_H
#define IU_FUNC_MY_ENGINE_LIST_H

#include "atlheaders.h"
#include "Library.h"

#include "Core/UploadEngineList.h"

class CMyEngineList: public CUploadEngineList
{
    public:
        CMyEngineList();
        ~CMyEngineList();
        CString ErrorStr() const;
        CUploadEngineData* byName(const CString &name);
        int getUploadEngineIndex(const CString& Name) const;
        bool loadFromFile(const CString& filename);
        HICON getIconForServer(const std::string& name);
        CString getIconNameForServer(const std::string& name);
        static char DefaultServer[];
        static char RandomServer[];
    private:
        std::map<std::string, HICON> serverIcons_;
        CString m_ErrorStr;
        typedef HRESULT(WINAPI *LoadIconWithScaleDownFuncType)(HINSTANCE hinst, PCWSTR pszName, int cx, int cy, _Out_ HICON *phico);

        LoadIconWithScaleDownFuncType LoadIconWithScaleDownFunc_;
        Library dllModule_;
};
#endif