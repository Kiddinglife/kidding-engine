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

#ifndef MONOBIND_MODULE_HPP
#define MONOBIND_MODULE_HPP

#include <string>
#include <monobind/scope.hpp>

namespace monobind
{

class module
{
public:
	module( MonoImage* monoImage, MonoDomain* monoDomain )
		: m_monoImage( monoImage ), m_monoDomain( monoDomain )
	{
	}

	void SetImage(MonoImage* monoImage) { m_monoImage = monoImage; }
	void SetDomain(MonoDomain* monoDomain) { m_monoDomain = monoDomain; }

	void operator[]( const scope& s )
	{
		boost::shared_ptr< scope_info > next = s.m_scopeInfos;
		while ( next )
		{
			next->register_( m_monoImage, m_monoDomain, "" );
			next = next->m_next;
		}
	}

protected:
	MonoImage* m_monoImage;
	MonoDomain* m_monoDomain;
};

}

#endif
