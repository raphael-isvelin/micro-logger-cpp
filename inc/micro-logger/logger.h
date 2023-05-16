/*******************************************************************************\

micro-logger-cpp - Header-only C++ logging lib using streams

https://github.com/raphael-isvelin/micro-logger-cpp

--------------------------------------------------------------------------------

License: MIT License (http://www.opensource.org/licenses/mit-license.php)
Copyright (C) 2023 Raphaël Isvelin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*******************************************************************************/

#pragma once

#include <string>
#include <ostream>

#include "log_stream.h"
#include "logs_observer.h"

class Logger {
 public:
  static Logger create(std::ostream& stream, const std::string& tag) {
    return Logger(stream, tag);
  }

  static std::unique_ptr<Logger> createUnique(std::ostream& stream, const std::string& tag) {
    return std::unique_ptr<Logger>(new Logger(stream, tag));
  }

  std::ostream& rawOutputStream;

  LogStream debug;
  LogStream info;
  LogStream warning;
  LogStream error;

  friend class LoggerFactory;

 protected:
  static constexpr const char* DEFAULT_APP_NAME = "";
  static constexpr const char* DEFAULT_DEBUG_TAG = "\033[36mINFO\033[0m   ";
  static constexpr const char* DEFAULT_INFO_TAG = "\033[36mINFO\033[0m   ";
  static constexpr const char* DEFAULT_WARNING_TAG = "\033[33;1mWARNING\033[0m";
  static constexpr const char* DEFAULT_ERROR_TAG = "\033[31;1mERROR\033[0m  ";

  Logger(const std::string& appName, std::ostream& stream, const std::string& tag,
         const std::string& debugTag, const std::string& infoTag, const std::string& warningTag, const std::string& errorTag,
         LogsObserver* callback)
      : rawOutputStream(stream),
        debug(appName, stream, debugTag, tag, callback),
        info(appName, stream, infoTag, tag, callback),
        warning(appName, stream, warningTag, tag, callback),
        error(appName, stream, errorTag, tag, callback) {}

 private:
  Logger(std::ostream& stream, const std::string& tag)
      : Logger(DEFAULT_APP_NAME, stream, tag, DEFAULT_DEBUG_TAG, DEFAULT_INFO_TAG, DEFAULT_WARNING_TAG, DEFAULT_ERROR_TAG, nullptr) {}
};