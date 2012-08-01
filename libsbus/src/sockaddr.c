/*
      SBUS library: Simple BUS communcations library for distributed software developments
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
/** @file sockaddr.h

 @brief Funciones de manejo de las direcciones IP

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errdefs.h>

#include <sockaddr.h>

/**
  Obtención de la IP en forma de texto asociada a esta dirección
 */
char* sockaddr_getIPSTR(char *str, struct sockaddr_in* addr) {
  char *astr=inet_ntoa(addr->sin_addr);
  if(addr->sin_family != AF_INET) {
    sprintf(str,"Not AF_INET!");
    return str;
  }
  memcpy(str,astr,strlen(astr)+1);
  return str;
}

/**
  Conversión de ip numerica a IP en texto
 */
char* sockaddr_int2ip(char *str, int ip) {
  unsigned char *ptr=(unsigned char*)&ip;  
  sprintf(str,"%u.%u.%u.%u",ptr[3],ptr[2],ptr[1],ptr[0]);
  return str;  
}

/**
  Obtención del puerto asociado a esta dirección
 */
int sockaddr_getPort(struct sockaddr_in* addr) {
  return ntohs(addr->sin_port);
}


/**
  Obtención de la IP numérica asociada a esta dirección
 */
int sockaddr_getIP(struct sockaddr_in* addr) {
  return ntohl(addr->sin_addr.s_addr);
}

/**
 Inicia una direccion IP dadas la IP y el puerto
 */
int sockaddr_set(struct sockaddr_in* addr, const char *ip, int port) {
  bzero(addr, sizeof(struct sockaddr_in));
  addr->sin_family = AF_INET;
  RET_ON_FALSE(inet_aton(ip,&addr->sin_addr)!=0);
  addr->sin_port=htons(port);
  return 0;
}

/**
  Obtiene la MAC del dispositivo dado

  @param mac es la MAC del
  @param device es el nombre del dispositivo

  @return 0 si fue bien y -1 en caso de error
*/
int sockaddr_getMAC(macaddress mac, const char *device) {
  int fd;
  struct ifreq ifreq;

  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

  /* hardware address */
  strcpy( ifreq.ifr_name, device );
  RET_ON_PERROR(ioctl(fd, SIOCGIFHWADDR, &ifreq));
  memcpy((char*)mac,ifreq.ifr_hwaddr.sa_data,MAC_ADDR_LENGTH);
  return 0;
}

/**
  Devuelve el número de interfaces disponibles
*/
int sockaddr_interfaceCount() {
  int fd;
  struct ifconf ifc;
  struct ifreq  ifrs[ MAX_EXPECTED_INTERFACES ];
  int count;

  bzero(&ifc,sizeof(struct ifconf));
  bzero(ifrs,sizeof(struct ifreq) * MAX_EXPECTED_INTERFACES);  
  ifc.ifc_len = sizeof( ifrs );
  ifc.ifc_buf = (caddr_t)ifrs;  
  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  RET_ON_PERROR(ioctl(fd, SIOCGIFCONF, &ifc));
  close(fd);
  count=0;
  while(ifrs[count].ifr_name[0]!='\0') {
    count++;
  } 
  return count;
}

/**
  Devuelve las ips locales
*/
int sockaddr_ips(unsigned int *ips) {
  int fd;
  struct ifconf ifc;
  struct ifreq  ifrs[ MAX_EXPECTED_INTERFACES ];
  int count;

  bzero(&ifc,sizeof(struct ifconf));
  bzero(ifrs,sizeof(struct ifreq) * MAX_EXPECTED_INTERFACES);  
  ifc.ifc_len = sizeof( ifrs );
  ifc.ifc_buf = (caddr_t)ifrs;  
  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  RET_ON_PERROR(ioctl(fd, SIOCGIFCONF, &ifc));
  close(fd);
  count=0;
  while(ifrs[count].ifr_name[0]!='\0') {
    struct sockaddr_in* sai = (struct sockaddr_in*)&ifrs[count].ifr_addr;
    ips[count]=ntohl(sai->sin_addr.s_addr);
    count++;
  }    
  return count;
}

/**
  Obtiene la IP de red de un dispositivo dado en forma de entero

  @param device es el nombre del dispositivo / interfaz de red

  @return -1 en caso de error o la máscara
*/
int sockaddr_device2ip(const char *device) {
  int fd;
  struct ifreq ifreq;
  struct sockaddr_in* sai = (struct sockaddr_in*)&ifreq.ifr_addr;

  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

  /* ip */
  strcpy( ifreq.ifr_name, device );
  RET_ON_PERROR(ioctl(fd, SIOCGIFADDR, &ifreq));
  close(fd);
  return ntohl(sai->sin_addr.s_addr);
}

/**
  Obtiene la máscara de red de un dispositivo dado en forma de entero

  @param device es el nombre del dispositivo / interfaz de red

  @return -1 en caso de error o la máscara
*/
int sockaddr_getMask(const char *device) {
  int fd;
  struct ifreq ifreq;
  struct sockaddr_in* sai = (struct sockaddr_in*)&ifreq.ifr_addr;

  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

  /* netmask */
  strcpy( ifreq.ifr_name, device );
  RET_ON_PERROR(ioctl(fd, SIOCGIFNETMASK, &ifreq));
  close(fd);
  return ntohl(sai->sin_addr.s_addr);
}

/**
  Obtiene la dirección de broadcast

  @param device es el nombre del dispositivo / interfaz de red

  @return -1 en caso de error o la máscara
*/
int sockaddr_getBroadcast(const char *device) {
  int fd;
  struct ifreq ifreq;
  struct sockaddr_in* sai = (struct sockaddr_in*)&ifreq.ifr_addr;

  fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  /* netmask */
  strcpy( ifreq.ifr_name, device );
  RET_ON_PERROR(ioctl(fd, SIOCGIFBRDADDR, &ifreq));
  close(fd);
  return ntohl(sai->sin_addr.s_addr);
}


