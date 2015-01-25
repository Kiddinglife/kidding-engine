#include "stdafx.h"
#include "CppUnitTest.h"
#include "net\Packet.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{		
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(TestMet1)
		{
			// TODO: Your test code here
			Packet p;
			printf("length = %d, ",p.length());
				ACE_DEBUG((LM_DEBUG, "HELLO"));
		}

	};
}