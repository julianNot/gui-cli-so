#pragma once
#include "../Command.hpp"
#include "../Shell.hpp"

class HelpCommand : public Command
{
private:
  Shell &shell;

public:
  explicit HelpCommand(Shell &shell);
  void execute(const std::vector<std::string> &args) override;
  std::string getHelp() const override;
  std::string getName() const override;
};