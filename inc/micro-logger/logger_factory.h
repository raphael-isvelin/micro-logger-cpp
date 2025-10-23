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
#include <utility>
#include <memory>
#include <mutex>

#include "logger.h"

namespace ulog {

  class LogsObserver;

  enum LogLevel { DEBUG, INFO, WARNING, ERROR };

  class LoggerFactory {
   public:
    LoggerFactory(
      std::ostream* outputStream,
      const bool alwaysFlush,
      LogsObserver* callback,
      const std::string& appName,
      std::string debugTag, std::string infoTag, std::string warningTag, std::string errorTag,
      const bool useAnsiEscape,
      const int loggerNamePadding,
      const bool threadSafe
    ) : _outputStream(outputStream),
        _alwaysFlush(alwaysFlush),
        _callback(callback),
        _appName(appName),
        _formattedAppName(formatAppName(appName, useAnsiEscape)),
        _debugTag(std::move(debugTag)),
        _infoTag(std::move(infoTag)),
        _warningTag(std::move(warningTag)),
        _errorTag(std::move(errorTag)),
        _useAnsiEscape(useAnsiEscape),
        _loggerNamePadding(loggerNamePadding),
        _streamMutex(new std::mutex()),
        _callbackMutex(new std::mutex()),
        _threadSafe(threadSafe),
        logger(create("LoggerFactory", "\033[31;1m")) {
      if (!threadSafe) {
        logger.warning << "Thread safety is disabled";
      }
    }

    LoggerFactory(
      std::ostream* stream,
      const std::string& appName,
      LogsObserver* callback,
      const bool alwaysFlush,
      const bool useAnsiEscape = DEFAULT_USE_ANSI_ESCAPE,
      const int loggerNamePadding = DEFAULT_LOGGER_NAME_PADDING
    ) : LoggerFactory(
        stream, alwaysFlush,
        callback,
        appName,
        formatLogLevel(DEBUG, useAnsiEscape),
        formatLogLevel(INFO, useAnsiEscape),
        formatLogLevel(WARNING, useAnsiEscape),
        formatLogLevel(ERROR, useAnsiEscape),
        useAnsiEscape,
        loggerNamePadding,
        true) {
      // empty
    }

    LoggerFactory(std::ostream* stream, const std::string& appName, LogsObserver* callback)
        : LoggerFactory(stream, appName, callback, false,  true) {
      // empty
    }

    LoggerFactory(std::ostream* stream, const std::string& appName)
        : LoggerFactory(stream, appName, nullptr) {
      // empty
    }

    explicit LoggerFactory(std::ostream* stream)
        : LoggerFactory(stream, DEFAULT_APP_NAME) {
      // empty
    }

    static LoggerFactory factoryFrom(
      std::ostream* newOutputStream,
      const LoggerFactory& baseFactory,
      LogsObserver* newCallback
    ) {
      return {
        newOutputStream,
        baseFactory._alwaysFlush,
        newCallback,
        baseFactory._appName,
        baseFactory._debugTag, baseFactory._infoTag, baseFactory._warningTag, baseFactory._errorTag,
        baseFactory._useAnsiEscape,
        baseFactory._loggerNamePadding,
        baseFactory._threadSafe
      };
    }

//// Factory methods
    Logger create(const std::string& loggerName, const std::string& ansiEscape = "") const {
      const auto formattedLoggerName = formatLoggerName(loggerName, ansiEscape, _useAnsiEscape, _loggerNamePadding);
      return {
        *_outputStream,
        _alwaysFlush,
        _callback,
        _formattedAppName, formattedLoggerName,
        _debugTag, _infoTag, _warningTag, _errorTag,
        _threadSafe ? _streamMutex : nullptr,
        _threadSafe ? _callbackMutex : nullptr
      };
    }

    std::unique_ptr<Logger> createUnique(const std::string& loggerName, const std::string& ansiEscape = "") const {
      const auto formattedLoggerName = formatLoggerName(loggerName, ansiEscape, _useAnsiEscape, _loggerNamePadding);
      return std::unique_ptr<Logger>(new Logger(
        *_outputStream,
        _alwaysFlush,
        _callback,
        _formattedAppName, formattedLoggerName,
        _debugTag, _infoTag, _warningTag, _errorTag,
        _threadSafe ? _streamMutex : nullptr,
        _threadSafe ? _callbackMutex : nullptr
      ));
    }

//// Getter
    [[nodiscard]] std::ostream* outputStream() const {
      return _outputStream;
    }

    [[nodiscard]] LogsObserver* logsObserver() const {
      return _callback;
    }

//// Setter
    void outputStream(std::ostream* newStream) {
      _outputStream = newStream;
    }

    void loggerNamePadding(const int loggerNamePadding) {
      _loggerNamePadding = loggerNamePadding;
    }

    void threadSafe(const bool threadSafe) {
      _threadSafe = threadSafe;
      if (!threadSafe) {
        logger.warning << "Thread safety is disabled";
      }
    }

   private:
    std::ostream*   _outputStream;
    bool            _alwaysFlush;

    LogsObserver*   _callback;

    std::string     _appName;
    std::string     _formattedAppName;

    std::string     _debugTag;
    std::string     _infoTag;
    std::string     _warningTag;
    std::string     _errorTag;

    bool            _useAnsiEscape;
    int             _loggerNamePadding;

    std::mutex*     _streamMutex;
    std::mutex*     _callbackMutex;

    bool            _threadSafe;

    Logger          logger;

//// Formatting
    static constexpr auto* DEFAULT_APP_NAME = "";
    static constexpr bool DEFAULT_USE_ANSI_ESCAPE = true;
    static constexpr int DEFAULT_LOGGER_NAME_PADDING = 16;

    static std::string formatAppName(const std::string& appName, const bool useAnsiEscape) {
      if (appName.empty()) {
        return "";
      }
      return useAnsiEscape
        ? "\033[1m(" + appName + ")\033[0m "
        : "(" + appName + ") ";
    }

    static std::string formatLoggerName(
      const std::string& loggerName,
      const std::string& ansiEscape,
      const bool useAnsiEscape,
      const int loggerNamePadding
    ) {
      std::string paddedName = loggerName;
      if (loggerNamePadding > 0 && static_cast<int>(loggerName.length()) < loggerNamePadding) {
        paddedName = std::string(loggerNamePadding - loggerName.length(), ' ') + paddedName;
      }

      return (useAnsiEscape ? ansiEscape + "\033[1m" : "")
          + paddedName
          + (useAnsiEscape ? "\033[0m" : "");
    }

    static std::string formatLogLevel(const LogLevel level, const bool useAnsiEscape) {
      if (useAnsiEscape) {
        switch (level) {
          case DEBUG:   return   "\033[35mDEBUG  \033[0m";
          case INFO:    return   "\033[36mINFO   \033[0m";
          case WARNING: return "\033[33;1mWARNING\033[0m";
          case ERROR:   return "\033[31;1mERROR  \033[0m";
        }
      } else {
        switch (level) {
          case DEBUG:   return "DEBUG  ";
          case INFO:    return "INFO   ";
          case WARNING: return "WARNING";
          case ERROR:   return "ERROR  ";
        }
      }
      return "UNKNOWN"; // unreachable, but don't remove - needed with GCC -Werror -Werror
    }
  };

} // namespace ulog
