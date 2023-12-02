#ifndef S2_DIRECTORY_H
#define S2_DIRECTORY_H

#include <string>
#include <utility>
#include <Windows.h>

class Directory {
private:
    std::wstring fullPath;
    static bool validateDirectory(const std::wstring& directoryPath);

public:
    explicit Directory(std::wstring path) : fullPath(std::move(path)) {} 
    ~Directory() = default;

    void createDirectories() const;
    [[nodiscard]] std::wstring getPath() const;
};

#endif //S2_DIRECTORY_H