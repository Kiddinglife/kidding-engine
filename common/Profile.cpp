#include "Profile.h"
#include "ace\Singleton.h"
#include "ace\Null_Mutex.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

ACE_UINT64 runningTime()
{
	return ACE_Singleton<Profiles, ACE_Null_Mutex>::instance()->runningTime();
}

/////////////////////////////////////////////////////////////////////////////////////////
Profiles::Profiles(std::string name) : name_(name)
{
	stampsPerSecond();
	Profile* pRunningTime = new Profile("RunningTime", this);
	pRunningTime->start();
}
Profiles::~Profiles()
{
	delete this->pRunningTime();
}
bool Profiles::initializeWatcher()
{
	return true;
}
TimeStamp Profiles::runningTime() const
{
	return timestamp() - this->pRunningTime()->lastTime_;
}
void Profiles::add(Profile * pVal)
{
	profiles_.push_back(pVal);
}
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////
TimeStamp Profile::warningPeriod_(stampsPerSecond() / 100); // 10ms
Profile::Profile(std::string name, Profiles* profiles) :
strName_(name),
pProfiles_(profiles),
lastTime_(0),
sumTime_(0),
lastInternalTime_(0),
sumInternalTime_(0),
lastVal2Stop_(0),
sumAllVals2Stop_(0),
stopCounts_(0),
inProgress_(0),
initWatcher_(false)
{
	if( pProfiles_ == NULL )
	{
		pProfiles_ = ACE_Singleton<Profiles, ACE_Null_Mutex>::instance();
	}

	if( !strName_.empty() )
	{
		pProfiles_->add(this);
	}
}
Profile::~Profile()
{
	if( pProfiles_ )
	{
		// pProfileGroup_.erase(std::remove( pProfileGroup_->begin(), pProfileGroup_->end(), this ), pProfileGroup_->end());
	}
}
void Profile::start(void)
{
	//TRACE("Profile::start()");

	if( !initWatcher_ && stopCounts_ > 10 )
	{
		initializeWatcher();
	}

	TimeStamp now = timestamp();

	// 记录第几次处理
	if( inProgress_++ == 0 )
	{
		lastTime_ = now;
	}

	Profiles::ProfilesType& stack = pProfiles_->stack_;
	if( !stack.empty() )
	{
		Profile* profile = stack.back();
		profile->lastInternalTime_ = now - profile->lastInternalTime_;
		profile->sumInternalTime_ += profile->lastInternalTime_;
	}

	stack.push_back(this);
	lastInternalTime_ = now;

	//TRACE_RETURN_VOID();
}
void Profile::stop(ACE_UINT32 qty)
{
	//TRACE("Profile::stop()");

	TimeStamp now = timestamp();

	/// 如果为0则表明自己是调用栈的产生着 在此我们可以得到这个函数总共耗费的时间
	if( --inProgress_ == 0 )
	{
		lastTime_ = now - lastTime_;
		sumTime_ += lastTime_;
	}

	lastVal2Stop_ = qty;
	sumAllVals2Stop_ += qty;
	++stopCounts_;

	Profiles::ProfilesType& stack = pProfiles_->stack_;
	stack.pop_back();

	// 得到本函数调用所耗费的时间
	lastInternalTime_ = now - lastInternalTime_;
	sumInternalTime_ += lastInternalTime_;

	// 我们需要在此重设上一个函数中的profile对象的最后一次内部时间
	// 使其能够在start时正确得到自调用完本函数之后进入下一个profile函数中时所消耗
	// 的时间片段
	if( !stack.empty() )
		stack.back()->lastInternalTime_ = now;

	//TRACE_RETURN_VOID();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
ACE_KBE_END_VERSIONED_NAMESPACE_DECL