#pragma once
#include <vector>
#include <string>

class Command
{
public:
  virtual ~Command() = default;
  virtual void execute(const std::vector<std::string> &args) = 0;
  virtual std::string getHelp() const = 0;
  virtual std::string getName() const = 0;
};