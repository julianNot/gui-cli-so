#include "../../include/commands/MkdirCommand.hpp"
#include "../../include/Colors.hpp"
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <errno.h>

MkdirCommand::MkdirCommand(Shell &s) : shell(s) {}

void MkdirCommand::execute(const std::vector<std::string> &args)
{
  if (args.size() < 2)
  {
    std::cout << ShellColors::RED << "Error: Debe especificar el nombre del directorio"
              << ShellColors::RESET << std::endl;
    std::cout << "Uso: mkdir [-p] <nombre_directorio>" << std::endl;
    return;
  }

  bool createParents = false;
  std::string dirPath;

  if (args[1] == "-p")
  {
    if (args.size() < 3)
    {
      std::cout << ShellColors::RED << "Error: Debe especificar el nombre del directorio después de -p"
                << ShellColors::RESET << std::endl;
      return;
    }
    createParents = true;
    dirPath = args[2];
  }
  else
  {
    dirPath = args[1];
  }

  // Si la ruta es relativa, agregarla al directorio actual
  if (dirPath[0] != '/')
  {
    dirPath = shell.getCurrentPath() + "/" + dirPath;
  }

  if (createParents)
  {
    // Crear directorios recursivamente
    std::string currentDir;
    std::istringstream pathStream(dirPath);
    std::string token;

    // Dividir la ruta por '/'
    while (std::getline(pathStream, token, '/'))
    {
      if (token.empty())
        continue;

      if (currentDir.empty())
      {
        currentDir = "/";
      }
      currentDir += token + "/";

      // Intentar crear el directorio
      if (mkdir(currentDir.c_str(), 0755) != 0)
      {
        // Ignorar error si el directorio ya existe
        if (errno != EEXIST)
        {
          std::cout << ShellColors::RED << "Error al crear el directorio '" << currentDir
                    << "': " << strerror(errno) << ShellColors::RESET << std::endl;
          return;
        }
      }
      else
      {
        std::cout << ShellColors::GREEN << "Creado directorio: " << currentDir
                  << ShellColors::RESET << std::endl;
      }
    }
  }
  else
  {
    // Crear un solo directorio
    if (mkdir(dirPath.c_str(), 0755) != 0)
    {
      if (errno == EEXIST)
      {
        std::cout << ShellColors::YELLOW << "El directorio '" << dirPath
                  << "' ya existe" << ShellColors::RESET << std::endl;
      }
      else
      {
        std::cout << ShellColors::RED << "Error al crear el directorio '" << dirPath
                  << "': " << strerror(errno) << ShellColors::RESET << std::endl;
      }
    }
    else
    {
      std::cout << ShellColors::GREEN << "Creado directorio: " << dirPath
                << ShellColors::RESET << std::endl;
    }
  }
}

std::string MkdirCommand::getHelp() const
{
  return "mkdir [-p] <directorio>  - Crea un nuevo directorio\n"
         "  -p: Crea directorios padre si no existen\n"
         "Ejemplos:\n"
         "  mkdir mi_carpeta              - Crea una carpeta\n"
         "  mkdir -p ruta/a/mi/carpeta    - Crea carpetas anidadas";
}

std::string MkdirCommand::getName() const
{
  return "mkdir";
}