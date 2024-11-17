#include "../include/Shell.hpp"
#include <iostream>

int main()
{
  try
  {
    Shell shell;
    shell.run();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error fatal: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}