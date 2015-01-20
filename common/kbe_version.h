#ifndef KBE_VERSION_H_
#define KBE_VERSION_H_
#include "ace/pre.h"
#include "strulti.h"
#include "ace/ACE_export.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
#define KBE_VERSION_MAJOR 0
#define KBE_VERSION_MINOR 2
#define KBE_VERSION_PATCH 0
namespace Version
{
	extern std::string g_scriptversion;
	extern std::string g_kbengine_version;

	const std::string & get_kbengine_version();
	void set_script_version(const std::string& ver);
	const std::string& get_script_version();
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace/post.h"
#endif