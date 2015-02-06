#ifndef ACE_KBE_OBJECTPOOL_HPP_
#define ACE_KBE_OBJECTPOOL_HPP_

#include "ace/pre.h"
#include "config.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Synch_Traits.h"
#include "ace/Assert.h"
#include "ace/Basic_Types.h"
#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/mytrace.h"
#include "ace/Thread_Mutex.h"
#include "ace/Null_Mutex.h"
#include "ace/Singleton.h"
#if (ACE_HAS_STANDARD_CPP_LIBRARY == 1) && (ACE_USES_STD_NAMESPACE_FOR_STDCPP_LIB == 1)
#include <map>	
#include <list>	
#include <vector>
#include <queue> 
#include <memory>
using namespace std;
#else
#include "ace/Hashable.h"
#include "ace/Free_List.h"
#include "ace/Unbounded_Queue.h"
#include "ace/Array_Map.h"
#endif

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL /////////////////////////////////////////////////////////////

#define NEW_POOL_OBJECT(TYPE) TYPE::ObjPool().createObject();
#define OBJECT_POOL_INIT_SIZE	16
#define OBJECT_POOL_INIT_MAX_SIZE	OBJECT_POOL_INIT_SIZE * 16

/**
* @class ObjectPool
* @brief  ¶ÔÏó³Ø£¬@type T ±ØÐë¼Ì³Ð @class PoolObject
* ²»ÄÜÓ¦ÓÃÓÚbasic type ÀýÈçint, Ä¬ÈÏÊÇÓÐËø¡£
*/
template< typename T, typename ACE_LOCK = ACE_Thread_Mutex> class ObjectPool //¶àÏß³Ì
{
	protected:
	typedef list<T*> OBJECTS;
	OBJECTS objects_;							// ¶ÔÏó»º³åÆ÷
	size_t maxCount_; //the up numbers of objects in this pool
	bool isDestroyed_;
	ACE_LOCK mutex_; 	///KBEngine::thread::ThreadMutex mutex_;
	std::string name_;
	size_t totalAlloc_; // total number of objects that are assigned 
	size_t obj_count_; //curretn number of objects in this pool

	public:
	ObjectPool(string name) :
		objects_(), //ÏÔÊ½µ÷ÓÃÄ¬ÈÏ¹¹Ôìº¯Êý£¬·ÀÖ¹³öÏÖ¶à¸öÄ¬ÈÏ¹¹Ôìº¯ÊýµÄ´íÎó
		maxCount_(OBJECT_POOL_INIT_MAX_SIZE), isDestroyed_(false), mutex_(),
		name_(name), totalAlloc_(0), obj_count_(0)
	{
	}

	ObjectPool(string naem, ACE_UINT32 preAssignVal, size_t max) :
		objects_(), maxCount_(( max == 0 ? 1 : max )), isDestroyed_(false),
		mutex_(), name_(name), totalAlloc_(0), obj_count_(0)
	{

	}

	~ObjectPool()
	{
		destroy();
	}

	size_t maxCount()const { return maxCount_; }
	size_t totalAlloc()const { return totalAlloc_; }
	size_t obj_count(void)const { return obj_count_; }
	bool isDestroyed()const { return isDestroyed_; }
	const OBJECTS& objects(void) const { return objects_; }

	///destroy all the assigned objects in this pool
	void destroy(void)
	{
		mutex_.acquire();
		isDestroyed_ = true;
		typename OBJECTS::iterator iter = objects_.begin();
		for( ; iter != objects_.end(); iter++ )
		{
			if( !( *iter )->destructorPoolObject() )
			{
				delete ( *iter );
			}
		}

		objects_.clear();
		obj_count_ = 0;
		mutex_.release();
	}

	///pre-assign @OBJECT_POOL_INIT_SIZE objects in this pool
	void assign_objects(ACE_UINT32 preAssignVal = OBJECT_POOL_INIT_SIZE)
	{
		while( preAssignVal > 0 )
		{
			objects_.push_back(new T);
			++totalAlloc_;
			++obj_count_;
			--preAssignVal;
		}
	}

	///Ç¿ÖÆ×ªÐÍ´´½¨Ò»¸öÖ¸¶¨ÀàÐÍµÄ¶ÔÏó¡£ Èç¹û»º³åÀïÒÑ¾­´´½¨Ôò·µ»ØÏÖÓÐµÄ£¬
	///·ñÔò´´½¨Ò»¸öÐÂµÄ£¬ Õâ¸ö¶ÔÏó±ØÐëÊÇ¼Ì³Ð×ÔTµÄ¡£
	///return a T type object if it is n the pool otherwise create a new one
	///@OBJECT_TYPE is the child of @T
	template<typename OBJECT_TYPE> T* createObject(void)
	{
		T* t = 0;
		mutex_.acquire();
		while( true )
		{
			if( obj_count_ > 0 )
			{
				t = static_cast<OBJECT_TYPE*>( *objects_.begin() );
				objects_.pop_front();
				--obj_count_;
				mutex_.release();
				return t;
			}
			assign_objects();
		}
		mutex_.release();
		ACE_ERROR(( LM_ERROR, "createObject -> unkonow err\n" ));
	}

	///»ØÊÕÒ»¸ö¶ÔÏó  reclaim  an object 
	void reclaimObject_(T* obj)
	{
		if( obj == nullptr )
		{
			ACE_ERROR(( LM_ERROR, "reclaimObject_ -> obj cannot be NULL" ));
			return;
		}

		if( obj_count_ >= maxCount_ || isDestroyed_ )
		{
			delete obj;
			--totalAlloc_;
		} else
		{
			objects_.push_back(obj);
			++obj_count_;
		}

	}

	///»ØÊÕÒ»¸ö¶ÔÏó reclaim  an object 
	void reclaimObject(T* obj)
	{
		mutex_.acquire();
		reclaimObject_(obj);
		mutex_.release();
	}

	///»ØÊÕÒ»¸ö¶ÔÏó³Ø reclaim an object pool
	void reclaimObject(list<T*>& objs)
	{
		mutex_.acquire();
		typename list< T* >::iterator iter = objs.begin();
		for( ; iter != objs.end(); iter++ )
		{
			reclaimObject_(( *iter ));
		}
		objs.clear();
		mutex_.release();
	}

	///»ØÊÕÒ»¸ö¶ÔÏó³Ø reclaim an object pool
	void reclaimObject(vector< T* >& objs)
	{
		mutex_.acquire();

		typename vector< T* >::iterator iter = objs.begin();
		for( ; iter != objs.end(); iter++ )
		{
			reclaimObject_(( *iter ));
		}

		objs.clear();
		mutex_.release();
	}

	///»ØÊÕÒ»¸ö¶ÔÏó³Ø reclaim an object pool
	void reclaimObject(queue<T*>& objs)
	{
		mutex_.acquire();

		while( !objs.empty() )
		{
			T* t = objs.front();
			objs.pop();
			reclaimObject_(t);
		}

		mutex_.release();
	}

	void dump(void)
	{
		ACE_DEBUG(( LM_INFO,
			"objectpool::name=%s, objs=%d/%d, isdestroyed=%s, ACE_LOCK=%s\n",
			name_.c_str(), (int) obj_count_, (int) maxCount_, ( isDestroyed_ ? "true" : "false" ),
			( mutex_.acquire() == 0 ? "ace_null_mutex" : "ace_thread_mutex" )
			));
	}
};

/**
* @class PoolObject
* @brief  ³Ø¶ÔÏó£¬ ËùÓÐÊ¹ÓÃ³ØµÄ¶ÔÏó±ØÐëÊµÏÖ»ØÊÕ¹¦ÄÜ¡£
* @brief  objects in pool need to be recycled
*/
class PoolObject
{
	public:
	virtual ~PoolObject() { }
	virtual void onReclaimObject() = 0;
	virtual size_t getPoolObjectBytes() { return 0; }

	///³Ø¶ÔÏó±»Îö¹¹Ç°Í¨ÖªÄ³Ð©¶ÔÏó¿ÉÒÔÔÚ´Ë×öÉ¨Î²¹¤×÷
	///clearup before object is dtor
	virtual bool destructorPoolObject() { return false; }
};

/**
* @class SmartPoolObject
* @brief  ³Ø¶ÔÏó£¬ ËùÓÐÊ¹ÓÃ³ØµÄ¶ÔÏó±ØÐëÊµÏÖ»ØÊÕ¹¦ÄÜ¡£
* @brief  objects in pool need to be recycled
*/
template< typename T, typename ACE_LOCK> class SmartPoolObject
{
	private:
	T* ptrPoolObject_;
	ObjectPool<T, ACE_LOCK>& refObjectPool_;


	public:
	SmartPoolObject(T* pPoolObject, ObjectPool<T, ACE_LOCK>& objectPool) :
		pPoolObject_(pPoolObject), objectPool_(objectPool)
	{
	}

	~SmartPoolObject()
	{
		reclaim_self();
	}

	public:
	void reclaim_self()
	{
		if( ptrPoolObject_ != NULL )
		{
			refObjectPool_.reclaimObject(ptrPoolObject_);
			ptrPoolObject_ = NULL;
		}
	}

	T* get()
	{
		return ptrPoolObject_;
	}

	T* operator->( )
	{
		return pPoolObject_;
	}

	T& operator*( )
	{
		return *pPoolObject_;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ACE_OBJECT_POOL_INIT_SIZE 1024
///¶ÔÏó³ØÖ¸Õë¶¨Òå
#define ACE_OBJECT_POOL_PTR_SETTER(OBJECTTYPE, POOL)  ACE_ObjectPool* POOL
///»ñµÃÌØ¶¨¶ÔÏó³ØÖ¸Õë
#define ACE_OBJECT_POOL_PTR_GETTER(OBJECTTYPE)  ////
///ACE_ObjectPoolµÄÇ°ÖÃÉùÃ÷
template< typename T, typename ACE_LOCK>  class ACE_ObjectPool;

//struct SmallType {/*½«¶ÔÏó×ª³ÉÇáÐÍ¶ÔÏó£¬Í¬Ê±£¬Ò²ÊÇ±ÜÃâMakeTÊ±T»¹Ã»±»´´½¨µÄÞÏÞÎ*/ };

/**
* @class ACE_ObjectPoolFactory
* @brief ³Ø¶ÔÏó±»»ØÊÕÊ±£¬»áµ÷ÓÃÆäÎö¹¹º¯Êý£¬ÕâÀï¿ÉÒÔ×öÒ»Ð©ÇåÀí¹¤×÷£¬
* ÀýÈç£¬Îö¹¹¸Ã¶ÔÏóµÄ¶¯Ì¬ÄÚ´æ Ò²¿ÉÒÔ²»Îö¹¹ ±ÈÈçmemorystreamÀïµÄ¶¯Ì¬ÄÚ´æ¡£
*/
template< typename T, typename ACE_LOCK>
class ACE_ObjectPoolFactory
{
	public:
	typedef  ACE_Singleton < ACE_ObjectPoolFactory<T, ACE_LOCK>, ACE_LOCK > Singlton;
	typedef ACE_ObjectPool<T, ACE_LOCK> ACEObjectPool;
	typedef ACEObjectPool* ACEObjectPoolPTR;
	///typedef for ACE_ObjectPool pointer
	typedef std::map<std::string, ACEObjectPoolPTR> PoolMap;

	private:
	friend class ACE_Singleton < ACE_ObjectPoolFactory<T, ACE_LOCK>, ACE_LOCK > ;

	PoolMap mPoolMap;
	ACE_LOCK mLocker;
	ACE_ObjectPoolFactory() :mLocker(), mPoolMap()
	{
		//TRACE("ACE_ObjectPoolFactory -> ctor()");
		//TRACE_RETURN_VOID();
	}

	public:
	~ACE_ObjectPoolFactory()
	{
		//TRACE("ACE_ObjectPoolFactory -> dtor()");
		gc_all_pools();
		//TRACE_RETURN_VOID();
	}

	///get object pool
	ACEObjectPoolPTR get_obj_pool(/*const SmallType& t2t, */ const std::string& sname)
	{
		//TRACE("ACE_ObjectPoolFactory -> get_obj_pool()");
		mLocker.acquire();
		ACEObjectPoolPTR ptr = nullptr;
		PoolMap::iterator it = mPoolMap.find(sname);
		if( it == mPoolMap.end() )
		{
			ptr = new ACEObjectPool;
			mPoolMap.insert(std::make_pair(sname, ptr));
		} else
		{
			ptr = it->second;
		}
		mLocker.release();
		return ptr;
		//TRACE_RETURN(ptr);
	}

	///GCËùÓÐµÄ¶ÔÏó³Ø ¸Ã·½·¨Ö»ÔÚ³ÌÐòÍË³öÊ±Ö´ÐÐ£¬³ÌÐòÖ´ÐÐÊ± Ö®·ÖÅäÄÚ´æ¶ø²»ÊÍ·Å¡£
	void gc_all_pools()
	{
		//TRACE("ACE_ObjectPoolFactory -> gc_all_pools()");
		mLocker.acquire();
		std::for_each(mPoolMap.begin(), mPoolMap.end(),
			[ ](PoolMap::reference a)
		{
			delete a.second;
		});
		mLocker.release();
		//TRACE_RETURN_VOID();
	}
	void dump()
	{
		TRACE("DEBUG ACE_ObjectPoolFactory -> dump()");
		ACE_DEBUG(( LM_INFO, "DEBUG ACE_Pool_Factory Dump Result:\n" ));
		ACE_DEBUG(( LM_INFO, "DEBUG Total Pools: %d\n", mPoolMap.size() ));
		std::for_each(mPoolMap.begin(), mPoolMap.end(),
			[ ](std::map<std::string, ACEObjectPoolPTR>::reference a)
		{
			ACE_DEBUG(( LM_INFO, "DEBUG PoolName: %s, ", a.first.c_str() ));
			a.second->dump();
			ACE_DEBUG(( LM_INFO, "\n" ));
		});

		//ACE_DEBUG
		//	(( LM_TRACE,
		//	ACE_TEXT("%s%*s(%t) leaving %s in file `%s'")
		//	ACE_TEXT(" on line %d\n"),
		//	this->prefix_,
		//	Trace::nesting_indent_ * lm->dec(),
		//	ACE_TEXT(""),
		//	this->name_,
		//	this->file_,
		//	this->line_ ));
		//TRACE_RETURN_VOID();
	}
};

/**
* @class IACE_ObjectPool
* @brief ¶ÔÏó³Ø»ùÀà½Ó¿Ú£¬Ìá¹©ÁËgcÀ¬»øÊÕ¼¯µÄÄÜÁ¦
* £¡£¡£¡Ö®·ÖÅäÄÚ´æ²»ÊÇ·ÅµÄ»° Ã»ÄãÒªÊ¹ÓÃÕâ¸ö½Ó¿Ú£¡£¡£¡
*/
//class IACE_ObjectPool
//{
//	public:
//	//gabby collection
//	virtual void GabageCollection(bool bEnforceGC) = 0;
//	virtual ~IACE_ObjectPool() { }
//};

/**
* @class ACE_ObjectPool
* @brief ¶ÔÏó³Ø»ùÀà½Ó¿Ú£¬Ìá¹©ÁËgcÀ¬»øÊÕ¼¯µÄÄÜÁ¦
*/
template< typename T, typename ACE_LOCK = ACE_Thread_Mutex>
class ACE_ObjectPool /*: public IACE_ObjectPool //Ö®·ÖÅäÄÚ´æ²»ÊÇ·ÅµÄ»° Ã»ÄãÒªÊ¹ÓÃ*/
{
	public: ///typedefs
	typedef T                                                    ObjectType;
	typedef T*                                                   ObjectPtr;
	typedef std::map<ACE_UINT32, ACE_Byte*> FreePointer;
	typedef std::vector<ObjectPtr>                   FreeIndex;
	typedef ACE_Thread_Mutex                         Locker;

	private:
	FreePointer                  mFreePointerIndexes;       //¿ÕÏÐÖ¸ÕëË÷Òýmap,keyÎªgrowSize
	FreeIndex                     mFreeIndexes;                    //¿ÕÏÐË÷ÒýÁÐ±í

	//!!!Ö®·ÖÅäÄÚ´æ²»ÊÇ·ÅµÄ»° Ã»ÄãÒªÊ¹ÓÃÒ»ÏÂtypedefs!!!
	//typedef std::vector <ObjectPtr>      deleteIndex;
	//typedef std::map<ObjectPtr, bool> UnusedIndex; 	/// ¹¹ÔìÒ»¸ömapÀ´Ê¹²éÕÒm_FreeIndexs¸ü¼Ó¿ì½ÝÒ»Ð©
	//UnusedIndex                mUnusedObjectsStatus;                //¶ÔÏó·ÖÅä×´Ì¬(·ÖÅäºÍÎ´·ÖÅä)

	ACE_UINT32                 mGrowSize;                        //ÄÚ´æÔö³¤µ½Ð¡
	ACE_UINT32                 mUnusedCount;  //µ±Ç°Î´·ÖÅä¶ÔÏóµÄÊýÁ¿
	ACE_UINT32                 mUsedCount;  //µ±Ç°ÒÑ·ÖÅä¶ÔÏóµÄÊýÁ¿
	ACE_UINT32                 mTotalCount;  //¸ÃpoolËùÓÐ¶ÔÏóµÄÊýÁ¿µÈÓÚmUnusedObjectsCount+mUsedObjectsCount
	ACE_LOCK                    mLocker;

	public:  ///ctor and dtor
	ACE_ObjectPool() :
		mGrowSize(ACE_OBJECT_POOL_INIT_SIZE), mFreePointerIndexes() /*,mUnusedObjectsStatus()*/,
		mFreeIndexes(), mLocker(), mUnusedCount(0), mUsedCount(0), mTotalCount(0)
	{
		//TRACE("ACE_ObjectPool -> ctor()");
		Grow();
		//TRACE_RETURN_VOID();
	}

	virtual ~ACE_ObjectPool()
	{
		//TRACE("ACE_ObjectPool -> dtor()");
		gc();
		//TRACE_RETURN_VOID();
	}

	void Grow()
	{
		//TRACE("ACE_ObjectPool -> Grow()");

		ACE_Byte* ptr = nullptr;
		ACE_UINT32 objSize = sizeof(ObjectType);
		do
		{
			ptr = new ACE_Byte[mGrowSize*objSize];
		} while( ptr == nullptr );

		//¼ÓÈëÖ¸ÕëmapÖÐÈ¥
		mFreePointerIndexes.insert(std::make_pair(mGrowSize, ptr));
		//¼ÓÈë¿ÕÏÐË÷ÒýÖÐ
		ObjectPtr objectPtr = reinterpret_cast<ObjectPtr>( ptr );
		for( ACE_UINT32 i = 0; i < mGrowSize; ++i )
		{
			mFreeIndexes.push_back(objectPtr + i);
		}

		mUnusedCount += mGrowSize;
		mTotalCount += mGrowSize;

		//ÏÂ´ÎÔö³¤Ò»±¶
		mGrowSize *= 2;

		//TRACE_RETURN_VOID();
	}
	//TRACE("ACE_ObjectPool -> gc()");
	//TRACE_RETURN_VOID();
	//TRACE_RETURN();

	ACE_Byte* GetFreePointer()
	{
		//TRACE("ACE_ObjectPool -> GetFreePointer()");
		if( mFreeIndexes.empty() ) Grow();
		ACE_Byte* ptr = reinterpret_cast<ACE_Byte*>( mFreeIndexes.back() );
		mFreeIndexes.pop_back();
		++mUsedCount;
		--mUnusedCount;
		return ptr;
	//	TRACE_RETURN(ptr);
	}

	///´´½¨Ò»¸ö¶ÔÏó[Ä¬ÈÏ¹¹Ôìº¯Êý]
	///construct an object from pool [default ctor]
	ObjectPtr Ctor()
	{
		//TRACE("ACE_ObjectPool -> Ctor()");
		mLocker.acquire();
		ACE_Byte* ptr = GetFreePointer();
		ObjectPtr ptrr = new (ptr) ObjectType();
		mLocker.release();
		return ( ptrr == nullptr ) ? nullptr : ptrr;
		//TRACE_RETURN(( ptrr == nullptr ) ? nullptr : ptrr);
	}

	///´´½¨Ò»¸ö¶ÔÏó[Ò»¸ö²ÎÊý¹¹Ôìº¯Êý] »ù±¾ÀàÐÍ+¶ÔÏóÒýÓÃ[µ÷ÓÃ¸Ã¶ÔÏó¸´ÖÆctor]
	///construct an object from pool [using one para]
	template<typename Type1>
	ObjectPtr Ctor(Type1& para1)
	{
		//TRACE("ACE_ObjectPool -> Ctor(Type1& para1)");
		mLocker.acquire();
		ACE_Byte* ptr = GetFreePointer();
		ObjectPtr ptrr = new (ptr) ObjectType(para1);
		mLocker.release();
		return ( ptrr == nullptr ) ? nullptr : ptrr;
		//TRACE_RETURN(( ptrr == nullptr ) ? nullptr : ptrr);
	}
	template<typename Type1, typename Type2>
	ObjectPtr Ctor(Type1& para1, Type2& para2)
	{
		mLocker.acquire();
		ACE_Byte* ptr = GetFreePointer();
		ObjectPtr ptrr = new (ptr) ObjectType(para1, para2);
		mLocker.release();
		return ( ptrr == nullptr ) ? nullptr : ptrr;
	}
	template<typename Type1, typename Type2, typename Type3>
	ObjectPtr Ctor(Type1& para1, Type2& para2, Type3& para3)
	{
		mLocker.acquire();
		ACE_Byte* ptr = GetFreePointer();
		ObjectPtr ptrr = new (ptr) ObjectType(para1, para2, para3);
		mLocker.release();
		return ( ptrr == nullptr ) ? nullptr : ptrr;
	}
	template<typename Type1, typename Type2, typename Type3, typename Type4>
	ObjectPtr Ctor(Type1& para1, Type2& para2, Type3& para3, Type4& para4)
	{
		mLocker.acquire();
		ACE_Byte* ptr = GetFreePointer();
		ObjectPtr ptrr = new (ptr) ObjectType(para1, para2, para3, para4);
		mLocker.release();
		return ( ptrr == nullptr ) ? nullptr : ptrr;
	}
	template<typename Type1, typename Type2, typename Type3, typename Type4, typename Type5>
	ObjectPtr Ctor(Type1& para1, Type2& para2, Type3& para3, Type4& para4, Type5& para5)
	{
		mLocker.acquire();
		ACE_Byte* ptr = GetFreePointer();
		ObjectPtr ptrr = new (ptr) ObjectType(para1, para2, para3, para4, para5);
		mLocker.release();
		return ( ptrr == nullptr ) ? nullptr : ptrr;
	}
	template<typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6>
	ObjectPtr Ctor(Type1& para1, Type2& para2, Type3& para3, Type4& para4, Type5& para5, Type6& para6)
	{
		mLocker.acquire();
		ACE_Byte* ptr = GetFreePointer();
		ObjectPtr ptrr = new (ptr) ObjectType(para1, para2, para3, para4, para5, para6);
		mLocker.release();
		return ( ptrr == nullptr ) ? nullptr : ptrr;
	}

	// Ïú»ÙÒ»¸ö¶ÔÏó
	void Dtor(ObjectPtr const  _ptr)
	{
		//TRACE("ACE_ObjectPool -> Dtor()");

		mLocker.acquire();
		_ptr->~ObjectType();
		//_ptr = static_cast<ACE_Byte*>( _ptr );
		mFreeIndexes.push_back(_ptr);
		--mUsedCount;
		++mUnusedCount;
		mLocker.release();

		//TRACE_RETURN_VOID();
	}

	//mLocker.acquire();
	//mLocker.release();

	/// !!Ö®·ÖÅäÄÚ´æ²»ÊÇ·ÅµÄ»° Ã»ÄãÒªÊ¹ÓÃÕâ¸ö½Ó¿Ú !!!
	///»ØÊÕ³ØÖÐ  ÎÒ¸Ð¾õÃ»±ØÒª»ØÊÕÄÚ´æ Ö»ÐèÒª°ÑÄã´æ·ÇÅãÁ£¶È±äÐ¡¼´¿É¡£
	//·ÇÅãÒ»¶¨ÊýÁ¿ºó£¬¿Ï¶¨ÄÜ¹»»á¹»ÓÃ¡£  
	//¸Ãº¯Êý»áÊÍ·ÅÄÚ´æ µ«Ö»ÔÚ³ÌÐòÍË³öÊ±·¢Éú
	void gc(/*bool bEnforce = false*/)
	{
		//TRACE("ACE_ObjectPool -> gc()");

		mLocker.acquire();
		std::for_each(mFreePointerIndexes.begin(), mFreePointerIndexes.end(),
			[ ](typename FreePointer::reference a)
		{
			delete[ ] a.second;
		});
		mLocker.release();

		//TRACE_RETURN_VOID();
	}

	void dump()
	{
		ACE_DEBUG(( LM_INFO,
			"Total objects in pool:%d, "
			"Used objects:%d, "
			"Unused objects:%d",
			mTotalCount,
			mUsedCount,
			mUnusedCount ));
	}

};

#define ACE_PoolPtr_Getter(PoolName,ObjectType, LockerType) \
ACE_ObjectPool<ObjectType,LockerType>* PoolName = \
ACE_ObjectPoolFactory<ObjectType, LockerType>::Singlton::instance()->get_obj_pool("ACE_"#ObjectType"_Pool")

#define ACE_PoolFactoryPtr_Getter(PoolFactoryName,ObjectType, LockerType) \
ACE_ObjectPoolFactory<ObjectType, LockerType>* PoolFactoryName = \
ACE_ObjectPoolFactory<ObjectType, LockerType>::Singlton::instance()

/**
* @class ACEPoolObject_Auto_Ptr
*
* @brief Implements the draft C++ standard auto_ptr abstraction.
* This class allows one to work on non-object (basic) types
*/
template <typename PoolObjectType, typename ACE_LOCK>
class ACEPoolObject_Auto_Ptr
{
	private:
	typedef PoolObjectType element_type;
	typedef element_type* element_type_ptr;
	ACE_LOCK mLocker;
	element_type_ptr p_;

	void reclaim_pool_object()
	{
		//TRACE("ACEPoolObject_Auto_Ptr::reclaim_pool_object()");
		ACE_PoolPtr_Getter(ObjPool, PoolObjectType, ACE_LOCK);
		ObjPool->Dtor(p_);
		//TRACE_RETURN_VOID();
	}

	public: 	// = Initialization and termination methods
	explicit ACEPoolObject_Auto_Ptr(element_type_ptr p = 0) : p_(p), mLocker()
	{
		//TRACE("ACEPoolObject_Auto_Ptr::ctor()");
		//TRACE_RETURN_VOID();
	}
	ACEPoolObject_Auto_Ptr(ACEPoolObject_Auto_Ptr<element_type, ACE_LOCK> & ap) : p_(rhs.release())
	{
		//TRACE("ACEPoolObject_Auto_Ptr::copy ctor()");
		//TRACE_RETURN_VOID();
	}


	ACEPoolObject_Auto_Ptr<element_type, ACE_LOCK>& operator= ( ACEPoolObject_Auto_Ptr
		< element_type, ACE_LOCK >& rhs )
	{
		//TRACE("ACE_Auto_Basic_Ptr<X>::operator=()");
		if( this != &rhs )
		{
			this->reset(rhs.release());
		}
		//TRACE_RETURN(*this);
	}
	virtual ~ACEPoolObject_Auto_Ptr(void)
	{
		//TRACE("ACEPoolObject_Auto_Ptr :: dtor()");
		reclaim_pool_object();
		//TRACE_RETURN_VOID();
	}

	// = Accessor methods.
	element_type& operator *( ) const
	{
		//TRACE("ACE_Auto_Basic_Ptr<X>::operator *()");
		//TRACE_RETURN(p_);
	}

	element_type_ptr get(void) const
	{
		//mLocker.acquire(););
		//mLocker.release();
		//TRACE("ACEPoolObject_Auto_Ptr->get()");
		//TRACE_RETURN(p_);
	}

	element_type_ptr release(void)
	{
		//mLocker.acquire();
		//TRACE("ACEPoolObject_Auto_Ptr->release()");
		element_type_ptr old = this->p_;
		this->p_ = 0;
		//mLocker.acquire();
		//TRACE_RETURN(old);
	}

	void reset(element_type_ptr p = 0)
	{
		mLocker.acquire();
		//TRACE("ACEPoolObject_Auto_Ptr :: reset()");
		if( this->get() != p )
			reclaim_pool_object() //delete this->get();
			this->p_ = p;
		mLocker.acquire();
		//TRACE_RETURN_VOID();
	}

	/// Dump the state of an object.
	void dump(void) const
	{
#if defined (ACE_HAS_DUMP)
		ACE_TRACE("ACEPoolObject_Auto_Ptr->dump()");
#endif /* ACE_HAS_DUMP */
	}

	/// Declare the dynamic allocation hooks.
	///ACE_ALLOC_HOOK_DECLARE;
};

#define ACE_PoolObject_AutoPtr_Getter(autoptr,ObjectType, LockerType) \
ACE_PoolPtr_Getter(objPoolPtr, ObjectType, LockerType);\
ACEPoolObject_Auto_Ptr<ObjectType, LockerType> autoptr(objPoolPtr->Ctor())

ACE_KBE_END_VERSIONED_NAMESPACE_DECL //////////////////////////////////////////////////////////////////
#include "ace/post.h"
#endif