#include "ProcessManager.h"
#include <psapi.h>
#include <iostream>

bool ProcessManager::createProcess(const std::string& programPath, const std::string& args) {
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Convertir string a wstring
    std::wstring wCommandLine;
    wCommandLine.assign(programPath.begin(), programPath.end());
    if (!args.empty()) {
        wCommandLine += L" ";
        wCommandLine.append(args.begin(), args.end());
    }

    // Crear un buffer modificable
    wchar_t* cmd = new wchar_t[wCommandLine.length() + 1];
    wcscpy_s(cmd, wCommandLine.length() + 1, wCommandLine.c_str());

    // Crear el proceso
    bool success = CreateProcessW(
        NULL,           // No module name (use command line)
        cmd,           // Command line
        NULL,          // Process handle not inheritable
        NULL,          // Thread handle not inheritable
        FALSE,         // Set handle inheritance to FALSE
        0,             // No creation flags
        NULL,          // Use parent's environment block
        NULL,          // Use parent's starting directory
        &si,           // Pointer to STARTUPINFO structure
        &pi            // Pointer to PROCESS_INFORMATION structure
    );

    delete[] cmd;

    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }

    return false;
}

bool ProcessManager::terminateProcess(DWORD processId) {
    HANDLE hProcess = openProcess(processId, PROCESS_TERMINATE);
    if (hProcess == NULL) return false;

    bool result = TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
    return result;
}

std::vector<ProcessInfo> ProcessManager::listProcesses() {
    std::vector<ProcessInfo> processes;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) return processes;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(pe32);

    if (!Process32FirstW(snapshot, &pe32)) {
        CloseHandle(snapshot);
        return processes;
    }

    do {
        ProcessInfo info;
        info.processId = pe32.th32ProcessID;
        info.parentProcessId = pe32.th32ParentProcessID;
        info.threadCount = pe32.cntThreads;

        // Convertir wchar a string
        char processName[MAX_PATH];
        wcstombs(processName, pe32.szExeFile, MAX_PATH);
        info.processName = std::string(processName);

        // Obtener información adicional
        HANDLE hProcess = openProcess(info.processId, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);
        if (hProcess != NULL) {
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                info.workingSetSize = pmc.WorkingSetSize;
            }

            info.priorityClass = GetPriorityClass(hProcess);
            CloseHandle(hProcess);
        }

        processes.push_back(info);
    } while (Process32NextW(snapshot, &pe32));

    CloseHandle(snapshot);
    return processes;
}

ProcessInfo ProcessManager::getProcessInfo(DWORD processId) {
    ProcessInfo info = {0};
    HANDLE hProcess = openProcess(processId, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);
    
    if (hProcess == NULL) return info;

    // Obtener nombre del proceso
    CHAR processName[MAX_PATH];
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameA(hProcess, 0, processName, &size)) {
        info.processName = std::string(processName);
    }

    // Obtener información de memoria
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        info.workingSetSize = pmc.WorkingSetSize;
    }

    info.processId = processId;
    info.priorityClass = GetPriorityClass(hProcess);

    CloseHandle(hProcess);
    return info;
}

bool ProcessManager::setProcessPriority(DWORD processId, DWORD priorityClass) {
    HANDLE hProcess = openProcess(processId, PROCESS_SET_INFORMATION);
    if (hProcess == NULL) return false;

    bool result = SetPriorityClass(hProcess, priorityClass);
    CloseHandle(hProcess);
    return result;
}

bool ProcessManager::suspendProcess(DWORD processId) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    THREADENTRY32 te32;
    te32.dwSize = sizeof(te32);

    if (!Thread32First(snapshot, &te32)) {
        CloseHandle(snapshot);
        return false;
    }

    // Suspender todos los hilos del proceso
    do {
        if (te32.th32OwnerProcessID == processId) {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
            if (hThread) {
                SuspendThread(hThread);
                CloseHandle(hThread);
            }
        }
    } while (Thread32Next(snapshot, &te32));

    CloseHandle(snapshot);
    return true;
}

bool ProcessManager::resumeProcess(DWORD processId) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    THREADENTRY32 te32;
    te32.dwSize = sizeof(te32);

    if (!Thread32First(snapshot, &te32)) {
        CloseHandle(snapshot);
        return false;
    }

    // Reanudar todos los hilos del proceso
    do {
        if (te32.th32OwnerProcessID == processId) {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
            if (hThread) {
                ResumeThread(hThread);
                CloseHandle(hThread);
            }
        }
    } while (Thread32Next(snapshot, &te32));

    CloseHandle(snapshot);
    return true;
}

double ProcessManager::getProcessCPUUsage(DWORD processId) {
    HANDLE hProcess = openProcess(processId, PROCESS_QUERY_INFORMATION);
    if (hProcess == NULL) return -1;

    FILETIME creation_time, exit_time, kernel_time, user_time;
    if (!getProcessTimes(hProcess, creation_time, exit_time, kernel_time, user_time)) {
        CloseHandle(hProcess);
        return -1;
    }

    ULARGE_INTEGER kernelTime, userTime;
    kernelTime.LowPart = kernel_time.dwLowDateTime;
    kernelTime.HighPart = kernel_time.dwHighDateTime;
    userTime.LowPart = user_time.dwLowDateTime;
    userTime.HighPart = user_time.dwHighDateTime;

    double cpuTime = (kernelTime.QuadPart + userTime.QuadPart) / 10000000.0;  // Convertir a segundos

    CloseHandle(hProcess);
    return cpuTime;
}

SIZE_T ProcessManager::getProcessMemoryUsage(DWORD processId) {
    HANDLE hProcess = openProcess(processId, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ);
    if (hProcess == NULL) return 0;

    PROCESS_MEMORY_COUNTERS pmc;
    if (!GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        CloseHandle(hProcess);
        return 0;
    }

    CloseHandle(hProcess);
    return pmc.WorkingSetSize;
}

HANDLE ProcessManager::openProcess(DWORD processId, DWORD accessRights) {
    return OpenProcess(accessRights, FALSE, processId);
}

bool ProcessManager::getProcessTimes(HANDLE hProcess, FILETIME& creation, FILETIME& exit, FILETIME& kernel, FILETIME& user) {
    return GetProcessTimes(hProcess, &creation, &exit, &kernel, &user);
}

bool ProcessManager::adjustProcessPrivileges() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }

    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
        CloseHandle(hToken);
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    bool result = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    CloseHandle(hToken);

    return result;
}