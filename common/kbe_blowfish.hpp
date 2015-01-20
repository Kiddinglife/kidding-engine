#ifndef BLOWFISH_HPP_
#define BLOWFISH_HPP_

#include "ace/pre.h"
#include "openssl/include/openssl\blowfish.h"
#include "common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

/*
* @class KBE_Blowfish
* @brief:
* BlowFishËã·¨ÓÃÀ´¼ÓÃÜ64Bit³¤¶ÈµÄ×Ö·û´®¡£
* BlowFishËã·¨Ê¹ÓÃÁ½¸ö¡°ºÐ¡±¡ª¡ªunsignedlongpbox[18]ºÍunsignedlongsbox[4,256]¡£
* BlowFishËã·¨ÖÐ£¬ÓÐÒ»¸öºËÐÄ¼ÓÃÜº¯Êý:BF_En(ºóÎÄÏêÏ¸½éÉÜ£©¡£¸Ãº¯ÊýÊäÈë64Î»ÐÅÏ¢£¬ÔËËãºó£¬
* ÒÔ64Î»ÃÜÎÄµÄÐÎÊ½Êä*³ö¡£ÓÃBlowFishËã·¨¼ÓÃÜÐÅÏ¢£¬ÐèÒªÁ½¸ö¹ý³Ì£º
* 1.ÃÜÔ¿Ô¤´¦Àí
* 2.ÐÅÏ¢¼ÓÃÜ
* ·Ö±ðËµÃ÷ÈçÏÂ£º
* ÃÜÔ¿Ô¤´¦Àí£º
* BlowFishËã·¨µÄÔ´ÃÜÔ¿¡ª¡ªpboxºÍsboxÊÇ¹Ì¶¨µÄ¡£ÎÒÃÇÒª¼ÓÃÜÒ»¸öÐÅÏ¢£¬ÐèÒª×Ô¼ºÑ¡ÔñÒ»¸ökey£¬
* ÓÃÕâ¸ökey¶ÔpboxºÍsbox½øÐÐ±ä»»£¬µÃµ½ÏÂÒ»²½ÐÅÏ¢¼ÓÃÜËùÒªÓÃµÄkey_pboxºÍkey_sbox¡£
*/
class KBEBlowfish
{
	public:
	// Ã¿¿é´óÐ¡
	static const int BLOCK_SIZE = 64 / 8;

	// keyµÄ×îÐ¡ºÍ×î´ó´óÐ¡
	static const int MIN_KEY_SIZE = 32 / 8;
	static const int MAX_KEY_SIZE = 448 / 8;

	// Ä¬ÈÏkeyµÄ´óÐ¡
	static const int DEFAULT_KEY_SIZE = 128 / 8;

	typedef std::string Key;

	virtual ~KBEBlowfish();
	KBEBlowfish(const Key & key);
	KBEBlowfish(int keySize = DEFAULT_KEY_SIZE);

	const Key & key() const { return key_; }
	const char * strBlowFishKey() const;
	bool isGood() const { return isGood_; }

	int encrypt(const ACE_Byte * src, ACE_Byte * dest, int length);
	int decrypt(const ACE_Byte * src, ACE_Byte * dest, int length);

	BF_KEY * pBlowFishKey() { return (BF_KEY*) pBlowFishKey_; }

	protected:
	bool init();

	Key key_;
	int keySize_;
	bool isGood_;

	void * pBlowFishKey_;
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace/post.h"
#endif