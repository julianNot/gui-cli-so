#pragma once
#include "../Command.hpp"

class LsCommand : public Command
{
public:
  void execute(const std::vector<std::string> &args) override;
  std::string getHelp() const override;
  std::string getName() const override;

private:
  void listDirectory(const std::string &path, bool showHidden);
};