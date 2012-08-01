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
/** @file hashdefs.h

     @brief Hash common definitions wrapper
*/
#include <string>

using namespace std;

/// Soporte de strings como claves de Hashmaps de STL
#define STL_HASHMAP_SUPPORT_STRING_KEYS_GCC3 \
namespace __gnu_cxx { \
  template<> struct hash<string> { \
    size_t operator()(const string& __s) const { return __stl_hash_string(__s.c_str()); } \
  }; \
}

/// Soporte de strings como claves de Hashmaps de STL
#define STL_HASHMAP_SUPPORT_STRING_KEYS_GCC29X \
template<> struct hash<string> { \
  size_t operator()(const string& __s) const { return __stl_hash_string(__s.c_str()); } \
}; \

#if __GNUC__ >= 3
  #include <ext/hash_map>
  STL_HASHMAP_SUPPORT_STRING_KEYS_GCC3;
  using namespace __gnu_cxx;
#else
  #include <hash_map>
  STL_HASHMAP_SUPPORT_STRING_KEYS_GCC29X;
#endif
