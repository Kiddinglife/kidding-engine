#include "ErrorStatsMgr.h"
#include "net\Nub.h"
#include <iostream>
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

const ACE_UINT32 ErrorStatMgr::ERROR_REPORT_MIN_PERIOD_MS = 2000; // 2 seconds

/**
 * The nominal maximum time that a report count for a Network address and
 * error is kept after the last raising of the error.  10 seconds
 */
const ACE_UINT32 ErrorStatMgr::ERROR_REPORT_COUNT_MAX_LIFETIME_MS = 10000;

ErrorStatMgr::ErrorStatMgr(Nub* nub) : ACE_Event_Handler(), timerID_(-1), nub_(nub)
{
	ACE_Time_Value interval(ERROR_REPORT_MIN_PERIOD_MS, 0);
	timerID_ = nub->rec->schedule_timer(this, ( void* ) this, ACE_Time_Value::zero, interval);
}


ErrorStatMgr::~ErrorStatMgr()
{
	if( timerID_ != -1 ) nub_->rec->cancel_timer(timerID_);
}

std::string ErrorStatMgr::addressErrorToString(const ACE_INET_Addr& address,
	const std::string& errorString)
{
	std::ostringstream out;
	char buf[MAX_BUF];
	address.addr_to_string(buf, MAX_BUF);
	std::string addrstr(buf);
	out << addrstr << " : " << errorString;
	return out.str();
}

std::string ErrorStatMgr::addressErrorToString(
	const ACE_INET_Addr& address,
	const std::string& errorString,
	const ErrorSat& reportAndCount,
	const ACE_UINT64& now)
{
	ACE_INT64 deltaStamps = now - reportAndCount.lastReportStamps;
	double deltaMillis = 1000 * deltaStamps / stampsPerSecondD();

	char buf[1024];
	int len = kbe_snprintf(buf, 1024,
		"%d reports of %s in the last %.00f ms",
		reportAndCount.count,
		addressErrorToString(address, errorString).c_str(),
		deltaMillis);

	if( len >= 1024 )
	{
		return std::string("the error string length  >= 1024 butes\n");
	}
	return std::string(buf);
}

/**
*	Report a general error with printf style format string. If repeatedly the
*	resulting formatted string is reported within the minimum output window,
*	they are accumulated and output after the minimum output window has passed.
*/
void ErrorStatMgr::reportError(const ACE_INET_Addr& address, const char* format, ...)
{
	char  buf[1024];
	va_list va;
	va_start(va, format);
	int strLen = _vsnprintf(buf, 1024, format, va);
	va_end(va);
	buf[1024 - 1] = '\0';
	std::string error(buf);
	this->addReport(address, error);
}

void ErrorStatMgr::addReport(const ACE_INET_Addr& address, const std::string & errorString)
{
	std::cout << errorString << "\n";
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL