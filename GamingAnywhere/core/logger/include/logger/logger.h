/*
 Copyright (2016) Intel Corporation

 The source code contained or described herein and all documents related to
 the source code ("Material") are owned by Intel Corporation or its suppliers
 or licensors. Title to the Material remains with Intel Corporation or its
 suppliers and licensors. The Material contains trade secrets and proprietary
 and confidential information of Intel or its suppliers and licensors. The
 Material is protected by worldwide copyright and trade secret laws and treaty
 provisions. No part of the Material may be used, copied, reproduced, modified,
 published, uploaded, posted, transmitted, distributed, or disclosed in any way
 without Intel's prior express written permission.

 No license under any patent, copyright, trade secret or other intellectual
 property right is granted to or conferred upon you by disclosure or delivery
 of the Materials, either expressly, by implication, inducement, estoppel or
 otherwise. Any license under such intellectual property rights must be express
 and approved by Intel in writing.
 */

#pragma once

#include "igpa-config.h"

#include <cstdint>
#include <utility>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif  // __clang__

namespace gpa {
namespace logger {

enum Severity : uint32_t { kTrace = 0,
                           kDebug,
                           kInfo,
                           kWarn,
                           kError,
                           kFatal };

enum Module : uint32_t {
    kGeneric = 0x01,
    kPageTracker = 0x02,
};

enum {
    kMaxNameLength = 1024  // purely arbitrary
};

enum TimestampFormat : uint32_t { kNoTimestamp,
                                  kShortTimestamp,
                                  kFullTimestamp };

struct GPA_CLASS_EXPORT Options
{
    Options();
    Options(char const* appName);
    ~Options();

    char name[kMaxNameLength] = {};
    TimestampFormat timestampFormat = kNoTimestamp;
    Severity logLevel = kInfo;
    uint32_t showPid = 0;
    uint32_t showLevel = 0;
    uint32_t useColors = 0;
    uint32_t mask = kGeneric;
};

enum MessageState {
    // this is the whole message (append a newline, insert tags)
    kMessageComplete,
    // this is the beginning of a multi-part message (do not append a newline, insert tags)
    kMessageBegin,
    // this is a continuation of a multi-part message (do not append a newline, do not insert tags)
    kMessageContinue,
    // this is the end of a multipart message, or is the whole message (append a newline, do not
    // insert tags)
    kMessageEnd,
};

struct GPA_CLASS_EXPORT LogAppender
{
    virtual ~LogAppender();
    virtual void AppendMessage(Options const& options, Severity level, char const* message,
                               MessageState state = kMessageComplete) = 0;
};

GPA_EXPORT Severity SeverityFromString(char const* severityStr);
GPA_EXPORT uint32_t ModuleMaskFromString(char const* moduleStr);
GPA_EXPORT void LogMessage(Severity severity, uint32_t moduleMask, MessageState state,
                           char const* format, ...);
GPA_EXPORT void Init(Options const& logOptions);
GPA_EXPORT void AddLogAppender(LogAppender* appender);
GPA_EXPORT void RemoveLogAppender(LogAppender* appender);
GPA_EXPORT void Shutdown();

}  // namespace logger
}  // namespace gpa

namespace {

#if NDEBUG
template<typename... ArgsT>
void GPA_LOG_TRACE(char const*, ArgsT&&...)
{
}
#else
template<typename... ArgsT>
void GPA_LOG_TRACE(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kTrace, gpa::logger::kGeneric,
                            gpa::logger::kMessageComplete, format, std::forward<ArgsT>(args)...);
}
#endif

template<typename... ArgsT>
void GPA_LOG_DEBUG(uint32_t module, char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kDebug, module, gpa::logger::kMessageComplete, format,
                            std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_DEBUG(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kDebug, gpa::logger::kGeneric,
                            gpa::logger::kMessageComplete, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_INFO(uint32_t module, char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kInfo, module, gpa::logger::kMessageComplete, format,
                            std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_INFO(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kInfo, gpa::logger::kGeneric,
                            gpa::logger::kMessageComplete, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_WARN(uint32_t module, char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kWarn, module, gpa::logger::kMessageComplete, format,
                            std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_WARN(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kWarn, gpa::logger::kGeneric,
                            gpa::logger::kMessageComplete, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_ERROR(uint32_t module, char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kError, module, gpa::logger::kMessageComplete, format,
                            std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_ERROR(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kError, gpa::logger::kGeneric,
                            gpa::logger::kMessageComplete, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_FATAL(uint32_t module, char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kFatal, module, gpa::logger::kMessageComplete, format,
                            std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_FATAL(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kFatal, gpa::logger::kGeneric,
                            gpa::logger::kMessageComplete, format, std::forward<ArgsT>(args)...);
}

// "BEGIN" versions start a multi-part message

template<typename... ArgsT>
void GPA_LOG_TRACE_BEGIN(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kTrace, gpa::logger::kGeneric, gpa::logger::kMessageBegin,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_DEBUG_BEGIN(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kDebug, gpa::logger::kGeneric, gpa::logger::kMessageBegin,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_INFO_BEGIN(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kInfo, gpa::logger::kGeneric, gpa::logger::kMessageBegin,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_WARN_BEGIN(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kWarn, gpa::logger::kGeneric, gpa::logger::kMessageBegin,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_ERROR_BEGIN(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kError, gpa::logger::kGeneric, gpa::logger::kMessageBegin,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_FATAL_BEGIN(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kFatal, gpa::logger::kGeneric, gpa::logger::kMessageBegin,
                            format, std::forward<ArgsT>(args)...);
}

// "CONTINUE" versions continue a previously-begun multi-part message

template<typename... ArgsT>
void GPA_LOG_TRACE_CONTINUE(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kTrace, gpa::logger::kGeneric,
                            gpa::logger::kMessageContinue, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_DEBUG_CONTINUE(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kDebug, gpa::logger::kGeneric,
                            gpa::logger::kMessageContinue, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_INFO_CONTINUE(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kInfo, gpa::logger::kGeneric,
                            gpa::logger::kMessageContinue, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_WARN_CONTINUE(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kWarn, gpa::logger::kGeneric,
                            gpa::logger::kMessageContinue, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_ERROR_CONTINUE(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kError, gpa::logger::kGeneric,
                            gpa::logger::kMessageContinue, format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_FATAL_CONTINUE(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kFatal, gpa::logger::kGeneric,
                            gpa::logger::kMessageContinue, format, std::forward<ArgsT>(args)...);
}

// "END" versions end a previously-begun multi-part message

template<typename... ArgsT>
void GPA_LOG_TRACE_END(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kTrace, gpa::logger::kGeneric, gpa::logger::kMessageEnd,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_DEBUG_END(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kDebug, gpa::logger::kGeneric, gpa::logger::kMessageEnd,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_INFO_END(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kInfo, gpa::logger::kGeneric, gpa::logger::kMessageEnd,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_WARN_END(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kWarn, gpa::logger::kGeneric, gpa::logger::kMessageEnd,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_ERROR_END(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kError, gpa::logger::kGeneric, gpa::logger::kMessageEnd,
                            format, std::forward<ArgsT>(args)...);
}

template<typename... ArgsT>
void GPA_LOG_FATAL_END(char const* format, ArgsT&&... args)
{
    gpa::logger::LogMessage(gpa::logger::kFatal, gpa::logger::kGeneric, gpa::logger::kMessageEnd,
                            format, std::forward<ArgsT>(args)...);
}
}  // namespace
