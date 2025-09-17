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

#include "log_stream.h"

namespace ulog {

  class LogsObserver;

  class Logger {
   public:
    std::ostream& rawOutputStream;

    LogStream debug;
    LogStream info;
    LogStream warning;
    LogStream error;

    friend class LoggerFactory;

//// Setter
    void formattedAppName(const std::string& formattedAppName) {
      debug.formattedAppName(formattedAppName);
      info.formattedAppName(formattedAppName);
      warning.formattedAppName(formattedAppName);
      error.formattedAppName(formattedAppName);
    }

   protected:
    Logger(
      std::ostream& stream,
      const bool alwaysFlush,
      LogsObserver* callback,
      const std::string& formattedAppName, const std::string& formattedLoggerName,
      const std::string& debugTag, const std::string& infoTag, const std::string& warningTag, const std::string& errorTag,
      std::mutex* streamMutex, std::mutex* callbackMutex
    ) : rawOutputStream(stream),
        debug(stream, alwaysFlush, callback, formattedAppName, debugTag, formattedLoggerName, streamMutex, callbackMutex),
        info(stream, alwaysFlush, callback, formattedAppName, infoTag, formattedLoggerName, streamMutex, callbackMutex),
        warning(stream, alwaysFlush, callback, formattedAppName, warningTag, formattedLoggerName, streamMutex, callbackMutex),
        error(stream, alwaysFlush, callback, formattedAppName, errorTag, formattedLoggerName, streamMutex, callbackMutex) {
      // empty
    }
  };

} // namespace ulog
