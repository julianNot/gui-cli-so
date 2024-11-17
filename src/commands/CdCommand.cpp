#include "../../include/commands/CdCommand.hpp"
#include "../../include/Colors.hpp"
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <cstring>

CdCommand::CdCommand(Shell &s) : shell(s) {}

void CdCommand::execute(const std::vector<std::string> &args)
{
    std::string path;

    if (args.size() < 2)
    {
        // Si no se proporciona ruta, ir al directorio home
        const char *home = getenv("HOME");
        if (!home)
        {
            std::cout << ShellColors::RED << "Error: No se pudo obtener el directorio HOME"
                      << ShellColors::RESET << std::endl;
            return;
        }
        path = home;
    }
    else
    {
        path = args[1];
    }

    // Cambiar directorio
    if (chdir(path.c_str()) != 0)
    {
        std::cout << ShellColors::RED << "Error al cambiar al directorio '" << path
                  << "': " << strerror(errno) << ShellColors::RESET << std::endl;
        return;
    }

    // Actualizar la ruta actual en la shell
    char buff[PATH_MAX];
    if (getcwd(buff, PATH_MAX) != nullptr)
    {
        shell.setCurrentPath(buff);
    }
    else
    {
        std::cout << ShellColors::RED << "Error al obtener el directorio actual"
                  << ShellColors::RESET << std::endl;
    }
}

std::string CdCommand::getHelp() const
{
    return "cd [directorio]  - Cambia el directorio actual\n"
           "  Sin argumentos: cambia al directorio home\n"
           "Ejemplos:\n"
           "  cd           - Ir al directorio home\n"
           "  cd ..        - Subir un nivel\n"
           "  cd /ruta     - Ir a una ruta específica";
}

std::string CdCommand::getName() const
{
    return "cd";
}