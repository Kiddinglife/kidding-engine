#ifndef Watcher_H_
#define Watcher_H_

#include "ace\pre.h"
#include "common\common.h"
#include "net\Bundle.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

typedef ACE_UINT16 WATCHER_ID;
typedef ACE_UINT8   WATCHER_VALUE_TYPE;

#define WATCHER_VALUE_TYPE_UNKNOWN				0
#define WATCHER_VALUE_TYPE_UINT8				            1
#define WATCHER_VALUE_TYPE_UINT16				        2
#define WATCHER_VALUE_TYPE_UINT32				        3
#define WATCHER_VALUE_TYPE_UINT64				        4
#define WATCHER_VALUE_TYPE_INT8					        5
#define WATCHER_VALUE_TYPE_INT16				            6
#define WATCHER_VALUE_TYPE_INT32				            7
#define WATCHER_VALUE_TYPE_INT64				            8
#define WATCHER_VALUE_TYPE_FLOAT				        9
#define WATCHER_VALUE_TYPE_DOUBLE				       10
#define WATCHER_VALUE_TYPE_CHAR					       11
#define WATCHER_VALUE_TYPE_STRING				       12
#define WATCHER_VALUE_TYPE_BOOL					       13
#define WATCHER_VALUE_TYPE_COMPONENT_TYPE   14

struct Watchers;
struct Watcher
{
	std::string path_, name_, strval_;
	WATCHER_ID id_;
	ACE_INT32 numWitness_;

	Watcher(std::string path = "NULL");
	virtual ~Watcher();

	virtual void addToInitStream(Bundle* s) { };
	virtual void addToStream(Bundle* s) { };
	template <class T> void updateStream(Bundle* s)
	{
		T v;
		*s >> v;
		strval_ = StringConv::val2str(v);
	}

	void increment_num_witness() { numWitness_++; }
	void decrement_num_witness() { numWitness_--; }

	template <class T>
	WATCHER_VALUE_TYPE get_type_tpl()const { return WATCHER_VALUE_TYPE_UNKNOWN; }
	virtual WATCHER_VALUE_TYPE get_type_vtl() { return WATCHER_VALUE_TYPE_UNKNOWN; }
};

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<ACE_UINT8>()const
{
	return WATCHER_VALUE_TYPE_UINT8;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<ACE_UINT16>()const
{
	return WATCHER_VALUE_TYPE_UINT16;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<ACE_UINT32>()const
{
	return WATCHER_VALUE_TYPE_UINT32;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<ACE_UINT64>()const
{
	return WATCHER_VALUE_TYPE_UINT64;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<ACE_INT8>()const
{
	return WATCHER_VALUE_TYPE_INT8;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<ACE_INT16>()const
{
	return WATCHER_VALUE_TYPE_INT16;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<ACE_INT32>()const
{
	return WATCHER_VALUE_TYPE_INT32;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<ACE_INT64>()const
{
	return WATCHER_VALUE_TYPE_INT64;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<float>()const
{
	return WATCHER_VALUE_TYPE_FLOAT;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<double>()const
{
	return WATCHER_VALUE_TYPE_DOUBLE;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<bool>()const
{
	return WATCHER_VALUE_TYPE_BOOL;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<char*>()const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<const char*>()const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<std::string>()const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<std::string&>()const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<const std::string&>()const
{
	return WATCHER_VALUE_TYPE_STRING;
}

template <>
inline WATCHER_VALUE_TYPE Watcher::get_type_tpl<KBE_SRV_COMPONENT_TYPE>()const
{
	return WATCHER_VALUE_TYPE_COMPONENT_TYPE;
}

template <>
inline void Watcher::updateStream<std::string>(Bundle* s)
{
	( *s ) >> strval_;
}

template <>
inline void Watcher::updateStream<std::string&>(Bundle* s)
{
	updateStream<std::string>(s);
}

template <>
inline void Watcher::updateStream<const std::string&>(Bundle* s)
{
	updateStream<std::string>(s);
}

template <>
inline void Watcher::updateStream<char*>(Bundle* s)
{
	updateStream<std::string>(s);
}

template <>
inline void Watcher::updateStream<const char*>(Bundle* s)
{
	updateStream<std::string>(s);
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif