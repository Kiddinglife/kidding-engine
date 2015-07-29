/*
* test.cpp
*  Created on: 2010-8-12
*      Author: lihaibo
*/

/// Since Python may define some pre - processor definitions which affect the standard headers
/// on some systems, you must include Python.h before any standard headers are included.
#include "Python.h"

#include <iostream>
#include <string>

static PyObject* spam_error;
/// The self argument points to the module object for module - level functions; 
/// for a method it would point to the object instance.
static PyObject* spam_system(PyObject* self, PyObject* args)
{
	int sts;
	const char * command;
	if( !PyArg_ParseTuple(args, "s", &command) ) return NULL;
	sts = system(command);
	if( sts < 0 ) PyErr_SetString(spam_error, "system call fails");
	return PyLong_FromLong(sts);
}


static PyMethodDef SpamMethods[ ] = {
	{
		"system",
		spam_system,
		METH_VARARGS,
		"Execute a shell command."
	},

	{ NULL, NULL, 0, NULL } /* Sentinel */
};

static struct PyModuleDef spammodule = {
	PyModuleDef_HEAD_INIT,
	"spam", /* name of module */
	"spam_doc",  /* module documentation, may be NULL */
	-1, /* size of per-interpreter state of the module,or -1 if the module keeps state in global variables. */
	SpamMethods
};

PyMODINIT_FUNC PyInit_spam(void)
{
	PyObject* m = PyModule_Create(&spammodule);
	if( m == NULL ) return NULL;

	spam_error = PyErr_NewException("spam_module.error", NULL, NULL);
	if( spam_error == NULL ) return NULL;
	Py_XINCREF(spam_error);

	if( !PyModule_AddObject(m, "error", spam_error) ) return NULL;
	return m;
}

int main(int argc, char *argv[ ])
{
	/* Add a built-in module, before Py_Initialize */
	PyImport_AppendInittab("spam", PyInit_spam);

	/* Initialize the Python interpreter. Required. */
	Py_Initialize();

	/* Optionally import the module; alternatively,
	import can be deferred until the embedded script
	imports it. */
	PyImport_ImportModule("spam");

	return 0;
}