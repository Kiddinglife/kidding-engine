/**
* Reviewed by Jackie Zhang on 21/07/2015
* Reviewed by Jackie Zhang on 22/07/2015 : combine resobj.h and resmgr.h into one file
*/

#ifndef ResourceManager_h_
#define ResourceManager_h_

#include "ace\pre.h"
#include "ace\Null_Mutex.h"
#include "ace\Refcounted_Auto_Ptr.h"
#include "ace\Event_Handler.h"
#include "common\common.h"
#include "common\timestamp.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

#define RESOURCE_NORMAL	 0x00000000
#define RESOURCE_RESIDENT   0x00000001
#define RESOURCE_READ          0x00000002
#define RESOURCE_WRITE        0x00000004
#define RESOURCE_APPEND     0x00000008

struct ResourceObject;
typedef ACE_Refcounted_Auto_Ptr<ResourceObject, ACE_Null_Mutex> ResourceObjectRefAutoPtr;

#define ResourceManagerPtr(instance) \
ResourceManager* instance = ACE_Singleton<ResourceManager, ACE_Null_Mutex>::instance()
/**
* struct FixedMessages
*
* getcwd()
* 不过要注意一点，如果你使用VC编译调试，那么在里面点F5运行的时候，
* 那个当前目录是指工程目录，而不是Debug文件夹里面。当然，你直接把
* exe文件拿出来用的话就是一般意义下的当前目录了。
*
* @Brief
*
* @Notes
* size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
*
* buffer 用于接收数据的内存地址
* size 要读的每个数据项的字节数，单位是字节
* count 要读count个数据项，每个数据项size个字节.
* stream 输入流
*
* 返回值
* 实际读取的元素个数。如果返回值与count不相同，则可能文件结尾或发生错误。
* 从ferror和feof获取错误息或检测是否到达文件结尾。　
*
* long ftell(FILE *stream);
*
* 函数功能
* 使用fseek函数后再调用函数ftell()就能非常容易地确定文件的当前位置。
* 约束条件
*
* 因为ftell返回long型，根据long型的取值范围-231~231-1（-2147483648～2147483647），
* 故对大于2.1G的文件进行操作时出错。
*
* int fseek(FILE *stream, long offset, int fromwhere);
* 函数功能
* 函数设置文件指针stream的位置。如果执行成功，stream将指向以fromwhere
*（偏移起始位置：文件头 0 (SEEK_SET)，当前位置 1 (SEEK_CUR)，文件尾 2 (SEEK_END)）为基准，
* 偏移offset（指针偏移量）个字节的位置。如果执行失败(比如offset超过文件自身大小)，
* 则不改变stream指向的位置。
*/
struct ResourceObject
{
	bool invalid_;
	ACE_UINT32 flags_;
	ACE_UINT64 timeout_;
	std::string res_name_;

	ResourceObject(const char* res, ACE_UINT32 flags);

	virtual ~ResourceObject();

	inline bool valid() const;

	inline void update();

	virtual void* get() { return NULL; }
};

struct FileObject : public ResourceObject
{
	public:
	FileObject(const char* res, ACE_UINT32 flags, const char* model);
	virtual ~FileObject();

	virtual void* get() { return fd_; }

	bool seek(ACE_UINT32 idx, int flags = SEEK_SET);
	ACE_UINT32 read(char* buf, ACE_UINT32 cnt);
	ACE_UINT32 tell();
	protected:
	FILE* fd_;
};

struct ResourceManager : public ACE_Event_Handler
{
	// 引擎环境变量
	struct Env
	{
		std::string root_path;
		std::string all_used_paths;
		std::string bin_path;
		std::string python_native_libs_path;
		std::string python_user_scripts_path;
		std::string python_user_res_path;
	};

	static ACE_UINT64 respool_timeout;
	static ACE_UINT32 respool_buffersize;
	static ACE_UINT32 respool_checktick;

	bool isInit_;
	Env env_; // store all the paths used in one string
	std::vector<std::string> all_used_paths_vector_; // store all the paths in many strings
	ACE_Thread_Mutex mutex_;
	UnorderedMap< std::string, ResourceObjectRefAutoPtr> respool_; // store all the opened res

	ResourceManager();
	virtual ~ResourceManager();

	virtual int handle_timeout(const ACE_Time_Value &current_time, const void * data = 0);

	bool init_watchers();

	/// 初始化所有资源对应的路径
	/// initialize the paths of all the resources
	bool init();

	/// 所有路径的分隔符统一调整为 斜杠 /
	/// ajust path slash /
	void adjust_paths();

	/// 遍历检查每一个res的有效性，删除过期或者无效的res
	/// loop every resource objct and delete the overdue or invalid objects
	void update_respool();

	/// 遍历所有的资源路径(环境变量中指定的)，匹配到完整的资源地址
	/// 检查文件是否存在，存在返回完整的路径，否则将文件名称原封不动的返回
	/// get resource complete path based on the given resource name
	std::string get_res_path(const std::string& res_name) { return get_res_path(res_name.c_str()); };
	std::string get_res_path(const char* res_name);

	/// 遍历所有的资源路径(环境变量中指定的)，匹配到完整的资源地址
	/// 检查文件是否存在，存在返回true，否则返回false
	/// Tested
	bool exist(const std::string res);

	/// 遍历所有的资源路径(环境变量中指定的)，匹配到完整的资源地址 使用该路径打开资源
	/// open the resource based on the conpleted path
	FILE* open_res_fd(const std::string res_name, const char* mode = "r");
	ResourceObjectRefAutoPtr open_res_obj(const char* res, const char* model = "r",
		ACE_UINT32 flags = RESOURCE_NORMAL);
	/// 获得当前编译时的文件夹路径，并由此获取根文件夹路径，将所有需要的完整路径都
	/// 存入env.all_res_paths中，以；来分割不同的文件夹路径
	/// Tested
	void auto_set_env_res_paths(void);

	/** Sample Results for this project dir structure

	ZMD_ROOT_PATH ( env_.root_path )
	{ D:/Programming/VS2013Projects/zmd-server-copy/ }

	ZMD_BINS_PATH ( env_.bin_path )
	{ D:/Programming/VS2013Projects/zmd-server-copy/zmd/bins/ }

	ZMD_ALL_USED_PATHS ( env_.all_used_paths )
	{
	D:/Programming/VS2013Projects/zmd-server-copy/zmd/resources/;
	D:/Programming/VS2013Projects/zmd-server-copy/zmd/assets/;
	D:/Programming/VS2013Projects/zmd-server-copy/assets/scripts/;
	D:/Programming/VS2013Projects/zmd-server-copy/assets/resources/
	}

	USER_SCRIPTS_PATH ( env_.python_user_scripts_path )
	{ D:/Programming/VS2013Projects/zmd-server-copy/assets/scripts/ }

	USER_RESOURCES_PATH ( env_.python_user_res_path )
	{ D:/Programming/VS2013Projects/zmd-server-copy/assets/resources/ }

	PYTHON_NATIVE_LIBS_PATH ( env_.python_native_libs_path )
	{ D:/Programming/VS2013Projects/zmd-server-copy/zmd/resources/scripts/common/Lib/ }
	*/
	void print_env_paths(void);

};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace/post.h"
#endif
