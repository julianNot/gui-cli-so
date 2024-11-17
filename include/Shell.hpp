#pragma once
#include <map>
#include <memory>
#include <string>
#include "Command.hpp"

class Shell
{
private:
  std::map<std::string, std::unique_ptr<Command>> commands;
  std::string currentPath;
  std::string username;

  std::vector<std::string> splitCommand(const std::string &command);
  void initCommands();

public:
  Shell();
  void run();

  // Getters y setters
  const std::string &getCurrentPath() const;
  void setCurrentPath(const std::string &path);

  // Agregado: Método para acceder a los comandos
  const std::map<std::string, std::unique_ptr<Command>> &getCommands() const;
};