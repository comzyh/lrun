////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012-2014 Jun Wu <quark@zju.edu.cn>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __linux
# error Linux required
#endif

#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif

#ifndef VERSION
# define VERSION "v1.0.1"
#endif

#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

extern double now();

#ifdef __cplusplus
}
#endif

#ifndef STDIN_FILENO
# define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
# define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif


#ifdef NDEBUG
# define SHOW_SOURCE_LOCATION ;
# define PRINT_TIMESTAMP ;
# define INFO(...) ;
# define PROGRESS_INFO(...) ;
# define DEBUG_DO if (0)
# define SCOPED_LOG_LOCK ;
#else
extern int DEBUG_ENABLED;
extern double DEBUG_START_TIME;
extern int DEBUG_TIMESTAMP;
extern int DEBUG_PROGRESS;
extern int DEBUG_PID;
extern FILE* flog;
struct ScopedLogLock {
    ScopedLogLock();
    ~ScopedLogLock();
    int fd_;
};

# define SCOPED_LOG_LOCK ScopedLogLock lock;
# define SHOW_SOURCE_LOCATION \
    if (DEBUG_ENABLED && flog) fprintf(flog, "  at %s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
# define PRINT_TIMESTAMP \
    {   \
      if (DEBUG_TIMESTAMP && flog) fprintf(flog, "[%8.3f]", TIMESTAMP); \
      if (DEBUG_PID && flog) fprintf(flog, "[%6lu] ", (unsigned long)getpid()); \
    }
# define INFO(...) \
    if (__builtin_expect(DEBUG_ENABLED && flog, 0)) { \
        ScopedLogLock lock;\
        PRINT_TIMESTAMP; \
        fprintf(flog, "INFO: "); \
        fprintf(flog, __VA_ARGS__); \
        fprintf(flog, "\n"); \
        fflush(flog); \
    }
# define PROGRESS_INFO(...) \
    if (__builtin_expect(DEBUG_PROGRESS && flog, 0)) { \
        SCOPED_LOG_LOCK; \
        fprintf(flog, __VA_ARGS__); \
        fprintf(flog, "        \r"); \
        fflush(flog); \
    }
# define DEBUG_DO if (DEBUG_ENABLED)
#endif


#define NOW now()
#define TIMESTAMP (now() - DEBUG_START_TIME)

#define FATAL(...) \
    { \
        SCOPED_LOG_LOCK; \
        PRINT_TIMESTAMP; \
        FILE* fp = flog ? flog : stderr; \
        fprintf(fp, "FATAL: "); \
        fprintf(fp, __VA_ARGS__); \
        if (errno) fprintf(fp ? fp : stderr, " (%s)", strerror(errno)); \
        fprintf(fp, "\n"); \
        SHOW_SOURCE_LOCATION; \
        fflush(fp); \
        exit(-1); \
    }

#define ERROR(...) \
    { \
        SCOPED_LOG_LOCK; \
        PRINT_TIMESTAMP; \
        FILE* fp = flog ? flog : stderr; \
        fprintf(fp, "ERROR: "); \
        fprintf(fp, __VA_ARGS__); \
        if (errno) fprintf(fp, " (%s)", strerror(errno)); \
        fprintf(fp, "\n"); \
        SHOW_SOURCE_LOCATION; \
        fflush(fp); \
    }

#define WARNING(...) \
    { \
        SCOPED_LOG_LOCK; \
        PRINT_TIMESTAMP; \
        FILE* fp = flog ? flog : stderr; \
        fprintf(fp, "WARNING: "); \
        fprintf(fp, __VA_ARGS__); \
        fprintf(fp, "\n"); \
        SHOW_SOURCE_LOCATION; \
        fflush(fp); \
    }

#define ensure_zero(exp) { if ((exp) != 0) FATAL("%s", "failed: " # exp); }

// old compiler does not like for (auto i : v)
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
# define FOR_EACH_CONST(i, v) \
    for (const auto& i : v)
# define FOR_EACH(i, v) \
    for (auto& i : v)
#else
# define VAR_CONCAT_(a, b) a ## b
# define VAR_CONCAT(a, b) VAR_CONCAT_(a ## _, b)
# define VAR_UNIQUE(a) VAR_CONCAT(a, __LINE__)
# define FOR_EACH(i, v) \
    __typeof(v.begin()) VAR_UNIQUE(_i) = v.begin(); \
    int VAR_UNIQUE(_fes) = 0; \
    for (; VAR_UNIQUE(_fes) = 1, VAR_UNIQUE(_i) != v.end(); ++VAR_UNIQUE(_i)) \
    for (__typeof(*(v.begin()))& i = *VAR_UNIQUE(_i); VAR_UNIQUE(_fes); VAR_UNIQUE(_fes) = 0)
# define FOR_EACH_CONST(i, v) \
    __typeof(v.begin()) VAR_UNIQUE(_i) = v.begin(); \
    int VAR_UNIQUE(_fes) = 0; \
    for (; VAR_UNIQUE(_fes) = 1, VAR_UNIQUE(_i) != v.end(); ++VAR_UNIQUE(_i)) \
    for (const __typeof(*(v.begin()))& i = *VAR_UNIQUE(_i); VAR_UNIQUE(_fes); VAR_UNIQUE(_fes) = 0)
#endif
