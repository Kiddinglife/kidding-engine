#ifndef Scriptable_H_
#define Scriptable_H_

#include "ace/pre.h"
#include "common/common.h"
#include "py_macros.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace PythonScripts
{
	struct Scriptable : public PyObject
	{
		// 子类化 将一些py操作填充进派生类 
		SCRIPT_OBJECT_HREADER(Scriptable, Scriptable);

		Scriptable();
		virtual ~Scriptable();
	};
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif