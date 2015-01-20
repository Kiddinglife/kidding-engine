
#ifndef KBENGINE_MD5_HPP_
#define KBENGINE_MD5_HPP_

#include "ace/pre.h"
#include "openssl/md5.h"
#include "common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
// openssl md5µÄ·â×°
class KBE_MD5
{
	public:
	KBE_MD5::KBE_MD5()
	{
		MD5_Init(&state_);
		isFinal_ = false;
	}

	KBE_MD5::KBE_MD5(const void * data, int numBytes)
	{
		MD5_Init(&state_);
		isFinal_ = false;

		append(data, numBytes);
	}

	~KBE_MD5() { }

	inline void append(const void * data, int numBytes);
	inline const unsigned char* getDigest();
	inline std::string getDigestStr();
	inline void clear();
	inline void final();
	inline bool operator==( const KBE_MD5 & other ) const;
	bool operator!=( const KBE_MD5 & other ) const { return !( *this == other ); }
	inline bool operator<( const KBE_MD5 & other ) const;
	static std::string getDigest(const void * data, int numBytes);
	bool isFinal()const { return isFinal_; }

	private:
	MD5_CTX state_;
	unsigned char bytes_[16];
	bool isFinal_;
};
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif // KBENGINE_MD5_HPP
