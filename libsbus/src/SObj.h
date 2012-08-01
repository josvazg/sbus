/*
      SBUS library: Simple BUS communications library for distributed software developments
      Copyright (C) 2004  Jos� Luis V�zquez Gonz�lez <josvazg@terra.es>
      
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
/** SObj.cpp

 Objeto personalizable simple.
 
 Permite crear un objeto con un conjunto cualquiera de atributos, incluso con construcciones complejas.
 Es muy util para la creaci�n r�pida de nuevos mensajes a enviar por SBus.

  Jos� Luis V�quez Gonz�lez <josvazg@terra.es><br>
  LGPL
*/
#include <iostream.h>

using namespace std;

namespace simple {

class SObj {
  private:
  public:
  	/// Constructor por defecto
  	SObj();
	/// Destructor por defecto
	~Sobj();
	/// A�ade un tipo char con el valor dado
	int addChar(string name, char c);	
	/// A�ade un tipo short con el valor dado
	int addShort(string name, short s);	
	/// A�ade un tipo int con el valor dado
	int addInt(string name, int i);
	/// A�ade un tipo long long con el valor dado
	int addLongLong(string name, long long int ll);
	/// A�ade un tipo float con el valor dado
	int addFloat(string name, float f);	
	/// A�ade un tipo double con el valor dado
	int addDouble(string name, double d);
	/// A�ade un SObj con el valor dado
	int addSObj(string name, SObj sobj);	
	/// Solidifica el objeto, ya no se pueden a�adir m�s tipos
	int solidify();
	/// Obtiene un tipo char llamado 'name'
	char getChar(string name);
	/// Obtiene un tipo short llamado 'name'
	short addShort(string name);
	/// Obtiene un tipo int llamado 'name'
	int addInt(string name);
	/// Obtiene un tipo long long llamdo 'name'
	long long int addLongLong(string name);
	/// Obtiene un tipo float llamado 'name'
	float addFloat(string name);
	/// Obtiene un tipo double llamado 'name'
	double addDouble(string name);
	/// Obtiene un tipo sobj llamado 'name'
	SObj addSObj(string name);
};

}