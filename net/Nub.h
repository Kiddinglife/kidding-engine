#ifndef NUB_H_
#define NUB_H_

#include "ace\pre.h"
#include "ace/Reactor.h"
#include "ace/Reactor_Impl.h"
#include "ace/Timer_Queue.h"
#include "common\timestamp.h"
#include "common\tasks.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct NetworkInterface;
struct ErrorStatMgr;

struct Nub
{
	public:
	ACE_Reactor* rec;
	Tasks frequentTasks_;
	ErrorStatMgr* pErrorReporter_;
	//Timers64* pTimers_;

	// Statistics
	TimeStamp      spareTime_;
	TimeStamp		accSpareTime_;
	TimeStamp		oldSpareTime_;
	TimeStamp		totSpareTime_;
	TimeStamp		lastStatisticsGathered_;
	ACE_UINT32     numTimerCalls_;
	ACE_Time_Value      timeout_;

	Nub::Nub();
	~Nub();

	/// geterrs and setters
	const TimeStamp& spareTime() const
	{
		return spareTime_;
	}
	double proportionalSpareTime()  const
	{
		double ret = totSpareTime_.stamp() - oldSpareTime_.stamp();
		return ret / stampsPerSecondD();
	}
	void zeroSpareTime()  ///clearSpareTime
	{
		accSpareTime_.stamp_ += spareTime_.stamp_;
		spareTime_.stamp_ = 0;
	}
	void increment_numTimerCalls()
	{
		++numTimerCalls_;
	}
	double timeout() const
	{
		return static_cast<double>( timeout_.msec() / 1000 ); // 1.2 sec = 1 sec + 200 ms
	}
	/// Initializes the ACE_Time_Value from a double, which is assumed to be
	/// in second format, with any remainder treated as microseconds.
	///  // 1.2 sec = 1 sec + 200 ms
	void timeout(double seconds)
	{
		timeout_.set(seconds);
	}
	///when inteval = 0, it is a once off timers otherwise it is repeated timer
	long addTimer(
		ACE_Event_Handler *handler,
		void *arg,
		const time_t &delay_time_secs,
		const time_t &interval_secs)
	{
		ACE_Time_Value initialDelay(delay_time_secs);
		ACE_Time_Value interval(interval_secs);
		return rec->schedule_timer(handler, arg, initialDelay, interval);
	}

	int startLoop(ACE_Reactor::REACTOR_EVENT_HOOK eh);
	int startLoop(NetworkInterface* ni);
	int startLoop();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif