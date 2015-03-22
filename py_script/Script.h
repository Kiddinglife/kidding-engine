#ifndef Script_H_
#define Script_H_

#include "ace/pre.h"
#include "common/common.h"
#include "Scriptable.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace PythonScripts
{
	struct ScriptStdOutErr;
	struct Script
	{
		PyObject* 					module_;
		PyObject*					    extraModule_;		// 扩展脚本模块
		ScriptStdOutErr*			pyStdouterr_;

		Script();
		virtual ~Script();

		/** 安装和卸载脚本模块*/
		virtual bool install(const wchar_t* pythonHomeDir, std::wstring pyPaths, 
        const char* moduleName, KBE_SRV_COMPONENT_TYPE componentType);
	};
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif
