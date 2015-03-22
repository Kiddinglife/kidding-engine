#include "Scriptable.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
namespace PythonScripts
{
	ScriptObject::SCRIPTOBJECT_TYPES ScriptObject::scriptObjectTypes;

	//SCRIPT_METHOD_DECLARE_BEGIN(ScriptObject)
	//SCRIPT_METHOD_DECLARE_END()
	bool ScriptObject::_ScriptObject_py_installed = false;
	PyMethodDef ScriptObject::_ScriptObject_scriptMethods[ ] = { { NULL } };

	//SCRIPT_MEMBER_DECLARE_BEGIN(ScriptObject)
	//SCRIPT_MEMBER_DECLARE_END()
	PyMemberDef ScriptObject::_ScriptObject_scriptMembers[ ] = { { NULL } };

	//SCRIPT_GETSET_DECLARE_BEGIN(ScriptObject)
	//SCRIPT_GETSET_DECLARE_END()
	PyGetSetDef ScriptObject::_ScriptObject_scriptGetSeters[ ] = { { NULL } };

	//SCRIPT_INIT(ScriptObject, 0, 0, 0, 0, 0)
	//#define SCRIPT_INIT(CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)  TEMPLATE_SCRIPT_INIT(;,CLASS, CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)	
	PyMethodDef* ScriptObject::_ScriptObject_lpScriptmethods = NULL;
	PyMemberDef* ScriptObject::_ScriptObject_lpScriptmembers = NULL;
	PyGetSetDef* ScriptObject::_ScriptObject_lpgetseters = NULL;
	PyTypeObject ScriptObject::_scriptType =
	{
		PyVarObject_HEAD_INIT(&PyType_Type, 0)
		"ScriptObject",							        /* tp_name            */
		sizeof(ScriptObject),					        /* tp_basicsize       */
		0,														/* tp_itemsize       */
		(destructor) ScriptObject::_tp_dealloc,/* tp_dealloc        */
		0,														/* tp_print            */
		0,														/* tp_getattr         */
		0,														/* tp_setattr         */
		0,														/* tp_compare     */
		ScriptObject::_tp_repr,				        /* tp_repr            */
		0,														/* tp_as_number  */
		0,													    /* tp_as_sequence */
		0,											            /* tp_as_mapping  */
		0,														/* tp_hash */
		0,												        /* tp_call  */
		ScriptObject::_tp_str,					        /* tp_str   */
		(getattrofunc) ScriptObject::_tp_getattro,	/* tp_getattro */
		(setattrofunc) ScriptObject::_tp_setattro,	/* tp_setattro  */
		0,														    /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,/* tp_flags  */
		"KBEngine::" "ScriptObject" " objects.",	/* tp_doc */
		0,														/* tp_traverse */
		0,														/* tp_clear */
		0,														/* tp_richcompare */
		0,														/* tp_weaklistoffset*/
		0,													    /* tp_iter             */
		0,											            /* tp_iternext      */
		0,														/* tp_methods    */
		0,														/* tp_members   */
		0,														/* tp_getset        */
		ScriptObject::getBaseScriptType(),	    /* tp_base          */
		0,														/* tp_dict           */
		0,														/* tp_descr_get  */
		0,														/* tp_descr_set  */
		ScriptObject::calcDictOffset(),            /* tp_dictoffset */
		(initproc) ScriptObject::_tp_init,	        /* tp_init */
		0,														/* tp_alloc */
		ScriptObject::_tp_new,				        /* tp_new  */
		PyObject_GC_Del,							    /* tp_free  */
	};

	ScriptObject::ScriptObject(PyTypeObject* pyType, bool isInitialised)
	{
		if( PyType_Ready(pyType) < 0 )
		{
			ACE_DEBUG(( LM_ERROR, "ScriptObject: Type {%s} is not ready\n", pyType->tp_name ));
		}

		if( !isInitialised )
		{
			PyObject_INIT(static_cast<PyObject*>( this ), pyType);
		}
	}

	ScriptObject::~ScriptObject()
	{
		ACE_ASSERT(this->ob_refcnt == 0 && "OB_REFCNT != 0 when destruct ");
	}

	PyTypeObject* ScriptObject::getScriptObjectType(const std::string& name)
	{
		ScriptObject::SCRIPTOBJECT_TYPES::iterator iter = scriptObjectTypes.find(name);
		if( iter != scriptObjectTypes.end() )
			return iter->second;
		return NULL;
	}

	int ScriptObject::__py_readonly_descr(PyObject* self, PyObject* value, void* closure)
	{
		PyErr_Format(PyExc_TypeError,
			"Sorry, this attribute %s.%s is read-only",
			( self != NULL ? self->ob_type->tp_name : "ScriptObject" ),
			( closure != NULL ? (char*) closure : "unknown" ));
		PyErr_PrintEx(0);
		return 0;
	}

	int ScriptObject::__py_writeonly_descr(PyObject* self, PyObject* value, void* closure)
	{
		PyErr_Format(PyExc_TypeError,
			"Sorry, this attribute %s.%s is write-only",
			( self != NULL ? self->ob_type->tp_name : "ScriptObject" ),
			( closure != NULL ? (char*) ( closure ) : "unknown" ));
		PyErr_PrintEx(0);
		return 0;
	}

	int ScriptObject::calcTotalMethodCount(void)
	{
		int nlen = 0;
		while( true )
		{
			PyMethodDef* pmf = &_ScriptObject_scriptMethods[nlen];
			if( !pmf->ml_doc && !pmf->ml_flags && !pmf->ml_meth && !pmf->ml_name )
				break;
			nlen++;
		}
		if( strcmp("ScriptObject", "ScriptObject") == 0 )
			return nlen;
		return ScriptObject::calcTotalMethodCount() + nlen;
	}

	int ScriptObject::calcTotalMemberCount(void)
	{
		int nlen = 0;
		while( true )
		{
			PyMemberDef* pmd = &_ScriptObject_scriptMembers[nlen];
			if( !pmd->doc && !pmd->flags && !pmd->type && !pmd->name && !pmd->offset )
				break;
			nlen++;
		}

		if( strcmp("ScriptObject", "ScriptObject") == 0 )
			return nlen;
		return ScriptObject::calcTotalMemberCount() + nlen;
	}

	int ScriptObject::calcTotalGetSetCount(void)
	{
		int nlen = 0;
		while( true )
		{
			PyGetSetDef* pgs = &_ScriptObject_scriptGetSeters[nlen];
			if( !pgs->doc && !pgs->get && !pgs->set && !pgs->name && !pgs->closure )
				break;
			nlen++;
		}

		if( strcmp("ScriptObject", "ScriptObject") == 0 )
			return nlen;
		return ScriptObject::calcTotalGetSetCount() + nlen;
	}

	void ScriptObject::setupScriptMethodAndAttribute(PyMethodDef* lppmf, PyMemberDef* lppmd, PyGetSetDef* lppgs)
	{
		int i = 0;
		PyMethodDef* pmf = NULL;
		PyMemberDef* pmd = NULL;
		PyGetSetDef* pgs = NULL;

		while( true )
		{

			pmf = &_ScriptObject_scriptMethods[i];
			if( !pmf->ml_doc && !pmf->ml_flags && !pmf->ml_meth && !pmf->ml_name )
				break;
			i++;
			*( lppmf++ ) = *pmf;
		}

		i = 0;
		while( true )
		{
			pmd = &_ScriptObject_scriptMembers[i];
			if( !pmd->doc && !pmd->flags && !pmd->type && !pmd->name && !pmd->offset )
				break;
			i++;
			*( lppmd++ ) = *pmd;
		}

		i = 0;
		while( true )
		{
			pgs = &_ScriptObject_scriptGetSeters[i];
			if( !pgs->doc && !pgs->get && !pgs->set && !pgs->name && !pgs->closure )
				break;
			i++;
			*( lppgs++ ) = *pgs;
		}

		if( strcmp("ScriptObject", "ScriptObject") == 0 )
		{
			*( lppmf ) = *pmf;
			*( lppmd ) = *pmd;
			*( lppgs ) = *pgs;
			return;
		}

		ScriptObject::setupScriptMethodAndAttribute(lppmf, lppmd, lppgs);
	}

	void ScriptObject::installScript(PyObject* mod, const char* name)
	{
		int nMethodCount = ScriptObject::calcTotalMethodCount();
		int nMemberCount = ScriptObject::calcTotalMemberCount();
		int nGetSetCount = ScriptObject::calcTotalGetSetCount();

		_ScriptObject_lpScriptmethods = new PyMethodDef[nMethodCount + 2];
		_ScriptObject_lpScriptmembers = new PyMemberDef[nMemberCount + 2];
		_ScriptObject_lpgetseters = new PyGetSetDef[nGetSetCount + 2];

		setupScriptMethodAndAttribute(_ScriptObject_lpScriptmethods,
			_ScriptObject_lpScriptmembers,
			_ScriptObject_lpgetseters);

		_scriptType.tp_methods = _ScriptObject_lpScriptmethods;
		_scriptType.tp_members = _ScriptObject_lpScriptmembers;
		_scriptType.tp_getset = _ScriptObject_lpgetseters;

		ScriptObject::onInstallScript(mod);
		if( PyType_Ready(&_scriptType) < 0 )
		{
			ACE_DEBUG(( LM_ERROR, "PyType_Ready(" "ScriptObject" ") is error!" ));
			PyErr_Print();
			return;
		}

		if( mod )
		{
			Py_INCREF(&_scriptType);
			if( PyModule_AddObject(mod, name, (PyObject *) &_scriptType) < 0 )
			{
				ACE_DEBUG(( LM_ERROR, "PyModule_AddObject({%s}) is error!", name ));
				PyErr_Print();
				return;
			}
		}

		SCRIPT_ERROR_CHECK();
		_ScriptObject_py_installed = true;
		ScriptObject::scriptObjectTypes[name] = &_scriptType;
	}

	void ScriptObject::uninstallScript(void)
	{
		SAFE_RELEASE_ARRAY(_ScriptObject_lpScriptmethods);
		SAFE_RELEASE_ARRAY(_ScriptObject_lpScriptmembers);
		SAFE_RELEASE_ARRAY(_ScriptObject_lpgetseters);
		ScriptObject::onUninstallScript();
		if( _ScriptObject_py_installed )
			Py_DECREF(&_scriptType);
	}
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL