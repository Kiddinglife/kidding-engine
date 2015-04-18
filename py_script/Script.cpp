#include "Script.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace PythonScripts
{
	static PyObject* __py_genUUID64(PyObject *self, void *closure)
	{
		static ACE_INT8 check = -1;
		if( check < 0 )
		{
			if( g_componentGlobalOrder <= 0 || g_componentGlobalOrder > 65535 )
			{
				ACE_DEBUG(( LM_DEBUG,
					"globalOrder({}) is not in the range of 0~65535, genUUID64 is not safe, "
					"in the multi process may be repeated.\n",
					g_componentGlobalOrder ));
			}
			check = 1;
		}
		//return PyLong_FromUnsignedLongLong(kbe_gen_uuid64());
		return PyUnicode_FromString(kbe_gen_uuid64()->to_string()->c_str());
	}

	bool install(const wchar_t* pythonHomeDir, std::wstring pyPaths,
		const char* moduleName, KBE_SRV_COMPONENT_TYPE componentType)
	{
		std::wstring pySysPaths = SCRIPT_PATH;

	}
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
