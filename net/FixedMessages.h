/**
* Reviewed by Jackie Zhang on 20/07/2015 at 21:19 PM
*/

#ifndef FixedMessages_H_
#define FixedMessages_H_

#include "ace\pre.h"
#include <unordered_map>
#include "net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

#define FixedMessagesPtrGetter(FixedMessagesName) \
FixedMessages* FixedMessagesName = \
ACE_Singleton<FixedMessages, ACE_Null_Mutex>::instance();

/**
* struct FixedMessages
*
* @Brief
* 用来对中间协议(前端与后端之间)进行强制约定。
* 没有使用到kbe整套协议自动绑定机制的前端可以使用此处来强制约定协议。
*
* @Notes
* 定长消息指的是参数未固定长度的调用，例如 void hello(int)
* 变长消息指的是参数不固定的条用，例如 void hello(char * str, void* binary_data)
*
* @Notes
* unordered_map与map的区别
* boost::unordered_map， 它与 stl::map的区别就是，stl::map是按照operator<
* 比较判断元素是否相同及比较元素的大小，然后选择合适的位置插入到树中。所以，
* 如果对map进行遍历（中序遍历）的话，输出的结果是有序的。顺序就是按照operator<
* 定义的大小排序。而boost::unordered_map是计算元素的Hash值，根据Hash值判断元素
* 是否相同。所以，对unordered_map进行遍历，结果是无序的。用法的区别就是，stl::map
* 的key需要定义operator< 。 而boost::unordered_map需要定义hash_value函数并且重载
* operator==。对于内置类型，如string，这些都不用操心。对于自定义的类型做key，
* 就需要自己重载operator== 或者hash_value()了。
* unordered_map的插入、删除、查找 性能都优于 hash_map 优于 map，所以首选为
* unordered_map.它的缺陷是元素是无序的，当使用时候需要元素是有序的时候，不可以用它。
* 最后，说，当不需要结果排好序时，最好用unordered_map。
*/
struct FixedMessages
{
	/// 固定的协议数据结构
	struct MSGInfo { MessageID msgid; std::string msgname; };

	typedef UnorderedMap<std::string, MSGInfo> MSGINFO_MAP;
	MSGINFO_MAP infomap_;
	bool loaded_;

	FixedMessages() : infomap_(), loaded_(false) { }
	~FixedMessages() { infomap_.clear(); }

	/**
	* if the msg with the specific name is stored in map, it is  fixed-len msg
	* @param-msgName : it is actually the function signature
	* @ret NULL if it is variable-len msg otherwise the pointer if it is fixed-len msg
	*/
	const FixedMessages::MSGInfo*  isFixed(const std::string& msgName);

	/**
	* if the msg with the specific msgid is stored in map, it is  fixed-len msg
	* @param-msgid : 消息id
	* @ret false if it is variable-len msg or true if it is fixed-len msg
	*/
	const bool  isFixed(const MessageID msgid);

	//@todo
	const bool loadConfig(const std::string fileName);
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif