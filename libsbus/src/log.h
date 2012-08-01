/*
      SBUS library: 
      Simple BUS communcations library for distributed software developments
      Copyright (C) 2004  José Luis Vázquez González <josvazg@terra.es>

      This library is free software; you can redistribute it and/or modify it
      under the terms of the GNU Lesser General Public License as published by
      the Free Software Foundation; either version 2.1 of the License, or (at
      your option) any later version.

      This library is distributed in the hope that it will be useful, but
      WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
      Lesser General Public License for more details.

      You should have received a copy of the GNU Lesser General Public
      License along with this library; if not, write to the Free Software
      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
      USA.
*/
 /**@file log.h

  @brief Definitions for logs, traces and debugging

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <stdio.h>

#include <timing.h>

#ifndef LOG
#define LOG

/// Log levels
typedef enum { Debug, Info, Warn, Error } LogLevelType;

#define THROW(MSG, ...) { \
  char datetxt[MAX_LOG_BUF]; \
  char msg[MAX_LOG_BUF]; \
  char line[MAX_LOG_BUF*3]; \
  sprintf(msg,MSG,__VA_ARGS__); \
  sprintf(line,"[%s %s %s():%d Excep] %s\n", \
    timing_time2str(datetxt),__FILE__,__FUNCTION__,__LINE__,msg); \
  throw new string(line); \
}

/// Max log buffer
#define MAX_LOG_BUF 512

/// Log prefix
#define PREFIX(level) { \
char datetxt[MAX_LOG_BUF]; \
log_trace(level,"[%s " #level "] ",timing_time2str(datetxt)); \
}

/// Long log prefix
#define LONG_PREFIX(level) { \
}
/*char datetxt[MAX_LOG_BUF]; \
log_trace(level,"[%s %s %s():%d ] ",\
timing_time2str(const_cast<char *>(datetxt)),__FILE__,__FUNCTION__,__LINE__); \
}*/

/// Macro de errores
#define ERROR LONG_PREFIX(Error);log_error

/// Macro de avisos
#define WARN LONG_PREFIX(Warn);log_warn

/// Macro de informaciónes
#define INFO PREFIX(Info);log_info

/// Macro de trazas de depuración
#define DEBUG PREFIX(Debug);log_debug 

/// Macro del log
#define TRACE va_list argp;\
int w;\
char buf[MAX_LOG_BUF];\
va_start(argp,str);\
vsprintf(buf,str,argp);\
w=write(logfd,buf,strlen(buf));\
va_end(argp);\

/// Macro de escritura de linea de log
#define TRACELN va_list argp;\
int w;\
char buf[MAX_LOG_BUF];\
va_start(argp,str);\
vsprintf(buf,const_cast<char*>(str),argp);\
w=write(logfd,buf,strlen(buf));\
va_end(argp);\
w=write(logfd,"\n",1);\

#ifdef __cplusplus
extern "C" {
#endif

/// Pone el nivel de log
void log_setLevel(LogLevelType level);
/// Recoge el nivel de log
LogLevelType log_getLevel();
/// Pone el descriptor de fichero de salida
void log_setFD(int fd);
/// Trazas
void log_trace(LogLevelType thisLevel,const char *str,...);
/// Errores
void log_error(const char *str,...);
/// Avisos
void log_warn(const char *str,...);
/// Info
void log_info(const char *str,...);
/// Debug
void log_debug(const char *str,...);

#ifdef __cplusplus
}
#endif

#endif
