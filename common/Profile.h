#include "ace\pre.h"
#include "common.h"
#include "timestamp.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

// 由此可得到系统profile时间
extern ACE_UINT64 runningTime();
struct Profiles;
struct Profile
{
	std::string            strName_;
	Profiles*              pProfiles_;
	ACE_INT32		   inProgress_;  	// 记录第几次处理, 如递归等
	bool                    initWatcher_;

	static TimeStamp warningPeriod_;
	TimeStamp		   sumTime_;                       	// count_次的总时间
	TimeStamp          sumInternalTime_; 	 /// Total time counted
	TimeStamp          lastTime_;                   /// time after start
	TimeStamp          lastInternalTime_; 	/// the last internal time

	ACE_UINT32		   lastVal2Stop_;	///< The last value passed into stop.
	ACE_UINT32		   sumAllVals2Stop_;	///< The total of all values passed into stop.
	ACE_UINT32		   stopCounts_;			///< The number of times stop has been called.

	Profile(std::string name = "", Profiles* profiles = NULL);
	virtual ~Profile();

	void start(void);
	void stop(ACE_UINT32 qty = 0);
	bool Profile::initializeWatcher(void)
	{
		TRACE("Profile::initializeWatcher()");
		TRACE_RETURN(true);
	}
	void Profile::stop1(const char * filename, int lineNum, ACE_UINT32 qty = 0)
	{
		this->stop(qty);
		if( isTooLong() )
		{
			ACE_DEBUG(( LM_WARNING,
				"{%s}:{%d}: Profile {%s} took {:.2f} seconds\n",
				filename, lineNum,
				strName_.c_str(), stampsToSeconds(lastTime_) ));
		}
	}
	bool Profile::running() const
	{
		return inProgress_ > 0;
	}
	const char * Profile::c_str() const
	{
		return strName_.c_str();
	}
	bool Profile::isTooLong() const
	{
		return !this->running() && ( lastTime_ > warningPeriod_ );
	}
	const TimeStamp& Profile::sumTime() const
	{
		return sumTime_;
	}
	const TimeStamp& Profile::lastIntTime() const
	{
		return lastInternalTime_;
	}
	const TimeStamp& Profile::sumIntTime() const
	{
		return sumInternalTime_;
	}
	const TimeStamp Profile::lastTime() const
	{
		return this->running() ? TimeStamp(0) : lastTime_;
	}
	double Profile::lastTimeInSeconds() const
	{
		return stampsToSeconds(lastTime_);
	}
	double Profile::sumTimeInSeconds() const
	{
		return stampsToSeconds(sumTime_);
	}
	double Profile::lastIntTimeInSeconds() const
	{
		return stampsToSeconds(lastInternalTime_);
	}
	double Profile::sumIntTimeInSeconds() const
	{
		return stampsToSeconds(sumInternalTime_);
	}
	const char* Profile::name() const
	{
		return this->strName_.c_str();
	}
	ACE_UINT32 Profile::count() const
	{
		return stopCounts_;
	}
};
struct Profiles
{
	typedef std::vector<Profile*> ProfilesType;
	typedef ProfilesType::iterator iterator;
	ProfilesType profiles_;
	ProfilesType stack_;
	std::string name_;

	Profiles(std::string name = "");
	virtual ~Profiles();

	void add(Profile * pVal);
	iterator begin() { return profiles_.begin(); }
	iterator end() { return profiles_.end(); }
	Profile* pRunningTime() { return profiles_[0]; }
	const Profile * pRunningTime() const { return profiles_[0]; }
	TimeStamp runningTime() const;
	inline const char* name()const;
	bool initializeWatcher();
	inline const ProfilesType& profiles()const;
};
struct AutoScopedProfile
{
	AutoScopedProfile(Profile& profile, const char * filename, int lineNum) :
		profile_(profile),
		filename_(filename),
		lineNum_(lineNum)
	{
		profile_.start();
	}

	~AutoScopedProfile()
	{
		profile_.stop1(filename_, lineNum_);
	}

	Profile& profile_;
	const char * filename_;
	int lineNum_;
};

#define START_PROFILE( PROFILE ) PROFILE.start();
#define STOP_PROFILE( PROFILE )	   PROFILE.stop( __FILE__, __LINE__ );

#define AUTO_SCOPED_PROFILE( NAME )	\
static Profile _localProfile( NAME );\
AutoScopedProfile _autoScopedProfile( _localProfile, __FILE__, __LINE__ );

#define SCOPED_PROFILE(PROFILE)\
AutoScopedProfile PROFILE##_AutoScopedProfile(PROFILE, __FILE__, __LINE__);

#define STOP_PROFILE_WITH_CHECK( PROFILE ) if (PROFILE.stop( __FILE__, __LINE__ ))
#define STOP_PROFILE_WITH_DATA( PROFILE, DATA ) PROFILE.stop( __FILE__, __LINE__ , DATA );

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace/post.h"
