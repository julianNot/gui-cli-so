#include "../../include/commands/LsCommand.hpp"
#include "../../include/Colors.hpp"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>

void LsCommand::execute(const std::vector<std::string> &args)
{
  std::string path = ".";
  bool showHidden = false;

  // Procesar argumentos
  for (size_t i = 1; i < args.size(); ++i)
  {
    if (args[i] == "-a")
    {
      showHidden = true;
    }
    else
    {
      path = args[i];
    }
  }

  listDirectory(path, showHidden);
}

void LsCommand::listDirectory(const std::string &path, bool showHidden)
{
  DIR *dir = opendir(path.c_str());
  if (dir == nullptr)
  {
    std::cout << ShellColors::RED << "Error al abrir el directorio '" << path
              << "': " << strerror(errno) << ShellColors::RESET << std::endl;
    return;
  }

  std::vector<std::string> entries;
  struct dirent *entry;

  while ((entry = readdir(dir)) != nullptr)
  {
    std::string name = entry->d_name;

    // Saltar archivos ocultos si no se solicitan
    if (!showHidden && name[0] == '.')
    {
      continue;
    }

    entries.push_back(name);
  }
  closedir(dir);

  // Ordenar entradas alfabéticamente
  std::sort(entries.begin(), entries.end());

  // Calcular el ancho de columna óptimo
  size_t maxWidth = 0;
  for (const auto &name : entries)
  {
    maxWidth = std::max(maxWidth, name.length());
  }
  maxWidth += 2; // Espacio entre columnas

  // Calcular número de columnas basado en el ancho de terminal
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int termWidth = w.ws_col;
  int numCols = std::max(1, termWidth / static_cast<int>(maxWidth));

  // Mostrar entradas en columnas
  int col = 0;
  for (const auto &name : entries)
  {
    std::cout << std::left << std::setw(maxWidth) << name;
    if (++col >= numCols)
    {
      std::cout << std::endl;
      col = 0;
    }
  }
  if (col != 0)
  {
    std::cout << std::endl;
  }
}

std::string LsCommand::getHelp() const
{
  return "ls [-a] [directorio]  - Lista el contenido del directorio\n"
         "  -a: Muestra archivos ocultos\n"
         "Ejemplos:\n"
         "  ls         - Lista el directorio actual\n"
         "  ls -a      - Lista incluyendo archivos ocultos\n"
         "  ls /ruta   - Lista el directorio especificado";
}

std::string LsCommand::getName() const
{
  return "ls";
}