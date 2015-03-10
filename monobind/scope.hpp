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

#ifndef MONOBIND_SCOPE_HPP
#define MONOBIND_SCOPE_HPP

#include <vector>
#include <boost/shared_ptr.hpp>

namespace monobind
{

	struct scope_info
	{
		public:
		virtual ~scope_info() { }
		virtual void register_(MonoImage* monoImage, MonoDomain* monoDomain, const std::string& currentScope) { }
		virtual void registerChildren(MonoImage* monoImage, MonoDomain* monoDomain, const std::string& thisScope);
		private:
		friend class scope;
		friend class module;
		std::vector< boost::shared_ptr< scope_info > > m_children;
		boost::shared_ptr< scope_info > m_next;
	};

	class scope
	{
		public:
		scope()
			: m_scopeInfos(new scope_info)
		{
		}
		scope(const boost::shared_ptr< scope_info >& scopeInfos)
			: m_scopeInfos(scopeInfos)
		{
		}
		scope(const scope& s)
			: m_scopeInfos(s.m_scopeInfos)
		{
		}

		scope& operator[](const scope& s)
		{
			m_scopeInfos->m_children.push_back(s.m_scopeInfos);
			return *this;
		}
		scope& operator,( scope& s );
		protected:
		static std::vector< boost::shared_ptr< void > > ms_wrappers;
		boost::shared_ptr< scope_info > m_scopeInfos;
		friend class module;
	};

}

#endif
