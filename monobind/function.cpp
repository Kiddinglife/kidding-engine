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
#include <monobind/function.hpp>

namespace monobind
{

void function_info::register_( MonoImage* monoImage, MonoDomain* monoDomain, const std::string& currentScope )
{
	std::string fullFunctionName = currentScope + m_functionName;

	//throw std::exception( "not implemented yet" );

	/*if ( function->second.second != 0 )
	{
		MonoClassField* monoClassField = mono_class_get_field_from_name( m_monoClass, ( m_functionName + "_wrapper" ).c_str() );
		if ( monoClassField )
		{
			mono_field_static_set_value( mono_class_vtable( monoDomain, m_monoClass ), monoClassField, (void*)&m_function );
		}
	}
	mono_add_internal_call( fullFunctionName.c_str(), m_functionWrapper );*/
	//mono_add_internal_call_param( fullFunctionName.c_str(), m_function );

	registerChildren( monoImage, monoDomain, currentScope );
}

}