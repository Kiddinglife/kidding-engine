/**
*  ÑÝÊ¾ACE_Log_MsgÀàµÄÊ¹ÓÃ
*
*    ACEÈÕÖ¾ÏµÍ³¼¸ºõÌá¹©ÁËÓ¦ÓÃ³ÌÐòËùÐèÒªµÄÈ«²¿ÈÕÖ¾¹¦ÄÜ£¬ÓÈÆä¶ÔÓÚ
*  ACE_Log_Msg_BackendÌá¹©ÁË±ØÒªµÄ½Ó¿Ú½«ÈÕÖ¾·ÖÃÅ±ðÀà£¨logger_key£©
*  µÄÊä³öµ½²»Í¬Éè±¸£¨Èç£ºÆÁÄ»¡¢ÎÄ¼þ¡¢ÈÕÖ¾·þÎñÆ÷DeamonµÈ£©£¬Õâ¶ÔÓÚÍ¬Ò»
*  Ä£¿éÖÐ¶Ô²»Í¬Âß¼­£¨Èç£ºÄÚ´æ£¬Ïß³ÌµÈ×ÊÔ´µÄÊ¹ÓÃ£©µÄ¼à²âÌá¹©ÁË±ãÀû¡£
*
*  Àí½âTRACE£¨ACE_TRACE£©ºÍµ÷ÊÔ£¨ACE_DEBUG£©ÐÅÏ¢µÄº¬Òå£º
*      -ACE_TRACE
*          ¸ú×Ùº¯ÊýµÄµ÷ÓÃ¶ÑÕ»£¨Í¨¹ýtabËõ½ø±íÊ¾£©¡ª¡ª¹Û²ì¶ÔÏóµÄÉúÃü
*          ÖÜÆÚµÈ·Ç³£ÓÐÓÃ¡£
*          Í¨¹ýÔÚ±àÒëÆ÷ÖÐ¼ÓÈë»òÉ¾³ýACE_NTRACE=0Ô¤¶¨ÒåºêÁé»î¿ØÖÆÐÅÏ¢
*          Êä³öÓë·ñ¡£
*
*      -ACE_DEBUG
*          Ö÷ÒªÓÃÓÚ¼à²â³ÌÐòÂß¼­Êý¾ÝµÄÕýÈ·ÐÔ¡£
*
*  £¡£¡£¡×¢Òâ£¡£¡£¡
*      ²»ÒªÔÚACE_Log_Msg_Callback»òACE_Log_Msg_BackendµÄ×ÓÀàÖÐµ÷ÓÃ
*      ACE_TRACEºÍACE_DEBUGµÈºêÊä³öÐÅÏ¢¡£
*/

#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Record.h"
#include "ace/Log_Msg_Callback.h"
#include "ace/Log_Msg_Backend.h"

#include <string>
#include <fstream>

//*******************************************************************************
const ACE_TCHAR *prog_name = ACE_TEXT("ACE_Log_Msg_test");
const ACE_TCHAR *logger_key = ACE_TEXT("ACE_Log_Msg_test*");

//*******************************************************************************
class My_Log_Msg_Callback : public ACE_Log_Msg_Callback
{
	ACE_Log_Msg_Callback *oldcallback_;
	public:
	~My_Log_Msg_Callback()
	{
		ACE_OS::printf(ACE_TEXT("My_Log_Msg_Callback::~My_Log_Msg_Callback() "));
		// £¡£¡£¡×¢ÒâÏú»ÙºóÇåÀíACE_Log_MsgÏàÓ¦×´Ì¬£¬ACE_Log_Msg×÷Îª
		// SingletonÐÎÊ½´æÔÚ£¬ÏµÍ³ÍË³öÊ±ACE_Object_ManagerÇåÀíËü¡£
		// ÕâÀï¼ÙÉèMy_Log_Msg_CallbackÊµÀýÉúÃüÖÜÆÚ¶ÌÓÚACE_Log_MsgÊµÀý£¬
		// ²¢·ÇËùÓÐÇé¿ö¶¼ÊÇÕýÈ·µÄ
		ACE_LOG_MSG->clr_flags(ACE_Log_Msg::MSG_CALLBACK);
		ACE_LOG_MSG->msg_callback(oldcallback_);
	}

	void oldcallback(const ACE_Log_Msg_Callback *backend)
	{
		oldcallback_ = const_cast<ACE_Log_Msg_Callback *>( backend );
	}
	virtual void log(ACE_Log_Record &log_record)
	{
		ACE_OS::printf("[Log_Msg_Callback]%s", log_record.msg_data());
	}
};

class My_Log_Msg_Backend : public ACE_Log_Msg_Backend
{
	ACE_Log_Msg_Backend *oldbackend_;
	public:
	My_Log_Msg_Backend() : oldbackend_(0) { }
	~My_Log_Msg_Backend()
	{
		ACE_OS::printf(ACE_TEXT("My_Log_Msg_Backend::~My_Log_Msg_Backend() "));
		// £¡£¡£¡×¢ÒâÏú»ÙºóÇåÀíACE_Log_MsgÏàÓ¦×´Ì¬£¬ACE_Log_Msg×÷Îª
		// SingletonÐÎÊ½´æÔÚ£¬ÏµÍ³ÍË³öÊ±ACE_Object_ManagerÇåÀíËü¡£
		// ÕâÀï¼ÙÉèMy_Log_Msg_BackendÊµÀýÉúÃüÖÜÆÚ¶ÌÓÚACE_Log_MsgÊµÀý£¬
		// ²¢·ÇËùÓÐÇé¿ö¶¼ÊÇÕýÈ·µÄ
		ACE_LOG_MSG->clr_flags(ACE_Log_Msg::CUSTOM);
		ACE_LOG_MSG->msg_backend(oldbackend_);
	}
	void oldbackend(const ACE_Log_Msg_Backend *backend)
	{
		oldbackend_ = const_cast<ACE_Log_Msg_Backend *>( backend );
	}
	virtual int open(const ACE_TCHAR *logger_key)
	{
		ACE_OS::printf("My_Log_Msg_Backend::open() logger_key[%s] ", logger_key);
		return 0;
	}

	virtual int reset(void)
	{
		return 0;
	}

	virtual int close(void)
	{
		return 0;
	}

	virtual int log(ACE_Log_Record &log_record)
	{
		ACE_OS::printf("[Log_Msg_Backend]%s", log_record.msg_data());

		return 0;
	}
};