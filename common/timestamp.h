#ifndef KBE_TIMESTAMP_HPP
#define KBE_TIMESTAMP_HPP
#include "ace/pre.h"
#include "common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

// Ö¸Ê¾ÊÇ·ñ¿ÉÒÔÍ¨¹ýµ÷ÓÃRDTSC£¨Ê±¼ä´Á¼ÆÊýÆ÷£©
// ¼ÆËãÊ±¼ä´Á¡£Ê¹ÓÃ´ËµÄºÃ´¦ÊÇ£¬ËüÄÜ¿ìËÙºÍ¾«È·µÄ·µ»ØÊµ¼ÊµÄÊ±ÖÓµÎ´ð
// ¡£²»×ãÖ®´¦ÊÇ£¬Õâ²¢²»Ê¹ÓÃSpeedStep¼¼ÊõÀ´¸Ä±äËûÃÇµÄÊ±ÖÓËÙ¶ÈµÄCPU¡£
#ifdef unix
//#define KBE_USE_RDTSC
#else // unix
//#define KBE_USE_RDTSC
#endif // unix

enum KBETimingMethod
{
	RDTSC_TIMING_METHOD, // ×ÔCPUÉÏµçÒÔÀ´Ëù¾­¹ýµÄÊ±ÖÓÖÜÆÚÊý,´ïµ½ÄÉÃë¼¶µÄ¼ÆÊ±¾«¶È
	GET_TIME_OF_DAY_TIMING_METHOD,
	GET_TIME_TIMING_METHOD,
	NO_TIMING_METHOD,
};
extern KBETimingMethod g_timingMethod;
const char* getTimingMethodName();

#if (KBE_PLATFORM == PLATFORM_UNIX)

inline ACE_UINT64 timestamp_rdtsc()
{
	ACE_UINT32 rethi, retlo;
	__asm__ __volatile__ (
		"rdtsc":
	"=d"    (rethi),
		"=a"    (retlo)
		);
	return ACE_UINT64(rethi) << 32 | retlo; 
}

// Ê¹ÓÃ gettimeofday. ²âÊÔ´ó¸Å±ÈRDTSC20±¶-600±¶¡£
// ´ËÍâ£¬ÓÐÒ»¸öÎÊÌâ 2.4ÄÚºËÏÂ£¬Á¬ÐøÁ½´Îµ÷ÓÃgettimeofdayµÄ¿ÉÄÜ
// ·µ»ØÒ»¸ö½á¹ûÊÇµ¹×Å×ß¡£
//#include <sys/time.h>
inline ACE_UINT64 timestamp_gettimeofday()
{
	//timeval tv;
	//gettimeofday( &tv, NULL );
	//return 1000000ULL * ACE_UINT64( tv.tv_sec ) + ACE_UINT64( tv.tv_usec );
	ACE_UINT64 t;
	ACE_OS::gettimeofday().usec(t);
	return t;
}

#include <time.h>
#include <asm/unistd.h>
inline ACE_UINT64 timestamp_gettime()
{
	timespec tv;
	KBE_VERIFY(syscall( __NR_clock_gettime, CLOCK_MONOTONIC, &tv ) == 0);
	return 1000000000ULL * tv.tv_sec + tv.tv_nsec;
}

inline ACE_UINT64 timestamp()
{
#ifdef KBE_USE_RDTSC
	return timestamp_rdtsc();
#else // KBE_USE_RDTSC
	if (g_timingMethod == RDTSC_TIMING_METHOD)
		return timestamp_rdtsc();
	else if (g_timingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return timestamp_gettimeofday();
	else //if (g_timingMethod == GET_TIME_TIMING_METHOD)
		return timestamp_gettime();
#endif // KBE_USE_RDTSC
}

#elif (KBE_PLATFORM == PLATFORM_WIN32)

#ifdef KBE_USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
inline ACE_UINT64 timestamp()
{
	__asm rdtsc
}
#pragma warning (pop)
#else // KBE_USE_RDTSC
#include <windows.h>
inline ACE_UINT64 timestamp()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}
#endif // KBE_USE_RDTSC

#elif defined( PLAYSTATION3 )
inline ACE_UINT64 timestamp()
{
	ACE_UINT64 ts;
	SYS_TIMEBASE_GET( ts );
	return ts;
}
#else
#error Unsupported platform!
#endif

ACE_UINT64 stampsPerSecond();
double stampsPerSecondD();

ACE_UINT64 stampsPerSecond_rdtsc();
double stampsPerSecondD_rdtsc();

ACE_UINT64 stampsPerSecond_gettimeofday();
double stampsPerSecondD_gettimeofday();

inline double stampsToSeconds(ACE_UINT64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}

// -----------------------------------------------------------------------------
class TimeStamp
{
	public:
	TimeStamp(ACE_UINT64 stamps = 0) : stamp_(stamps) { }

	operator ACE_UINT64 &( ) { return stamp_; }
	operator ACE_UINT64() const { return stamp_; }

	const ACE_UINT64 stamp() const { return stamp_; }
	void  stamp(ACE_UINT64 t) { stamp_ = t; }

	inline double inSeconds() const;
	inline void setInSeconds(double seconds);

	inline TimeStamp ageInStamps() const;
	inline double ageInSeconds() const;

	inline static double toSeconds(ACE_UINT64 stamps);
	inline static TimeStamp fromSeconds(double seconds);

	ACE_UINT64	stamp_;
};
inline double TimeStamp::toSeconds(ACE_UINT64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}
inline TimeStamp TimeStamp::fromSeconds(double seconds)
{
	return ACE_UINT64(seconds * stampsPerSecondD());
}
inline double TimeStamp::inSeconds() const
{
	return toSeconds(stamp_);
}
inline void TimeStamp::setInSeconds(double seconds)
{
	stamp_ = fromSeconds(seconds);
}
inline TimeStamp TimeStamp::ageInStamps() const
{
	return timestamp() - stamp_;
}
inline double TimeStamp::ageInSeconds() const
{
	return toSeconds(this->ageInStamps());
}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif // KBE_TIMESTAMP_HPP
