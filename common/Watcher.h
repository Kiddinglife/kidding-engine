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

template <class T>
struct ValueWatcher : public Watcher
{
	const T& watchVal_;
	T val_;

	ValueWatcher(std::string path, const T& pVal) :Watcher(path), watchVal_(pVal) { }
	virtual ~ValueWatcher() { }

	virtual void addToInitStream(Bundle* s)
	{
		( *s ) << path_.c_str() << name_ << id_ << get_type_tpl<T>() << watchVal_;
	}

	virtual void addToStream(Bundle* s)
	{
		( *s ) << id_ << watchVal_;
	}

	T getValue() { return watchVal_; }
	virtual WATCHER_VALUE_TYPE get_type_vtl() { return get_type_tpl<T>(); }
};

template <class RETURN_TYPE>
struct FunctionWatcher : public Watcher
{
	typedef RETURN_TYPE(*FUNC)( );
	FUNC func_;

	FunctionWatcher(std::string path, FUNC func) :
		Watcher(path),
		func_(func)
	{
	}

	virtual ~FunctionWatcher()
	{
	}

	virtual void addToInitStream(Bundle* s)
	{
		( *s ) << path_ << name_ << id_ << get_type_tpl<RETURN_TYPE>() << ( *func_ )( );
	};

	virtual void addToStream(Bundle* s)
	{
		( *s ) << id_ << ( *func_ )( );
	};

	RETURN_TYPE getValue() { return ( *func_ )( ); }
	virtual WATCHER_VALUE_TYPE get_type_vtl() { return get_type_tpl<RETURN_TYPE>(); }
};

/*
watcher: 监视一个成员函数返回的值
*/
template <class RETURN_TYPE, class OBJ_TYPE>
struct MethodWatcher : public Watcher
{
	typedef RETURN_TYPE(OBJ_TYPE::*FUNC)( );
	FUNC func_;
	OBJ_TYPE* obj_;

	MethodWatcher(std::string path, OBJ_TYPE* obj, RETURN_TYPE(OBJ_TYPE::*func)( )) :
		Watcher(path),
		func_(func),
		obj_(obj)
	{
	}

	virtual ~MethodWatcher()
	{
	}

	void addToInitStream(Bundle* s)
	{
		( *s ) << path_ << name_ << id_ << get_type_tpl<RETURN_TYPE>() << ( obj_->*func_ )( );
	};

	void addToStream(Bundle* s)
	{
		( *s ) << id_ << ( obj_->*func_ )( );
	};

	RETURN_TYPE getValue() { return ( obj_->*func_ )( ); }
	WATCHER_VALUE_TYPE get_type_vtl() { return get_type_tpl<RETURN_TYPE>(); }
};

template <class RETURN_TYPE, class OBJ_TYPE>
struct ConstMethodWatcher : public Watcher
{
	public:
	typedef RETURN_TYPE(OBJ_TYPE::*FUNC)( )const;

	ConstMethodWatcher(std::string path, OBJ_TYPE* obj, RETURN_TYPE(OBJ_TYPE::*func)( )const) :
		Watcher(path),
		func_(func),
		obj_(obj)
	{
	}

	virtual ~ConstMethodWatcher()
	{
	}

	void addToInitStream(Bundle* s)
	{
		RETURN_TYPE v = ( obj_->*func_ )( );
		( *s ) << path_ << name_ << id_ << get_type_tpl<RETURN_TYPE>() << v;
	};

	void addToStream(Bundle* s)
	{
		( *s ) << id_ << ( obj_->*func_ )( );
	};

	RETURN_TYPE getValue() { return ( obj_->*func_ )( ); }
	WATCHER_VALUE_TYPE get_type_vtl() { return get_type_tpl<RETURN_TYPE>(); }

	protected:
	FUNC func_;
	OBJ_TYPE* obj_;
};

/*
watcher管理器
*/
struct Watchers
{
	typedef UnorderedMap<std::string, Shared_ptr< Watcher > > WatchersMap;
	WatchersMap watcherObjs_;

	Watchers();
	~Watchers();

	void clear();

	static Watchers& rootWatchers();

	void addToStream(Bundle* s);

	void readWatchers(Bundle* s);

	bool addWatcher(const std::string& path, Watcher* pwo);
	bool delWatcher(const std::string& name);
	bool hasWatcher(const std::string& name);

	Shared_ptr< Watcher > getWatcher(const std::string& name);

	void updateStream(Bundle* s);

	WatchersMap& get_watchers_map() { return watcherObjs_; }
};

struct WatcherPaths
{
	typedef UnorderedMap<std::string, Shared_ptr<WatcherPaths> > WatcherPathsMap;
	WatcherPathsMap watcherPaths_;
	Watchers watchers_;

	WatcherPaths();
	~WatcherPaths();

	WatcherPaths& WatcherPaths::root();
	void WatcherPaths::addToStream(Bundle* s);
	void WatcherPaths::updateStream(Bundle* s);
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif