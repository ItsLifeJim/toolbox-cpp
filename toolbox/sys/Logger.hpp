// The Reactive C++ Toolbox.
// Copyright (C) 2013-2019 Swirly Cloud Limited
// Copyright (C) 2021 Reactive Markets Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TOOLBOX_SYS_LOGGER_HPP
#define TOOLBOX_SYS_LOGGER_HPP

#include <toolbox/sys/Limits.hpp>

#include <toolbox/util/Storage.hpp>

namespace toolbox {
inline namespace sys {

class Logger;

enum class LogLevel : int {
    /// Critical.
    Crit,
    /// Error.
    Error,
    /// Warning.
    Warning,
    /// Notice.
    Notice,
    /// Information.
    Info,
    /// Debug.
    Debug
};

using LogMsgPtr = StoragePtr<MaxLogLine>;

/// Null logger. This logger does nothing and is effectively /dev/null.
TOOLBOX_API Logger& null_logger() noexcept;

/// Standard logger. This logger writes to stdout if the log level is greater than LogWarn, and
/// stdout otherwise.
TOOLBOX_API Logger& std_logger() noexcept;

/// System logger. This logger calls syslog().
TOOLBOX_API Logger& sys_logger() noexcept;

/// Return log label for given log level.
TOOLBOX_API const char* log_label(LogLevel level) noexcept;

/// Return current log level.
TOOLBOX_API LogLevel get_log_level() noexcept;

/// Return true if level is less than or equal to current log level.
inline bool is_log_level(LogLevel level) noexcept
{
    return level <= get_log_level();
}

/// Set log level globally for all threads.
TOOLBOX_API LogLevel set_log_level(LogLevel level) noexcept;

/// Return current logger.
TOOLBOX_API Logger& get_logger() noexcept;

/// Set logger globally for all threads.
TOOLBOX_API Logger& set_logger(Logger& logger) noexcept;

inline Logger& set_logger(std::nullptr_t) noexcept
{
    return set_logger(null_logger());
}

/// Unconditionally write log message to the logger. Specifically, this function does not check that
/// level is allowed by the current log level; users are expected to call is_log_level() first,
/// before formatting the log message.
TOOLBOX_API void write_log(LogLevel level, LogMsgPtr&& msg, std::size_t size) noexcept;

/// The Logger is implemented by types that may be woken-up, interrupted or otherwise notified
/// asynchronously.
class TOOLBOX_API Logger {
  public:
    Logger() noexcept = default;
    virtual ~Logger();

    // Copy.
    Logger(const Logger&) noexcept = default;
    Logger& operator=(const Logger&) noexcept = default;

    // Move.
    Logger(Logger&&) noexcept = default;
    Logger& operator=(Logger&&) noexcept = default;

    void write_log(LogLevel level, LogMsgPtr&& msg, std::size_t size) noexcept
    {
        do_write_log(level, std::move(msg), size);
    }

  protected:
    virtual void do_write_log(LogLevel level, LogMsgPtr&& msg, std::size_t size) noexcept = 0;
};

/// ScopedLogger provides a convenient RAII-style utility for setting the backend logger for the
/// duration of a scoped block.
class TOOLBOX_API ScopedLogger {
  public:
    explicit ScopedLogger(Logger& logger) noexcept
    : prev_{set_logger(logger)}
    {
    }
    ~ScopedLogger() { set_logger(prev_); }

    // Copy.
    ScopedLogger(const ScopedLogger&) = delete;
    ScopedLogger& operator=(const ScopedLogger&) = delete;

    // Move.
    ScopedLogger(ScopedLogger&&) = delete;
    ScopedLogger& operator=(ScopedLogger&&) = delete;

  private:
    Logger& prev_;
};

inline std::ostream& operator<<(std::ostream& os, LogLevel level)
{
    return os << log_label(level);
}

} // namespace sys
} // namespace toolbox

#endif // TOOLBOX_SYS_LOGGER_HPP
