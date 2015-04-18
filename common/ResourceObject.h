#ifndef RESOURCE_OBJECT_H_
#define RESOURCE_OBJECT_H_

#include "ace/pre.h"
#include "common/common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

struct ResourceObject : public Intrusive_Auto_Ptr
{
	std::string resName_;
	ACE_UINT32 flags_;
	bool invalid_;
	ACE_UINT64 timeout_;

	ResourceObject(const char* res, ACE_UINT32 flags);
	virtual ~ResourceObject();

	bool valid()const;
	void update();
};

struct FileObject : public ResourceObject
{
	public:
	FileObject(const char* res, ACE_UINT32 flags, const char* model);
	virtual ~FileObject();

	FILE* fd() { return fd_; }

	bool seek(ACE_UINT32 idx, int flags = SEEK_SET);
	ACE_UINT32 read(char* buf, ACE_UINT32 limit);
	ACE_UINT32 tell();
	protected:
	FILE* fd_;
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif