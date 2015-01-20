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
#ifndef KBE_STRING_CONV_HPP
#define KBE_STRING_CONV_HPP
#include "common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
namespace StringConv
{

	template<typename T>
	T str2value(const std::string& s)
	{
		std::istringstream is(s);
		T t;
		is >> t;
		return t;
	}

	template<typename T>
	T str2value(const char* s)
	{
		std::istringstream is(s);
		T t;
		is >> t;
		return t;
	}

	template <class TYPE>
	inline std::string val2str(const TYPE& value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	inline void str2value(float& value, const char * pstr)
	{
#if KBE_PLATFORM == PLATFORM_WIN32
		std::stringstream ss;
		ss << pstr;
		ss >> value;
#else
		char* endptr;
		value = ACE_OS::strtof(pstr, &endptr);

		if( *endptr )
			throw std::runtime_error("not a number");
#endif
	}

	inline void str2value(double& value, const char * pstr)
	{
		char* endptr;
		value = ACE_OS::strtod(pstr, &endptr);
		if( *endptr )
			throw std::runtime_error("not a number");
	}

	inline void str2value(ACE_INT32& value, const char * pstr)
	{
		char* endptr;
		value = ACE_OS::strtol(pstr, &endptr, 10);
		if( *endptr )
			throw std::runtime_error("not a number");
	}

	inline void str2value(ACE_INT8& value, const char * pstr)
	{
		ACE_INT32 i;
		str2value(i, pstr);
		value = ACE_INT8(i);
		if( value != i )
			throw std::runtime_error("out of range");
	}

	inline void str2value(ACE_INT16& value, const char * pstr)
	{
		ACE_INT32 i;
		str2value(i, pstr);
		value = ACE_INT16(i);
		if( value != i )
			throw std::runtime_error("out of range");
	}

	inline void str2value(ACE_UINT32& value, const char * pstr)
	{
		char* endptr;
		value = ACE_OS::strtoul(pstr, &endptr, 10);
		if( *endptr )
			throw std::runtime_error("not a number");
	}

	inline void str2value(ACE_UINT8& value, const char * pstr)
	{
		ACE_UINT32 ui;
		str2value(ui, pstr);
		value = ACE_UINT8(ui);
		if( value != ui )
			throw std::runtime_error("out of range");
	}

	inline void str2value(ACE_UINT16& value, const char * pstr)
	{
		ACE_UINT32 ui;
		str2value(ui, pstr);
		value = ACE_UINT16(ui);
		if( value != ui )
			throw std::runtime_error("out of range");
	}

	inline void str2value(ACE_INT64& value, const char * pstr)
	{
		char* endptr;
		value = ACE_OS::strtoll(pstr, &endptr, 10);
		if( *endptr )
			throw std::runtime_error("not a number");
	}

	inline void str2value(ACE_UINT64& value, const char * pstr)
	{
		char* endptr;
		value = ACE_OS::strtoull(pstr, &endptr, 10);
		if( *endptr )
			throw std::runtime_error("not a number");
	}


}
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif // KBE_STRING_CONV_HPP


