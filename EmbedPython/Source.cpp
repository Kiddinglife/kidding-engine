/*
* test.cpp
*  Created on: 2010-8-12
*      Author: lihaibo
*/
#include <iostream>
#include <string>
#include "Python.h"

int main()
{
	Py_Initialize();
	Py_Finalize();
	return 0;
}

