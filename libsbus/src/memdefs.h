/**@file memdefs.h

  @brief Wrappers para memoria din�mica en C

  Jos� Luis V�zquez Gonz�lez <josvazg@terra.es><br>
  LGPL
*/

// Evita algunos errores de memoria din�mica
#define NEWTYPE(type) \
(type*)calloc(sizeof(type),sizeof(char))

// Evita algunos errores de memoria din�mica
#define NEWARRAY(type,size) \
(type*)calloc(size,sizeof(type))

// Permite detectar errores al intentar reacceder a algo liberado
#define DELETE(freed) \
free(freed);freed=NULL

