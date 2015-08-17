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

/*
	×Ö½ÚÐò½»»»´¦ÀíÄ£¿é£º
	ÓÉÓÚÍøÂçÍ¨ÐÅÒ»°ã²ÉÓÃBIG×Ö½ÚÐò\Ò²½Ð×öÍøÂç×Ö½ÚÐò.
	ÎÒÃÇÊ¹ÓÃµÄPC»ú»òÕßÇ¶ÈëÊ½ÏµÍ³¿ÉÄÜÊ¹ÓÃBIG×Ö½ÚÐòÒ²¿ÉÄÜÊ¹ÓÃLITTEN(Ð¡×Ö½ÚÐò)
	ËùÒÔÎÒÃÇ±ØÐëÔÚ´ËÖ®¼ä×öÒ»¸ö×Ö½ÚÐòµÄ×ª»»¡£
	*/
#ifndef KBE_MEMORYSTREAMCONVERTER_HPP
#define KBE_MEMORYSTREAMCONVERTER_HPP
// common include
#include "common.h"
#include <algorithm>
//// windows include	
//#if KBE_PLATFORM == PLATFORM_WIN32
//#else
//// linux include
//#include <errno.h>
//#endif

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

namespace MemoryStreamConverter
{
	template<size_t T>
	inline void convert(char *val)
	{
		std::swap(*val, *( val + T - 1 ));
		convert<T - 2>(val + 1);
	}

	template<> inline void convert<0>(char *) { }
	template<> inline void convert<1>(char *) { }            // ignore central byte

	template<typename T> inline void apply(T *val)
	{
		convert<sizeof(T)>((char *) ( val ));
	}

	inline void convert(char *val, size_t size)
	{
		if( size < 2 )
			return;

		std::swap(*val, *( val + size - 1 ));
		convert(val + 1, size - 2);
	}
}

#if KBE_BYTE_ORDR == KBE_BIG_ENDIAN	// ¿ÉÒÔÊ¹ÓÃsys.isPlatformLittleEndian() ½øÐÐ²âÊÔ
template<typename T> inline void EndianConvert(T& val) { MemoryStreamConverter::apply<T>(&val); }
template<typename T> inline void EndianConvertReverse(T&) { }
#else
template<typename T> inline void EndianConvert(T&) { }
template<typename T> inline void EndianConvertReverse(T& val) { MemoryStreamConverter::apply<T>(&val); }
#endif

template<typename T> void EndianConvert(T*);         // will generate link error
template<typename T> void EndianConvertReverse(T*);  // will generate link error

inline void EndianConvert(ACE_UINT8&) { }
inline void EndianConvert(ACE_INT8&) { }
inline void EndianConvertReverse(ACE_UINT8&) { }
inline void EndianConvertReverse(ACE_INT8&) { }

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif
