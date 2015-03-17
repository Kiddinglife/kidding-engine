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