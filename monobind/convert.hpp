/**
 * Copyright (c) 2007 Virgile 'xen' Bello
 * http://monobind.sourceforge.net
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef MONOBIND_CONVERT_HPP
#define MONOBIND_CONVERT_HPP

namespace monobind
{

template < typename T >
class convert_param
{
	typedef T converted;
};

template < typename T >
class convert_result
{
	typedef T base;
};

// Builtins types
// TODO : add string, etc...

template <>
class convert_result< void >
{
public:
	typedef void converted;
};

template <>
class convert_result< const char* >
{
public:
	typedef MonoString* converted;
	static inline MonoString* convert( const char* sz )
	{
		return mono_string_new( mono_domain_get(), sz );
	}
};

template <>
class convert_result< std::string >
{
public:
	typedef MonoString* converted;
	static inline MonoString* convert( const std::string& str )
	{
		return mono_string_new( mono_domain_get(), str.c_str() );
	}
};

template <>
class convert_result< const std::string& >
{
public:
	typedef MonoString* converted;
	static inline MonoString* convert( const std::string& str )
	{
		return mono_string_new( mono_domain_get(), str.c_str() );
	}
};

template <>
class convert_result< int >
{
public:
	typedef int converted;
	static inline int convert( int t )
	{
		return t;
	}
};

template <>
class convert_result< float >
{
public:
	typedef float converted;
	static inline float convert( float t )
	{
		return t;
	}
};

template <>
class convert_param< const char* >
{
public:
	typedef MonoString* base;
	static inline const char* convert( MonoString* str )
	{
		return mono_string_to_utf8( str );
	}
};

template <>
class convert_param< std::string >
{
public:
	typedef MonoString* base;
	static inline std::string convert( MonoString* str )
	{
		return std::string( mono_string_to_utf8( str ) );
	}
};

template <>
class convert_param< const std::string& >
{
public:
	typedef MonoString* base;
	static inline std::string convert( MonoString* str )
	{
		return std::string( mono_string_to_utf8( str ) );
	}
};

template <>
class convert_param< int >
{
public:
	typedef int base;
	static inline int convert( int t )
	{
		return t;
	}
};

template <>
class convert_param< float >
{
public:
	typedef float base;
	static inline float convert( float t )
	{
		return t;
	}
};

}

#endif
