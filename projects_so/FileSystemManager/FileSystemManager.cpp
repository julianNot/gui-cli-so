#include "FileSystemManager.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <system_error>
#include <locale>
#include <codecvt>

FileSystemManager::FileSystemManager() {
    updateCurrentPath();
}

bool FileSystemManager::changeDirectory(const std::string& path) {
    if (SetCurrentDirectoryA(path.c_str())) {
        updateCurrentPath();
        return true;
    }
    return false;
}

std::string FileSystemManager::getCurrentDirectory() const {
    return currentPath;
}

std::vector<FileInfo> FileSystemManager::listDirectory(const std::string& path) {
    std::vector<FileInfo> files;
    std::string searchPath = path + "\\*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return files;
    }

    do {
        std::string filename(findData.cFileName);
        if (filename != "." && filename != "..") {
            FileInfo info;
            info.name = filename;
            info.isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            info.size = (static_cast<size_t>(findData.nFileSizeHigh) << 32) + findData.nFileSizeLow;
            
            FILETIME ft = findData.ftLastWriteTime;
            ULARGE_INTEGER ull;
            ull.LowPart = ft.dwLowDateTime;
            ull.HighPart = ft.dwHighDateTime;
            info.lastModified = ull.QuadPart / 10000000ULL - 11644473600ULL;

            // Configurar permisos
            std::string perms;
            perms += (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? "r" : "w";
            perms += (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? "h" : "-";
            perms += (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? "s" : "-";
            info.permissions = perms;

            files.push_back(info);
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
    return files;
}

bool FileSystemManager::createDirectory(const std::string& name) {
    return CreateDirectoryA(name.c_str(), NULL) != 0;
}

bool FileSystemManager::removeDirectory(const std::string& name) {
    return RemoveDirectoryA(name.c_str()) != 0;
}

bool FileSystemManager::createFile(const std::string& name, const std::string& content) {
    HANDLE hFile = CreateFileA(
        name.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    bool success = true;
    if (!content.empty()) {
        DWORD bytesWritten;
        success = WriteFile(
            hFile,
            content.c_str(),
            static_cast<DWORD>(content.length()),
            &bytesWritten,
            NULL
        ) != 0;
    }

    CloseHandle(hFile);
    return success;
}

bool FileSystemManager::deleteFile(const std::string& name) {
    return DeleteFileA(name.c_str()) != 0;
}

bool FileSystemManager::copyFile(const std::string& source, const std::string& destination) {
    return CopyFileA(source.c_str(), destination.c_str(), FALSE) != 0;
}

bool FileSystemManager::moveFile(const std::string& source, const std::string& destination) {
    return MoveFileA(source.c_str(), destination.c_str()) != 0;
}

std::string FileSystemManager::readFile(const std::string& name) {
    std::ifstream file(name, std::ios::binary);
    if (!file) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystemManager::writeFile(const std::string& name, const std::string& content) {
    std::ofstream file(name, std::ios::binary);
    if (!file) {
        return false;
    }

    file << content;
    return true;
}

bool FileSystemManager::appendToFile(const std::string& name, const std::string& content) {
    std::ofstream file(name, std::ios::app | std::ios::binary);
    if (!file) {
        return false;
    }

    file << content;
    return true;
}

FileInfo FileSystemManager::getFileInfo(const std::string& name) {
    FileInfo info;
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(name.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return info;
    }

    info.name = findData.cFileName;
    info.isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    info.size = (static_cast<size_t>(findData.nFileSizeHigh) << 32) + findData.nFileSizeLow;

    FILETIME ft = findData.ftLastWriteTime;
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;
    info.lastModified = ull.QuadPart / 10000000ULL - 11644473600ULL;

    // Configurar permisos
    std::string perms;
    perms += (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? "r" : "w";
    perms += (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? "h" : "-";
    perms += (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? "s" : "-";
    info.permissions = perms;

    FindClose(hFind);
    return info;
}

bool FileSystemManager::setFileAttributes(const std::string& name, DWORD attributes) {
    return SetFileAttributesA(name.c_str(), attributes) != 0;
}

bool FileSystemManager::exists(const std::string& path) const {
    DWORD attrs = GetFileAttributesA(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES;
}

std::string FileSystemManager::getAbsolutePath(const std::string& relativePath) const {
    char absolutePath[MAX_PATH];
    if (GetFullPathNameA(relativePath.c_str(), MAX_PATH, absolutePath, NULL) == 0) {
        return relativePath;
    }
    return std::string(absolutePath);
}

bool FileSystemManager::isDirectory(const std::string& path) const {
    DWORD attrs = GetFileAttributesA(path.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES) && 
           (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

void FileSystemManager::updateCurrentPath() {
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    currentPath = buffer;
}

std::string FileSystemManager::getLastErrorAsString() const {
    DWORD error = GetLastError();
    if (error == 0) {
        return std::string();
    }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}