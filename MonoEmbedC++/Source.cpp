#include "ace/ACE.h"

#include <mono/jit/jit.h>

#include <mono/metadata/mono-config.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/Object.h>
#include <stdlib.h>
#include <math.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <string>
#include <sstream>
#include <vector>

#include <monobind/function.hpp>
#include <monobind/class.hpp>
#include <monobind/namespace.hpp>
#include <monobind/scope.hpp>
#include <monobind/module.hpp>
#include <monobind/PathUtil.hpp>

#include "common\Profile.h"
#include <iostream>
using namespace monobind;

/*
* Very simple mono embedding example.
* Compile with:
* 	gcc -o teste teste.c `pkg-config --cflags --libs mono` -lm
* 	mcs TestFun.cs
* Run with:
* 	./teste TestFun.exe
*/
static MonoString* gimme(int b, int a) { return mono_string_new(mono_domain_get(), "All your monos are belong to us!"); }


class Base
{
	public:
	Base() { zz = 98; }
	Base(int a) { zz = a; }
	//Base( int a ) { zz = 912; }
	int test(float b)
	{
		//std::cout << "cpp->Base->test()" << std::endl;
		return zz;
	}
	std::string strfunc(const std::string& str)
	{
		//std::cout << "cpp->Base->strfunc()" << std::endl;
		std::stringstream result;
		result << zz << str;
		return result.str();
	}
	static int static_function(float b)
	{
		//std::cout << "cpp->Base->static_function()" << std::endl;
		return b;
	}

	int zz;

	class TestClass1
	{
	};
};

namespace monobind
{

	template <> class convert_param < Base* >
	{
		public:
		typedef MonoObject* base;
		static Base* convert(MonoObject* monoObject)
		{
			//ACE_DEBUG((LM_DEBUG, "COVERT\n"));
			//Base* ptr;
			//mono_field_get_value(monoObject, mono_class_get_field_from_name(mono_object_get_class(monoObject), "_native"), &ptr);
			//return ptr;
			return 0;
		}
	};

}

class TestClass3
{
	public:
	TestClass3() { }
	TestClass3(int a) { }
	void TestFun3(Base* testClass)
	{
		testClass->test(40.0f);
	}
};



class Test
{
	public:
	virtual void TestFun()
	{
		//return 0;
	}
};


class MObject
{
	public:
	MObject(MonoImage* monoImage, MonoDomain* monoDomain)
		: m_monoImage(monoImage), m_monoDomain(monoDomain)
	{
	}
	protected:
	MonoDomain* m_monoDomain;
	MonoImage* m_monoImage;
	MonoClass* m_monoClass;
	MonoObject* m_monoObject;
	uint32_t         m_gc_handle_;
	MonoMethod* method_;
	MonoObject* result_;
};


class TestCS : public MObject
{
	public:
	TestCS(MonoImage* monoImage, MonoDomain* monoDomain) : MObject(monoImage, monoDomain)
	{
		m_monoClass = mono_class_from_name(monoImage, "KBEngine", "TestClass");
		m_monoObject = mono_object_new(m_monoDomain, m_monoClass);
		m_gc_handle_ = mono_gchandle_new(m_monoObject, false);
		MonoMethod* method = mono_class_get_method_from_name(m_monoClass, ".ctor", 0);
		mono_runtime_invoke(method, m_monoObject, NULL, NULL);
		method_ = mono_class_get_method_from_name(m_monoClass, "test", 0);
	}
	void TestFun()
	{
		result_ = mono_runtime_invoke(method_, mono_gchandle_get_target(m_gc_handle_), NULL, NULL);
		//return *(int*) mono_object_unbox(result_);
	}

	~TestCS()
	{
		mono_gchandle_free(m_gc_handle_);
	}
};

typedef void ( MonoObject::*FUNC )( );
FUNC my_callback;

void RegisterCallback(FUNC cb)
{
	my_callback = cb;
}

void InvokeManagedCode(MonoObject* obj)
{
	if( my_callback == NULL )
	{
		printf("Managed code has not initialized this library yet");
		abort();
	}
	( obj->*my_callback )( );
}
int main(int argc, char* argv[ ])
{
	MonoDomain *domain;
	mono_set_dirs(GetLibDirectory().c_str(), GetConfigDirectory().c_str());
	domain = mono_jit_init_version("MonoApplication", "v4.0.30319");
	MonoAssembly* monoAssembly = mono_domain_assembly_open(mono_domain_get(), GetBinDirectory().append("MonoEmbedCsharp.dll").c_str());
	MonoImage* monoImage = mono_assembly_get_image(monoAssembly);

	module(monoImage, domain)
		[
			namespace_("KBEngine")
			[
				class_< Base >("Base")
				.def(constructor())
				.def(constructor< int >())
				.def("TestFun", &Base::test)
				.def("strfunc", &Base::strfunc)
				.def_readonly("aa", &Base::zz)
				.def_readwrite("bb", &Base::zz)
				.scope
				[
					//def( "static_function", &Base::static_function ),
					class_< Base::TestClass1 >("TestClass1")
					.def(constructor())
				],

				class_< TestClass3 >("TestClass3")
				.def(constructor< int >())
				.def("TestFun3", &TestClass3::TestFun3)
			]
		];

	mono_add_internal_call("MonoEmbed::gimme", gimme);
	mono_add_internal_call("MonoEmbed::static_function", Base::static_function);

	TestCS* testCS = new TestCS(monoImage, domain);
	Profile _localProfile;
	for( int i = 0; i < 10; i++ )
	{
		{
			{
				SCOPED_PROFILE(_localProfile);
				testCS->TestFun();
			};
			ACE_DEBUG(( LM_DEBUG,
				"%s::lastIntTime(%f ms), lastTime(%f ms), sumTime(%f ms),"
				"sumIntTime(%f ms),runningTime(%f ms) \n",
				_localProfile.name(),
				_localProfile.lastIntTimeInSeconds()*1000,
				_localProfile.lastTimeInSeconds()*1000,
				_localProfile.sumTimeInSeconds()*1000,
				_localProfile.sumIntTimeInSeconds()*1000,
				(double) runningTime() / stampsPerSecondD()*1000 ));
		}
	}
	delete testCS;

	int retval = mono_environment_exitcode_get();
	mono_jit_cleanup(domain);
	return retval;
}

