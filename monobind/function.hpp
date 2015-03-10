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

#if !BOOST_PP_IS_ITERATING

#ifndef MONOBIND_FUNCTION_HPP
#define MONOBIND_FUNCTION_HPP

#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <string>

#include <monobind/config.hpp>
#include <monobind/scope.hpp>
#include <monobind/convert.hpp>

namespace monobind
{

template < typename R >
struct function_result_type
{
#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/function.hpp>, 1 ) )
#include BOOST_PP_ITERATE()
};

template <>
struct function_result_type< void >
{
#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/function.hpp>, 2 ) )
#include BOOST_PP_ITERATE()
};

struct function_info : public scope_info
{
#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/function.hpp>, 3 ) )
#include BOOST_PP_ITERATE()

	std::string m_functionName;
	void* m_functionWrapper;
	void* m_function;

	virtual void register_( MonoImage* monoImage, MonoDomain* monoDomain, const std::string& currentScope );
};

#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/function.hpp>, 4 ) )
#include BOOST_PP_ITERATE()

}

#endif

#elif BOOST_PP_ITERATION_FLAGS() == 1

#define MONOBIND_TYPE( z, n, _ ) typename convert_param< A ## n >::base
#define MONOBIND_PARAMS( z, n, _ ) typename convert_param< A ## n >::base a ## n
#define MONOBIND_CONVERTER( z, n, _ ) convert_param< A ## n >::convert( a ## n )

#if BOOST_PP_ITERATION() > 0
template < BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), class A ) >
#endif
static typename convert_result< R >::converted call_function( R(*f)( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) ) BOOST_PP_COMMA_IF( BOOST_PP_ITERATION() ) BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_PARAMS, nil ) )
{
	return convert_result< R >::convert( f( BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_CONVERTER, nil ) ) );
}

#undef MONOBIND_PARAMS
#undef MONOBIND_CONVERTER
#undef MONOBIND_TYPE

#elif BOOST_PP_ITERATION_FLAGS() == 2

#define MONOBIND_TYPE( z, n, _ ) typename convert_param< A ## n >::base
#define MONOBIND_PARAMS( z, n, _ ) typename convert_param< A ## n >::base a ## n
#define MONOBIND_CONVERTER( z, n, _ ) convert_param< A ## n >::convert( a ## n )

#if BOOST_PP_ITERATION() > 0
template < BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), class A ) >
#endif
static void call_function( void(*f)( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) ) BOOST_PP_COMMA_IF( BOOST_PP_ITERATION() ) BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_PARAMS, nil ) )
{
	f( BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_CONVERTER, nil ) );
}

#undef MONOBIND_PARAMS
#undef MONOBIND_CONVERTER
#undef MONOBIND_TYPE

#elif BOOST_PP_ITERATION_FLAGS() == 3

#define MONOBIND_TYPE( z, n, _ ) typename convert_param< A ## n >::base
#define MONOBIND_PARAMS( z, n, _ ) typename convert_param< A ## n >::base a ## n
#define MONOBIND_CONVERTER( z, n, _ ) convert_param< A ## n >::convert( a ## n )

template < class R BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), class A ) >
function_info( const std::string& functionName, R( *f )( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) ) )
	: m_functionName( functionName ), m_function( ( void* )f )
{
	typedef typename convert_result< R >::converted converted_result;
	converted_result (*f2)( R(*)( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) ) BOOST_PP_COMMA_IF( BOOST_PP_ITERATION() ) BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_TYPE, nil ) ) = &function_result_type< R >::template call_function< BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) >;
	m_functionWrapper = ( void* )f2;
}

#undef MONOBIND_PARAMS
#undef MONOBIND_CONVERTER
#undef MONOBIND_TYPE

#elif BOOST_PP_ITERATION_FLAGS() == 4

#define MONOBIND_PARAMS( z, n, _ ) typename convert_param< A ## n >::base a ## n
#define MONOBIND_CONVERTER( z, n, _ ) convert_param< A ## n >::convert( a ## n )

template < class R BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), class A ) >
scope def( const std::string& functionName, R( *f )( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) ) )
{
	return scope( boost::shared_ptr< scope_info >( new function_info( functionName, f ) ) );
}

#undef MONOBIND_PARAMS
#undef MONOBIND_CONVERTER

#endif
