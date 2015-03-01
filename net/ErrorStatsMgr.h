#ifndef Channel_H_
#define Channel_H_

#include "ace\pre.h"
#include "ace\Event_Handler.h"
#include "ace\INET_Addr.h"
#include "common\common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Nub;

struct ErrorSat
{
	ACE_UINT64 lastReportStamps;	// When this error was last reported
	ACE_UINT64 lastRaisedStamps;	// When this error was last raised
	ACE_UINT64 count;	                // How many of this exception have beenreported since
};

typedef std::pair<ACE_INET_Addr, std::string > AdrrStrPair;
typedef std::map < AdrrStrPair, ErrorSat > ErrorStats;

/**
 * @By Jackie Zhang at 6:10 PM on 01/03/2015
 */
struct ErrorStatMgr : public ACE_Event_Handler
{
	static const ACE_UINT32 ERROR_REPORT_MIN_PERIOD_MS;
	static const ACE_UINT32 ERROR_REPORT_COUNT_MAX_LIFETIME_MS;

	ErrorStats errorStats_;
	long         timerID_; 	/// tinmer id used to canncel the timmer
	Nub*        nub_;
	ErrorStatMgr(Nub* nub);
	virtual ~ErrorStatMgr();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif
