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

#ifndef MONOBIND_NAMESPACE_HPP
#define MONOBIND_NAMESPACE_HPP

#include <string>
#include <monobind/scope.hpp>

namespace monobind
{

struct namespace_info : public scope_info
{
	std::string m_namespaceName;
	virtual void register_( MonoImage* monoImage, MonoDomain* monoDomain, const std::string& currentScope )
	{
		registerChildren( monoImage, monoDomain, currentScope + m_namespaceName + "." );
	}
};

class namespace_ : public scope
{
public:
	namespace_( const std::string& namespaceName )
		: scope( boost::shared_ptr< scope_info >( m_namespaceInfos = new namespace_info ) )
	{
		m_namespaceInfos->m_namespaceName = namespaceName;
	}
	namespace_& operator[]( const scope& s )
	{
		scope::operator[]( s );
		return *this;
	}

protected:
	namespace_info* m_namespaceInfos;
};

}

#endif
