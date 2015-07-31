/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2012 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef STRUTIL_HPP_
#define STRUTIL_HPP_
#include /**/ "ace/pre.h"

#include <vector>
#include <sstream>

#include "ace/OS.h" //includes all the system headers
#include "ace/Basic_Types.h"
#include "ace/Tokenizer_T.h"

#include "utf8cpp/utf8.h"
#include "config.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
namespace STRUTIL
{
#define kbe_is_not_num isnan
#define kbe_is_infinite_number isinf

	/////////////////////////////////////////////////  ace ÒÑ¾­·â×°ÁË¿í×Ö·ûwchar  /////////////////////////////////////
#define kbe_snprintf                                  ACE_OS::snprintf
#define kbe_snwprintf                               ACE_OS::snprintf
#define kbe_vsnprintf                                ACE_OS::vsnprintf  
#define kbe_vsnwprintf                             ACE_OS::vsnprintf
#define kbe_stricmp                                  ACE_OS::strcasecmp 
#define kbe_fileno                                      ACE_OS::ace_tolower
#define kbe_filesize                                    ACE_OS::filesize
#define kbe_va_copy                                 va_copy
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////str to ll ///////////////////////////////////////////////////////////
	/**ÓÃ·¨£º
	// strtol example
	#include <stdio.h>      //printf
	#include <stdlib.h>     //strtol
	int main()
	{
	char szNumbers[ ] = "20011221 60c0c0 -1101110100110100100000 0x6fffff";
	char * pEnd;
	long int li1, li2, li3, li4;
	li1 = strtol(szNumbers, &pEnd, 10);
	li2 = strtol(pEnd, &pEnd, 16);
	li3 = strtol(pEnd, &pEnd, 2);
	li4 = strtol(pEnd, NULL, 0);
	int a = atoi("123");
	printf("The decimal equivalents are: %ld, %ld, %ld and %ld.and %d\n", li1, li2, li3, li4, a);
	return 0;
	}*/
#define kbe_strtoq   ACE_OS::strtoll
#define kbe_strtouq  ACE_OS::strtoull
#define kbe_strtoll ACE_OS::strtoll
#define kbe_strtoull ACE_OS::strtoull
#define kbe_atoll    ACE_OS::atol
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////// ×ª»»´óÐ¡Ð´ ////////////////////////////////////////////////
	extern inline char* kbe_strtoupper(char* s)
	{
		ACE_ASSERT(s != NULL);
		while( *s )
		{
			*s = ACE_OS::ace_toupper(*s);
			s++;
		}
		return s; //return the 0/
	}
	extern inline char* kbe_strtolower(char* s)
	{
		ACE_ASSERT(s != NULL);
		while( *s )
		{
			*s = ACE_OS::ace_tolower(*s);
			s++;
		}
		return s;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////// str trim ////////////////////////////////////////////////
	inline void kbe_trim(std::string& str, const std::string& delimintor)
	{
		str.erase(0, str.find_first_not_of(delimintor));//È¥µô×Ö·û´®Ç°ÃæµÄ¿Õ°×·û
		size_t pos = str.find_last_not_of(delimintor) + delimintor.size();
		if( pos < str.size() )
		{
			str.erase(pos); 		//È¥µô×Ö·û´®ºóÃæµÄ¿Õ°×·û
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////// str replace&split////////////////////////////////////////////////
	/*replace*/
	extern inline int kbe_replace(std::string& str, const std::string& pattern, const std::string& newpat)
	{
		int count = 0;
		const size_t nsize = newpat.size();
		const size_t psize = pattern.size();

		for( size_t pos = str.find(pattern, 0);
			pos != std::string::npos;
			pos = str.find(pattern, pos + nsize) )
		{
			str.replace(pos, psize, newpat);
			count++;
		}

		return count;
	}
	extern inline int kbe_replace(std::wstring& str, const std::wstring& pattern, const std::wstring& newpat)
	{
		int count = 0;
		const size_t nsize = newpat.size();
		const size_t psize = pattern.size();

		for( size_t pos = str.find(pattern, 0);
			pos != std::string::npos;
			pos = str.find(pattern, pos + nsize) )
		{
			str.replace(pos, psize, newpat);
			count++;
		}

		return count;
	}
	/*split*/
	template<typename T>
	extern inline void kbe_split(const std::basic_string<T>& s, T c, std::vector< std::basic_string<T> > &v)
	{
		/*
		if( s.size() == 0 )
		return;

		typename std::basic_string< T >::size_type i = 0;
		typename std::basic_string< T >::size_type j = s.find(c);

		while( j != std::basic_string<T>::npos )
		{
		std::basic_string<T> buf = s.substr(i, j - i);

		if( buf.size() > 0 )
		v.push_back(buf);

		i = ++j; j = s.find(c, j);
		}

		if( j == std::basic_string<T>::npos )
		{
		std::basic_string<T> buf = s.substr(i, s.length() - i);
		if( buf.size() > 0 )
		v.push_back(buf);
		}*/
		const std::basic_string<T> ss = s;
		ACE_Tokenizer_T<T> tok(const_cast<T*>( ss.c_str() ));
		tok.delimiter_replace(c, 0);
		for( T *p = tok.next(); p; p = tok.next() )
			v.push_back(p);
	}

	extern inline void kbe_splits(const std::string& s, const std::string& delim, std::vector< std::string >&result,
		const bool keep_empty)
	{
		//std::vector< std::string > result;
		if( delim.empty() )
		{
			result.push_back(s);
			//return result;
		}

		std::string::const_iterator substart = s.begin(), subend;

		while( true )
		{
			subend = std::search(substart, s.end(), delim.begin(), delim.end());
			std::string temp(substart, subend);
			if( keep_empty || !temp.empty() )
			{
				result.push_back(temp);
			}
			if( subend == s.end() )
			{
				break;
			}
			substart = subend + delim.size();
		}
		//return result;
	}

	extern inline int bytes2string(unsigned char *src, int srcsize, unsigned char *dst, int dstsize)
	{
		if( dst != NULL )
		{
			*dst = 0;
		}

		if( src == NULL || srcsize <= 0 || dst == NULL || dstsize <= srcsize * 2 )
		{
			return 0;
		}

		const char szTable[ ] = "0123456789ABCDEF";

		for( int i = 0; i < srcsize; i++ )
		{
			*dst++ = szTable[src[i] >> 4];
			*dst++ = szTable[src[i] & 0x0f];
		}

		*dst = 0;
		return  srcsize * 2;
	}
	extern inline int string2bytes(unsigned char* src, unsigned char* dst, int dstsize)
	{
		if( src == NULL )
			return 0;

		int iLen = strlen((char *) src);
		if( iLen <= 0 || iLen % 2 != 0 || dst == NULL || dstsize < iLen / 2 )
		{
			return 0;
		}

		iLen /= 2;
		kbe_strtoupper((char *) src);
		for( int i = 0; i < iLen; i++ )
		{
			int iVal = 0;
			unsigned char *pSrcTemp = src + i * 2;
			sscanf_s((char *) pSrcTemp, "%02x", &iVal);
			dst[i] = (unsigned char) iVal;
		}

		return iLen;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////// ×Ö·û×ª»» //////////////////////////////////////////////////
	/**
	* @class find_vec_string_exist_handle
	* @brief  -> vector<string>Ö®ÀàµÄÈÝÒ×Ê¹ÓÃ std::find_if À´²éÕÒÊÇ·ñ´æÔÚÄ³¸ö×Ö·û´® */
	template<typename T> class find_vec_string_exist_handle
	{
		public:
		find_vec_string_exist_handle(std::basic_string< T > str)
			: str_(str) { }

		bool operator()(const std::basic_string< T > &strSrc)
		{
			return strSrc == str_;
		}

		bool operator()(const T* strSrc)
		{
			return strSrc == str_;
		}
		private:
		std::basic_string< T > str_;
	};

	/* wchar 2 char */
	class MemoryStream;
	extern inline void kbe_wchar2char(const wchar_t* ts, MemoryStream* pStream)
	{
		//char* ptr = ACE_TEXT_ANTI_TO_TCHAR(ts);
		//size_t len = ACE_OS::strlen(ptr)+1;
		//pStream->data_resize(pStream->wpos() + len);
		//ACE_OS::memcpy(&pStream->data()[pStream->wpos()], ptr, len);
		//pStream->wpos(pStream->wpos() + len);
	}
	extern inline char* kbe_wchar2char(const wchar_t* ts, size_t* outlen = NULL)
	{
		return ACE_Wide_To_Ascii::convert(ts);
	}
	extern inline ACE_OS::WChar* kbe_char2wchar(const char* cs, size_t* outlen = NULL)
	{
		return ACE_Ascii_To_Wide::convert(cs);
	}

	/* utf8 to wchar */
	extern inline bool kbe_utf8_wchar(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize)
	{
		try
		{
			size_t len = utf8::distance(utf8str, utf8str + csize);

			if( len > wsize )
			{
				if( wsize > 0 )
					wstr[0] = L'\0';
				wsize = 0;
				return false;
			}

			wsize = len;
			utf8::utf8to16(utf8str, utf8str + csize, wstr);
			wstr[len] = L'\0';
		} catch( std::exception )
		{
			if( wsize > 0 )
				wstr[0] = L'\0';
			wsize = 0;
			return false;
		}

		return true;
	}
	extern inline bool kbe_utf8_wchar(const std::string& utf8str, wchar_t* wstr, size_t& wsize)
	{
		return kbe_utf8_wchar(utf8str.c_str(), utf8str.size(), wstr, wsize);
	}
	extern inline bool kbe_utf8_wchar(const std::string& utf8str, std::wstring& wstr)
	{
		try
		{
			size_t len = utf8::distance(utf8str.c_str(),
				utf8str.c_str() + utf8str.size());
			wstr.resize(len);

			if( len )
				utf8::utf8to16(utf8str.c_str(),
				utf8str.c_str() + utf8str.size(), &wstr[0]);
		} catch( std::exception )
		{
			wstr = L"";
			return false;
		}

		return true;
	}

	extern inline bool kbe_wchar_utf8(const wchar_t* wstr, size_t size, std::string& utf8str)
	{
		try
		{
			std::string utf8str2;
			utf8str2.resize(size * 4);                          // allocate for most long case

			char* oend = utf8::utf16to8(wstr, wstr + size, &utf8str2[0]);
			utf8str2.resize(oend - ( &utf8str2[0] ));             // remove unused tail
			utf8str = utf8str2;
		} catch( std::exception )
		{
			utf8str = "";
			return false;
		}

		return true;
	}
	extern inline bool kbe_wchar_utf8(const std::wstring& wstr, std::string& utf8str)
	{
		try
		{
			std::string utf8str2;
			utf8str2.resize(wstr.size() * 4);                   // allocate for most long case

			char* oend = utf8::utf16to8(wstr.c_str(),
				wstr.c_str() + wstr.size(), &utf8str2[0]);

			utf8str2.resize(oend - ( &utf8str2[0] ));             // remove unused tail
			utf8str = utf8str2;
		} catch( std::exception )
		{
			utf8str = "";
			return false;
		}

		return true;
	}

	inline size_t utf8length(std::string& utf8str)
	{
		try
		{
			return utf8::distance(utf8str.c_str(),
				utf8str.c_str() + utf8str.size());
		} catch( std::exception )
		{
			utf8str = "";
			return 0;
		}
	}
	////////////////////////////////////////////////// ×Ö·û×ª»» //////////////////////////////////////////////////
}
using namespace STRUTIL;
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include /**/ "ace/post.h"
#endif