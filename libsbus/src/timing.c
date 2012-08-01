/* timing.c

  Rutinas de tratamiento de tiempo

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include <timing.h>

/**

  Devuelve el tiempo en segundos

  @return el tiempo transcurrido en segundos
 */
int timing_current_seconds() {
  struct timeval tv;
  if(gettimeofday(&tv,NULL)!=0) {
    return -1;
  }
  return (int)tv.tv_sec;
}

/**

  Devuelve el tiempo en milisegundos (sin incluir microsegundos)

  @return el tiempo transcurrido en milisegundos
 */
long long int timing_current_millis() {
  struct timeval tv;
  if(gettimeofday(&tv,NULL)!=0) {
    return -1;
  }
  return (long long int)((long long int)tv.tv_sec*(long long int)1000)+(long long int)(tv.tv_usec/1000);
}

/**
  Calcula la diferencia de tiempo en milisegundos
  (Solo sirve para tiempos pequeños, puede estar desborado si es muy grande)

  @param t1 es el tiempo menor en ms
  @param t2 es el tiempo mayor en ms
*/
int timing_elapsed_time(long long int t1, long long int t2) {
  return (int)(t2-t1);
}

/**
  Convierte una fecha time_t a una cadena de fecha ISO '%Y/%m/%d %H:%M:%S'

  @param date es puntero que va a contener ls fecha
  @param

  @return puntero a la fecha
 */
char* timing_time2isostr(char *date, time_t t) {
  struct tm *d;
  d=localtime(&t);
  strftime(date,50,"%Y/%m/%d %H:%M:%S",d);
  return date;
}

/**
  Muestra la fecha en formato ISO '%Y/%m/%d %H:%M:%S'

  @param date es puntero que va a contener ls fecha

  @return puntero a la fecha
 */
char* timing_isotime(char *date) {
  struct tm *d;
  time_t t=(time_t)timing_current_seconds();
  d=localtime(&t);
  strftime(date,50,"%Y/%m/%d %H:%M:%S",d);
  return date;
}

/**
  Muestra la fecha en formato ISO  comprimido '%Y%m%d%H%M%S'

  @param date es puntero que va a contener ls fecha

  @return puntero a la fecha
 */
char* timing_isotime_compressed(char *date) {
  struct tm *d;
  time_t t=(time_t)timing_current_seconds();
  d=localtime(&t);
  strftime(date,50,"%Y%m%d%H%M%S",d);
  return date;
}

/**
  Muestra la fecha en formato '%H:%M:%S %d/%b'

  @param date es puntero que va a contener ls fecha

  @return puntero a la fecha
 */
char* timing_time2str(char *date) {
  struct tm *d;
  time_t t=(time_t)timing_current_seconds();
  d=localtime(&t);
  strftime(date,50,"%H:%M:%S %d/%b",d);
  return date;
}

/**
  Convierte una fecha en formato ISO a un struct tm

  @param date es puntero que  contiiene la fecha
  @param d es el struct tm a rellanar con la fecha

  @return 0 en caso de éxito y -1 en caso de error
 */
int timing_isostr2tm(char *date, struct tm* d) {

  int sscc;
  if((sscc=sscanf(date,"%04d/%02d/%02d %02d:%02d:%02d",
	    &d->tm_year,&d->tm_mon,&d->tm_mday,
	    &d->tm_hour,&d->tm_min,&d->tm_sec))!=6)  {
    //printf("sscc=%d\n%04d/%02d/%02d %02d:%02d:%02d\n", sscc,
	    //d->tm_year,d->tm_mon,d->tm_mday,
	    //d->tm_hour,d->tm_min,d->tm_sec);

    return -1;
  }
  //  printf("sscc=%d\n%04d/%02d/%02d %02d:%02d:%02d\n", sscc,
	//    d->tm_year,d->tm_mon,d->tm_mday,
	//    d->tm_hour,d->tm_min,d->tm_sec);
  //d->tm_year+=1900;
  d->tm_year-=1900;
  d->tm_mon--;
  return 0;
}

/**
  Convierte una fecha en formato ISO a un time_t

  @param date es puntero que  contiiene la fecha

  @return la fecha pedida en form a time_t o -1 en caso de error
 */
int timing_isostr2time_t(char *date, time_t * t) {
  struct tm dd;
  if( timing_isostr2tm(date,&dd)<0 ) return -1;
  *t =mktime(&dd);
    //printf("timing_isostr2time_t: %04d/%02d/%02d %02d:%02d:%02d\n",
	//    dd.tm_year,dd.tm_mon,dd.tm_mday,
	//    dd.tm_hour,dd.tm_min,dd.tm_sec);
  return( *t<0 ? -1 : 0);
}
