#include "ErrorStatsMgr.h"
#include "net\Nub.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

const ACE_UINT32 ErrorStatMgr::ERROR_REPORT_MIN_PERIOD_MS = 2000; // 2 seconds

/**
 * The nominal maximum time that a report count for a Network address and
 * error is kept after the last raising of the error.  10 seconds
 */
const ACE_UINT32 ErrorStatMgr::ERROR_REPORT_COUNT_MAX_LIFETIME_MS = 10000;

ErrorStatMgr::ErrorStatMgr(Nub* nub)
{

}


ErrorStatMgr::~ErrorStatMgr()
{
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL