using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Embed
{
	class TestClass2
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public TestClass2();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public TestClass2(int a);

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
		public string strfunc(string param)
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
		public IntPtr _native;
	}
	class TestClass3
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public TestClass3();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public int TestFun3(IntPtr wrapper, TestClass2 test);
		static readonly IntPtr TestFun3_wrapper;
		public int TestFun3(TestClass2 test)
		{
			return TestFun3(TestFun3_wrapper, test);
		}

		public IntPtr _native;
	}

	class TestClass
	{
		int b;
		Embed.TestClass2 tc2;
		Embed.TestClass2 tc3;
		Embed.TestClass3 tc4;
		Embed.TestClass tc;

		public TestClass()
		{
			bb = 2131;
			tc2 = new Embed.TestClass2(30);
			tc3 = new Embed.TestClass2(40);
			tc4 = new Embed.TestClass3();
		}
		public int bb
		{
			get { return b; }
			set { b = value; }
		}
		public int test()
		{
			tc3.bb = 14323;
			//Console.WriteLine("tc3.bb->"+tc3.bb);
			//Console.WriteLine("tc3.strfunc(didum)->"+tc3.strfunc("didum"));
			//tc4.TestFun3(tc2);
			//Console.WriteLine( tc2.test() );
			//Console.WriteLine("tc2.TestFun(52.5f)->" + tc2.TestFun(52.5f));
			//Console.WriteLine("tc3.TestFun(52.5f)->" + tc3.TestFun(52.5f));
			//Console.WriteLine("tc2.bb->" + tc2.bb);

			Embed.TestClass tc = new Embed.TestClass();
			//Console.WriteLine(tc.bb);

			//int a = 0;
			//for (int i = 0; i < 100000; ++i)
			//{
			//	//a = i;
			//	tc3.bb = i;
			//}
			return bb;
		}
	}
}

public class MonoEmbed
{
	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	extern static string gimme(int a);

	[MethodImplAttribute(MethodImplOptions.InternalCall)]
	extern static string gimmebis(int a);
}

