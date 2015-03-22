#ifndef Script_H_
#define Script_H_

#include "ace/pre.h"
#include "common/common.h"
#include "Scriptable.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace PythonScripts
{
/** 脚本系统路径 */
#ifdef _LP64
#define SCRIPT_PATH												\
					L"../../res/scripts;"						\
					L"../../res/scripts/common;"				\
					L"../../res/scripts/common/lib-dynload;"	\
					L"../../res/scripts/common/DLLs;"			\
					L"../../res/scripts/common/Lib"
#else
#define SCRIPT_PATH												\
					L"../../res/scripts;"						\
					L"../../res/scripts/common;"				\
					L"../../res/scripts/common/lib-dynload;"	\
					L"../../res/scripts/common/DLLs;"			\
					L"../../res/scripts/common/Lib"
#endif

	class Script
	{
		public:
		Script();
		virtual ~Script();
	};
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif
