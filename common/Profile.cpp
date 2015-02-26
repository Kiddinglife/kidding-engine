#include "Profile.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

Profile::Profile(std::string name, Profiles* profiles)
{
}


Profile::~Profile()
{
}

void Profile::start(void)
{
	TRACE("Profile::start()");

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

	TRACE_RETURN_VOID();
}

void Profile::stop(ACE_UINT32 qty)
{
	TRACE("Profile::stop()");

	TimeStamp now = timestamp();

	/// 如果为0则表明自己是调用栈的产生着 在此我们可以得到这个函数总共耗费的时间
	if( --inProgress_ == 0 )
	{
		lastTime_ = now - lastTime_;
		sumInternalTime_ += lastTime_;
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

	TRACE_RETURN_VOID();
}

bool Profile::initializeWatcher(void)
{
	TRACE("Profile::initializeWatcher()");
	TRACE_RETURN(true);
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL