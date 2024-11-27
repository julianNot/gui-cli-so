#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <windows.h>
#include <string>
#include <vector>
#include <tlhelp32.h>

struct ProcessInfo {
    DWORD processId;
    std::string processName;
    DWORD parentProcessId;
    DWORD threadCount;
    DWORD priorityClass;
    SIZE_T workingSetSize;  // Memoria en uso
};

class ProcessManager {
public:
    // Crear un nuevo proceso
    bool createProcess(const std::string& programPath, const std::string& args = "");
    
    // Terminar un proceso por su ID
    bool terminateProcess(DWORD processId);
    
    // Obtener lista de procesos en ejecución
    std::vector<ProcessInfo> listProcesses();
    
    // Obtener información detallada de un proceso
    ProcessInfo getProcessInfo(DWORD processId);
    
    // Cambiar la prioridad de un proceso
    bool setProcessPriority(DWORD processId, DWORD priorityClass);
    
    // Suspender un proceso
    bool suspendProcess(DWORD processId);
    
    // Reanudar un proceso
    bool resumeProcess(DWORD processId);
    
    // Obtener uso de CPU del proceso
    double getProcessCPUUsage(DWORD processId);
    
    // Obtener uso de memoria del proceso
    SIZE_T getProcessMemoryUsage(DWORD processId);

private:
    // Funciones auxiliares
    bool adjustProcessPrivileges();
    HANDLE openProcess(DWORD processId, DWORD accessRights);
    bool getProcessTimes(HANDLE hProcess, FILETIME& creation, FILETIME& exit, FILETIME& kernel, FILETIME& user);
};

#endif // PROCESS_MANAGER_H