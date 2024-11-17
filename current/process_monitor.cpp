#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>  // Para std::sort y std::all_of
#include <cctype>     // Para isdigit

#ifdef _WIN32
    #include <windows.h>
    #include <tlhelp32.h>
    #include <psapi.h>
#else
    #include <dirent.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include <fstream>
    #include <sstream>
#endif

struct ProcessInfo {
    unsigned long pid;
    std::string name;
    double cpu_usage;
    size_t memory_usage;
    std::string status;
};

class ProcessMonitor {
private:
    std::vector<ProcessInfo> processes;

    #ifdef _WIN32
    double calculateCPUUsage(HANDLE process) {
        static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(process, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));

        double percent = (sys.QuadPart - lastSysCPU.QuadPart) +
                        (user.QuadPart - lastUserCPU.QuadPart);
        percent /= (now.QuadPart - lastCPU.QuadPart);
        percent *= 100;

        lastCPU = now;
        lastSysCPU = sys;
        lastUserCPU = user;

        return percent;
    }
    #else
    double getCPUUsage(const std::string& pid) {
        std::string stat_path = "/proc/" + pid + "/stat";
        std::ifstream stat_file(stat_path);
        if (!stat_file.is_open()) return 0.0;

        std::string line;
        std::getline(stat_file, line);
        std::istringstream iss(line);

        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.size() < 14) return 0.0;

        unsigned long utime = std::stoul(tokens[13]);
        unsigned long stime = std::stoul(tokens[14]);
        
        // Simplified CPU calculation
        return (utime + stime) / (double)sysconf(_SC_CLK_TCK);
    }

    size_t getMemoryUsage(const std::string& pid) {
        std::string statm_path = "/proc/" + pid + "/statm";
        std::ifstream statm_file(statm_path);
        if (!statm_file.is_open()) return 0;

        size_t total_mem;
        statm_file >> total_mem;
        return total_mem * sysconf(_SC_PAGESIZE) / 1024; // Convert to KB
    }
    #endif

public:
    void updateProcessList() {
        processes.clear();

        #ifdef _WIN32
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) return;

        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (!Process32First(snapshot, &processEntry)) {
            CloseHandle(snapshot);
            return;
        }

        do {
            HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                                            FALSE, processEntry.th32ProcessID);
            if (processHandle != NULL) {
                ProcessInfo info;
                info.pid = processEntry.th32ProcessID;
                info.name = processEntry.szExeFile;
                
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo(processHandle, &pmc, sizeof(pmc))) {
                    info.memory_usage = pmc.WorkingSetSize / 1024; // Convert to KB
                }
                
                info.cpu_usage = calculateCPUUsage(processHandle);
                info.status = "Running"; // Simplified status

                processes.push_back(info);
                CloseHandle(processHandle);
            }
        } while (Process32Next(snapshot, &processEntry));

        CloseHandle(snapshot);

        #else
        DIR* dir = opendir("/proc");
        if (dir == nullptr) return;

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_DIR) {
                std::string pid_str = entry->d_name;
                if (std::all_of(pid_str.begin(), pid_str.end(), ::isdigit)) {
                    ProcessInfo info;
                    info.pid = std::stoul(pid_str);

                    // Get process name
                    std::string cmd_path = "/proc/" + pid_str + "/comm";
                    std::ifstream cmd_file(cmd_path);
                    if (cmd_file.is_open()) {
                        std::getline(cmd_file, info.name);
                    }

                    // Get CPU and memory usage
                    info.cpu_usage = getCPUUsage(pid_str);
                    info.memory_usage = getMemoryUsage(pid_str);

                    // Get status
                    std::string status_path = "/proc/" + pid_str + "/status";
                    std::ifstream status_file(status_path);
                    if (status_file.is_open()) {
                        std::string line;
                        while (std::getline(status_file, line)) {
                            if (line.substr(0, 7) == "State:\t") {
                                info.status = line.substr(7);
                                break;
                            }
                        }
                    }

                    processes.push_back(info);
                }
            }
        }
        closedir(dir);
        #endif
    }

    void displayProcesses(int limit = 20) {
        // Clear screen
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif

        // Print header
        std::cout << std::setw(8) << "PID" << " | "
                  << std::setw(30) << std::left << "Nombre" << " | "
                  << std::setw(10) << std::right << "CPU %" << " | "
                  << std::setw(10) << "Mem (KB)" << " | "
                  << std::setw(15) << std::left << "Estado" << std::endl;
        std::cout << std::string(80, '-') << std::endl;

        // Sort processes by CPU usage
        std::sort(processes.begin(), processes.end(),
                 [](const ProcessInfo& a, const ProcessInfo& b) {
                     return a.cpu_usage > b.cpu_usage;
                 });

        // Display processes
        int count = 0;
        for (const auto& proc : processes) {
            if (count++ >= limit) break;

            std::cout << std::setw(8) << proc.pid << " | "
                      << std::setw(30) << std::left << proc.name << " | "
                      << std::setw(10) << std::fixed << std::setprecision(1) << proc.cpu_usage << " | "
                      << std::setw(10) << proc.memory_usage << " | "
                      << std::setw(15) << std::left << proc.status << std::endl;
        }
    }
};

int main() {
    ProcessMonitor monitor;

    try {
        while (true) {
            monitor.updateProcessList();
            monitor.displayProcesses();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}