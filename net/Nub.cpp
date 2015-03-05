#include "Nub.h"
#include "net\ErrorStatsMgr.h"
#include "net\NetworkInterface.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
//ProfileVal g_idleProfile("Idle");
NETWORK_NAMESPACE_BEGIN_DECL

Nub::Nub() :
rec(ACE_Reactor::instance()),
numTimerCalls_(0),
pErrorReporter_(new ErrorStatMgr(this))
{
	//pErrorReporter_ = new ErrorReporter(*this);
	//Timers64* pTimers_;
}

int Nub::startLoop(ACE_Reactor::REACTOR_EVENT_HOOK eh)
{
	if( rec->reactor_event_loop_done() )
		return 0;

	ACE_UINT64 startTime = 0;
	ACE_Time_Value maxWaitTime(1); // use 1 sec to ensure ther is always a timerout returned
	while( 1 )
	{
		/// process tasks
		frequentTasks_.process();

		///process stats
		if( timestamp() - lastStatisticsGathered_.stamp() >= stampsPerSecond() )
		{
			oldSpareTime_.stamp(totSpareTime_.stamp());
			totSpareTime_.stamp(accSpareTime_.stamp() + spareTime_.stamp());
			lastStatisticsGathered_.stamp(timestamp());
		}


		///process network events

		//calculate max wait time for next epoll
		rec->timer_queue()->calculate_timeout(&maxWaitTime, &timeout_);

#if ENABLE_WATCHERS
		g_idleProfile.start();
#else
		startTime = timestamp();
#endif

		int const result = rec->implementation()->handle_events(timeout_);

#if ENABLE_WATCHERS
		g_idleProfile.stop();
		spareTime_.stamp(spareTime_.stamp() + g_idleProfile.lastTime_);
#else
		//spareTime_.stamp() += timestamp() - startTime;
		spareTime_.stamp(spareTime_.stamp() + timestamp() - startTime);
#endif

		if( ( *eh )( rec ) )
			continue;

		else if( result == -1 && rec->implementation()->deactivated() )
			return 0;
		else if( result == -1 )
			return -1;
	}
	return 0;
}
int Nub::startLoop()
{
	if( rec->reactor_event_loop_done() )
		return 0;

	ACE_UINT64 startTime = 0;
	ACE_Time_Value maxWaitTime(1); // use 1 sec to ensure ther is always a timerout returned
	while( 1 )
	{
		/// process tasks
		frequentTasks_.process();

		///process stats
		if( timestamp() - lastStatisticsGathered_.stamp() >= stampsPerSecond() )
		{
			oldSpareTime_.stamp(totSpareTime_.stamp());
			totSpareTime_.stamp(accSpareTime_.stamp() + spareTime_.stamp());
			lastStatisticsGathered_.stamp(timestamp());
		}


		///process network events

		//calculate max wait time for next epoll
		rec->timer_queue()->calculate_timeout(&maxWaitTime, &timeout_);

#if ENABLE_WATCHERS
		g_idleProfile.start();
#else
		startTime = timestamp();
#endif

		int const result = rec->implementation()->handle_events(timeout_);

#if ENABLE_WATCHERS
		g_idleProfile.stop();
		spareTime_.stamp(spareTime_.stamp() + g_idleProfile.lastTime_);
#else
		//spareTime_.stamp() += timestamp() - startTime;
		spareTime_.stamp(spareTime_.stamp() + timestamp() - startTime);
#endif
		if( result == -1 && rec->implementation()->deactivated() )
			return 0;
		else if( result == -1 )
			return -1;
	}
	return 0;
}
int Nub::startLoop(NetworkInterface* ni)
{
	if( rec->reactor_event_loop_done() )
		return 0;

	ACE_UINT64 startTime = 0;
	ACE_Time_Value maxWaitTime(1); // use 1 sec to ensure ther is always a timerout returned
	while( 1 )
	{
		/// process tasks
		frequentTasks_.process();

		///process stats
		if( timestamp() - lastStatisticsGathered_.stamp() >= stampsPerSecond() )
		{
			oldSpareTime_.stamp(totSpareTime_.stamp());
			totSpareTime_.stamp(accSpareTime_.stamp() + spareTime_.stamp());
			lastStatisticsGathered_.stamp(timestamp());
		}


		///process network events

		//calculate max wait time for next epoll
		rec->timer_queue()->calculate_timeout(&maxWaitTime, &timeout_);

#if ENABLE_WATCHERS
		g_idleProfile.start();
#else
		startTime = timestamp();
#endif

		int const result = rec->implementation()->handle_events(timeout_);

#if ENABLE_WATCHERS
		g_idleProfile.stop();
		spareTime_.stamp(spareTime_.stamp() + g_idleProfile.lastTime_);
#else
		//spareTime_.stamp() += timestamp() - startTime;
		spareTime_.stamp(spareTime_.stamp() + timestamp() - startTime);
#endif

		ni->process_all_channels_buffered_sending_packets();

		if( result == -1 && rec->implementation()->deactivated() )
			return 0;
		else if( result == -1 )
			return -1;
	}
	return 0;
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_END_DECL