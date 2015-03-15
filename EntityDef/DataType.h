#ifndef DataType_H_
#define DataType_H_

#include "ace\pre.h"
#include <ace\Intrusive_Auto_Ptr.h>
#include <common\common.h>
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#pragma warning (disable : 4661)
#else
// linux include
#endif

struct DataType : public Intrusive_Auto_Ptr
{
	DataType();
	virtual ~DataType();
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif