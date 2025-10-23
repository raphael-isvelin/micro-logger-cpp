#include <iostream>
#include <fstream>
#include <micro-logger/logger_factory.h>

int main(void) {
  ulog::LoggerFactory loggerFactory(&std::cout);
  loggerFactory.loggerNamePadding(-1);
  const auto logger = loggerFactory.create("My Class");
  logger.info << "I'm " << 27 << " years old!";
  logger.info << "Test " << 123.456 << " test";
  logger.warning << "This is a warning";

  std::ofstream file;
  file.open("test.logs", std::ios::out | std::ios::app);
  ulog::LoggerFactory loggerFactory2(&file);
  const auto logger2 = loggerFactory2.create(
    "A cooler class", "\033[35m");
  logger2.error << "An error occurred";
  logger2.debug << "Debug information";

  return 0;
}

