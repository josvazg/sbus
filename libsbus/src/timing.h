/* @file timing.h

  Rutinas de tratamiento de tiempo

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIMING_ISODATE_SIZE 20

/**

  Devuelve el tiempo en segundos

  @return el tiempo transcurrido en segundos
 */
int timing_current_seconds();

/**

  Devuelve el tiempo en milisegundos

  @return el tiempo transcurrido en milisegundos
 */
long long int timing_current_millis();

/**
  Calcula la diferencia de tiempo en milisegundos
  (Solo sirve para tiempos pequeños, puede estar desborado si es muy grande)

  @param t1 es el tiempo menor en ms
  @param t2 es el tiempo mayor en ms
*/
int timing_elapsed_time(long long int t1, long long int t2);

/**
  Convierte una fecha time_t a una cadena de fecha ISO '%Y/%m/%d %H:%M:%S'

  @param date es puntero que va a contener ls fecha
  @param

  @return puntero a la fecha
 */
char* timing_time2isostr(char *date, time_t t);

/**
  Muestra la fecha en formato ISO '%Y/%m/%d %H:%M:%S'

  @param date es puntero que va a contener ls fecha

  @return puntero a la fecha
 */
char* timing_isotime(char *date);

/**
  Muestra la fecha en formato ISO  comprimido '%Y%m%d%H%M%S'

  @param date es puntero que va a contener ls fecha

  @return puntero a la fecha
 */
char* timing_isotime_compressed(char *date);

/**
  Muestra la fecha en formato '%H:%M:%S %d/%b'

  @param date es puntero que va a contener ls fecha

  @return puntero a la fecha
 */
char* timing_time2str(char *date);

/**
  Convierte una fecha en formato ISO a un struct tm

  @param date es puntero que  contiiene la fecha
  @param d es el struct tm a rellanar con la fecha

  @return 0 en caso de éxito y -1 en caso de error
 */
int timing_isostr2tm(char *date, struct tm* d);

/**
  Convierte una fecha en formato ISO a un time_t

  @param date es puntero que contiene la fecha en formato cadena
  @param t es el time_t a rellena con la fecha

  @return 0 en caso de éxito y -1 en caso de error
 */
int timing_isostr2time_t(char *date, time_t * t);

/**
  Convierte una fecha en formato ISO a un time_t

  @param date es puntero al string donde escribir la fecha formateada
  @param t es el time_t con la fecha a formatear

  @return puntero al string con la fecha formateada
 */
char* timing_time_t2isostr(char *date, time_t t);

#ifdef __cplusplus
}
#endif
