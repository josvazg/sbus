/**@file memdefs.h

  @brief Wrappers para memoria dinámica en C

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/

// Evita algunos errores de memoria dinámica
#define NEWTYPE(type) \
(type*)calloc(sizeof(type),sizeof(char))

// Evita algunos errores de memoria dinámica
#define NEWARRAY(type,size) \
(type*)calloc(size,sizeof(type))

// Permite detectar errores al intentar reacceder a algo liberado
#define DELETE(freed) \
free(freed);freed=NULL

