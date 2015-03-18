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
// which means “error” in most contexts, as we have seen.


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
// the Python function object (be careful to Py_INCREF() it!) in a global variable — or wherever you see fit. For example, 
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

//Before you do this, however, it is important to check that the return value isn’t NULL.
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
#ifndef PY_MACROS_H_
#define PY_MACROS_H_

#include <ace/pre.h>
#include <common/common.h>


ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
namespace PythonScripts
{
	//////////////////////////////////////// PY_METHOD_ARG DEFINES ////////////////////////////////////
#define PY_METHOD_ARG_char								char
#define PY_METHOD_ARG_char_ARG						char
#define PY_METHOD_ARG_char__PYARGTYPE			"b"

#define PY_METHOD_ARG_charptr							char *
#define PY_METHOD_ARG_charptr_ARG					char *
#define PY_METHOD_ARG_charptr_PYARGTYPE		"s"

#define PY_METHOD_ARG_const_charptr						const char *
#define PY_METHOD_ARG_const_charptr_ARG				char *
#define PY_METHOD_ARG_const_charptr_PYARGTYPE	"s"

#define PY_METHOD_ARG_PyObject_ptr						PyObject *
#define PY_METHOD_ARG_PyObject_ptr_ARG				PyObject *
#define PY_METHOD_ARG_PyObject_ptr_PYARGTYPE	"O"

#define PY_METHOD_ARG_float								float
#define PY_METHOD_ARG_float_ARG						float
#define PY_METHOD_ARG_float_PYARGTYPE			"f"

#define PY_METHOD_ARG_double							double
#define PY_METHOD_ARG_double_ARG					double
#define PY_METHOD_ARG_double_PYARGTYPE			"d"

#define PY_METHOD_ARG_uint8							ACE_UINT8
#define PY_METHOD_ARG_uint8_ARG					ACE_UINT8
#define PY_METHOD_ARG_uint8_PYARGTYPE		"B"

#define PY_METHOD_ARG_uint16							ACE_UINT16
#define PY_METHOD_ARG_uint16_ARG					ACE_UINT16
#define PY_METHOD_ARG_uint16_PYARGTYPE		"H"

#define PY_METHOD_ARG_uint32							ACE_UINT32
#define PY_METHOD_ARG_uint32_ARG					ACE_UINT32
#define PY_METHOD_ARG_uint32_PYARGTYPE		"I"

#define PY_METHOD_ARG_uint64							ACE_UINT64
#define PY_METHOD_ARG_uint64_ARG					ACE_UINT64
#define PY_METHOD_ARG_uint64_PYARGTYPE		"K"

#define PY_METHOD_ARG_int8								ACE_INT8
#define PY_METHOD_ARG_int8_ARG						ACE_INT8
#define PY_METHOD_ARG_int8_PYARGTYPE			"b"

#define PY_METHOD_ARG_int16							ACE_INT16
#define PY_METHOD_ARG_int16_ARG					ACE_INT16
#define PY_METHOD_ARG_int16_PYARGTYPE		"h"

#define PY_METHOD_ARG_int								ACE_INT32
#define PY_METHOD_ARG_int_ARG						ACE_INT32
#define PY_METHOD_ARG_int_PYARGTYPE			"i"

#define PY_METHOD_ARG_int32							ACE_INT32
#define PY_METHOD_ARG_int32_ARG					ACE_INT32
#define PY_METHOD_ARG_int32_PYARGTYPE		"i"

#define PY_METHOD_ARG_int64							ACE_INT64
#define PY_METHOD_ARG_int64_ARG					ACE_INT32
#define PY_METHOD_ARG_int64_PYARGTYPE		"L"


#define PY_METHOD_ARG_ScriptID						ScriptID
#define PY_METHOD_ARG_ScriptID_ARG				ScriptID
#define PY_METHOD_ARG_ScriptID_PYARGTYPE	"i"

#define PY_METHOD_ARG_TIMER_ID					TIMER_ID
#define PY_METHOD_ARG_TIMER_ID_ARG			TIMER_ID
#define PY_METHOD_ARG_TIMER_ID_PYARGTYPE "I"

#define PY_METHOD_ARG_SPACE_ID					SPACE_ID
#define PY_METHOD_ARG_SPACE_ID_ARG			SPACE_ID
#define PY_METHOD_ARG_SPACE_ID_PYARGTYPE "I"

#define PY_METHOD_ARG_ENTITY_ID					ENTITY_ID
#define PY_METHOD_ARG_ENTITY_ID_ARG			ENTITY_ID
#define PY_METHOD_ARG_ENTITY_ID_PYARGTYPE "i"


	///////////////////////////////// SCRIPT METHOD EXPOSURE DEFINES ////////////////////////////////

/* 定义暴露给脚本的方法宏 */
#define SCRIPT_METHOD_DECLARE_BEGIN(CLASS)                                                                                                bool CLASS::_##CLASS##_py_installed = false; PyMethodDef CLASS::_##CLASS##_scriptMethods[] = {
#define TEMPLATE_SCRIPT_METHOD_DECLARE_BEGIN(TEMPLATE_HEADER, TEMPLATE_CLASS, CLASSNAME)       TEMPLATE_HEADER bool TEMPLATE_CLASS::_##CLASSNAME##_py_installed = false;\
                                                                                                                                                                             TEMPLATE_HEADER PyMethodDef TEMPLATE_CLASS::_##CLASSNAME##_scriptMethods[] = {
#define SCRIPT_METHOD_DECLARE(METHOD_NAME, METHOD_FUNC, FLAGS, DOC)                                            {METHOD_NAME, (PyCFunction)&__py_##METHOD_FUNC, FLAGS, DOC},
#define SCRIPT_DIRECT_METHOD_DECLARE(METHOD_NAME, METHOD_FUNC, FLAGS, DOC)                               {METHOD_NAME, (PyCFunction)&METHOD_FUNC, FLAGS, DOC},
#define SCRIPT_METHOD_DECLARE_END() {0, 0, 0, 0}};

/* 向模块追加方法 */
#define APPEND_SCRIPT_MODULE_METHOD(MODULE, NAME, FUNC, FLAGS, SELF)	 \
	static PyMethodDef __pymethod_##NAME = {#NAME, (PyCFunction) FUNC, FLAGS, NULL};\
	if(PyModule_AddObject(MODULE, #NAME, PyCFunction_New(&__pymethod_##NAME, SELF)) != 0) \
		{\
		SCRIPT_ERROR_CHECK();\
		ERROR_MSG("append " #NAME " to pyscript is error!\n");	\
		}	


/** 定义暴露给脚本的属性宏 */
#define SCRIPT_MEMBER_DECLARE_BEGIN(CLASS)                                                                                               PyMemberDef CLASS::_##CLASS##_scriptMembers[] =	{
#define TEMPLATE_SCRIPT_MEMBER_DECLARE_BEGIN(TEMPLATE_HEADER, TEMPLATE_CLASS, CLASSNAME)	    TEMPLATE_HEADER PyMemberDef TEMPLATE_CLASS::_##CLASSNAME##_scriptMembers[] =	{
#define SCRIPT_MEMBER_DECLARE(MEMBER_NAME, MEMBER_REF, MEMBER_TYPE, FLAGS, DOC)				        {const_cast<char*>(MEMBER_NAME), MEMBER_TYPE, offsetof(ThisClass, MEMBER_REF), FLAGS, DOC},
#define SCRIPT_MEMBER_DECLARE_END()															                                                {0, 0, 0, 0, 0}};

/** 定义暴露给脚本的getset属性宏 */
#define SCRIPT_GETSET_DECLARE_BEGIN(CLASS)													                                               PyGetSetDef CLASS::_##CLASS##_scriptGetSeters[] = {
#define TEMPLATE_SCRIPT_GETSET_DECLARE_BEGIN(TEMPLATE_HEADER, TEMPLATE_CLASS, CLASSNAME)	       TEMPLATE_HEADER PyGetSetDef TEMPLATE_CLASS::_##CLASSNAME##_scriptGetSeters[] =	{
#define SCRIPT_GETSET_DECLARE(NAME, GET, SET, DOC, CLOSURE)									                                   {const_cast<char*>(NAME), (getter)__pyget_##GET, (setter)__pyset_##SET, DOC, CLOSURE},
#define SCRIPT_GET_DECLARE(NAME, GET, DOC, CLOSURE)											                                       {const_cast<char*>(NAME), (getter)__pyget_##GET, (setter)__py_readonly_descr, DOC, const_cast<char*>(NAME)},
#define SCRIPT_SET_DECLARE(NAME, SET, DOC, CLOSURE)											                                       {const_cast<char*>(NAME), (getter)__pyset_##SET, (setter)__py_writeonly_descr, DOC, const_cast<char*>(NAME)},
#define SCRIPT_GETSET_DECLARE_END()															                                                   {0, 0, 0, 0, 0}};

}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif