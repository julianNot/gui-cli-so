#include "../../include/commands/ClearCommand.hpp"
#include <iostream>

void ClearCommand::execute(const std::vector<std::string> &args)
{
  // Usar secuencia ANSI para limpiar la pantalla y mover el cursor
  std::cout << "\033[2J\033[H";
}

std::string ClearCommand::getHelp() const
{
  return "clear  - Limpia la pantalla de la terminal";
}

std::string ClearCommand::getName() const
{
  return "clear";
}