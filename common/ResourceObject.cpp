#include "ResourceObject.h"
#include "common\timestamp.hpp"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

ResourceObject::ResourceObject(const char* res, ACE_UINT32 flags) :
resName_(res),
flags_(flags),
invalid_(false),
timeout_(0)
{
	update();
}


ResourceObject::~ResourceObject()
{
	//if( Resmgr::respool_timeout > 0 )
	//{
	//	DEBUG_MSG(fmt::format("ResourceObject::~ResourceObject(): {}\n", resName_));
	//}
}

bool ResourceObject::valid() const
{
	return invalid_ || timestamp() < timeout_;
}

FileObject::FileObject(const char* res, ACE_UINT32 flags, const char* model) : ResourceObject(res, flags)
{
	fd_ = ACE_OS::fopen(res, model);
	if( fd_ == NULL )
	{
		invalid_ = true;
		ACE_DEBUG(( LM_ERROR, "FileObject::FileObject(): open({%s}) {%s} error!\n", model, resName_ ));
	}
}

FileObject::~FileObject()
{
	ACE_OS::fclose(fd_);
}

bool FileObject::seek(ACE_UINT32 idx, int flags)
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_DEBUG(( LM_ERROR, "FileObject::seek: { %s } invalid!\n", resName_ ));
		return false;
	}

	update();
	return ACE_OS::fseek(fd_, idx, flags) != -1;
}

ACE_UINT32 FileObject::read(char* buf, ACE_UINT32 limit)
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_DEBUG(( LM_ERROR, "FileObject::read: {%s} invalid!\n", resName_ ));
		return 0;
	}

	update();
	return ACE_OS::fread(buf, sizeof(char), limit, fd_);
}

ACE_UINT32 FileObject::tell()
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_DEBUG(( LM_ERROR, "FileObject::tell: {%s} invalid!\n", resName_ ));
		return 0;
	}

	update();
	return ACE_OS::ftell(fd_);
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL