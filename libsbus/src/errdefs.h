 /**@file errdefs.h

  @brief Tipos de datos comunes de errores

  Jos� Luis V�zquez Gonz�lez <josvazg@terra.es><br>
  LGPL
*/
#include <errno.h>

#include <log.h>

#ifndef ERRDEFS
#define ERRDEFS

/// Macro de localizaci�n de errores
#define ASSERT(condition) \
if(!(condition)) {\
    ERROR("Check " #condition " failed\n");\
    exit(-1);\
} 

/// Macro de localizaci�n y visualizaci�n de c�digos de error
#define PERROR LONG_PREFIX(Error);log_trace(Error,"%s\n",strerror(errno));ERROR

/// Muestra un error si la expresi�n no es cero
#define TRY(expression) \
if(expression!=0) { \
  ERROR(#expression " failed"); \
}

/// Muestra un error si la expresi�n no es cero y sale de la funci�n
#define RET_ON_ERROR(expression) \
if(expression<0) { \
  ERROR(const_cast<char*>(#expression " failed")); \
  return -1; \
}

/// Muestra un perror si la expresi�n no es cero y sale de la funci�n
#define RET_ON_PERROR(expression) \
if(expression<0) { \
  PERROR(#expression " failed"); \
  return -1; \
}

/// Muestra un error si la expresi�n no es cero y sale termina
#define TRY_EXIT(expression) \
if(expression<0) { \
  ERROR("Fatal " #expression " failed"); \
  exit(-1); \
}

/// Muestra un error si la expresi�n no es cero y sale de la funci�n
#define TRY_VAL_RET(expression,val) \
if(expression!=val) { \
  ERROR(#expression " failed"); \
  return -1; \
}

/// Muestra un error si la expresi�n no es cero y sale termina
#define TRY_VAL_EXIT(expression,val) \
if(expression!=val) { \
  ERROR("Fatal " #expression " failed"); \
  exit(-1); \
}

/// Retorna de la funci�n con fallo si la expresi�n es nula
#define RET_ON_FALSE(expression) \
if(!expression) { \
  ERROR(#expression " is FALSE"); \
  return -1;\
}

/// Retorna de la funci�n con fallo si la expresi�n es nula
#define RET_NULL_ON_FALSE(expression) \
if(!expression) { \
  ERROR(#expression " is FALSE"); \
  return NULL;\
}

/// Retorna de la funci�n con fallo si la expresi�n es nula
#define RET_NULL_ON_ERROR(expression) \
if(expression<0) { \
  ERROR(#expression " failed"); \
  return NULL;\
}

#endif
