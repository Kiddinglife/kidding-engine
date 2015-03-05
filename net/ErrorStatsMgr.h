/**
 * @By Jackie Zhang at 6:10 PM on 01/03/2015
 */
#ifndef ErrorSat_H_
#define ErrorSat_H_

#include "ace\pre.h"
#include "ace\Event_Handler.h"
#include "ace\INET_Addr.h"
#include "net\net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Nub;

struct ErrorSat
{
	ACE_UINT64 lastReportStamps;	// When this error was last reported
	ACE_UINT64 lastRaisedStamps;	// When this error was last raised
	ACE_UINT32 count;	                // How many of this exception have beenreported since
};

typedef std::pair<ACE_INET_Addr, std::string > AdrrStrPair;
typedef std::map < AdrrStrPair, ErrorSat > ErrorStats;

struct ErrorStatMgr : public ACE_Event_Handler
{
	static const ACE_UINT32 ERROR_REPORT_MIN_PERIOD_MS;
	static const ACE_UINT32 ERROR_REPORT_COUNT_MAX_LIFETIME_MS;

	ErrorStats errorStats_;
	long         timerID_; 	/// tinmer id used to canncel the timmer
	Nub*        nub_;

	ErrorStatMgr(Nub* nub);
	virtual ~ErrorStatMgr();

	std::string addressErrorToString(const ACE_INET_Addr& address, const std::string& errorString);

	std::string addressErrorToString(
		const ACE_INET_Addr& address,
		const std::string& errorString,
		const ErrorSat& reportAndCount,
		const ACE_UINT64& now);

	/**
	*	Report a general error with printf style format string. If repeatedly the
	*	resulting formatted string is reported within the minimum output window,
	*	they are accumulated and output after the minimum output window has passed.
	*/
	void reportError(const ACE_INET_Addr& address, const char* format, ...);

	/**
	*	Output the exception if it has not occurred before, otherwise only
	*	output the exception if the minimum period has elapsed since the
	*	last outputting of this exception.
	*
	*	@param ne 		the NubException
	*	@param prefix 	any prefix to add to the error message, or NULL if no prefix
	*
	*/

	void reportException(Reason reason, const ACE_INET_Addr* addr = NULL, const char* prefix = NULL);
	/**
	*	Adds a new error message for an address to the reporter count map.
	*	Emits an error message if there has been no previous equivalent error
	*	string provider for this address.
	*/
	void addReport(const ACE_INET_Addr& address, const std::string & errorString);

	/**
	*	Output all exception's reports that have not yet been output.
	*/
	void reportPendingExceptions(bool reportBelowThreshold = false);

	/**
	*	This method handles the timer event and checks to see if any delayed
	*	messages should be reported.
	*/
	virtual int handle_timeout(const ACE_Time_Value &current_time, const void * = 0);
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif
