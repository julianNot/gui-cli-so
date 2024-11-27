#ifndef FILESYSTEM_MANAGER_H
#define FILESYSTEM_MANAGER_H

#include <windows.h>
#include <string>
#include <vector>
#include <ctime>

struct FileInfo {
    std::string name;
    bool isDirectory;
    size_t size;
    std::time_t lastModified;
    std::string permissions;
};

class FileSystemManager {
public:
    FileSystemManager();

    // Navegación
    bool changeDirectory(const std::string& path);
    std::string getCurrentDirectory() const;
    std::vector<FileInfo> listDirectory(const std::string& path = ".");

    // Operaciones con directorios
    bool createDirectory(const std::string& name);
    bool removeDirectory(const std::string& name);
    
    // Operaciones con archivos
    bool createFile(const std::string& name, const std::string& content = "");
    bool deleteFile(const std::string& name);
    bool copyFile(const std::string& source, const std::string& destination);
    bool moveFile(const std::string& source, const std::string& destination);
    
    // Lectura/Escritura de archivos
    std::string readFile(const std::string& name);
    bool writeFile(const std::string& name, const std::string& content);
    bool appendToFile(const std::string& name, const std::string& content);

    // Información y atributos
    FileInfo getFileInfo(const std::string& name);
    bool setFileAttributes(const std::string& name, DWORD attributes);
    
    // Utilidades
    bool exists(const std::string& path) const;
    std::string getAbsolutePath(const std::string& relativePath) const;
    bool isDirectory(const std::string& path) const;
    
private:
    std::string currentPath;
    std::wstring stringToWString(const std::string& str);
    std::string wStringToString(const std::wstring& wstr);
    void updateCurrentPath();
    std::string getLastErrorAsString() const;
};

#endif // FILESYSTEM_MANAGER_H