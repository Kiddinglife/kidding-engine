#ifndef KBENGINE_RSA_HPP_
#define KBENGINE_RSA_HPP_

#include "ace/pre.h"
#include "strulti.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

/**
 *	@class KBE_RSA
 * @brief: openssl ·â×°
 * opensslÊÇÒ»¸ö¹¦ÄÜÇ¿´óµÄ¹¤¾ß°ü£¬Ëü¼¯³ÉÁËÖÚ¶àÃÜÂëËã·¨¼°ÊµÓÃ¹¤¾ß¡£
 * ÎÒÃÇ¼´¿ÉÒÔÀûÓÃËüÌá¹©µÄÃüÁîÌ¨¹¤¾ßÉú³ÉÃÜÔ¿¡¢Ö¤ÊéÀ´¼ÓÃÜ½âÃÜÎÄ¼þ£¬
 * Ò²¿ÉÒÔÔÚÀûÓÃÆäÌá¹©µÄAPI½Ó¿ÚÔÚ´úÂëÖÐ¶Ô´«ÊäÐÅÏ¢½øÐÐ¼ÓÃÜ¡£
 * RSAÊÇÒ»¸ö·Ç¶Ô³Æ¼ÓÃÜËã·¨¡£¼òµ¥ËµÀ´£¬·Ç¶Ô³Æ¼ÓÃÜËã·¨¾ÍÊÇËµ¼ÓÃÜ½âÃÜ
 * Ò»¸öÎÄ¼þÐèÒªÓÐÁ½¸öÃÜÔ¿£¬Ò»¸öÓÃÀ´¼ÓÃÜ£¬Îª¹«Ô¿£¬Ò»¸öÓÃÀ´½âÃÜ£¬
 * ÎªË½Ô¿¡£Ö¤Êé¿ÉÒÔÓÃÀ´ÊÚÈ¨¹«Ô¿µÄÊ¹ÓÃ¡£
 */
class KBE_RSA
{
	public:
	KBE_RSA(const std::string& pubkeyname,
		const std::string& prikeyname);

	KBE_RSA();

	virtual ~KBE_RSA();

	bool generateKey(const std::string& pubkeyname,
		const std::string& prikeyname, int keySize = 1024, int e = 65537);

	std::string encrypt(const std::string& instr);
	int encrypt(const std::string& instr, std::string& outCertifdata);
	int decrypt(const std::string& inCertifdata, std::string& outstr);
	std::string decrypt(const std::string& instr);

	static void hexCertifData(const std::string& inCertifdata);

	bool loadPublic(const std::string& keyname);
	bool loadPrivate(const std::string& keyname);

	virtual bool isGood()const { return rsa_public != NULL && rsa_private != NULL; }
	protected:
	void* rsa_public, *rsa_private;
};
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace/post.h"
#endif
