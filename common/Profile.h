#include "ace\pre.h"
#include "common.h"
#include "timestamp.hpp"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

struct Profile
{
	std::string            strName_;
	Profiles*              pProfiles_;
	ACE_INT32		   inProgress_;  	// 记录第几次处理, 如递归等
	bool                    initWatcher_;

	static TimeStamp warningPeriod_;
	TimeStamp          sumInternalTime_; 	                 /// Total time counted
	TimeStamp          lastTime_;                   /// time after start
	TimeStamp          lastInternalTime_; 	/// the last internal time

	ACE_UINT32		   lastVal2Stop_;	///< The last value passed into stop.
	ACE_UINT32		   sumAllVals2Stop_;	///< The total of all values passed into stop.
	ACE_UINT32		   stopCounts_;			///< The number of times stop has been called.

	Profile(std::string name="NULL", Profiles* profiles = NULL);
	virtual ~Profile();

	void start(void);
	void stop(ACE_UINT32 qty = 0);

	bool initializeWatcher(void);
};

struct Profiles
{
	typedef std::vector<Profile*> ProfilesType;
	ProfilesType profiles_;
	ProfilesType stack_;

	Profiles();
	virtual ~Profiles();
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace/post.h"
