#include "../../include/commands/ExitCommand.hpp"
#include "../../include/Colors.hpp"
#include <iostream>

void ExitCommand::execute(const std::vector<std::string> &args)
{
  std::cout << ShellColors::GREEN << "¡Hasta luego!" << ShellColors::RESET << std::endl;
  exit(0);
}

std::string ExitCommand::getHelp() const
{
  return "exit  - Salir de la shell";
}

std::string ExitCommand::getName() const
{
  return "exit";
}