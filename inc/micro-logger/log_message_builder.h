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
#include <sstream>
#include <iomanip>
#include <chrono>

#include "logs_observer.h"

class LogMessageBuilder {
 public:
  LogMessageBuilder(
      std::string formattedAppName,
      std::ostream& stream,
      std::string logType,
      std::string tag,
      LogsObserver* callback,
      bool alwaysFlush
  ) : _formattedAppName(std::move(formattedAppName)), _stream(&stream),
      _logType(std::move(logType)), _tag(std::move(tag)),
      _callback(callback), _alwaysFlush(alwaysFlush), _ss() {}

  LogMessageBuilder(LogMessageBuilder&& other) noexcept
      : _formattedAppName(std::move(other._formattedAppName)),
        _stream(other._stream),
        _logType(std::move(other._logType)),
        _tag(std::move(other._tag)),
        _callback(other._callback),
        _alwaysFlush(other._alwaysFlush),
        _ss(std::move(other._ss)) {}

  LogMessageBuilder& operator=(LogMessageBuilder&& other) noexcept {
    if (this != &other) {
      _formattedAppName = std::move(other._formattedAppName);
      _stream = other._stream;
      _logType = std::move(other._logType);
      _tag = std::move(other._tag);
      _callback = other._callback;
      _alwaysFlush = other._alwaysFlush;
      _ss = std::move(other._ss);
    }
    return *this;
  }

  ~LogMessageBuilder() {
    std::stringstream log;
    log << _formattedAppName << getCurrentTime() << " | " << _logType << " | \033[1m" << _tag << "\033[0m | " << _ss.str() << std::endl;

    (*_stream) << log.str();
    if (_alwaysFlush) {
      _stream->flush();
    }
    if (_callback != nullptr) {
      _callback->onOutputLogMessage(log.str());
    }
  }

  template <typename T>
  LogMessageBuilder& operator<<(const T& message) {
    _ss << message;
    return *this;
  }

 private:
  std::string		      _formattedAppName;
  std::ostream*		    _stream;
  std::string		      _logType;
  std::string		      _tag;
  LogsObserver*       _callback;
  bool                _alwaysFlush;
  std::stringstream	  _ss;

  static std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_ms - now_s);

    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
  }
};
