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

#include "log_message_builder.h"
#include "logs_observer.h"

class LogStream {
 public:
  LogStream(
      const std::string& appName,
      std::ostream& stream,
      std::string logType,
      std::string tag,
      LogsObserver* callback,
      bool alwaysFlush
  ) : _formattedAppName(appName.empty() ? "" : "\033[2m(" + appName + ")\033[0m "),
      _stream(stream),
      _logType(std::move(logType)),
      _tag(std::move(tag)),
      _callback(callback),
      _alwaysFlush(alwaysFlush) {}

  template <typename T>
  LogMessageBuilder operator<<(const T& message) const {
    auto builder = LogMessageBuilder(_formattedAppName, _stream, _logType, _tag, _callback, _alwaysFlush);
    builder << message;
    return builder;
  }

 private:
  std::string	    _formattedAppName;
  std::ostream&   _stream;
  std::string	    _logType;
  std::string	    _tag;
  LogsObserver*   _callback;
  bool            _alwaysFlush;
};