/*******************************************************************************\

micro-logger-cpp - Header-only C++ logging lib using streams

https://github.com/raphael-isvelin/micro-logger-cpp

--------------------------------------------------------------------------------

License: MIT License (http://www.opensource.org/licenses/mit-license.php)
Copyright (C) 2025 Raphaël Isvelin

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
#include <mutex>

#include "logs_observer.h"

namespace ulog {

  class LogMessageBuilder {
  public:
    LogMessageBuilder(
      std::ostream& stream,
      const bool alwaysFlush,
      LogsObserver* callback,
      std::string formattedTime,
      std::string formattedAppName,
      std::string formattedLogLevel,
      std::string formattedLoggerName,
      std::mutex* streamMutex,
      std::mutex* callbackMutex
    ) : _stream(&stream),
        _alwaysFlush(alwaysFlush),
        _callback(callback),
        _formattedTime(std::move(formattedTime)),
        _formattedAppName(std::move(formattedAppName)),
        _formattedLogLevel(std::move(formattedLogLevel)),
        _formattedLoggerName(std::move(formattedLoggerName)),
        _streamMutex(streamMutex),
        _callbackMutex(callbackMutex) {
      // empty
    }

    LogMessageBuilder(LogMessageBuilder&& other) noexcept
        : _stream(other._stream),
          _alwaysFlush(other._alwaysFlush),
          _callback(other._callback),
          _formattedTime(std::move(other._formattedTime)),
          _formattedAppName(std::move(other._formattedAppName)),
          _formattedLogLevel(std::move(other._formattedLogLevel)),
          _formattedLoggerName(std::move(other._formattedLoggerName)),
          _ss(std::move(other._ss)),
          _streamMutex(other._streamMutex),
          _callbackMutex(other._callbackMutex) {}

    LogMessageBuilder& operator=(LogMessageBuilder&& other) noexcept {
      if (this != &other) {
        _stream = other._stream;
        _alwaysFlush = other._alwaysFlush;
        _callback = other._callback;
        _formattedTime = std::move(other._formattedTime);
        _formattedAppName = std::move(other._formattedAppName);
        _formattedLogLevel = std::move(other._formattedLogLevel);
        _formattedLoggerName = std::move(other._formattedLoggerName);
        _ss = std::move(other._ss);
        _streamMutex = other._streamMutex;
        _callbackMutex = other._callbackMutex;
      }
      return *this;
    }

    ~LogMessageBuilder() {
      std::stringstream log;
      log << _formattedAppName << _formattedTime << " | " << _formattedLogLevel << " | " << _formattedLoggerName << " | " << _ss.str() << std::endl;
      const std::string logMessage = log.str();

      if (_streamMutex != nullptr) {
        std::lock_guard<std::mutex> lock(*_streamMutex);
        (*_stream) << logMessage;
        if (_alwaysFlush) {
          _stream->flush();
        }
      } else {
        (*_stream) << logMessage;
        if (_alwaysFlush) {
          _stream->flush();
        }
      }

      if (_callback != nullptr) {
        if (_callbackMutex != nullptr) {
          std::lock_guard<std::mutex> lock(*_callbackMutex);
          _callback->onOutputLogMessage(logMessage);
        } else {
          _callback->onOutputLogMessage(logMessage);
        }
      }
    }

    template <typename T>
    LogMessageBuilder& operator<<(const T& message) {
      _ss << message;
      return *this;
    }

  private:
    std::ostream*     _stream;
    bool              _alwaysFlush;

    LogsObserver*     _callback;

    std::string       _formattedTime;
    std::string       _formattedAppName;
    std::string       _formattedLogLevel;
    std::string       _formattedLoggerName;

    std::stringstream _ss;

    std::mutex*       _streamMutex;
    std::mutex*       _callbackMutex;
  };

} // namespace ulog
