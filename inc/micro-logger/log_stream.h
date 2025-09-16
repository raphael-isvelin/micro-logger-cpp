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
#include <iomanip>

#include "log_message_builder.h"

class LogsObserver;

class LogStream {
 public:
  mutable long lastCalledAtSecondsSinceEpoch;
  mutable long callsCounter;

  LogStream(
    std::ostream& stream,
    const bool alwaysFlush,
    LogsObserver* callback,
    std::string formattedAppName,
    std::string formattedLogLevel,
    std::string formattedLoggerName,
    std::mutex* streamMutex,
    std::mutex* callbackMutex
  ) : lastCalledAtSecondsSinceEpoch(-1),
      callsCounter(0),
      _stream(stream),
      _alwaysFlush(alwaysFlush),
      _callback(callback),
      _formattedAppName(std::move(formattedAppName)),
      _formattedLogLevel(std::move(formattedLogLevel)),
      _formattedLoggerName(std::move(formattedLoggerName)),
      _streamMutex(streamMutex),
      _callbackMutex(callbackMutex) {
    // empty
  }

  template <typename T>
  LogMessageBuilder operator<<(const T& message) const {
    const auto now = getCurrentTime();
    lastCalledAtSecondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    ++callsCounter;

    const auto formattedTime = formatCurrentTime(now);

    auto builder = LogMessageBuilder(
      _stream,
      _alwaysFlush,
      _callback,
      formattedTime, _formattedAppName, _formattedLogLevel, _formattedLoggerName,
      _streamMutex, _callbackMutex
    );
    builder << message;
    return builder;
  }

 private:
  std::ostream&     _stream;
  bool              _alwaysFlush;

  LogsObserver*     _callback;

  std::string       _formattedAppName;
  std::string       _formattedLogLevel;
  std::string       _formattedLoggerName;

  std::mutex*       _streamMutex;
  std::mutex*       _callbackMutex;

//// Time
  static std::chrono::system_clock::time_point getCurrentTime() {
    return std::chrono::system_clock::now();
  }

  static std::string formatCurrentTime(std::chrono::system_clock::time_point now) {
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_ms - now_s);

    std::time_t time = std::chrono::system_clock::to_time_t(now);

    // Use thread-safe localtime_r if available, otherwise fall back to localtime with protection
    std::tm local_time_buf;
    std::tm* local_time;

#if defined(_WIN32)
    // Windows uses localtime_s
    if (localtime_s(&local_time_buf, &time) == 0) {
      local_time = &local_time_buf;
    } else {
      // Fallback to localtime (not thread-safe but better than crashing)
      local_time = std::localtime(&time);
    }
#elif defined(_POSIX_C_SOURCE) || defined(_GNU_SOURCE)
    // POSIX systems have localtime_r
    local_time = localtime_r(&time, &local_time_buf);
#else
    // Fallback: use a static mutex to protect localtime
    static std::mutex localtime_mutex;
    std::lock_guard<std::mutex> lock(localtime_mutex);
    std::tm* temp = std::localtime(&time);
    if (temp) {
      local_time_buf = *temp;
      local_time = &local_time_buf;
    } else {
      local_time = nullptr;
    }
#endif

    std::stringstream ss;
    if (local_time) {
      ss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0') << std::setw(3) << ms.count();
    } else {
      ss << "ERROR-TIME." << std::setfill('0') << std::setw(3) << ms.count();
    }
    return ss.str();
  }
};