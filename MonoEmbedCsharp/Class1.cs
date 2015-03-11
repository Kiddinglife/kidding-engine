using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace KBEngine
{
	public class Base
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public Base();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public Base(int a);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public int TestFun(IntPtr wrapper, float z);
		static readonly IntPtr TestFun_wrapper;
		public int TestFun(float z)
		{
			return TestFun(TestFun_wrapper, z);
		}

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public string strfunc(IntPtr wrapper, string param);
		static readonly IntPtr strfunc_wrapper;
		public override string strfunc(string param)
		{
			return strfunc(strfunc_wrapper, param);
		}

		//[MethodImplAttribute(MethodImplOptions.InternalCall)]
		//extern public int TestFun(float z);
		static readonly IntPtr get_bb_wrapper;
		static readonly IntPtr set_bb_wrapper;

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public int get_bb(IntPtr wrapper);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public int set_bb(IntPtr wrapper, int value);

		public int bb
		{
			get { return get_bb(get_bb_wrapper); }
			set { set_bb(set_bb_wrapper, value); }
		}

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static int static_function(float b);

		public IntPtr _native;
	}

	class TestClass3
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public TestClass3();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public int TestFun3(IntPtr wrapper, Base test);
		static readonly IntPtr TestFun3_wrapper;
		public int TestFun3(Base test)
		{
			return TestFun3(TestFun3_wrapper, test);
		}

		public IntPtr _native;
	}

	public class MyBase : Base
	{
		public string strfunc(string param)
		{
			base.strfunc(param);
			return "MyBase+" + base.strfunc(param);
		}
	}
	public class TestClass
	{
		int b;
		KBEngine.Base tc2;
		KBEngine.Base tc3;
		KBEngine.TestClass3 tc4;
		KBEngine.MyBase tc5;
		public TestClass()
		{
			bb = 2131;
			tc2 = new KBEngine.Base(30);
			tc3 = new KBEngine.Base(40);
			tc4 = new KBEngine.TestClass3();
			tc5 = new KBEngine.MyBase();
		}
		public int bb
		{
			get { return b; }
			set { b = value; }
		}
		public void test()
		{
			tc5.TestFun(12f);
			tc5.strfunc("hello");
			MonoEmbed.static_function(2f);
			MonoEmbed.gimme(2);
		}
	}
}

public class MonoEmbed
{
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	public extern static string gimme(int a);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	public extern static int static_function(float a);
}

