#pragma once
#include "../Command.hpp"
#include "../Shell.hpp"

class CdCommand : public Command
{
private:
  Shell &shell;

public:
  explicit CdCommand(Shell &shell);
  void execute(const std::vector<std::string> &args) override;
  std::string getHelp() const override;
  std::string getName() const override;
};