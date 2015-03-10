
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

using namespace monobind;

/*
* Very simple mono embedding example.
* Compile with:
* 	gcc -o teste teste.c `pkg-config --cflags --libs mono` -lm
* 	mcs test.cs
* Run with:
* 	./teste test.exe
*/

static MonoString* gimme(int b, int a)
{
	return mono_string_new(mono_domain_get(), "All your monos are belong to us!");
}

class TestClass2
{
	public:
	TestClass2() { zz = 98; }
	TestClass2(int a) { zz = a; }
	//TestClass2( int a ) { zz = 912; }
	int test(float b)
	{
		return zz;
	}
	std::string strfunc(const std::string& str)
	{
		std::stringstream result;
		result << zz << str;
		return result.str();
	}
	static int static_function(float b)
	{
		return b;
	}

	int zz;

	class TestClass1
	{
	};
};

namespace monobind
{

	template <>
	class convert_param < TestClass2* >
	{
		public:
		typedef MonoObject* base;
		static TestClass2* convert(MonoObject* monoObject)
		{
			MonoClassField* nativeClassField = mono_class_get_field_from_name(mono_object_get_class(monoObject), "_native");
			TestClass2* ptr;
			mono_field_get_value(monoObject, nativeClassField, &ptr);
			return ptr;
		}
	};

}

class TestClass3
{
	public:
	TestClass3() { }
	TestClass3(int a) { }
	void TestFun3(TestClass2* testClass)
	{
		testClass->test(40.0f);
	}
};

static MonoAssembly* main_function(MonoDomain *domain, const char *file)
{
	MonoAssembly *assembly;

	assembly = mono_domain_assembly_open(domain, file);
	if( !assembly )
		exit(2);

	return assembly;
}

class Test
{
	virtual int test()
	{
		return 10;
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
};

class TestCS : public Test, public MObject
{
	public:
	TestCS(MonoImage* monoImage, MonoDomain* monoDomain) : MObject(monoImage, monoDomain)
	{
		m_monoClass = mono_class_from_name(monoImage, "Embed", "TestClass");
		m_monoObject = mono_object_new(m_monoDomain, m_monoClass);
		MonoMethod* method = mono_class_get_method_from_name(m_monoClass, ".ctor", 0);
		mono_runtime_invoke(method, m_monoObject, NULL, NULL);
	}
	virtual int test()
	{
		MonoMethod* method = mono_class_get_method_from_name(m_monoClass, "test", 0);
		MonoObject* result = mono_runtime_invoke(method, m_monoObject, NULL, NULL);
		return *(int*) mono_object_unbox(result);
	}
};

int main(int argc, char* argv[ ])
{
	MonoDomain *domain;
	mono_set_dirs(GetLibDirectory().c_str(), GetConfigDirectory().c_str());
	domain = mono_jit_init_version("MonoApplication", "v4.0.30319");
	MonoAssembly* monoAssembly = mono_domain_assembly_open(mono_domain_get(), GetBinDirectory().append("MonoEmbedCsharp.dll").c_str());
	MonoImage* monoImage = mono_assembly_get_image(monoAssembly);

	module(monoImage, domain)
		[
			namespace_("Embed")
			[
				class_< TestClass2 >("TestClass2")
				.def(constructor())
				.def(constructor< int >())
				.def("TestFun", &TestClass2::test)
				.def("strfunc", &TestClass2::strfunc)
				.def_readonly("aa", &TestClass2::zz)
				.def_readwrite("bb", &TestClass2::zz)
				.scope
				[
					//def( "static_function", &TestClass2::static_function ),
					class_< TestClass2::TestClass1 >("TestClass1")
					.def(constructor())
				],

				class_< TestClass3 >("TestClass3")
				.def(constructor< int >())
				.def("TestFun3", &TestClass3::TestFun3)
			]
		];

	mono_add_internal_call("monoembed::gimmebis", gimme);
	mono_add_internal_call("monoembed::gimme", gimme);

	TestCS testCS(monoImage, domain);
	int a = testCS.test();

	int retval = mono_environment_exitcode_get();
	mono_jit_cleanup(domain);
	return retval;
}

