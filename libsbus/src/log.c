 /**@file log.h

  @brief Definiciones para trazas y registro de errores y mensajes de la aplicación

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#include <log.h>

/// Nivel actual de log
static LogLevelType logLevel=Info;

/// Descriptor del fichero de log
static int logfd=2;

/// Pone el nivel de log
void log_setLevel(LogLevelType level) {
  logLevel=level;
}
/// Recoge el nivel de log
LogLevelType log_getLevel() {
  return logLevel;
}
/// Pone el descriptor de fichero de salida
void log_setFD(int fd) {
  logfd=fd;
}
/// Trazas
void log_trace(LogLevelType thisLevel,const char *str,...) {
  if(thisLevel>=logLevel) { TRACE; }
}
/// Errores
void log_error(const char *str,...) {
  if(Error>=logLevel) { TRACELN; fsync(logfd); }
}
/// Avisos
void log_warn(const char *str,...) {
  if(Warn>=logLevel) { TRACELN; }
}
/// Info
void log_info(const char *str,...) {
  if(Info>=logLevel) { TRACELN; }
}
/// Debug
void log_debug(const char *str,...) {
  if(Debug>=logLevel) { TRACELN; }
}
