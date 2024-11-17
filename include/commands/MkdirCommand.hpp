#pragma once
#include "../Command.hpp"
#include "../Shell.hpp"

class MkdirCommand : public Command
{
private:
  Shell &shell;

public:
  explicit MkdirCommand(Shell &shell);
  void execute(const std::vector<std::string> &args) override;
  std::string getHelp() const override;
  std::string getName() const override;
};