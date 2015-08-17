#include "timestamp.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

#ifdef KBE_USE_RDTSC
KBETimingMethod g_timingMethod = RDTSC_TIMING_METHOD;
#else // KBE_USE_RDTSC
const KBETimingMethod DEFAULT_TIMING_METHOD = GET_TIME_TIMING_METHOD;
KBETimingMethod g_timingMethod = NO_TIMING_METHOD;
#endif // KBE_USE_RDTSC

const char* getTimingMethodName()
{
	switch( g_timingMethod )
	{
		case NO_TIMING_METHOD:
			return "none";

		case RDTSC_TIMING_METHOD:
			return "rdtsc";

		case GET_TIME_OF_DAY_TIMING_METHOD:
			return "gettimeofday";

		case GET_TIME_TIMING_METHOD:
			return "gettime";

		default:
			return "Unknown";
	}
}

#if KBE_PLATFORM == PLATFORM_UNIX
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static ACE_UINT64 calcStampsPerSecond_rdtsc()
{
	struct timeval	tvBefore,	tvSleep = {0, 500000},	tvAfter;
	ACE_UINT64 stampBefore,	stampAfter;

	gettimeofday(&tvBefore, NULL);
	gettimeofday(&tvBefore, NULL);

	gettimeofday(&tvBefore, NULL);
	stampBefore = timestamp();

	select(0, NULL, NULL, NULL, &tvSleep);

	gettimeofday(&tvAfter, NULL);
	gettimeofday(&tvAfter, NULL);

	gettimeofday(&tvAfter, NULL);
	stampAfter = timestamp();

	ACE_UINT64 microDelta =
		(tvAfter.tv_usec + 1000000 - tvBefore.tv_usec) % 1000000;

	ACE_UINT64 stampDelta = stampAfter - stampBefore;

	return (stampDelta * 1000000ULL) / microDelta;
}

static ACE_UINT64 calcStampsPerSecond_gettime()
{
	return 1000000000ULL;
}

static ACE_UINT64 calcStampsPerSecond_gettimeofday()
{
	return 1000000ULL;
}

static ACE_UINT64 calcStampsPerSecond()
{
	static bool firstTime = true;
	if (firstTime)
	{
		firstTime = false;
	}

#ifdef KBE_USE_RDTSC
	return calcStampsPerSecond_rdtsc();
#else // KBE_USE_RDTSC
	if (g_timingMethod == RDTSC_TIMING_METHOD)
		return calcStampsPerSecond_rdtsc();
	else if (g_timingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return calcStampsPerSecond_gettimeofday();
	else if (g_timingMethod == GET_TIME_TIMING_METHOD)
		return calcStampsPerSecond_gettime();
	else
	{
		char * timingMethod = getenv("KBE_TIMING_METHOD");
		if (!timingMethod)
		{
			g_timingMethod = DEFAULT_TIMING_METHOD;
		}
		else if (strcmp(timingMethod, "rdtsc") == 0)
		{
			g_timingMethod = RDTSC_TIMING_METHOD;
		}
		else if (strcmp(timingMethod, "gettimeofday") == 0)
		{
			g_timingMethod = GET_TIME_OF_DAY_TIMING_METHOD;
		}
		else if (strcmp(timingMethod, "gettime") == 0)
		{
			g_timingMethod = GET_TIME_TIMING_METHOD;
		}
		else
		{
			WARNING_MSG(fmt::format("calcStampsPerSecond: "
				"Unknown timing method '%s', using clock_gettime.\n",
				timingMethod));

			g_timingMethod = DEFAULT_TIMING_METHOD;
		}

		return calcStampsPerSecond();
	}
#endif // KBE_USE_RDTSC
}


ACE_UINT64 stampsPerSecond_rdtsc()
{
	static ACE_UINT64 stampsPerSecondCache = calcStampsPerSecond_rdtsc();
	return stampsPerSecondCache;
}

double stampsPerSecondD_rdtsc()
{
	static double stampsPerSecondCacheD = double(stampsPerSecond_rdtsc());
	return stampsPerSecondCacheD;
}

ACE_UINT64 stampsPerSecond_gettimeofday()
{
	static ACE_UINT64 stampsPerSecondCache = calcStampsPerSecond_gettimeofday();
	return stampsPerSecondCache;
}

double stampsPerSecondD_gettimeofday()
{
	static double stampsPerSecondCacheD = double(stampsPerSecond_gettimeofday());
	return stampsPerSecondCacheD;
}




#elif KBE_PLATFORM == PLATFORM_WIN32
#include <windows.h>
#ifdef KBE_USE_RDTSC
static ACE_UINT64 calcStampsPerSecond()
{	
	LARGE_INTEGER	tvBefore,	tvAfter;
	DWORD			tvSleep = 500;
	ACE_UINT64 stampBefore,	stampAfter;

	Sleep(100);

	QueryPerformanceCounter(&tvBefore);
	QueryPerformanceCounter(&tvBefore);

	QueryPerformanceCounter(&tvBefore);
	stampBefore = timestamp();

	Sleep(tvSleep);

	QueryPerformanceCounter(&tvAfter);
	QueryPerformanceCounter(&tvAfter);

	QueryPerformanceCounter(&tvAfter);
	stampAfter = timestamp();

	ACE_UINT64 countDelta = tvAfter.QuadPart - tvBefore.QuadPart;
	ACE_UINT64 stampDelta = stampAfter - stampBefore;

	LARGE_INTEGER	frequency;
	QueryPerformanceFrequency(&frequency);

	return (ACE_UINT64)((stampDelta * ACE_UINT64(frequency.QuadPart) ) / countDelta);
}
#else // KBE_USE_RDTSC
static ACE_UINT64 calcStampsPerSecond()
{
	LARGE_INTEGER rate;
	ACE_ASSERT(QueryPerformanceFrequency(&rate));
	return rate.QuadPart;
}
#endif // KBE_USE_RDTSC
#elif defined( PLAYSTATION3 )

static ACE_UINT64 calcStampsPerSecond()
{
	return sys_time_get_timebase_frequency();
}

#endif // PLAYSTATION3

/// 每秒cpu所耗时间
ACE_UINT64 stampsPerSecond()
{
	static ACE_UINT64 _stampsPerSecondCache = calcStampsPerSecond();
	return _stampsPerSecondCache;
}

//// 每秒cpu所耗时间 double版本
double stampsPerSecondD()
{
	static double stampsPerSecondCacheD = double(stampsPerSecond());
	return stampsPerSecondCacheD;
}
/* timestamp.cpp */
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
