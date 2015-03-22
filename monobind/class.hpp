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

#ifndef MONOBIND_CLASS_HPP
#define MONOBIND_CLASS_HPP

#include <string>
#include <map>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_shifted_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>

#include <monobind/config.hpp>
#include <monobind/convert.hpp>
#include <monobind/scope.hpp>

namespace monobind
{

#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/class.hpp>, 1 ) )
#include BOOST_PP_ITERATE()

template < class T, class A >
class member_field_wrapper
{
public:
	member_field_wrapper( const std::string& fieldName, A T::*field )
		: m_field( field ), m_nativeClassField( 0 ), m_memberClassField( 0 ), m_fieldName( fieldName )
	{
	}
	A T::*m_field;
	MonoClassField* m_memberClassField;
	MonoClassField* m_nativeClassField;
	std::string m_fieldName;
};

template < typename T, typename A >
typename convert_param< A >::base get_field( MonoObject* monoObject, member_field_wrapper< T, A >* wrapper )
{
	if ( wrapper->m_nativeClassField == 0 )
		wrapper->m_nativeClassField = mono_class_get_field_from_name( mono_object_get_class( monoObject ), "_native" );

	T* ptr;
	mono_field_get_value( monoObject, wrapper->m_nativeClassField, &ptr );

	return convert_param< A >::convert( ( ptr->*( wrapper->m_field ) ) );
}

template < typename T, typename A >
void set_field( MonoObject* monoObject, member_field_wrapper< T, A >* wrapper, typename convert_param< A >::base a )
{
	if ( wrapper->m_nativeClassField == 0 )
		wrapper->m_nativeClassField = mono_class_get_field_from_name( mono_object_get_class( monoObject ), "_native" );

	T* ptr;
	mono_field_get_value( monoObject, wrapper->m_nativeClassField, &ptr );

	ptr->*( wrapper->m_field ) = convert_param< A >::convert( a );
}

template < typename R >
struct class_member_return_type
{
#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/class.hpp>, 2 ) )
#include BOOST_PP_ITERATE()
};

template <>
struct class_member_return_type< void >
{
#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/class.hpp>, 3 ) )
#include BOOST_PP_ITERATE()
};

#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/class.hpp>, 4 ) )
#include BOOST_PP_ITERATE()

struct class_info : public scope_info
{
	MonoClass* m_monoClass;
	std::string m_className;
	std::map< std::string, std::pair< void*, void* > > m_functions;

	class_info() : m_monoClass( 0 ) {}
	virtual void register_( MonoImage* monoImage, MonoDomain* monoDomain, const std::string& currentScope );
};

class class_base : public scope
{
public:
	class_base( const std::string& className )
		: scope( boost::shared_ptr< scope_info >( m_classInfos = new class_info ) )
	{
		m_classInfos->m_className = className;
	}
protected:
	class_info* m_classInfos;
};

template < class T >
class inner_scope
{
public:
	inner_scope( T& base )
		: m_base( base )
	{
	}
	T& operator[]( scope s ) const
	{
		m_base[ s ];
		return m_base;
	}

	T& m_base;
};

#pragma warning(push)
#pragma warning(disable : 4355)

template < class T >
class class_ : public class_base
{
public:
	class_( const std::string& className )
		: class_base( className ), scope( *this )
	{
		m_classInfos->m_className = className;
	}

#define BOOST_PP_ITERATION_PARAMS_1 ( 4, ( 0, MONOBIND_MAX_ARITY, <monobind/class.hpp>, 5 ) )
#include BOOST_PP_ITERATE()

	template < typename A >
	class_& add_getter( const std::string& fieldName, A T::*member )
	{
		std::string functionName = "get_" + fieldName;
		boost::shared_ptr< member_field_wrapper< T, A > > member_field( new member_field_wrapper< T, A >( fieldName, member ) );
		ms_wrappers.push_back( member_field );
		A(*internal_getter)( MonoObject*, member_field_wrapper< T, A >* ) = get_field< T, A >;
		m_classInfos->m_functions[ functionName ] = std::pair< void*, void* >( ( void* )internal_getter, ( void* )member_field.get() );

		return *this;
	}

	template < typename A >
	class_& add_setter( const std::string& fieldName, A T::*member )
	{
		std::string functionName = "set_" + fieldName;
		boost::shared_ptr< member_field_wrapper< T, A > > member_field( new member_field_wrapper< T, A >( fieldName, member ) );
		ms_wrappers.push_back( member_field );
		void(*internal_setter)( MonoObject*, member_field_wrapper< T, A >*, A ) = set_field< T, A >;
		m_classInfos->m_functions[ functionName ] = std::pair< void*, void* >( ( void* )internal_setter, ( void* )member_field.get() );

		return *this;
	}

	template < typename A >
	class_& def_readwrite( const std::string& fieldName, A T::*member )
	{
		add_getter( fieldName, member );
		add_setter( fieldName, member );

		return *this;
	}

	template < typename A >
	class_& def_readonly( const std::string& fieldName, A T::*member )
	{
		add_getter( fieldName, member );

		return *this;
	}

	inner_scope< class_< T > > scope;
};
#pragma warning(pop)
}

#endif

#elif BOOST_PP_ITERATION_FLAGS() == 1

#define MONOBIND_MEMBER_FUNCTION_WRAPPER BOOST_JOIN( member_function_wrapper, BOOST_PP_ITERATION() )

template < typename T, typename R BOOST_PP_ENUM_TRAILING_PARAMS_Z(1, BOOST_PP_ITERATION(), class A) >
class MONOBIND_MEMBER_FUNCTION_WRAPPER
{
public:
	MONOBIND_MEMBER_FUNCTION_WRAPPER( R(T::*f)( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) ) )
		: m_f( f ), m_nativeClassField( 0 )
	{
	}
	R(T::*m_f)( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) );
	MonoClassField* m_nativeClassField;
};

#undef MONOBIND_MEMBER_FUNCTION_WRAPPER

#elif BOOST_PP_ITERATION_FLAGS() == 2

#define MONOBIND_MEMBER_FUNCTION_WRAPPER BOOST_JOIN( member_function_wrapper, BOOST_PP_ITERATION() )
#define MONOBIND_PARAMS( z, n, _ ) typename convert_param< A ## n >::base a ## n
#define MONOBIND_CONVERTER( z, n, _ ) convert_param< A ## n >::convert( a ## n )

template < typename T BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), class A ) >
static typename convert_result< R >::converted call_member( MonoObject* monoObject, MONOBIND_MEMBER_FUNCTION_WRAPPER< T, R BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), A ) >* wrapper BOOST_PP_COMMA_IF( BOOST_PP_ITERATION() ) BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_PARAMS, nil ) )
{
	if ( wrapper->m_nativeClassField == 0 )
		wrapper->m_nativeClassField = mono_class_get_field_from_name( mono_object_get_class( monoObject ), "_native" );

	T* ptr;
	mono_field_get_value( monoObject, wrapper->m_nativeClassField, &ptr );

	return convert_result< R >::convert( ( ptr->*( wrapper->m_f ) )( BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_CONVERTER, nil ) ) );
}

#undef MONOBIND_MEMBER_FUNCTION_WRAPPER
#undef MONOBIND_PARAMS
#undef MONOBIND_CONVERTER

#elif BOOST_PP_ITERATION_FLAGS() == 3

#define MONOBIND_MEMBER_FUNCTION_WRAPPER BOOST_JOIN( member_function_wrapper, BOOST_PP_ITERATION() )
#define MONOBIND_PARAMS( z, n, _ ) typename convert_param< A ## n >::base a ## n
#define MONOBIND_CONVERTER( z, n, _ ) convert_param< A ## n >::convert( a ## n )

template < typename T BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), class A ) >
static void call_member( MonoObject* monoObject, MONOBIND_MEMBER_FUNCTION_WRAPPER< T, void BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), A ) >* wrapper BOOST_PP_COMMA_IF( BOOST_PP_ITERATION() ) BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_PARAMS, nil ) )
{
	if ( wrapper->m_nativeClassField == 0 )
		wrapper->m_nativeClassField = mono_class_get_field_from_name( mono_object_get_class( monoObject ), "_native" );

	T* ptr;
	mono_field_get_value( monoObject, wrapper->m_nativeClassField, &ptr );

	( ptr->*( wrapper->m_f ) )( BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_CONVERTER, nil ) );
}

#undef MONOBIND_MEMBER_FUNCTION_WRAPPER
#undef MONOBIND_PARAMS
#undef MONOBIND_CONVERTER

#elif BOOST_PP_ITERATION_FLAGS() == 4

#define MONOBIND_CONSTRUCTOR BOOST_JOIN( constructor, BOOST_PP_ITERATION() )

#if BOOST_PP_ITERATION() == 0

class MONOBIND_CONSTRUCTOR
{
};

inline MONOBIND_CONSTRUCTOR constructor()
{
	return MONOBIND_CONSTRUCTOR();
}

#else

template < BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), class A ) >
class MONOBIND_CONSTRUCTOR
{
};

template < BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), class A ) >
MONOBIND_CONSTRUCTOR< BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) > constructor()
{
	return MONOBIND_CONSTRUCTOR< BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) >();
}

#endif

template < typename T BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), class A ) >
void construct( MonoObject* object BOOST_PP_COMMA_IF( BOOST_PP_ITERATION() ) BOOST_PP_ENUM_BINARY_PARAMS_Z( 1, BOOST_PP_ITERATION(), A, a ) )
{
	MonoClass* monoClass = mono_object_get_class( object );
	MonoClassField* field = mono_class_get_field_from_name( monoClass, "_native" );
	T* val = new T( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), a ) );
	mono_field_set_value( object, field, &val );
}

#elif BOOST_PP_ITERATION_FLAGS() == 5

#define MONOBIND_MEMBER_FUNCTION_WRAPPER BOOST_JOIN( member_function_wrapper, BOOST_PP_ITERATION() )
#define MONOBIND_TYPE( z, n, _ ) typename convert_param< A ## n >::base

template < class R BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), class A ) >
class_& def( const std::string& functionName, R(T::*f)( BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) ) )
{
	typedef typename convert_result< R >::converted converted_result;
	converted_result( *internal_call )( MonoObject*, MONOBIND_MEMBER_FUNCTION_WRAPPER< T, R BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), A ) >* BOOST_PP_COMMA_IF( BOOST_PP_ITERATION() ) BOOST_PP_ENUM( BOOST_PP_ITERATION(), MONOBIND_TYPE, nil ) ) = &class_member_return_type< R >::template call_member< T BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), A ) >;
	boost::shared_ptr< MONOBIND_MEMBER_FUNCTION_WRAPPER< T, R BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), A ) > > function_wrapper( new MONOBIND_MEMBER_FUNCTION_WRAPPER< T, R BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), A ) >( f ) );
	ms_wrappers.push_back( function_wrapper );
	m_classInfos->m_functions[ functionName ] = std::pair< void*, void* >( ( void* )internal_call, ( void* )function_wrapper.get() );
	return *this;
}

#undef MONOBIND_MEMBER_FUNCTION_WRAPPER
#undef MONOBIND_TYPE

#define MONOBIND_CONSTRUCTOR BOOST_JOIN( constructor, BOOST_PP_ITERATION() )

#if BOOST_PP_ITERATION() == 0

class_& def( MONOBIND_CONSTRUCTOR sig )
{
	void(*f2)( MonoObject* ) = construct< T >;
	m_classInfos->m_functions[ ".ctor" ] = std::pair< void*, void* >( ( void* )f2, ( void* )0 );
	return *this;
}

#else

template < BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), class A ) >
class_& def( MONOBIND_CONSTRUCTOR< BOOST_PP_ENUM_PARAMS( BOOST_PP_ITERATION(), A ) > sig )
{
	void(*f2)( MonoObject* BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), A ) ) = construct< T BOOST_PP_ENUM_TRAILING_PARAMS_Z( 1, BOOST_PP_ITERATION(), A ) >;
	m_classInfos->m_functions[ ".ctor" ] = std::pair< void*, void* >( ( void* )f2, ( void* )0 );
	return *this;
}

#endif

#endif
