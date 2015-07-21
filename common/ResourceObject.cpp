#include "ResourceObject.h"
#include "common\timestamp.hpp"
#include "common\ResourceManager.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL


FileObject::FileObject(const char* res, ACE_UINT32 flags, const char* model) :
ResourceObject(res, flags)
{
	fd_ = ACE_OS::fopen(res, model);
	if( fd_ == NULL )
	{
		invalid_ = true;
		ACE_DEBUG(( LM_ERROR,
			"FileObject::FileObject(): open({%s}) {%s} error!\n", model, res_name_ ));
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
		ACE_ERROR_RETURN(( LM_ERROR, "FileObject::seek: { %s } invalid!\n", res_name_ ), false);
	}
	update();
	return ACE_OS::fseek(fd_, idx, flags) != -1;
}

ACE_UINT32 FileObject::read(char* buf, ACE_UINT32 limit)
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_ERROR_RETURN(( LM_ERROR, "FileObject::read: {%s} invalid!\n", res_name_ ), 0);
	}
	update();
	return ACE_OS::fread(buf, sizeof(char), limit, fd_);
}

ACE_UINT32 FileObject::tell()
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_ERROR_RETURN(( LM_ERROR, "FileObject::tell: {%s} invalid!\n", res_name_ ), 0);
	}
	update();
	return ACE_OS::ftell(fd_);
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL