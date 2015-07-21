/**
* Reviewed by Jackie Zhang on 21/07/2015 
*/

#ifndef RESOURCE_OBJECT_H_
#define RESOURCE_OBJECT_H_

#include "ace/pre.h"
#include "common/common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

/**
* struct FixedMessages
*
* @Brief
*
* @Notes
* size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
* buffer 用于接收数据的内存地址
* size 要读的每个数据项的字节数，单位是字节
* count 要读count个数据项，每个数据项size个字节.
* stream 输入流
* 返回值
* 实际读取的元素个数。如果返回值与count不相同，则可能文件结尾或发生错误。
* 从ferror和feof获取错误息或检测是否到达文件结尾。　
*
*
*/
struct ResourceObject : public Intrusive_Auto_Ptr
{
	bool invalid_;
	ACE_UINT32 flags_;
	ACE_UINT64 timeout_;
	std::string res_name_;

	ResourceObject(const char* res, ACE_UINT32 flags) :
		res_name_(res),
		flags_(flags),
		invalid_(false),
		timeout_(0)
	{
		update();
	}

	virtual ResourceObject::~ResourceObject()
	{
		if( ResourceManager::respool_timeout > 0 )
		{
			ACE_DEBUG(( LM_DEBUG, "ResourceObject::~ResourceObject(): {%s}\n", res_name_ ));
		}
	}

	bool valid() const { return invalid_ || timestamp() < timeout_; }

	void update()
	{
		timeout_ = timestamp() + ResourceManager::respool_timeout * stampsPerSecond();
	}
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