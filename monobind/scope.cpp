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

#include <mono/jit/jit.h>
#include <monobind/scope.hpp>

namespace monobind
{
	std::vector< boost::shared_ptr< void > > scope::ms_wrappers;

	void scope_info::registerChildren(MonoImage* monoImage, MonoDomain* monoDomain, const std::string& thisScope)
	{
		for( unsigned int i = 0; i < m_children.size(); ++i )
		{
			m_children[i]->register_(monoImage, monoDomain, thisScope);
			boost::shared_ptr< scope_info > next = m_children[i]->m_next;
			while( next )
			{
				next->register_(monoImage, monoDomain, thisScope);
				next = next->m_next;
			}
		}
	}

	scope& scope::operator,( scope& s )
	{
		boost::shared_ptr< scope_info > scopeInfos = m_scopeInfos;
		while( scopeInfos->m_next )
		{
			scopeInfos = scopeInfos->m_next;
		}
		scopeInfos->m_next = s.m_scopeInfos;
		return *this;
	}


}