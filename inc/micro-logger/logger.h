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

class Logger {
 public:
  static Logger create(std::ostream& stream, const std::string& tag) {
    return Logger(stream, tag);
  }

  static std::unique_ptr<Logger> createUnique(std::ostream& stream, const std::string& tag) {
    return std::unique_ptr<Logger>(new Logger(stream, tag));
  }

  std::ostream& rawOutputStream;

  LogStream info;
  LogStream warning;
  LogStream error;
  LogStream debug;

 private:
  Logger(std::ostream& stream, const std::string& tag)
      : rawOutputStream(stream),
        info(stream, "\033[36mINFO\033[0m   ", tag),
        warning(stream, "\033[33;1mWARNING\033[0m", tag),
        error(stream, "\033[31;1mERROR\033[0m  ", tag),
        debug(stream, "\033[36;2mDEBUG\033[0m  ", tag) {}
};