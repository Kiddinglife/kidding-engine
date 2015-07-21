/*
// You can also define a new exception that is unique to your module.
// For this, you usually declare a static object variable at the beginning of your file:
static PyObject *SpamError;


// and initialize it in your module's initialization function (initspam())
// with an exception object (leaving out the error checking for now):
PyMODINIT_FUNC initspam(void)
{
PyObject *m;
m = Py_InitModule("spam", SpamMethods);
if (m == NULL) return;
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
if (!PyArg_ParseTuple(args, "s", &command)) return NULL;
sts = system(command);
if (sts < 0) 
{
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

//////////////////////////// Calling Python Functions from C ///////////////////////////////////////////
// Calling a Python function is easy. First, the Python program must somehow pass you the Python function object.
// You should provide a function (or some other interface) to do this. When this function is called, save a pointer to
// the Python function object (be careful to Py_INCREF() it!) in a global variable — or wherever you see fit. For example,
// the following function might be part of a module definition: This function must be registered with the interpreter using 
// the METH_VARARGS flag; The macros Py_XINCREF() and Py_XDECREF() increment/decrement the reference count of an object 
// and are safe in the presence of NULL pointers

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

// if you want to define a new object type, you need to create a new type object.
// This sort of thing can only be explained by example, so hers a minimal,
// but complete, module that defines a new type:
#include <Python.h>

typedef struct {
PyObject_HEAD
// Type-specific fields go here. 
} noddy_NoddyObject;

static PyTypeObject noddy_NoddyType = {

    //as the type of a type object is “type”, 
	//but this isn’t strictly conforming C and some compilers complain. 
	//Fortunately, this member will be filled in for us by PyType_Ready().
	PyObject_HEAD_INIT(NULL)

	//The ob_size field of the header is not used; its presence in the type
	//structure is a historical artifact that is maintained for binary compatibility 
	//with extension modules compiled for older versions of Python. 
	// Always set this field to zero.
	0, //ob_size

	// The name of our type. This will appear in the default textual representation
	// of our objects and in some error messages, for example
	// >>> "" + noddy.new_noddy()
	// Traceback (most recent call last):
	// File "<stdin>", line 1, in ?
	// TypeError: cannot add type "noddy.Noddy" to string
	// Note that the name is a dotted name that includes both the module 
	// name and the name of the type within the module. The module in this
	// case is noddy and the type is Noddy, so we set the type name to noddy.Noddy.
	"noddy.Noddy", // tp_name        

	// tp_basicsize
	sizeof(noddy_NoddyObject), 

	// this has to do with variable length objects like lists and strings. Ignore this for now.
	0, // tp_itemsize    

	0,                         //tp_dealloc
	0,                         //tp_print
	0,                         //tp_getattr
	0,                         //tp_setattr
	0,                         //tp_compare
	0,                         //tp_rep
	0,                         //tp_as_number
	0,                         //tp_as_sequence
	0,                         //tp_as_mapping
	0,                         //tp_hash 
	0,                         //tp_call
	0,                         //tp_str
	0,                         //tp_getattro
	0,                         //tp_setattro
	0,                         //tp_as_buffer

	//tp_flags All types should include this constant in their flags. 
	//It enables all of the members defined by the current version of Python.
	Py_TPFLAGS_DEFAULT, 

	"Noddy objects",           // tp_doc 
};

static PyMethodDef noddy_methods[ ] = { { NULL }  // Sentinel };

#ifndef PyMODINIT_FUNC	// declarations for DLL import/export 
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC initnoddy(void)
{
	PyObject* m;

	// Now we get into the type methods,
	// the things that make your objects different from the others.
	// For now, all we want to be able to do is to create new Noddy objects.
	// To enable object creation, we have to provide a tp_new implementation.
	noddy_NoddyType.tp_new = PyType_GenericNew;

	// This initializes the Noddy type, filing in a number of members,
	// including ob_type that we initially set to NULL. PyObject_HEAD_INIT(NULL)
	if( PyType_Ready(&noddy_NoddyType) < 0 ) return;

	// All the other type methods are NULL, so we will go over them later — thats for a later section!
	m = Py_InitModule3("noddy", noddy_methods, "Example module that creates an extension type.");
	Py_INCREF(&noddy_NoddyType);

	// This adds the type to the module dictionary.
	// This allows us to create Noddy instances by calling the Noddy class:
	// >>> import noddy   >>> mynoddy = noddy.Noddy()
	PyModule_AddObject(m, "Noddy", (PyObject *) &noddy_NoddyType);
}

//Thats it! All that remains is to build it; put the above code in a file called noddy.c and
from distutils.core import setup, Extension
setup(name="noddy", version="1.0",
ext_modules=[Extension("noddy", ["noddy.c"])])

in a file called setup.py; then typing
$ python setup.py build

// at a shell should produce a file noddy.so in a subdirectory; move to that directory and fire up Python 
// — you should be able to import noddy and play around with Noddy objects.
// That wasn’t so hard, was it? Of course, the current Noddy type is pretty uninteresting.
// It has no data and doesnt do anything. It cant even be subclassed.


////////////////////// Adding data and methods to the Basic example ///////////////////////////////

// Lets expend the basic example to add some data and methods. 
// Lets also make the type usable as a base class. 
// Well create a new module, noddy2 that adds these capabilities:

#include <Python.h>
#include "structmember.h"

typedef struct {
PyObject_HEAD
PyObject *first; // first name 
PyObject *last;  // last name 
int number;
} Noddy;

// Because we now have data to manage, 
// we have to be more careful about object allocation and deallocation.
// At a minimum, we need a deallocation method:
static void Noddy_dealloc(Noddy* self)
{
    //This method decrements the reference counts of the two Python attributes. 
    //We use Py_XDECREF() here because the first and last members could be NULL.
    //It then calls the tp_free member of the object’s type to free the object’s memory.
    //Note that the object’s type might not be NoddyType, because the object may be an instance of a subclass.
	Py_XDECREF(self->first);
	Py_XDECREF(self->last);
	self->ob_type->tp_free((PyObject*) self);
}

// We want to make sure that the first and last names are
// initialized to empty strings, so we provide a new method:

// One reason to implement a new method is to assure the initial values of instance variables


// The new method is a static method that is passed the type being instantiated and any arguments 
// passed when the type was called, and that returns the new object created.

// Note that if the type supports subclassing, the type passed may not be the type being defined. 
// The new method calls the tp_alloc slot to allocate memory. We don’t fill the tp_alloc slot ourselves.
// Rather PyType_Ready() fills it for us by inheriting it from our base class, 
// which is object by default. Most types use the default allocation

// Note If you are creating a co-operative tp_new (one that calls a base type’s tp_new or __new__()), 
// you must not try to determine what method to call using method resolution order at runtime. 
// Always statically determine what type you are going to call, and call its tp_new directly, or via type->tp_base->tp_new.
// If you do not do this, Python subclasses of your type that also inherit from other Python-defined classes may not work correctly. 
// (Specifically, you may not be able to create instances of such subclasses without getting a TypeError.)

static PyObject* Noddy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	Noddy *self;
	self = (Noddy *) type->tp_alloc(type, 0);

	if( self != NULL )
	{
		self->first = PyString_FromString("");
		if( self->first == NULL )
		{
			Py_DECREF(self);
			return NULL;
		}
		self->last = PyString_FromString("");
		if( self->last == NULL )
		{
			Py_DECREF(self);
			return NULL;
		}
		self->number = 0;
	}
	return (PyObject *) self;
}

// We provide an initialization function:
// The tp_init slot is exposed in Python as the __init__() method.
// It is used to initialize an object after it’s created.
// Initializers can be called multiple times. Anyone can call the __init__() method 
// on our objects. For this reason, we have to be extra careful when assigning the new values.

static int Noddy_init(Noddy *self, PyObject *args, PyObject *kwds)
{
	PyObject *first = NULL, *last = NULL, *tmp;
	static char *kwlist[ ] = { "first", "last", "number", NULL };
	if( !PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist, &first, &last, &self->number) ) return -1;

	if( first )
	{
		tmp = self->first;
		Py_INCREF(first);
		self->first = first;
		Py_XDECREF(tmp);
	}

	if( last )
	{
		tmp = self->last;
		Py_INCREF(last);
		self->last = last;
		Py_XDECREF(tmp);
	}
	return 0;
}


// We want to expose our instance variables as attributes. 
// There are a number of ways to do that.
// The simplest way is to define member definitions:
static PyMemberDef Noddy_members[ ] = 
{
	{ "first", T_OBJECT_EX, offsetof(Noddy, first), 0, "first name" },
	{ "last", T_OBJECT_EX, offsetof(Noddy, last), 0, "last name" },
	{ "number", T_INT, offsetof(Noddy, number), 0, "noddy number" },
	{ NULL }  // Sentinel 
};

//This method is equivalent to the Python method:
//def name(self):
//return "%s %s" % (self.first, self.last)
static PyObject* Noddy_name(Noddy* self)
{
	static PyObject *format = NULL;
	PyObject *args, *result;

	if( format == NULL )
	{
		format = PyString_FromString("%s %s");
		if( format == NULL )
			return NULL;
	}

	if( self->first == NULL )
	{
		PyErr_SetString(PyExc_AttributeError, "first");
		return NULL;
	}

	if( self->last == NULL )
	{
		PyErr_SetString(PyExc_AttributeError, "last");
		return NULL;
	}

	args = Py_BuildValue("OO", self->first, self->last);
	if( args == NULL ) return NULL;

	result = PyString_Format(format, args);
	Py_DECREF(args);

	return result;
}

// Note that we used the METH_NOARGS flag to
// indicate that the method is passed no arguments.
static PyMethodDef Noddy_methods[ ] = 
{
	{ 
	  "name",
	  (PyCFunction) Noddy_name, 
	  METH_NOARGS,
	  "Return the name, combining the first and last name"
	},
	{ NULL }  // Sentinel 
};

static PyTypeObject NoddyType =
{
	PyObject_HEAD_INIT(NULL)
	0,                         //ob_size
	"noddy.Noddy",   //tp_name
	sizeof(Noddy),     //tp_basicsize
	0,                         //tp_itemsize
	(destructor) Noddy_dealloc, //tp_dealloc

	0,                         //tp_print
	0,                         //tp_getattr
	0,                         //tp_setattr
	0,                         //tp_compare
	0,                         //tp_repr
	0,                         //tp_as_number
	0,                         //tp_as_sequence
	0,                         //tp_as_mapping
	0,                         //tp_hash 
	0,                         //tp_call
	0,                         //tp_str
	0,                         //tp_getattro
	0,                         //tp_setattro
	0,                         //tp_as_buffer

	// Finally, we’ll make our type usable as a base class. 
	// We've written our methods carefully so far so that they dont make any assumptions 
	// about the type of the object being created or used, so all we need to do is to add the 
	// Py_TPFLAGS_BASETYPE to our class flag definition:
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, //tp_flags

	"Noddy objects",  // tp_doc 

	0,		                   // tp_traverse 
	0,		                   // tp_clear 
	0,		                   // tp_richcompare 
	0,		                   // tp_weaklistoffset 
	0,		                   // tp_iter 
	0,		                   // tp_iternext 

	Noddy_methods,             // tp_methods 
	Noddy_members,            // tp_members 

	0,                         // tp_getset 
	0,                         // tp_base 
	0,                         // tp_dict 
	0,                         // tp_descr_get 
	0,                         // tp_descr_set 
	0,                         // tp_dictoffset 

	(initproc) Noddy_init,      // tp_init 
	0,                                   // tp_alloc 
	Noddy_new,                   // tp_new 
};

static PyMethodDef module_methods[ ] = {{ NULL } /// Sentinel };

#ifndef PyMODINIT_FUNC	// declarations for DLL import/export 
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC initnoddy2(void)
{
	PyObject* m;
	if( PyType_Ready(&NoddyType) < 0 ) return;
	m = Py_InitModule3("noddy2", module_methods, "Example module that creates an extension type.");
	if( m == NULL ) return;
	Py_INCREF(&NoddyType);
	PyModule_AddObject(m, "Noddy", (PyObject *) &NoddyType);
}
*/
#include "py_macros.h"
