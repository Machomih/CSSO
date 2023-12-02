#include "Directory.h"
#include <iostream>

std::wstring Directory::getPath() const {
    return fullPath;
}

void Directory::createDirectories() const {
    std::wstring builtPath;
    size_t pos = 0;

    while ((pos = fullPath.find(L'\\', pos + 1)) != std::wstring::npos) {
        builtPath = fullPath.substr(0, pos);

        if (!validateDirectory(builtPath)) {
            wprintf(L"Directory created at full path %ls \n", builtPath.c_str());
            Sleep(200);
        }
    }
    if (!validateDirectory(fullPath)) {
        wprintf(L"Directory created at full path %ls \n", fullPath.c_str());
    }
}

bool Directory::validateDirectory(const std::wstring& directoryPath) {
    if (!CreateDirectoryW(directoryPath.c_str(), nullptr)) {

        switch (DWORD error = GetLastError()) {
        case ERROR_ALREADY_EXISTS:
            break;
        case ERROR_PATH_NOT_FOUND:
            wprintf(L"Please enter the full path of the directory you want to create!\n");
            wprintf(L"%lu \n", error);
            break;
        default:
            wprintf(L"Error is out of program scope. Please handle it yourself! \n");
            wprintf(L"%lu \n", error);
        }
        return true;
    }
    return false;
}
