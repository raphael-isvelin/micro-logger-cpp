#include <iostream>
#include <fstream>
#include <micro-logger/logger.h>

int main(void) {
  auto logger = Logger::create(std::cout, "My Class");
  logger.info << "I'm " << 27 << " years old!";
  logger.info << "Test " << 123.456 << " test";
  logger.warning << "This is a warning";

  std::ofstream file;
  file.open("test.logs", std::ios::out | std::ios::app);
  auto logger2 = Logger::create(file, "\033[35mA cooler class");
  logger2.error << "An error occurred";
  logger2.debug << "Debug information";

  return 0;
}

