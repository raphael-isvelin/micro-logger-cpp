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
#include <utility>
#include <memory>

#include "logger.h"
#include "logs_observer.h"

class LoggerFactory {
 public:
  LoggerFactory(std::string appName, std::ostream* outputStream,
                std::string debugTag, std::string infoTag, std::string warningTag, std::string errorTag,
                LogsObserver* callback, bool alwaysFlush)
      : _appName(std::move(appName)),
        _outputStream(outputStream),
        _debugTag(std::move(debugTag)),
        _infoTag(std::move(infoTag)),
        _warningTag(std::move(warningTag)),
        _errorTag(std::move(errorTag)),
        _callback(callback),
        _alwaysFlush(alwaysFlush) {}

  LoggerFactory(const std::string& appName, std::ostream* stream, LogsObserver* callback, bool alwaysFlush)
      : LoggerFactory(appName, stream, Logger::DEFAULT_DEBUG_TAG, Logger::DEFAULT_INFO_TAG, Logger::DEFAULT_WARNING_TAG, Logger::DEFAULT_ERROR_TAG, callback, alwaysFlush) {}

  LoggerFactory(const std::string& appName, std::ostream* stream, LogsObserver* callback)
      : LoggerFactory(appName, stream, callback, false) {}

  LoggerFactory(const std::string& appName, std::ostream* stream)
      : LoggerFactory(appName, stream, nullptr) {}

  explicit LoggerFactory(std::ostream* stream)
      : LoggerFactory(Logger::DEFAULT_APP_NAME, stream) {}

  static LoggerFactory factoryFrom(LoggerFactory& baseFactory, std::ostream* newOutputStream, LogsObserver* newCallback) {
    return LoggerFactory(baseFactory._appName, newOutputStream,
                         baseFactory._debugTag, baseFactory._infoTag, baseFactory._warningTag, baseFactory._errorTag,
                         newCallback, baseFactory._alwaysFlush);
  }

//// Factory methods
  Logger create(const std::string& tag) {
    return Logger(_appName, *_outputStream, tag, _debugTag, _infoTag, _warningTag, _errorTag, _callback, _alwaysFlush);
  }

  std::unique_ptr<Logger> createUnique(const std::string& tag) {
    return std::unique_ptr<Logger>(new Logger(_appName, *_outputStream, tag, _debugTag, _infoTag, _warningTag, _errorTag, _callback, _alwaysFlush));
  }

//// Getter
  void outputStream(std::ostream* newStream) {
    _outputStream = newStream;
  }

  [[nodiscard]] LogsObserver* logsObserver() const {
    return _callback;
  }

 private:
  std::string	    _appName;
  std::ostream*   _outputStream;
  std::string	    _debugTag;
  std::string	    _infoTag;
  std::string	    _warningTag;
  std::string	    _errorTag;
  LogsObserver*   _callback;
  bool            _alwaysFlush;
};
