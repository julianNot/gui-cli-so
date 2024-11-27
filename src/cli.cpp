#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "../projects_so/DeviceManager/DeviceManager.h"
#include "../projects_so/MemoryManager/MemoryManager.h"
#include "../projects_so/ProcessManager/ProcessManager.h"
#include "../projects_so/FileSystemManager/FileSystemManager.h"

class CLI {
private:
    bool running;
    std::string currentPath;
    DeviceManager deviceManager;
    ProcessManager processManager;
    FileSystemManager fileSystem;

    std::vector<std::string> splitCommand(const std::string& command) {
        std::vector<std::string> tokens;
        std::stringstream ss(command);
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void displayPrompt() {
        std::cout << currentPath << "> ";
    }

    void processDeviceCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Uso: device <operación> [parámetros]\n";
            return;
        }

        std::string operation = tokens[1];
        
        if (operation == "write") {
            if (tokens.size() < 5) {
                std::cout << "Uso: device write <storage|display> <datos> [dirección]\n";
                return;
            }

            IORequest request;
            request.processId = 1;
            request.operation = OperationType::Write;
            request.data = tokens[3];

            if (tokens[2] == "storage") {
                request.device = DeviceType::Storage;
                request.address = std::stoi(tokens[4]);
            }
            else if (tokens[2] == "display") {
                request.device = DeviceType::Display;
            }
            else {
                std::cout << "Dispositivo no válido\n";
                return;
            }

            deviceManager.submitRequest(request);
            deviceManager.processRequests();
        }
        else if (operation == "read") {
            if (tokens.size() < 3) {
                std::cout << "Uso: device read <storage|keyboard> [dirección]\n";
                return;
            }

            IORequest request;
            request.processId = 1;
            request.operation = OperationType::Read;

            if (tokens[2] == "storage") {
                if (tokens.size() < 4) {
                    std::cout << "Se requiere dirección para lectura de almacenamiento\n";
                    return;
                }
                request.device = DeviceType::Storage;
                request.address = std::stoi(tokens[3]);
            }
            else if (tokens[2] == "keyboard") {
                request.device = DeviceType::Keyboard;
                deviceManager.setKeyboardBuffer("Entrada simulada del teclado");
            }
            else {
                std::cout << "Dispositivo no válido\n";
                return;
            }

            deviceManager.submitRequest(request);
            deviceManager.processRequests();
        }
        else if (operation == "status") {
            std::cout << "Estado del display:\n" << deviceManager.getDisplayBuffer() << "\n";
        }
        else {
            std::cout << "Operación de dispositivo no válida\n";
        }
    }

    void processMemoryCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Uso: memory <operación> [parámetros]\n";
            return;
        }

        std::string operation = tokens[1];

        if (operation == "allocate") {
            if (tokens.size() < 3) {
                std::cout << "Uso: memory allocate <process_id>\n";
                return;
            }
            int processId = std::stoi(tokens[2]);
            if (memoryAllocation(processId)) {
                std::cout << "Memoria asignada exitosamente para el proceso " << processId << "\n";
            } else {
                std::cout << "Error al asignar memoria\n";
            }
        }
        else if (operation == "free") {
            if (tokens.size() < 3) {
                std::cout << "Uso: memory free <process_id>\n";
                return;
            }
            int processId = std::stoi(tokens[2]);
            releaseMemory(processId);
            std::cout << "Memoria liberada para el proceso " << processId << "\n";
        }
        else if (operation == "swap") {
            if (tokens.size() < 5) {
                std::cout << "Uso: memory swap <segment_id> <page_number> <process_id>\n";
                return;
            }
            int segmentId = std::stoi(tokens[2]);
            int pageNumber = std::stoi(tokens[3]);
            int processId = std::stoi(tokens[4]);
            
            if (memorySwap(segmentId, pageNumber, processId)) {
                std::cout << "Swap realizado exitosamente\n";
            } else {
                std::cout << "Error al realizar swap\n";
            }
        }
        else if (operation == "status") {
            int availableMem = freeMem();
            std::cout << "Memoria disponible: " << availableMem << " KB\n";
        }
        else {
            std::cout << "Operación de memoria no válida\n";
        }
    }

     void processProcessCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Uso: process <operación> [parámetros]\n";
            return;
        }

        std::string operation = tokens[1];

        if (operation == "create") {
            if (tokens.size() < 3) {
                std::cout << "Uso: process create <ruta_programa> [argumentos]\n";
                return;
            }
            std::string args = tokens.size() > 3 ? tokens[3] : "";
            if (processManager.createProcess(tokens[2], args)) {
                std::cout << "Proceso creado exitosamente\n";
            } else {
                std::cout << "Error al crear el proceso\n";
            }
        }
        else if (operation == "list") {
            auto processes = processManager.listProcesses();
            std::cout << "Lista de procesos:\n";
            std::cout << std::setw(8) << "PID" << " | " 
                      << std::setw(30) << "Nombre" << " | "
                      << std::setw(10) << "Hilos" << " | "
                      << std::setw(15) << "Memoria (KB)" << "\n";
            std::cout << std::string(70, '-') << "\n";
            
            for (const auto& proc : processes) {
                std::cout << std::setw(8) << proc.processId << " | "
                          << std::setw(30) << proc.processName << " | "
                          << std::setw(10) << proc.threadCount << " | "
                          << std::setw(15) << proc.workingSetSize/1024 << "\n";
            }
        }
        else if (operation == "kill") {
            if (tokens.size() < 3) {
                std::cout << "Uso: process kill <PID>\n";
                return;
            }
            DWORD pid = std::stoul(tokens[2]);
            if (processManager.terminateProcess(pid)) {
                std::cout << "Proceso terminado exitosamente\n";
            } else {
                std::cout << "Error al terminar el proceso\n";
            }
        }
        else if (operation == "suspend") {
            if (tokens.size() < 3) {
                std::cout << "Uso: process suspend <PID>\n";
                return;
            }
            DWORD pid = std::stoul(tokens[2]);
            if (processManager.suspendProcess(pid)) {
                std::cout << "Proceso suspendido exitosamente\n";
            } else {
                std::cout << "Error al suspender el proceso\n";
            }
        }
        else if (operation == "resume") {
            if (tokens.size() < 3) {
                std::cout << "Uso: process resume <PID>\n";
                return;
            }
            DWORD pid = std::stoul(tokens[2]);
            if (processManager.resumeProcess(pid)) {
                std::cout << "Proceso reanudado exitosamente\n";
            } else {
                std::cout << "Error al reanudar el proceso\n";
            }
        }
        else if (operation == "info") {
            if (tokens.size() < 3) {
                std::cout << "Uso: process info <PID>\n";
                return;
            }
            DWORD pid = std::stoul(tokens[2]);
            auto info = processManager.getProcessInfo(pid);
            std::cout << "Información del proceso " << pid << ":\n";
            std::cout << "Nombre: " << info.processName << "\n";
            std::cout << "Memoria en uso: " << info.workingSetSize/1024 << " KB\n";
            std::cout << "Prioridad: " << info.priorityClass << "\n";
        }
        else {
            std::cout << "Operación de proceso no válida\n";
        }
    }

    void processFileSystemCommand(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) {
            std::cout << "Uso: fs <operación> [parámetros]\n";
            return;
        }

        std::string operation = tokens[1];

        if (operation == "cd") {
            if (tokens.size() < 3) {
                std::cout << "Uso: fs cd <ruta>\n";
                return;
            }
            if (fileSystem.changeDirectory(tokens[2])) {
                std::cout << "Directorio actual: " << fileSystem.getCurrentDirectory() << "\n";
            } else {
                std::cout << "Error al cambiar de directorio\n";
            }
        }
        else if (operation == "ls") {
            std::string path = tokens.size() > 2 ? tokens[2] : ".";
            auto files = fileSystem.listDirectory(path);
            
            std::cout << "\nContenido del directorio:\n";
            std::cout << std::setw(40) << "Nombre" << " | "
                    << std::setw(8) << "Tipo" << " | "
                    << std::setw(10) << "Tamaño" << " | "
                    << std::setw(10) << "Permisos" << "\n";
            std::cout << std::string(75, '-') << "\n";
            
            for (const auto& file : files) {
                std::cout << std::setw(40) << file.name << " | "
                        << std::setw(8) << (file.isDirectory ? "DIR" : "FILE") << " | "
                        << std::setw(10) << file.size << " | "
                        << std::setw(10) << file.permissions << "\n";
            }
        }
        else if (operation == "mkdir") {
            if (tokens.size() < 3) {
                std::cout << "Uso: fs mkdir <nombre>\n";
                return;
            }
            if (fileSystem.createDirectory(tokens[2])) {
                std::cout << "Directorio creado exitosamente\n";
            } else {
                std::cout << "Error al crear directorio\n";
            }
        }
        else if (operation == "rmdir") {
            if (tokens.size() < 3) {
                std::cout << "Uso: fs rmdir <nombre>\n";
                return;
            }
            if (fileSystem.removeDirectory(tokens[2])) {
                std::cout << "Directorio eliminado exitosamente\n";
            } else {
                std::cout << "Error al eliminar directorio\n";
            }
        }
        else if (operation == "touch") {
            if (tokens.size() < 3) {
                std::cout << "Uso: fs touch <nombre>\n";
                return;
            }
            if (fileSystem.createFile(tokens[2])) {
                std::cout << "Archivo creado exitosamente\n";
            } else {
                std::cout << "Error al crear archivo\n";
            }
        }
        else if (operation == "rm") {
            if (tokens.size() < 3) {
                std::cout << "Uso: fs rm <nombre>\n";
                return;
            }
            if (fileSystem.deleteFile(tokens[2])) {
                std::cout << "Archivo eliminado exitosamente\n";
            } else {
                std::cout << "Error al eliminar archivo\n";
            }
        }
        else if (operation == "cp") {
            if (tokens.size() < 4) {
                std::cout << "Uso: fs cp <origen> <destino>\n";
                return;
            }
            if (fileSystem.copyFile(tokens[2], tokens[3])) {
                std::cout << "Archivo copiado exitosamente\n";
            } else {
                std::cout << "Error al copiar archivo\n";
            }
        }
        else if (operation == "mv") {
            if (tokens.size() < 4) {
                std::cout << "Uso: fs mv <origen> <destino>\n";
                return;
            }
            if (fileSystem.moveFile(tokens[2], tokens[3])) {
                std::cout << "Archivo movido exitosamente\n";
            } else {
                std::cout << "Error al mover archivo\n";
            }
        }
        else if (operation == "pwd") {
            std::cout << "Directorio actual: " << fileSystem.getCurrentDirectory() << "\n";
        }
        else {
            std::cout << "Operación de sistema de archivos no válida\n";
        }
    }

    void processCommand(const std::string& commandLine) {
        std::vector<std::string> tokens = splitCommand(commandLine);
        if (tokens.empty()) return;

        const std::string& command = tokens[0];

        if (command == "exit") {
            running = false;
        }
        else if (command == "help") {
            showHelp();
        }
        else if (command == "device") {
            processDeviceCommand(tokens);
        }
        else if (command == "memory") {
            processMemoryCommand(tokens);
        }
        else if (command == "process") {
            processProcessCommand(tokens);
        }
        else if (command == "fs") {
            processFileSystemCommand(tokens);
        }
        else {
            std::cout << "Comando no reconocido. Use 'help' para ver los comandos disponibles.\n";
        }
    }

    void showHelp() {
        std::cout << "Comandos disponibles:\n";
        std::cout << "  help                                          - Muestra esta ayuda\n";
        std::cout << "  exit                                          - Salir del programa\n";
        std::cout << "\nComandos de dispositivos:\n";
        std::cout << "  device write storage <datos> <dirección>      - Escribe datos en almacenamiento\n";
        std::cout << "  device write display <texto>                  - Escribe texto en pantalla\n";
        std::cout << "  device read storage <dirección>               - Lee datos del almacenamiento\n";
        std::cout << "  device read keyboard                          - Lee entrada del teclado\n";
        std::cout << "  device status                                 - Muestra el estado del display\n";
        std::cout << "\nComandos de memoria:\n";
        std::cout << "  memory allocate <process_id>                  - Asigna memoria a un proceso\n";
        std::cout << "  memory free <process_id>                      - Libera la memoria de un proceso\n";
        std::cout << "  memory swap <segment_id> <page> <process_id>  - Realiza swap de una página\n";
        std::cout << "  memory status                                 - Muestra el estado de la memoria\n";
        std::cout << "\nComandos de procesos:\n";
        std::cout << "  process create <ruta_programa> [argumentos]   - Crea un nuevo proceso\n";
        std::cout << "  process list                                  - Lista todos los procesos\n";
        std::cout << "  process kill <PID>                           - Termina un proceso\n";
        std::cout << "  process suspend <PID>                        - Suspende un proceso\n";
        std::cout << "  process resume <PID>                         - Reanuda un proceso\n";
        std::cout << "  process info <PID>                           - Muestra información de un proceso\n";
        std::cout << "\nComandos del sistema de archivos:\n";
        std::cout << "  fs pwd                                       - Muestra el directorio actual\n";
        std::cout << "  fs cd <ruta>                                 - Cambia al directorio especificado\n";
        std::cout << "  fs ls [ruta]                                 - Lista el contenido del directorio\n";
        std::cout << "  fs mkdir <nombre>                            - Crea un nuevo directorio\n";
        std::cout << "  fs rmdir <nombre>                            - Elimina un directorio\n";
        std::cout << "  fs touch <nombre>                            - Crea un nuevo archivo\n";
        std::cout << "  fs rm <nombre>                               - Elimina un archivo\n";
        std::cout << "  fs cp <origen> <destino>                     - Copia un archivo\n";
        std::cout << "  fs mv <origen> <destino>                     - Mueve o renombra un archivo\n";
    }

public:
    CLI() : running(true) {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        currentPath = buffer;
    }

    void run() {
        std::cout << "Sistema Operativo - CLI\n";
        std::cout << "Escriba 'help' para ver los comandos disponibles\n\n";

        while (running) {
            displayPrompt();
            std::string command;
            std::getline(std::cin, command);

            if (!command.empty()) {
                processCommand(command);
            }
        }
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    CLI cli;
    cli.run();

    return 0;
}