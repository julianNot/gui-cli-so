#include "../../include/commands/HelpCommand.hpp"
#include "../../include/Colors.hpp"
#include <iostream>
#include <iomanip>

HelpCommand::HelpCommand(Shell &s) : shell(s) {}

void HelpCommand::execute(const std::vector<std::string> &args)
{
    const auto &commands = shell.getCommands();

    // Si se especifica un comando, mostrar ayuda detallada de ese comando
    if (args.size() > 1)
    {
        auto it = commands.find(args[1]);
        if (it != commands.end())
        {
            std::cout << ShellColors::CYAN << "\nAyuda detallada para '" << args[1] << "':\n"
                      << ShellColors::RESET << std::endl;
            std::cout << it->second->getHelp() << "\n"
                      << std::endl;
            return;
        }
        std::cout << ShellColors::RED << "Error: Comando '" << args[1] << "' no encontrado"
                  << ShellColors::RESET << std::endl;
        return;
    }

    // Mostrar lista de todos los comandos de manera organizada
    std::cout << ShellColors::CYAN << "\nComandos disponibles:\n"
              << ShellColors::RESET << std::endl;

    // Calcular el ancho máximo del nombre del comando para alinear la salida
    size_t maxWidth = 0;
    for (const auto &cmd : commands)
    {
        maxWidth = std::max(maxWidth, cmd.first.length());
    }
    maxWidth += 2; // Agregar espacio extra

    // Mostrar cada comando con su descripción básica
    for (const auto &cmd : commands)
    {
        std::cout << "  " << std::left << std::setw(maxWidth) << cmd.first;

        // Obtener la primera línea de la ayuda como descripción básica
        std::string help = cmd.second->getHelp();
        size_t pos = help.find('\n');
        if (pos != std::string::npos)
        {
            help = help.substr(0, pos);
        }
        std::cout << "- " << help << std::endl;
    }

    std::cout << "\nPara ayuda detallada sobre un comando específico:\n"
              << "  help <comando>\n"
              << std::endl;
}

std::string HelpCommand::getHelp() const
{
    return "help [comando]  - Muestra esta ayuda o la ayuda específica de un comando\n"
           "  Sin argumentos: muestra la lista de todos los comandos\n"
           "  Con argumento: muestra ayuda detallada del comando especificado\n"
           "Ejemplos:\n"
           "  help     - Muestra esta lista\n"
           "  help ls  - Muestra ayuda detallada del comando 'ls'";
}

std::string HelpCommand::getName() const
{
    return "help";
}