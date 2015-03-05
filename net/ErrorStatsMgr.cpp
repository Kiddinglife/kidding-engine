#include "ErrorStatsMgr.h"
#include "net\Nub.h"
#include <iostream>
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

const ACE_UINT32 ErrorStatMgr::ERROR_REPORT_MIN_PERIOD_MS = 2; // 2 seconds 

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

void ErrorStatMgr::reportException(Reason reason, const ACE_INET_Addr* addr,
	const char* prefix)
{
	if( !addr )
	{
		addr = &ACE_INET_Addr();
	}

	if( prefix )
	{
		this->reportError(*addr,
			"%s: Exception occurred: %s",
			prefix, reasonToString(reason));
	} else
	{
		this->reportError(*addr, "Exception occurred: %s", reasonToString(reason));
	}
}

void ErrorStatMgr::addReport(const ACE_INET_Addr& address, const std::string & errorString)
{
	//TRACE("ErrorStatMgr::addReport");
	ACE_UINT64 now = timestamp();
	AdrrStrPair addressError(address, errorString);
	ErrorStats::iterator searchIter = errorStats_.find(addressError);
	// see if we have ever reported this error
	if( searchIter != errorStats_.end() )
	{
		// this error has been reported recently..
		ErrorSat& reportAndCount = searchIter->second;
		reportAndCount.count++;
		reportAndCount.lastRaisedStamps = now;
		ACE_INT64 millisSinceLastReport = 1000 * ( now - reportAndCount.lastReportStamps ) /
			stampsPerSecond();

		if( millisSinceLastReport >= ERROR_REPORT_MIN_PERIOD_MS )
		{
			ACE_DEBUG(( LM_ERROR, "{%s}\n",
				addressErrorToString(address, errorString, reportAndCount, now).c_str() ));
			reportAndCount.count = 0;
			reportAndCount.lastReportStamps = now;
		}
	} else
	{
		{
			ACE_DEBUG(( LM_ERROR, "{first report of %s}\n",
				addressErrorToString(address, errorString).c_str() ));
			ErrorSat reportAndCount = {
				now, 	// lastReportStamps,
				now, 	// lastRaisedStamps,
				0,		// count
			};
			errorStats_[addressError] = reportAndCount;
		}
	}
	//TRACE_RETURN_VOID();
}

/**
*	Output all exception's reports that have not yet been output.
*/
void ErrorStatMgr::reportPendingExceptions(bool reportBelowThreshold)
{
	//TRACE("ErrorStatMgr::reportPendingExceptions");
	ACE_UINT64 now = timestamp();
	// this is set to any iterator slated for removal
	ErrorStats::iterator staleIter = this->errorStats_.end();
	for( ErrorStats::iterator exceptionCountIter = this->errorStats_.begin();
		exceptionCountIter != this->errorStats_.end();
		++exceptionCountIter )
	{
		// remove any stale mappings from the last loop's run
		if( staleIter != this->errorStats_.end() )
		{
			this->errorStats_.erase(staleIter);
			staleIter = this->errorStats_.end();
		}

		// check this iteration's last report and see if we need to output
		// anything
		const AdrrStrPair & addressError = exceptionCountIter->first;
		ErrorSat& reportAndCount = exceptionCountIter->second;
		ACE_INT64 millisSinceLastReport = 1000 * ( now - reportAndCount.lastReportStamps ) /
			stampsPerSecond();
		if( reportBelowThreshold || millisSinceLastReport >= ERROR_REPORT_MIN_PERIOD_MS )
		{
			if( reportAndCount.count )
			{
				ACE_DEBUG(( LM_ERROR, "{%s}\n",
					addressErrorToString(addressError.first, addressError.second,
					reportAndCount, now).c_str() ));
				reportAndCount.count = 0;
				reportAndCount.lastReportStamps = now;
			}
		}

		// see if we can remove this mapping if it has not been raised in a while
		ACE_UINT64 sinceLastRaisedMillis = 1000 * ( now - reportAndCount.lastRaisedStamps ) /
			stampsPerSecond();
		if( sinceLastRaisedMillis > ERROR_REPORT_COUNT_MAX_LIFETIME_MS )
		{
			// it's hung around for too long without being raised again,
			// so remove it in the next iteration
			staleIter = exceptionCountIter;
		}
	}

	// remove the last mapping if it is marked stale
	if( staleIter != this->errorStats_.end() )
	{
		this->errorStats_.erase(staleIter);
	}
	//TRACE_RETURN_VOID();
}

int ErrorStatMgr::handle_timeout(const ACE_Time_Value &current_time, const void* act)
{
	//TRACE("ErrorStatMgr::handle_timeout()");
	this->reportPendingExceptions();
	return 0;
	//TRACE_RETURN(0);
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL