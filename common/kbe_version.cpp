#include "kbe_version.h"
#include "strulti.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
namespace Version
{
	std::string g_scriptversion = "0.0.0";
	std::string g_kbengine_version;

	const std::string & get_kbengine_version()
	{
		if( g_kbengine_version.size() == 0 )
		{
			char buf[256];
			kbe_snprintf(buf,
				256,
				"%d.%d.%d",
				KBE_VERSION_MAJOR, KBE_VERSION_MINOR, KBE_VERSION_PATCH);
			g_kbengine_version = buf;
		}
		return g_kbengine_version;
	}

	void set_script_version(const std::string& ver)
	{
		g_scriptversion = ver;
	}

	const std::string& get_script_version()
	{
		return g_scriptversion;
	}
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL