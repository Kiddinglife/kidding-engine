#ifndef DataType_H_
#define DataType_H_

#include "ace\pre.h"
#include "ace\Intrusive_Auto_Ptr.h"
#include "common\common.h"
#include "net\Packet.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

#if KBE_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

struct PyObject;
struct DataType : public Intrusive_Auto_Ptr
{
	DataType();
	virtual ~DataType();

	virtual void addToStream(Packet* mstream, PyObject* pyValue) = 0;
	virtual PyObject* createFromStream(Packet* mstream) = 0;
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif