#include <Python.h>
#include <common\common.h>
static PyObject*
spam_system(PyObject *self, PyObject *args)
{
	const char *command;
	int sts;

	if( !PyArg_ParseTuple(args, "s", &command) )
		return nullptr;
	sts = ACE_OS::system(command);
	return Py_BuildValue("i", sts);
}
