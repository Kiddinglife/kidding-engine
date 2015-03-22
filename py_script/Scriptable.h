#ifndef Scriptable_H_
#define Scriptable_H_

#include "ace/pre.h"
#include "common/common.h"
#include "py_macros.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace PythonScripts
{
	struct ScriptObject : public PyObject
	{
		/**当前脚本模块的类别 */
		static PyTypeObject _scriptType;
		typedef ScriptObject ThisClass;

		/** 最终将要被安装到脚本模块中的方法和成员存放列表 */
		static PyMethodDef* _ScriptObject_lpScriptmethods;
		static PyMemberDef* _ScriptObject_lpScriptmembers;
		static PyGetSetDef* _ScriptObject_lpgetseters;

		/** 本模块所要暴漏给脚本的方法和成员， 最终会被导入到上面的2个指针列表中 */
		static PyMethodDef _ScriptObject_scriptMethods[ ];
		static PyMemberDef _ScriptObject_scriptMembers[ ];
		static PyGetSetDef _ScriptObject_scriptGetSeters[ ];

		static bool _ScriptObject_py_installed;

		// 所有的kbe脚本类别
		typedef UnorderedMap<std::string, PyTypeObject*> SCRIPTOBJECT_TYPES;
		static SCRIPTOBJECT_TYPES scriptObjectTypes;

		ScriptObject(PyTypeObject* pyType, bool isInitialised = false);
		virtual ~ScriptObject();

		// 脚本对象引用计数 
		void incRef() const { Py_INCREF(( PyObject* )this); }
		void decRef() const { Py_DECREF(( PyObject* )this); }
		int refCount() const { return int(( ( PyObject* )this )->ob_refcnt); }

		// 获得对象的描述
		PyObject* tp_repr();
		PyObject* tp_str();

		// 脚本请求获取属性或者方法
		PyObject* onScriptGetAttribute(PyObject* attr) { return PyObject_GenericGetAttr(this, attr); }

		// 脚本请求设置属性或者方法
		int onScriptSetAttribute(PyObject* attr, PyObject* value) { return PyObject_GenericSetAttr(static_cast<PyObject*>( this ), attr, value); }

		// 脚本请求删除一个属性
		int onScriptDelAttribute(PyObject* attr);

		// 脚本请求初始化
		int onScriptInit(PyObject* self, PyObject *args, PyObject* kwds) { return 0; }

		// 获取对象类别名称
		// For printing, in format "<module>.<name>" 
		const char* scriptName() const { return ob_type->tp_name; }

		static PyTypeObject* getScriptObjectType(const std::string& name);

		// 脚本请求创建一个该对象
		static PyObject* tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds) { return type->tp_alloc(type, 0); }

		// 脚本被安装时被调用
		static void onInstallScript(PyObject* mod) { }

		// 脚本被卸载时被调用
		static void onUninstallScript() { }

		//DECLARE_PY_GET_MOTHOD(py__module__);
		PyObject* py__module__() { return PyUnicode_FromString(scriptName()); }
		static PyObject* __pyget_py__module__(PyObject *self, void *closure) { return static_cast<ThisClass*>( self )->py__module__(); }

		//DECLARE_PY_GET_MOTHOD(py__qualname__);
		PyObject* py__qualname__() { return PyUnicode_FromString(scriptName()); }
		static PyObject* __pyget_py__qualname__(PyObject *self, void *closure) { return static_cast<ThisClass*>( self )->py__qualname__(); }

		//DECLARE_PY_GET_MOTHOD(py__name__);
		PyObject* py__name__() { return PyUnicode_FromString(scriptName()); }
		static PyObject* __pyget_py__name__(PyObject *self, void *closure) { return static_cast<ThisClass*>( self )->py__name__(); }

		/* python创建的对象则对象从python中释放*/
		static void _tp_dealloc(PyObject* self) { ScriptObject::_scriptType.tp_free(self); }

		static PyObject* _tp_repr(PyObject* self) { return static_cast<ScriptObject*>( self )->tp_repr(); }
		static PyObject* _tp_str(PyObject* self) { return static_cast<ScriptObject*>( self )->tp_str(); }

		/** 脚本模块对象从python中创建 */
		static PyObject* _tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds) { return ScriptObject::tp_new(type, args, kwds); }

		/** python 请求获取本模块的属性或者方法 */
		static PyObject* _tp_getattro(PyObject* self, PyObject* name) { return static_cast<ScriptObject*>( self )->onScriptGetAttribute(name); }

		/** python 请求初始化本模块对象 */
		static int _tp_init(PyObject* self, PyObject *args, PyObject* kwds) { return static_cast<ScriptObject*>( self )->onScriptInit(self, args, kwds); }

		/** python 请求设置本模块的属性或者方法 */
		static int _tp_setattro(PyObject* self, PyObject* name, PyObject* value)
		{
			return ( value != NULL ) ? static_cast<ScriptObject*>( self )->onScriptSetAttribute(name, value) : static_cast<ScriptObject*>( self )->onScriptDelAttribute(name);
		}

		/** getset的只读属性 */
		static int __py_readonly_descr(PyObject* self, PyObject* value, void* closure);

		/** getset的只写属性 */
		static int __py_writeonly_descr(PyObject* self, PyObject* value, void* closure);

		/** 这个接口可以获得当前模块的脚本类别 */
		static PyTypeObject* getScriptType(void) { return &_scriptType; }

		static PyTypeObject* getBaseScriptType(void)
		{
			if( strcmp("ScriptObject", "ScriptObject") == 0 )
				return NULL;
			return ScriptObject::getScriptType();
		}

		static long calcDictOffset(void)
		{
			if( strcmp("ScriptObject", "ScriptObject") == 0 )
				return 0;
			return 0;
		}

		/** 计算所有继承模块的暴露方法个数 */
		static int calcTotalMethodCount(void);

		/** 计算所有继承模块的暴露成员个数 */
		static int calcTotalMemberCount(void);

		/** 计算所有继承模块的暴露getset个数 */
		static int calcTotalGetSetCount(void);

		/** 将所有父类以及当前模块的暴露成员和方法安装到最终要导入脚本的列表中 */
		static void setupScriptMethodAndAttribute(PyMethodDef* lppmf, PyMemberDef* lppmd, PyGetSetDef* lppgs);

		/** 安装当前脚本模块 @param mod: 所要导入的主模块 */
		static void installScript(PyObject* mod, const char* name = "ScriptObject");

		/** 卸载当前脚本模块 */
		static void uninstallScript(void);
	};
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif