#ifndef BASE64_HPP_
#define BASE64_HPP_
#include /**/ "ace/pre.h"

#include <string>
#include "ace/Codecs.h"
#include "ace/Log_Msg.h"

#include "config.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
inline bool kbe_base64_encode(ACE_Byte const* inputsream, size_t inputlen, std::string& ouput)
{
	size_t encode_len = 0;
	char* encodeBuf = reinterpret_cast<char*>( ACE_Base64::encode(inputsream, inputlen, &encode_len) );
	if( encodeBuf == 0 || encode_len == 0 )
	{
		ACE_ERROR(( LM_ERROR, ACE_TEXT("Error in encoding stream\n") ));
		return false;
	}
	ouput = encodeBuf;
	return true;
}
inline bool kbe_base64_decode(std::string const& s, std::string& output)
{
	size_t decode_len = 0;

	char* decodeBuf = reinterpret_cast<char*>( ACE_Base64::decode(reinterpret_cast<ACE_Byte*>( const_cast<char*>( s.c_str() ) ), &decode_len) );
	if( decodeBuf == 0 || decode_len == 0 )
	{
		ACE_ERROR(( LM_ERROR, ACE_TEXT("Error in encoding stream\n") ));
		return false;
	}
	output = decodeBuf;
	return true;
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace/post.h"
#endif