#pragma once
#include "../Command.hpp"

class ExitCommand : public Command
{
public:
  void execute(const std::vector<std::string> &args) override;
  std::string getHelp() const override;
  std::string getName() const override;
};