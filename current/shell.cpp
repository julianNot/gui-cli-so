#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <functional>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <iomanip>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>    // Para strerror
#include <algorithm>  // Para std::sort

// Colores para la terminal
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"

class SimpleShell {
private:
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> commands;
    std::string currentPath;
    std::string username;

    void initCommands() {
        commands["help"] = [this](const std::vector<std::string>& args) { showHelp(); };
        commands["exit"] = [this](const std::vector<std::string>& args) { exit(0); };
        commands["clear"] = [](const std::vector<std::string>& args) { system("clear"); };
        commands["cd"] = [this](const std::vector<std::string>& args) { changeDirectory(args); };
        commands["ls"] = [this](const std::vector<std::string>& args) { listDirectory(args); };
        commands["mkdir"] = [this](const std::vector<std::string>& args) { createDirectory(args); };
    }

    void showHelp() {
        std::cout << CYAN << "\nComandos disponibles:" << RESET << std::endl;
        std::cout << "  help     - Muestra esta ayuda" << std::endl;
        std::cout << "  clear    - Limpia la pantalla" << std::endl;
        std::cout << "  cd       - Cambia el directorio actual" << std::endl;
        std::cout << "  ls       - Lista el contenido del directorio" << std::endl;
        std::cout << "  mkdir    - Crea un nuevo directorio" << std::endl;
        std::cout << "  exit     - Salir de la shell" << std::endl;
        std::cout << "\nEjemplos:" << std::endl;
        std::cout << "  mkdir mi_carpeta              - Crea una carpeta llamada 'mi_carpeta'" << std::endl;
        std::cout << "  mkdir -p ruta/a/mi/carpeta    - Crea carpetas anidadas" << std::endl;
    }

    void createDirectory(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << RED << "Error: Debe especificar el nombre del directorio" << RESET << std::endl;
            std::cout << "Uso: mkdir [-p] <nombre_directorio>" << std::endl;
            return;
        }

        bool createParents = false;
        std::string dirPath;

        // Verificar si se usa la opción -p
        if (args[1] == "-p") {
            if (args.size() < 3) {
                std::cout << RED << "Error: Debe especificar el nombre del directorio después de -p" << RESET << std::endl;
                return;
            }
            createParents = true;
            dirPath = args[2];
        } else {
            dirPath = args[1];
        }

        // Si la ruta es relativa, agregarla al directorio actual
        if (dirPath[0] != '/') {
            dirPath = currentPath + "/" + dirPath;
        }

        if (createParents) {
            // Crear directorios recursivamente
            std::string currentDir;
            std::istringstream pathStream(dirPath);
            std::string token;

            // Dividir la ruta por '/'
            while (std::getline(pathStream, token, '/')) {
                if (token.empty()) continue;
                
                if (currentDir.empty()) {
                    currentDir = "/";
                }
                currentDir += token + "/";

                // Intentar crear el directorio
                if (mkdir(currentDir.c_str(), 0755) != 0) {
                    // Ignorar error si el directorio ya existe
                    if (errno != EEXIST) {
                        std::cout << RED << "Error al crear el directorio '" << currentDir 
                                 << "': " << strerror(errno) << RESET << std::endl;
                        return;
                    }
                } else {
                    std::cout << GREEN << "Creado directorio: " << currentDir << RESET << std::endl;
                }
            }
        } else {
            // Crear un solo directorio
            if (mkdir(dirPath.c_str(), 0755) != 0) {
                if (errno == EEXIST) {
                    std::cout << YELLOW << "El directorio '" << dirPath 
                             << "' ya existe" << RESET << std::endl;
                } else {
                    std::cout << RED << "Error al crear el directorio '" << dirPath 
                             << "': " << strerror(errno) << RESET << std::endl;
                }
            } else {
                std::cout << GREEN << "Creado directorio: " << dirPath << RESET << std::endl;
            }
        }
    }

    void changeDirectory(const std::vector<std::string>& args) {
        std::string path = args.size() > 1 ? args[1] : getenv("HOME");
        if (chdir(path.c_str()) != 0) {
            std::cout << RED << "Error: No se pudo cambiar al directorio " << path << RESET << std::endl;
        } else {
            char buff[PATH_MAX];
            currentPath = getcwd(buff, PATH_MAX);
        }
    }

    void listDirectory(const std::vector<std::string>& args) {
        std::string path = args.size() > 1 ? args[1] : ".";
        DIR* dir = opendir(path.c_str());
        if (dir == nullptr) {
            std::cout << RED << "Error: No se pudo abrir el directorio " << path << RESET << std::endl;
            return;
        }

        struct dirent* entry;
        std::vector<std::string> entries;
        while ((entry = readdir(dir)) != nullptr) {
            entries.push_back(entry->d_name);
        }
        closedir(dir);

        std::sort(entries.begin(), entries.end());
        for (const auto& name : entries) {
            std::cout << name << "  ";
        }
        std::cout << std::endl;
    }

    std::vector<std::string> splitCommand(const std::string& command) {
        std::vector<std::string> args;
        std::istringstream iss(command);
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        return args;
    }

public:
    SimpleShell() {
        initCommands();
        username = getenv("USER");
        char buff[PATH_MAX];
        currentPath = getcwd(buff, PATH_MAX);
    }

    void run() {
        std::cout << CYAN << "Shell Simple v1.0" << RESET << std::endl;
        std::cout << "Escribe 'help' para ver los comandos disponibles\n" << std::endl;

        while (true) {
            std::cout << GREEN << username << "@shell:" << BLUE << currentPath << RESET << "$ ";
            
            std::string input;
            std::getline(std::cin, input);

            if (input.empty()) continue;

            auto args = splitCommand(input);
            if (args.empty()) continue;

            auto cmd = commands.find(args[0]);
            if (cmd != commands.end()) {
                cmd->second(args);
            } else {
                std::cout << RED << "Comando no reconocido: " << args[0] << RESET << std::endl;
            }
        }
    }
};

int main() {
    SimpleShell shell;
    shell.run();
    return 0;
}