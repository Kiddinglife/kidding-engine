#include <ace/Date_Time.h>   
#ifndef DEADLINE_HPP_
#define DEADLINE_HPP_
#include /**/ "ace/pre.h"
#include "ace/OS_NS_math.h"
#include "ace/Basic_Types.h"
#include <ace/Date_Time.h> 
#include "format.h"
#include "ace_kbe_version_namespace.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

/**
*	期限
*/
class Deadline
{
	public:
	Deadline(time_t secs) :
		days(0),
		hours(0),
		minutes(0),
		seconds(0),
		secs_(secs)
	{
		if( secs > 0 )
		{
			days = secs / ( 3600 * 24 );
			int m = secs % ( 3600 * 24 );
			hours = m / 3600;
			m = m % 3600;
			minutes = m / 60;
			seconds = m % 60;
		}
	}

	virtual ~Deadline() { }

	std::string print()
	{
		return fmt::format("{} days, {} hours, {} minutes, {} seconds", days, hours, minutes, seconds);
	}

	long days, hours, minutes, seconds;
	time_t secs_;
};
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include /**/"ace/post.h"
#endif
