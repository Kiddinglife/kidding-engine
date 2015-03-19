
/**
// You can also define a new exception that is unique to your module.
// For this, you usually declare a static object variable at the beginning of your file:
static PyObject *SpamError;


// and initialize it in your module's initialization function (initspam())
// with an exception object (leaving out the error checking for now):
PyMODINIT_FUNC initspam(void)
{
PyObject *m;

m = Py_InitModule("spam", SpamMethods);
if (m == NULL)
return;

SpamError = PyErr_NewException("spam.error", NULL, NULL);
Py_INCREF(SpamError);
PyModule_AddObject(m, "error", SpamError);
}


// Errors and Exceptions
// The spam.error exception can be raised in your extension module
// using a call to PyErr_SetString() as shown below:
static PyObject* spam_system(PyObject *self, PyObject *args)
{
const char *command;
int sts;

if (!PyArg_ParseTuple(args, "s", &command))
return NULL;
sts = system(command);
if (sts < 0) {
PyErr_SetString(SpamError, "System command failed");
return NULL;
}
return PyLong_FromLong(sts);
}

// If you have a C function that returns no useful argument (a function returning void),
// the corresponding Python function must return None. You need this idiom to do so
// (which is implemented by the Py_RETURN_NONE macro):

Py_INCREF(Py_None);
return Py_None;

//Py_None is the C name for the special Python object None.
// It is a genuine Python object rather than a NULL pointer,
// which means ¡°error¡± in most contexts, as we have seen.


// The Module's Method Table and Initialization Function
static PyMethodDef SpamMethods[] =
{{
"system",
spam_system,
METH_VARARGS,
"Execute a shell command."},
{NULL, NULL, 0, NULL
}};

//The method table must be passed to the interpreter in the module's initialization function.
//The initialization function must be named initname(), where name is the name of the module,
//and should be the only non-static item defined in the module file:
PyMODINIT_FUNC initspam(void)
{
(void) Py_InitModule("spam", SpamMethods);
}

The easiest way to handle this is to statically initialize your statically-linked modules
by directly calling initspam() after the call to Py_Initialize():
int main(int argc, char *argv[])
{
//Pass argv[0] to the Python interpreter
Py_SetProgramName(argv[0]);
//Initialize the Python interpreter.  Required.
Py_Initialize();
//Add a static module
initspam();
}

// Calling Python Functions from C
// Calling a Python function is easy. First, the Python program must somehow pass you the Python function object.
// You should provide a function (or some other interface) to do this. When this function is called, save a pointer to
// the Python function object (be careful to Py_INCREF() it!) in a global variable ¡ª or wherever you see fit. For example,
// the following function might be part of a module definition:
// This function must be registered with the interpreter using the METH_VARARGS flag;
// The macros Py_XINCREF() and Py_XDECREF() increment/decrement the reference count of an object and are safe in the presence of NULL pointers
static PyObject *my_callback = NULL;
static PyObject* my_set_callback(PyObject *dummy, PyObject *args)
{
PyObject *result = NULL;
PyObject *temp;

if( PyArg_ParseTuple(args, "O:set_callback", &temp) )
{
if( !PyCallable_Check(temp) )
{
PyErr_SetString(PyExc_TypeError, "parameter must be callable");
return NULL;
}
Py_XINCREF(temp);         // Add a reference to new callback
Py_XDECREF(my_callback);  // Dispose of previous callback
my_callback = temp;       // Remember new callback
// Boilerplate to return "None"
Py_INCREF(Py_None);
result = Py_None;
}
return result;
}

// Later, when it is time to call the function, you call the C function PyObject_CallObject().
// This function has two arguments, both pointers to arbitrary Python objects: the Python function, and the argument list.

int arg;
PyObject *arglist;
PyObject *result;
arg = 123;
// Time to call the callback
arglist = Py_BuildValue("(i)", arg);
result = PyObject_CallObject(my_callback, arglist);
Py_DECREF(arglist);

//Before you do this, however, it is important to check that the return value isn¡¯t NULL.
If it is, the Python function terminated by raising an exception. If the C code that called
PyObject_CallObject() is called from Python, it should now return an error indication to
its Python caller, so the interpreter can print a stack trace, or the calling Python code can
handle the exception. If this is not possible or desirable, the exception should be cleared
by calling PyErr_Clear(). For example://

if (result == NULL)
return NULL; // Pass error back
...use result...
Py_DECREF(result);


//You may also call a function with keyword arguments by using PyObject_Call(), which
supports arguments and keyword arguments. As in the above example, we use
Py_BuildValue() to construct the dictionary.//

PyObject *dict;
...
dict = Py_BuildValue("{s:i}", "name", val);
result = PyObject_Call(my_callback, NULL, dict);
Py_DECREF(dict);
if (result == NULL)
return NULL; // Pass error back
// Here maybe use the result
Py_DECREF(result);


*/
#include "py_macros.h"