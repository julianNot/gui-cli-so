#include "../include/Shell.hpp"
#include "../include/Colors.hpp"
#include "../include/commands/CdCommand.hpp"
#include "../include/commands/ClearCommand.hpp"
#include "../include/commands/HelpCommand.hpp"
#include "../include/commands/LsCommand.hpp"
#include "../include/commands/MkdirCommand.hpp"
#include "../include/commands/ExitCommand.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <limits.h>
#include <signal.h>

Shell::Shell()
{
    // Configurar el manejador de señales
    signal(SIGINT, [](int sig)
           {
               std::cout << "\n"; // Nueva línea después de ^C
           });

    // Inicializar variables de entorno
    username = getenv("USER") ? getenv("USER") : "usuario";
    char buff[PATH_MAX];
    currentPath = getcwd(buff, PATH_MAX);

    // Inicializar comandos
    initCommands();
}

void Shell::initCommands()
{
    commands["cd"] = std::make_unique<CdCommand>(*this);
    commands["clear"] = std::make_unique<ClearCommand>();
    commands["help"] = std::make_unique<HelpCommand>(*this);
    commands["ls"] = std::make_unique<LsCommand>();
    commands["mkdir"] = std::make_unique<MkdirCommand>(*this);
    commands["exit"] = std::make_unique<ExitCommand>();
}

std::vector<std::string> Shell::splitCommand(const std::string &command)
{
    std::vector<std::string> args;
    std::string token;
    bool inQuotes = false;
    std::stringstream ss;

    for (char c : command)
    {
        if (c == '"')
        {
            inQuotes = !inQuotes;
            continue;
        }

        if (c == ' ' && !inQuotes)
        {
            token = ss.str();
            if (!token.empty())
            {
                args.push_back(token);
                ss.str("");
                ss.clear();
            }
        }
        else
        {
            ss << c;
        }
    }

    // Añadir el último token
    token = ss.str();
    if (!token.empty())
    {
        args.push_back(token);
    }

    return args;
}

void Shell::run()
{
    std::cout << ShellColors::CYAN << "Shell Simple v1.0" << ShellColors::RESET << std::endl;
    std::cout << "Escribe 'help' para ver los comandos disponibles\n"
              << std::endl;

    while (true)
    {
        try
        {
            std::cout << ShellColors::GREEN << username << "@shell:"
                      << ShellColors::BLUE << currentPath
                      << ShellColors::RESET << "$ ";

            std::string input;
            std::getline(std::cin, input);

            if (input.empty())
                continue;

            auto args = splitCommand(input);
            if (args.empty())
                continue;

            auto cmd = commands.find(args[0]);
            if (cmd != commands.end())
            {
                cmd->second->execute(args);
            }
            else
            {
                std::cout << ShellColors::RED << "Comando no reconocido: " << args[0]
                          << ShellColors::RESET << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << ShellColors::RED << "Error: " << e.what()
                      << ShellColors::RESET << std::endl;
        }
    }
}

const std::string &Shell::getCurrentPath() const
{
    return currentPath;
}

void Shell::setCurrentPath(const std::string &path)
{
    currentPath = path;
}

const std::map<std::string, std::unique_ptr<Command>> &Shell::getCommands() const
{
    return commands;
}