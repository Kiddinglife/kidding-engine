/*
 * @ Error link 2019 in gtest:
 * the reason is that you need to link lib (dll or static) to the project.
 * usually I just include headers, iy van be complied successfully but
 * there must error 2019 when trying to linklibs
 * @ Soulutions:
 * 1. add refernce project to the gtest project will then automacially find the right libs to link
 * (prefered way because it wil detect the change to the src files and will complie the agian before testing.)
 * 2. maunally add lib directories and lib dependency in the gtest project.
 */
//#define ACE_NLOGGING
//#define ACE_NTRACE 0

#include <iostream>
#include <time.h>
#include <string>
#include "gtest\gtest.h"
#include "ace/Log_Msg.h"
#include "ace/mytrace.h"
#include  "ace/Refcounted_Auto_Ptr.h"
#include "ace/Auto_Ptr.h"
#include "ace/ACE.h"
#include "common/ace_object_pool.h"

//TEST(ObjectPoolTest, Test)
//{
//	class Obj :public PoolObject
//	{
//		public:
//		void onReclaimObject()
//		{
//
//		}
//	};
//
//	/*
//	/// test of 1 Ç§Íò¶ÔÏó·ÖÅäºÍÊÍ·Å
//
//	//kbeµÄpool
//	ObjectPool<Obj, ACE_Null_Mutex> pool("pool");
//
//	/// ×Ô¼ºµÄpool common way
//	//ACE_ObjectPoolFactory<Obj, ACE_Null_Mutex>* poolFactory = ACE_ObjectPoolFactory<Obj, ACE_Null_Mutex>::Singlton::instance();
//	//ACE_ObjectPoolFactory<Obj, ACE_Null_Mutex>::ACEObjectPoolPTR ObjPool = poolFactory->get_obj_pool("ObjPool");
//	/// EASY way
//	ACE_PoolPtr_Getter(ObjPool, Obj, ACE_Null_Mutex);
//	ACE_PoolFactoryPtr_Getter(objPoolFactory, Obj, ACE_Null_Mutex);
//
//	time_t t_start, t_end;
//	t_start = time(NULL);
//	for( ACE_UINT64 i = 0; i < 1000000; ++i )
//	{
//	Obj* PTR = ObjPool->Ctor();
//	PTR->onReclaimObject();
//	ObjPool->Dtor(PTR);
//	}
//	t_end = time(NULL);
//	ACE_DEBUG(( LM_INFO, "\nACE_OBJECT_POOL uses time = %d\n", (int) difftime(t_end, t_start) ));
//
//	t_start = time(NULL);
//	for( ACE_UINT64 i = 0; i < 1000000; ++i )
//	{
//	Obj* obj = pool.createObject<Obj>();
//	obj->onReclaimObject();
//	pool.reclaimObject(obj);
//	}
//	t_end = time(NULL);
//	ACE_DEBUG(( LM_INFO, "KBE_OBJECT_POOL uses time = %d\n", (int) difftime(t_end, t_start) ));*/
//
//	/*
//	///test auto ptr wrapper of ace pool object
//	//commonway
//	//ACE_PoolPtr_Getter(ObjPool1, Obj, ACE_Null_Mutex);
//	//ACE_PoolFactoryPtr_Getter(objPoolFactory1, Obj, ACE_Null_Mutex);
//	//ACEPoolObject_Auto_Ptr<Obj, ACE_Null_Mutex> autoptr(ObjPool1->Ctor());
//	//autoptr.get()->onReclaimObject();
//	*/
//
//	////easy way : use micro 
//	ACE_PoolFactoryPtr_Getter(objPoolFactory2, Obj, ACE_Null_Mutex);
//	objPoolFactory2->dump();
//
//	ACE_PoolObject_AutoPtr_Getter(auptor1, Obj, ACE_Null_Mutex);
//	auptor1.get()->onReclaimObject();
//
//};
//
//#include "common/strulti.h"
//TEST(StrUlti, Strtoull)
//{
//	//#define kbe_strtoq   ACE_OS::strtoll
//	//#define kbe_strtouq  ACE_OS::strtoull
//	//#define keb_strtoll ACE_OS::strtoll
//	//#define kbe_strtoull ACE_OS::strtoull
//	//#define keb_atoll    ACE_OS::atol
//	char* numbers = "123 0xFF  789";
//	char* pend;
//	EXPECT_EQ(123, kbe_strtoull(numbers, &pend, 10));
//	EXPECT_EQ(0xFF, kbe_strtoull(pend, &pend, 16));
//	EXPECT_EQ(789, kbe_strtoull(pend, nullptr, 10));
//
//	numbers = "-123 -0xFF  -789";
//	pend = 0;
//	EXPECT_EQ(-123, kbe_strtoull(numbers, &pend, 10));
//	EXPECT_EQ(-0xFF, kbe_strtoull(pend, &pend, 16));
//	EXPECT_EQ(-789, kbe_strtoull(pend, nullptr, 10));
//
//	numbers = "456";
//	ACE_UINT64 num = kbe_atoll(numbers);
//}
//TEST(StrUltii, TestStrCaseTranslaton)
//{
//	char name[ ] = "jack";
//	kbe_strtoupper(name);
//	ACE_DEBUG(( LM_DEBUG, "   DEBUG: name jackie is now: %s\n", name ));
//	kbe_strtolower(name);
//	ACE_DEBUG(( LM_DEBUG, "   DEBUG: name jackie is now: %s\n", name ));
//}
//TEST(StrUltii, TestStrTrim)
//{
//	std::string str = "               this is trim    "; //Ö»ÐèÒª°ÑÇ°±ßºÍºó±ßµÄ¿Õ¸ñÈ¥µô¼´¿É
//	kbe_trim(str, " ");
//	ACE_DEBUG(( LM_DEBUG, "   DEBUG: after kbe_tri2, now:%s----\n", str.c_str() ));
//}
//TEST(StrUltii, TestKbeRepalce)
//{
//	std::string name = "jackie";
//	kbe_replace(name, "ie", "y");
//	ACE_DEBUG(( LM_DEBUG, "    DEBUG: name jackie is now: %s\n", name.c_str() ));
//
//	std::wstring lname = L"jackie";
//	kbe_replace(lname, L"ie", L"y");
//	ACE_DEBUG(( LM_DEBUG, "    DEBUG: name L jackie is now: %W\n", lname.c_str() ));
//}
//TEST(StrUltii, TestKbeSplit)
//{
//	cout << "kbe_split<ACE_TCHAR>(name, '/', vec)" << endl;
//	std::string name = "ja/ck/i/e";
//	std::vector< std::basic_string<ACE_TCHAR>> vec;
//	kbe_split<ACE_TCHAR>(name, '/', vec);
//	for each ( std::basic_string<ACE_TCHAR> var in vec )
//	{
//		cout << var << endl;
//	}
//	//void kbe_splits(const std::string& s, const std::string& delim, std::vector< std::string >&result,
//	// bool keep_empty
//	cout << "\nkbe_splits(name, '/', vec, false)" << endl;
//	std::vector< std::string > v;
//	kbe_splits(name, "/", v, false);
//	for each ( std::string var in v )
//	{
//		cout << var << endl;
//	}
//}
//TEST(StrUltii, Test_wchar2char_Translation)
//{
//	cout << "   DEBUG: kbe_char2wchar" << endl;
//	std::string name = "jackie";
//	ACE_OS::WChar* ln = kbe_char2wchar(name.c_str());
//	ACE_DEBUG(( LM_DEBUG, "    DEBUG: name jackie is now: L %W\n", ln ));
//
//	cout << "   DEBUG: kbe_wchar2char" << endl;
//	std::wstring lname = L"jackie";
//	char* n = kbe_wchar2char(lname.c_str());
//	ACE_DEBUG(( LM_DEBUG, "    DEBUG: name L jackie is now: %s\n", n ));
//}
//
//#include "common/common.h"
//TEST(Commonhpp, TestByteTrans)
//{
//	cout << "Test Common.h" << endl;
//	EXPECT_EQ(true, kbe_is_little_endian());
//	int dw = 1;
//	int b = ACE_SWAP_LONG(dw);
//	EXPECT_EQ(0, dw&b);
//	EXPECT_EQ(0, HIGH_WORD_DW(dw));
//
//	EXPECT_EQ(0, HIGH_BYTE_DW(dw));
//	EXPECT_EQ(1, LOW_BYTE_DW(dw));
//	EXPECT_EQ(1, BYTE0DW(dw));
//	EXPECT_EQ(0, BYTE1DW(dw));
//	EXPECT_EQ(0, BYTE2DW(dw));
//	EXPECT_EQ(0, BYTE3DW(dw));
//
//	SHORT w = 1;
//	EXPECT_EQ(1, LOW_BYTE_W(w));
//	EXPECT_EQ(0, HIGH_BYTE_W(w));
//}
//TEST(Commonhpp, Test_uid_pid_sleep_time)
//{
//	ACE_UINT64 t1 = kbe_get_sys_time();
//	cout << "system time 1 (ms) = " << t1 << endl;
//	cout << "uid = " << kbe_get_uid() << endl;
//	cout << "pid = " << kbe_get_pid() << endl;
//	cout << "sleep 2 sec..." << endl;
//	kbe_sleep(1000);
//	ACE_UINT64 t2 = kbe_get_sys_time();
//	cout << "system time 2 (ms) = " << t2 << endl;
//	cout << "system time diff (ms) = " << kbe_get_sys_time_diff(t1, t2) << endl;
//	cout << "get current error = " << kbe_lasterror() << endl;
//	cout << "set error EAGAIN" << endl;
//	kbe_lasterror(EAGAIN);
//	cout << "get current error = " << kbe_lasterror() << endl;
//	cout << "do kbe_strerror = " << endl;
//	kbe_strerror(EAGAIN);
//}
//TEST(Commonhpp, TestGnUUID64)
//{
//	ACE_Utils::UUID* uptr = kbe_gen_uuid64();
//	cout << uptr->to_string()->c_str() << endl;
//}
//
//#include "common/kbe_version.h"
//TEST(KBEVERSIONHPP, ALLTest)
//{
//	EXPECT_EQ("0.0.0", Version::get_script_version());
//	Version::set_script_version("0.2.0");
//	EXPECT_EQ("0.2.0", Version::get_script_version());
//}
//#include "common/base64.hpp"
//TEST(BASE64HPP, ALLTest)
//{
//	//cout << Version::get_kbengine_version() << endl; 
//	const ACE_Byte normal_stream[ ] = "This is a sample test stream, to test simple Base64 encoding";
//	const ACE_Byte one_padded_stream[ ] = "This stream is different from the above in that, it results in one padding character to be adde";
//	const ACE_Byte two_padded_stream[ ] = "This stream is different from the above in that, it results in two padding characters to be addedddd";
//
//	ACE_DEBUG(( LM_DEBUG, ACE_TEXT("Testing ACE Base64 - normal stream\n\n") ));
//	std::string encodeoutput;
//	kbe_base64_encode(normal_stream, sizeof(normal_stream) - 1, encodeoutput);
//	cout << "encodeoutput: " << encodeoutput << ", length: " << encodeoutput.length() << endl;
//	std::string decodeoutpit;
//	kbe_base64_decode(encodeoutput, decodeoutpit);
//	cout << "decodeoutpit: " << decodeoutpit << ", length: " << decodeoutpit.length() << endl;
//	for( size_t i = 0; i < sizeof(normal_stream); ++i )
//	{
//		EXPECT_EQ(normal_stream[i], decodeoutpit[i]);
//	}
//
//	ACE_DEBUG(( LM_DEBUG, ACE_TEXT("Testing ACE Base64 - one_padded_stream \n\n") ));
//	encodeoutput.clear();
//	kbe_base64_encode(one_padded_stream, sizeof(one_padded_stream) - 1, encodeoutput);
//	cout << "encodeoutput: " << encodeoutput << ", length: " << encodeoutput.length() << endl;
//	decodeoutpit.clear();
//	kbe_base64_decode(encodeoutput, decodeoutpit);
//	cout << "decodeoutpit: " << decodeoutpit << ", length: " << decodeoutpit.length() << endl;
//	for( size_t i = 0; i < sizeof(one_padded_stream); ++i )
//	{
//		EXPECT_EQ(one_padded_stream[i], decodeoutpit[i]);
//	}
//
//	ACE_DEBUG(( LM_DEBUG, ACE_TEXT("Testing ACE Base64 - two_padded_stream \n\n") ));
//	encodeoutput.clear();
//	kbe_base64_encode(two_padded_stream, sizeof(two_padded_stream) - 1, encodeoutput);
//	cout << "encodeoutput: " << encodeoutput << ", length: " << encodeoutput.length() << endl;
//	decodeoutpit.clear();
//	kbe_base64_decode(encodeoutput, decodeoutpit);
//	cout << "decodeoutpit: " << decodeoutpit << ", length: " << decodeoutpit.length() << endl;
//	for( size_t i = 0; i < sizeof(two_padded_stream); ++i )
//	{
//		EXPECT_EQ(two_padded_stream[i], decodeoutpit[i]);
//	}
//}
//#include "common/deadline.hpp"
//TEST(DeadlineHpp, AllTests)
//{
//	time_t sec = 1 * 24 * 60 * 60;
//	cout << "sec = " << sec << endl;
//	Deadline date(sec);
//	//cout << date.print() << endl;
//
//	time_t sec1 = ACE_OS::gettimeofday().sec();
//	cout << "sec1 = " << sec1 << endl;
//	Deadline date1(sec1);
//	//cout << date1.print() << endl;
//}
//
//#include "common\kbe_blowfish.hpp"
//TEST(KBEBlowfishhpp, AllMethodsTest)
//{
//	unsigned char* res = new unsigned char[12];
//	KBEBlowfish fish;
//	ACE_UINT64 num = 123456789;
//	unsigned char*  src = (unsigned char*) &num;
//	cout << "before encrypt: text = " << num << endl;
//	ACE_HEX_DUMP(( LM_DEBUG, (char*) &num, sizeof(num), "befrore enpt srcBuf: " ));
//	fish.encrypt(src, res, sizeof(num));
//	ACE_HEX_DUMP(( LM_DEBUG, (char*) res, 12, "after enpt: srcBuf: " ));
//	unsigned char* res1 = new unsigned char[12];
//	fish.decrypt(res, res1, 8);
//	ACE_HEX_DUMP(( LM_DEBUG, (char*) res1, 8, "after denpt: srcBuf: " ));
//	cout << "before encrypt: text = " << *(ACE_UINT64*) res1 << endl;
//}
//
//#include "common\rsa.hpp"
//TEST(KBE_RSA_TEST, AllMethodsTest)
//{
//	KBE_RSA RSA;
//	RSA.generateKey("kbengine_public.key", "kbengine_private.key");
//	string name = "Jackie";
//	string res;
//	RSA.encrypt(name, res);
//	RSA.hexCertifData(res);
//}
//
//#include "common\timestamp.hpp"
//TEST(timestamp_test, AllMethodsTest)
//{
//	ACE_UINT64  timestam = timestamp();
//	cout << "timestamp : " << timestamp << endl;
//	TimeStamp stamp(timestam);
//	cout << "ageInStamps : " << stamp.ageInStamps() << endl;
//	cout << "ageInSecs : " << stamp.ageInSeconds() << endl;
//}
//
////#include "common\md5.hpp"
////TEST(md5_test, AllMethodsTest)
////{
////	std::string text = "hello Jackie";
////	void* t = static_cast<void*>( const_cast<char*>( text.c_str() ) );
////	KBE_MD5 md5(t, text.size());
////	cout << "hello Jackie md5  str is " << md5.getDigestStr() << endl;
////	ACE_HEX_DUMP(( LM_DEBUG, const_cast<char*>( (char*) md5.getDigest() ), 33, "dum result is :" ));
////}
//
//#include "ace/Get_Opt.h"
//#include "ace/Auto_Ptr.h"
//#include "ace/CDR_Stream.h"
//#include "ace/CDR_Size.h"
//#include "ace/SString.h"
//#include "ace/ACE.h"
//#include "ace/OS_NS_stdlib.h"
//#include "ace/OS_NS_string.h"
//#include "ace/OS_NS_wchar.h"
//struct test_types
//{
//	ACE_CDR::Octet o;
//	ACE_CDR::Short s;
//	ACE_CDR::Long l;
//	const ACE_CDR::Char *str;
//	const ACE_CDR::WChar *wstr;
//	ACE_CDR::Double d;
//	ACE_CDR::Short reps;
//	ACE_CDR::UShort repus;
//	ACE_CDR::Long repl;
//	ACE_CDR::ULong repul;
//	ACE_CDR::Boolean repb;
//	const ACE_CDR::Char repc;
//	ACE_CDR::LongLong repll;
//	ACE_CDR::ULongLong repull;
//	ACE_CDR::Octet repo;
//	ACE_CDR::Float repf;
//	ACE_CDR::Double repd;
//
//	test_types(void) :str("abc"), wstr(0), repc('d')
//	{
//		o = 1;
//		s = 2;
//		l = 4;
//		d = 8;
//		reps = -123;
//		repus = 456;
//		repl = -65800L;
//		repul = 65800L;
//		repb = false;
//		repll = -5000000ll;
//		repll = 50000000ll;
//		repo = 0x05;
//		repf = 3.14f;
//		repd = 6.00;
//
//		for( int i = 0; i < 10; ++i )
//		{
//			a[i] = i;
//		}
//	}
//
//	static int short_stream(void)
//	{
//		//couter
//		u_int i;
//
//		//build out stream
//		ACE_OutputCDR os;
//		ACE_SizeCDR size;
//
//		//basic types for output
//		ACE_CDR::Char ch = 'A';
//		ACE_CDR::Char wchtmp[ ] = { '\xF3' };
//		ACE_CDR::WChar wch = *wchtmp;
//		ACE_CDR::WChar wch2[ ] = { '\x00' }; //enpty wide string 
//		ACE_CDR::WChar *wstrptr = wch2;
//		ACE_CString str("Test String");
//		//std::string stdstr("test string std");
//		ACE_CDR::Short s = -123;
//		ACE_CDR::UShort us = 123;
//		ACE_CDR::Long l = -65800l;
//		ACE_CDR::ULong ul = 65800ul;
//		ACE_CDR::Float f = 1.23f;
//		ACE_CDR::Double d = 123.456789;
//
//		//arrays for output
//		ACE_CDR::Short sarr[3] = { 1, 2, 3 };
//		ACE_CDR::Long larr[3] = { -12345678, 0, 12345678 };
//		ACE_CDR::Float farr[3] = { -1.23f, 0.0f, 1.23f };
//		ACE_CDR::Double darr[3] = { -123.456789, 0.0, 123.456789 };
//		ACE_CDR::Boolean barr[3] = { false, true, false };
//
//		ACE_OutputCDR::from_char fc(ch);
//		ACE_OutputCDR::from_wchar fwc(wch);
//
//		os << fc;
//		os << wchtmp;
//		os << fwc;
//		os << str;
//		os << wstrptr;
//		os << s;
//		os << us;
//		os << l;
//		os << ul;
//		os << f;
//		os << d;
//
//		os.write_short_array(sarr, 3);
//		os.write_long_array(larr, 3);
//		os.write_float_array(farr, 3);
//		os.write_double_array(darr, 3);
//		os.write_boolean_array(barr, 3);
//
//
//		//do the same for cdr_size
//		size << fc;
//		size << fwc;
//		size << str;
//		size << wstrptr;
//		size << s;
//		size << us;
//		size << l;
//		size << ul;
//		size << f;
//		size << d;
//
//		size.write_short_array(sarr, 3);
//		size.write_long_array(larr, 3);
//		size.write_float_array(farr, 3);
//		size.write_double_array(darr, 3);
//		size.write_boolean_array(barr, 3);
//
//
//		// Check the size.
//		if( size.total_length() != os.total_length() )
//			ACE_ERROR_RETURN((
//			LM_ERROR,
//			"%p\n"
//			"representation length does not match\n"
//			"size.total_length(): %d\n"
//			"os.total_length(): %d\n",
//			size.total_length(),
//			os.total_length() ),
//			1);
//
//		//get the msg block
//		const ACE_Message_Block* out_mb = os.begin();
//		/** @name Message length and size operations
//		* Message length is (wr_ptr - rd_ptr)
//		* Message size is capacity of the message, including data outside
//		* the [rd_ptr,wr_ptr] range.*/
//		size_t len = out_mb->length();
//
//		//crreate input stream
//		ACE_InputCDR is(os);
//		const ACE_Message_Block* in_mb = is.start();
//
//		if( in_mb->length() != len )
//			ACE_ERROR_RETURN((
//			LM_ERROR,
//			ACE_TEXT("%p\n"),
//			ACE_TEXT("buffer length not preserved") ),
//			1);
//
//		for( i = 0; i < len; i++ )
//		{
//			unsigned long const in_chunk =
//				static_cast<unsigned long> ( *( in_mb->rd_ptr() + i ) );
//
//			unsigned long const out_chunk =
//				static_cast<unsigned long> ( *( out_mb->rd_ptr() + i ) );
//
//			if( in_chunk != out_chunk )
//				ACE_ERROR_RETURN(( LM_ERROR,
//				ACE_TEXT("%p\n"),
//				ACE_TEXT("buffer contents not preserved") ),
//				1);
//		}
//
//		//basic types for input
//		ACE_CDR::Char ch1 = '\0';
//		ACE_CDR::WChar wch1 = '\x00';
//		ACE_CDR::WChar *wstr1 = 0;
//		ACE_CString str1;
//		ACE_CDR::Short s1 = 0;
//		ACE_CDR::UShort us1 = 0;
//		ACE_CDR::Long l1 = 0L;
//		ACE_CDR::ULong ul1 = 0UL;
//		ACE_CDR::Float f1 = 0.0f;
//		ACE_CDR::Double d1 = 0.0;
//
//		// Arrays for input
//		ACE_CDR::Short s_array1[3];
//		ACE_CDR::Long l_array1[3];
//		ACE_CDR::Float f_array1[3];
//		ACE_CDR::Double d_array1[3];
//
//		ACE_DEBUG(( LM_DEBUG,
//			ACE_TEXT("Checking operators and arrays\n\n") ));
//
//
//		ACE_InputCDR::to_char tc(ch1);
//		is >> tc;
//
//		ACE_InputCDR::to_wchar twc(wch1);
//		is >> twc;
//
//		//char* ptr = const_cast<char*>( str1.c_str() );
//		//ACE_InputCDR::to_string tstr(ptr, str1.length());
//		is >> str1;
//
//		ACE_InputCDR::to_wstring twstr(wstr1, 0);
//		is >> twstr;
//
//		ACE_WString wcstr(wstr1);
//
//		is >> s1;
//		is >> us1;
//		is >> l1;
//		is >> ul1;
//		is >> f1;
//		is >> d1;
//
//		is.read_short_array(s_array1, 3);
//		is.read_long_array(l_array1, 3);
//		is.read_float_array(f_array1, 3);
//		is.read_double_array(d_array1, 3);
//
//		EXPECT_EQ(ch1, ch);
//		EXPECT_EQ(wch1, wch);
//
//		if( ACE_OS::wscmp(wstr1, wstrptr) )
//			ACE_ERROR_RETURN(( LM_ERROR,
//			ACE_TEXT("%p\n"),
//			ACE_TEXT("wide string transfer error") ),
//			1);
//
//		EXPECT_EQ(s, s1);
//		EXPECT_EQ(us, us1);
//		EXPECT_EQ(l1, l);
//		EXPECT_EQ(ul1, ul);
//		EXPECT_FLOAT_EQ(f1, f);
//		EXPECT_DOUBLE_EQ(d1, d);
//
//		for( i = 0; i < 3; i++ )
//			if( s_array1[i] != sarr[i] )
//				ACE_ERROR_RETURN(( LM_ERROR,
//				ACE_TEXT("%p\n"),
//				ACE_TEXT("short array transfer error") ),
//				1);
//
//		for( i = 0; i < 3; i++ )
//			if( l_array1[i] != larr[i] )
//				ACE_ERROR_RETURN(( LM_ERROR,
//				ACE_TEXT("%p\n"),
//				ACE_TEXT("long array transfer error") ),
//				1);
//
//		for( i = 0; i < 3; i++ )
//			if( !ACE::is_equal(f_array1[i], farr[i]) )
//				ACE_ERROR_RETURN(( LM_ERROR,
//				ACE_TEXT("%p\n"),
//				ACE_TEXT("float array transfer error") ),
//				1);
//
//		for( i = 0; i < 3; i++ )
//			if( !ACE::is_equal(d_array1[i], darr[i]) )
//				ACE_ERROR_RETURN(( LM_ERROR,
//				ACE_TEXT("%p\n"),
//				ACE_TEXT("double array transfer error") ),
//				1);
//
//		return 0;
//
//	}
//
//	int test_put(ACE_OutputCDR& cdr)
//	{
//
//	}
//	int test_get(ACE_InputCDR& cdr) const;
//	int test_put_placeholder(ACE_OutputCDR& cdr);
//	int test_get_placeholder(ACE_InputCDR& cdr) const;
//
//	ACE_CDR::Short a[10];
//};
//TEST(ACE_CDR_TEST, ALL_TESTS)
//{
//
//	//ACE_HEX_DUMP(( LM_DEBUG, in.start()->data_block()->base(), sizeof(us) + sizeof(ul), "out Result: " ));
//
//	//ACE_DEBUG(( LM_DEBUG,
//	//	ACE_TEXT("Testing ACE CDR functions - short stream\n\n") ));
//
//	//if( !test_types::short_stream() )
//	//	ACE_DEBUG(( LM_DEBUG,
//	//	ACE_TEXT("Testing ALL PASSED ! \n") ));
//}
//
//#include "common\memorystream_converter.hpp"
//TEST(memorystream_converter_test, ALL_TESTS)
//{
//	ACE_UINT32 num = 1;
//
//	ACE_HEX_DUMP(( LM_DEBUG,
//		(char*) &num, sizeof(num),
//		"brfore coverting, out Result: " ));
//
//	num = KBE_SWAP_DW(num);
//	ACE_HEX_DUMP(( LM_DEBUG,
//		(char*) &num, sizeof(num),
//		"after converting, out Result: " ));
//
//	EndianConvertReverse(num);
//	ACE_HEX_DUMP(( LM_DEBUG,
//		(char*) &num, sizeof(num),
//		"after converting, out Result: " ));
//
//}
//
//#include "net\Address.h"
//TEST(Address_test, all_tests)
//{
//	cout << "test null address\n";
//	NETWORK::Address none;
//	EXPECT_EQ(true, none.isNone());
//
//	cout << "\nTest an null address methods\n";
//	NETWORK::Address addr(20000, "127.0.0.1");
//	cout << "address: " << addr.c_str() << endl;
//
//	cout << "\nTest operators\n";
//	EXPECT_EQ(false, addr == none);
//	EXPECT_EQ(true, addr != none);
//	EXPECT_EQ(true, addr > none);
//	EXPECT_EQ(false, addr < none);
//}
//
//#include "net\Nub.h"
//TEST(NubTest, all_tests)
//{
//	NETWORK::Nub nub;
//	//nub.startLoop();
//}
//
//#include  "net\Packet.h"
//TEST(PacketTest, all_tests)
//{
//	Packet* p;
//	ACE_PoolPtr_Getter(pool, Packet, ACE_Null_Mutex);
//	MessageID id = 0;
//	cout << "create tcp packet...\n";
//	ProtocolType pt = PROTOCOL_TCP;
//	p = pool->Ctor<MessageID, ProtocolType>(id, pt);
//	cout << "p->length(): " << p->length()
//		<< "\np->sentSize" << p->sentSize
//		<< "\np->empty\n" << p->empty()
//		<< "\np->encrypted_\n" << p->encrypted_;
//	pool->Dtor(p);
//
//	cout << "create udp packet...\n";
//	pt = PROTOCOL_UDP;
//	p = pool->Ctor<MessageID, ProtocolType>(id, pt);
//	cout << "p->length(): " << p->length()
//		<< "\np->sentSize" << p->sentSize
//		<< "\np->empty\n" << p->empty()
//		<< "\np->encrypted_\n" << p->encrypted_;
//	pool->Dtor(p);
//}

//#include  "net\Bundle.h"
//TEST(BundleTest, write_fixed_msg)
//{
//	struct msgarg : public MessageArgs
//	{
//		virtual ACE_INT32 args_bytes_count(void)
//		{
//			return 77;
//		}
//		virtual void fetch_args_from(Packet* p)
//		{
//
//		}
//		virtual void add_args_to(Packet* p)
//		{
//
//		}
//	};
//
//	Bundle* p;
//	ACE_PoolPtr_Getter(pool, Bundle, ACE_Null_Mutex);
//	p = pool->Ctor();
//
//	FixedMessages::MSGInfo info = {1};
//	ACE_Singleton<FixedMessages, ACE_Null_Mutex>::instance()->infomap_.insert(pair<std::string, FixedMessages::MSGInfo>("currhandler", info));
//
//	ACE_PoolPtr_Getter(poolmsg, Message, ACE_Null_Mutex);
//	Message* currhandler = poolmsg->Ctor();
//	currhandler->msgArgsBytesCount_ = 77;
//	currhandler->msgID_ = 1;
//
//	ACE_PoolPtr_Getter(poolmsgarg, msgarg, ACE_Null_Mutex);
//	msgarg* ag = poolmsgarg->Ctor();
//
//	Messages msgs;
//	msgs.add_msg("currhandler", ag, NETWORK_FIXED_MESSAGE, currhandler);
//
//	p->start_new_curr_message(currhandler);
//
//	*p << (KBE_SRV_COMPONENT_TYPE) 5;
//	*p << (ENTITY_MAILBOX_TYPE) 5;
//
//	*p << (UCHAR) 1;
//	*p << (UINT16) 2;
//	*p << (UINT32) 3;
//	*p << (UINT64) 4;
//
//	*p << (CHAR) -5;
//	*p << (INT16) -6;
//	*p << (INT32) -7;
//	*p << (INT64) 8;
//
//	char *blob = "blob";
//	p->write_blob(blob, strlen(blob) + 1);
//
//	char *name0 = "name0";
//	*p << name0;
//
//	char *name1 = "name1";
//	*p << name1;
//
//	std::string n2 = "name2";
//	*p << n2;
//
//
//	char *n3 = "name3";
//	*p << n3;
//
//	std::string n4 = "name4";
//	*p << n4;
//
//	std::string n5 = "name5";
//	*p << n5;
//
//	p->end_new_curr_message();
//
//	p->dumpMsgs();
//
//	#pragma pack (push, 1)
//	struct Arg
//	{
//		KBE_SRV_COMPONENT_TYPE ctype;
//		ENTITY_MAILBOX_TYPE mailbox;
//
//		UCHAR uchar;
//		UINT16 us;
//		UINT32 u32;
//		UINT64 u64;
//
//		CHAR ch;
//		INT16 s;
//		INT32 int32;
//		INT64 int64;
//
//		UINT32 blobsize;
//		char* blob;
//
//		UINT32 n0_size;
//		char* n0;
//
//		UINT32 n1_size;
//		char* n1;
//
//		std::string n2;
//
//		UINT32 n1_size;
//		char* n3;
//
//		std::string n4;
//
//		std::string n5;
//	};
//	#pragma pack (pop)
//	Arg arg;
//	MessageID id = 0;
//
//	p->init_instream();
//
//	*p >> id;
//	cout << "id = " << id << endl;
//
//	*p >> arg.ctype;
//	cout << "arg.ctype = " << arg.ctype << endl;
//
//	*p >> arg.mailbox;
//	cout << "arg.mailbox = " << arg.mailbox << endl;
//
//	*p >> arg.uchar;
//	cout << "arg.uchar = " << (int) arg.uchar << endl;
//
//	*p >> arg.us;
//	cout << "arg.us = " << arg.us << endl;
//
//	*p >> arg.u32;
//	cout << "arg.u32 = " << arg.u32 << endl;
//
//	*p >> arg.u64;
//	cout << "arg.u64 = " << arg.u64 << endl;
//
//	*p >> arg.ch;
//	cout << "arg.ch = " << (int) arg.ch << endl;
//
//	*p >> arg.s;
//	cout << "arg.s = " << arg.s << endl;
//
//	*p >> arg.int32;
//	cout << "arg.int32 = " << arg.int32 << endl;
//
//	*p >> arg.int64;
//	cout << "arg.int64 = " << arg.int64 << endl;
//
//	*p >> arg.blobsize;
//	cout << "arg.blobsize = " << arg.blobsize << endl;
//
//	arg.blob = new char[arg.blobsize];
//	p->read_blob(arg.blob, arg.blobsize);
//	ACE_HEX_DUMP(( LM_DEBUG, arg.blob, arg.blobsize,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	delete arg.blob;
//
//	arg.n0 = new char[256];
//	*p >> arg.n0;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n0, strlen(arg.n0) + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.no  = " << arg.n0 << endl;
//	delete arg.n0;
//
//	arg.n1 = new char[256];
//	*p >> arg.n1;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n1, strlen(arg.n1) + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n1 = " << arg.n1 << endl;
//	delete arg.n1;
//
//
//	*p >> arg.n2;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n2.c_str(), arg.n2.size() + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n2  = " << arg.n2 << endl;
//
//	arg.n3 = new char[256];
//	*p >> arg.n3;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n3, strlen(arg.n3) + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n3 = " << arg.n3 << endl;
//	delete arg.n3;
//
//	*p >> arg.n4;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n4.c_str(), arg.n4.size() + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n4  = " << arg.n4 << endl;
//
//	*p >> arg.n5;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n5.c_str(), arg.n5.size() + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n5  = " << arg.n5 << endl;
//
//	p->clear();
//	pool->Dtor(p);
//}
//
//TEST(BundleTest, write_variable_msg)
//{
//	struct msgarg : public MessageArgs
//	{
//		virtual ACE_INT32 args_bytes_count(void)
//		{
//			return 20;
//		}
//		virtual void fetch_args_from(Packet* p)
//		{
//
//		}
//		virtual void add_args_to(Packet* p)
//		{
//
//		}
//	};
//	g_channelExternalEncryptType = 1;
//	g_channelExternalEncryptType = 0;
//
//	Bundle* p;
//	ACE_PoolPtr_Getter(pool, Bundle, ACE_Null_Mutex);
//	p = pool->Ctor();
//
//	cout << "curr packet max size = " << p->currPacketMaxSize << endl;
//
//	ACE_PoolPtr_Getter(poolmsg, Message, ACE_Null_Mutex);
//	Message* currhandler = poolmsg->Ctor();
//	currhandler->msgArgsBytesCount_ = 10;
//	currhandler->msgID_ = 1;
//
//	ACE_PoolPtr_Getter(poolmsgarg, msgarg, ACE_Null_Mutex);
//	msgarg* ag = poolmsgarg->Ctor();
//
//	Messages msgs;
//	msgs.add_msg("currhandler", ag, NETWORK_VARIABLE_MESSAGE, currhandler);
//
//	p->start_new_curr_message(currhandler);
//
//	*p << (KBE_SRV_COMPONENT_TYPE) 5;
//	*p << (ENTITY_MAILBOX_TYPE) 5;
//
//	*p << (UINT64) 64;
//
//	*p << (CHAR) -5;
//	*p << (INT16) -6;
//	*p << (INT32) -7;
//	*p << (INT64) 8;
//
//	char *blob = "blob";
//	p->write_blob(blob, strlen(blob) + 1);
//
//	char *name0 = "name0";
//	*p << name0;
//
//	char *name1 = "name1";
//	*p << name1;
//
//	std::string n2 = "name2";
//	*p << n2;
//
//
//	char *n3 = "name3";
//	*p << n3;
//
//	std::string n4 = "name4";
//	*p << n4;
//
//	std::string n5 = "name5";
//	*p << n5;
//
//	p->end_new_curr_message();
//
//	p->dumpMsgs();
//
//	#pragma pack (push, 1)
//	struct Arg
//	{
//		KBE_SRV_COMPONENT_TYPE ctype;
//		ENTITY_MAILBOX_TYPE mailbox;
//
//		UINT64 u64;
//
//		CHAR ch;
//		INT16 s;
//		INT32 int32;
//		INT64 int64;
//
//		UINT32 blobsize;
//		char* blob;
//
//		UINT32 n0_size;
//		char* n0;
//
//		UINT32 n1_size;
//		char* n1;
//
//		std::string n2;
//
//		UINT32 n1_size;
//		char* n3;
//
//		std::string n4;
//
//		std::string n5;
//	};
//	#pragma pack (pop)
//
//	Arg arg;
//	MessageID id = 0;
//	MessageLength len;
//
//	p->init_instream();
//
//	*p >> id;
//	cout << "id = " << id << endl;
//
//	*p >> len;
//	cout << "len = " << len << endl;
//
//	if( len == NETWORK_MESSAGE_MAX_SIZE )
//	{
//		MessageLength1 ex_len;
//		*p >> ex_len;
//		cout << "ex_len = " << ex_len << endl;
//	}
//
//	*p >> arg.ctype;
//	cout << "arg.ctype = " << arg.ctype << endl;
//
//	*p >> arg.mailbox;
//	cout << "arg.mailbox = " << arg.mailbox << endl;
//
//	*p >> arg.u64;
//	cout << "arg.u64 = " << arg.u64 << endl;
//
//	*p >> arg.ch;
//	cout << "arg.ch = " << (int) arg.ch << endl;
//
//	*p >> arg.s;
//	cout << "arg.s = " << arg.s << endl;
//
//	*p >> arg.int32;
//	cout << "arg.int32 = " << arg.int32 << endl;
//
//	*p >> arg.int64;
//	cout << "arg.int64 = " << arg.int64 << endl;
//
//	*p >> arg.blobsize;
//	cout << "arg.blobsize = " << arg.blobsize << endl;
//
//	arg.blob = new char[arg.blobsize];
//	p->read_blob(arg.blob, arg.blobsize);
//	ACE_HEX_DUMP(( LM_DEBUG, arg.blob, arg.blobsize,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	delete arg.blob;
//
//	arg.n0 = new char[256];
//	*p >> arg.n0;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n0, strlen(arg.n0) + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.no  = " << arg.n0 << endl;
//	delete arg.n0;
//
//	arg.n1 = new char[256];
//	*p >> arg.n1;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n1, strlen(arg.n1) + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n1 = " << arg.n1 << endl;
//	delete arg.n1;
//
//
//	*p >> arg.n2;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n2.c_str(), arg.n2.size() + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n2  = " << arg.n2 << endl;
//
//	arg.n3 = new char[256];
//	*p >> arg.n3;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n3, strlen(arg.n3) + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n3 = " << arg.n3 << endl;
//	delete arg.n3;
//
//	*p >> arg.n4;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n4.c_str(), arg.n4.size() + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n4  = " << arg.n4 << endl;
//
//	*p >> arg.n5;
//	ACE_HEX_DUMP(( LM_DEBUG, arg.n5.c_str(), arg.n5.size() + 1,
//		"PACKET_OUT_VALUE::Result: \n" ));
//	cout << "arg.n5  = " << arg.n5 << endl;
//
//	p->clear();
//	pool->Dtor(p);
//}

//#include "net\NetworkInterface.h"
//#include "net\Channel.h"
//#include "net\DelayedChannelHandler.h"
//TEST(NetworkInterfaceTest, get_ip_addr_str)
//{
//	Nub              pDispatcher;
//	ACE_INT32     extlisteningPort_min = 20001;
//	ACE_INT32     extlisteningPort_max = 20005;
//	const char *    extlisteningInterface = "192.168.2.47";
//	//const char *    extlisteningInterface = "";
//	//const char *    extlisteningInterface = "127.0.0.1";
//	//const char *    extlisteningInterface = USE_KBEMACHINED;
//	ACE_UINT32   extrbuffer = 512;
//	ACE_UINT32   extwbuffer = 512;
//	ACE_INT32      intlisteningPort = 20006;
//	const char *    intlisteningInterface = "192.168.2.47";
//	ACE_UINT32   intrbuffer = 512;
//	ACE_UINT32   intwbuffer = 512;
//
//	NetworkInterface in(&pDispatcher,
//		extlisteningPort_min,
//		extlisteningPort_max,
//		extlisteningInterface,
//		extrbuffer,
//		extwbuffer,
//		intlisteningPort,
//		intlisteningInterface,
//		intrbuffer,
//		intwbuffer);
//
//	//ACE_Time_Value tv;
//	//in.handle_timeout(tv, 0);
//	ACE_INET_Addr addr(20006, "192.168.2.47");
//	TCP_SOCK_Handler dg(&in);
//	dg.reactor(pDispatcher.rec);
//
//	Channel tcpchannel(&in, &dg);
//	dg.pChannel_ = &tcpchannel;
//	std::cout << "tcpchannel.c_str();\n" << tcpchannel.c_str();
//	std::cout << "tcpchannel.get_bundles_length();\n" << tcpchannel.get_bundles_length();
//	Messages msgs;
//#include "net\net_common.h"
//	Bundle* bundle = Bundle_Pool->Ctor();
//	tcpchannel.send(bundle);
//	in.register_channel(&tcpchannel);
//
//	in.channel(addr);
//	in.channel(dg.get_handle());
//	in.process_all_channels_packets(&msgs);
//	in.deregister_channel(&tcpchannel);
//	in.deregister_all_channels();
//	in.close_listenning_sockets();
//
//	DelayedChannelHandlers delay;
//	delay.init(&pDispatcher, &in);
//	delay.add(&tcpchannel);
//	delay.send_delayed_channel(&tcpchannel);
//	delay.process();
//	delay.fini(&pDispatcher);
//
//	//Bundle_Pool->Dtor(bundle);
//
//	//pDispatcher.startLoop();
//}

//#include "net\Nub.h"
//TEST(NubTest, start_loop)
//{
//	Nub nub;
//	nub.startLoop();
//}

//#include "net\PacketReader.h"
//#include "net\NetworkInterface.h"
//TEST(PacketReaderTests, ctor_dtor_test)
//{
//	struct msgarg : public MessageArgs
//	{
//		virtual MessageLength1 args_bytes_count(void)
//		{
//			return 12;
//		}
//		virtual void fetch_args_from(Packet* p)
//		{
//			INT32  para1 = *(INT32*) p->buff->rd_ptr();
//			p->buff->rd_ptr(4);
//			INT32  para2 = *(INT32*) p->buff->rd_ptr();
//			p->buff->rd_ptr(4);
//			INT32  para3 = *(INT32*) p->buff->rd_ptr();
//			ACE_DEBUG(( LM_DEBUG, "(%d)(%d)(%d)\n", para1, para2, para3 ));
//		}
//
//		virtual void add_args_to(Packet* p)
//		{
//		}
//	};
//
//	struct msgarg_variable : public MessageArgs
//	{
//		virtual MessageLength1 args_bytes_count(void)
//		{
//			return 0;
//		}
//		virtual void fetch_args_from(Packet* p)
//		{
//			for( int i = 0; i < 4; i++ )
//			{
//				INT64  para1 = *(INT64*) p->buff->rd_ptr();
//				p->buff->rd_ptr(8);
//				ACE_DEBUG(( LM_DEBUG, "(%d)", para1 ));
//			}
//
//			ACE_DEBUG(( LM_DEBUG, "\n" ));
//		}
//
//		virtual void add_args_to(Packet* p)
//		{
//		}
//	};
//
//	g_channelExternalEncryptType = 0;
//
//	Nub              pDispatcher;
//	ACE_INT32     extlisteningPort_min = 20001;
//	ACE_INT32     extlisteningPort_max = 20005;
//	const char *    extlisteningInterface = "192.168.2.47";
//	//const char *    extlisteningInterface = "";
//	//const char *    extlisteningInterface = "127.0.0.1";
//	//const char *    extlisteningInterface = USE_KBEMACHINED;
//	ACE_UINT32   extrbuffer = 512;
//	ACE_UINT32   extwbuffer = 512;
//	ACE_INT32      intlisteningPort = 20006;
//	const char *    intlisteningInterface = "192.168.2.47";
//	ACE_UINT32   intrbuffer = 512;
//	ACE_UINT32   intwbuffer = 512;
//
//	NetworkInterface in(&pDispatcher,
//		extlisteningPort_min,
//		extlisteningPort_max,
//		extlisteningInterface,
//		extrbuffer,
//		extwbuffer,
//		intlisteningPort,
//		intlisteningInterface,
//		intrbuffer,
//		intwbuffer);
//
//	ACE_INET_Addr addr(20006, "192.168.2.47");
//	TCP_SOCK_Handler dg(&in);
//	dg.reactor(pDispatcher.rec);
//
//	Channel channel(&in, &dg);
//	dg.pChannel_ = &channel;
//
//	in.register_channel(&channel);
//
//	ACE_PoolPtr_Getter(pool, Bundle, ACE_Null_Mutex);
//	ACE_PoolPtr_Getter(poolmsgarg, msgarg, ACE_Null_Mutex);
//	ACE_PoolPtr_Getter(poolmsgarg_variable, msgarg_variable, ACE_Null_Mutex);
//	ACE_PoolPtr_Getter(poolmsg, Message, ACE_Null_Mutex);
//	ACE_PoolPtr_Getter(poolpacket, Packet, ACE_Null_Mutex);
//
//	Bundle* p = pool->Ctor();
//	Messages msgs;
//	msgarg* ag = poolmsgarg->Ctor();
//	msgarg_variable* ag_va = poolmsgarg_variable->Ctor();
//
//	/// first msg is fixed msg
//	Message* currhandler1 = poolmsg->Ctor();
//	//FixedMessages::MSGInfo info1 = { 1 };
//	//ACE_Singleton<FixedMessages, ACE_Null_Mutex>::instance()->infomap_.insert(pair<std::string, FixedMessages::MSGInfo>("currhandler1", info1));
//	msgs.add_msg("currhandler1", ag, NETWORK_FIXED_MESSAGE, currhandler1);
//	p->start_new_curr_message(currhandler1);
//	*p << (INT32) -7;
//	*p << (INT32) -7;
//	*p << (INT32) -7;
//	p->end_new_curr_message();
//	//p->dumpMsgs();
//
//	/// second msg is variable msg
//	Message* currhandler2 = poolmsg->Ctor();
//	msgs.add_msg("currhandler2", ag_va, NETWORK_VARIABLE_MESSAGE, currhandler2);
//	p->start_new_curr_message(currhandler2);
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	p->end_new_curr_message();
//	//p->dumpMsgs();
//
//	///// second msg is variable msg
//	Message* currhandler3 = poolmsg->Ctor();
//	msgs.add_msg("currhandler3", ag, NETWORK_FIXED_MESSAGE, currhandler3);
//	p->start_new_curr_message(currhandler3);
//	*p << (INT32) -7;
//	*p << (INT32) -7;
//	*p << (INT32) -7;
//	p->end_new_curr_message();
//	//p->dumpMsgs();
//
//	///// second msg is variable msg
//	Message* currhandler4 = poolmsg->Ctor();
//	msgs.add_msg("currhandler4", ag_va, NETWORK_VARIABLE_MESSAGE, currhandler4);
//	p->start_new_curr_message(currhandler4);
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	p->end_new_curr_message();
//
//	Bundle::Packets::iterator iter = p->packets_.begin();
//	for( ; iter != p->packets_.end(); iter++ )
//	{
//		channel.recvPackets_.push_back(( *iter ));
//	}
//	channel.process_packets(&msgs);
//
//	//PacketReader pr(&channel);
//	//pr.processMessages(&msgs, p->packets_);
//
//	pool->Dtor(p);
//
//}
//#include "common\Profile.h"
//static Profile _localProfile;
//void hello()
//{
//	SCOPED_PROFILE(_localProfile);
//	Sleep(100);
//}
//TEST(pROFILETESTS, aLLTESTS)
//{
//	hello();
//	ACE_DEBUG(( LM_DEBUG,
//		"%s::lastIntTime(%f s), lastTime(%f s), sumTime(%f s),"
//		"sumIntTime(%f s),runningTime(%f s) \n",
//		_localProfile.name(),
//		_localProfile.lastIntTimeInSeconds(),
//		_localProfile.lastTimeInSeconds(),
//		_localProfile.sumTimeInSeconds(),
//		_localProfile.sumIntTimeInSeconds(),
//		(double) runningTime() / stampsPerSecondD() ));
//}
//#include "net\ErrorStatsMgr.h"
//#include "net\Nub.h"
//TEST(ErrorStatsMgr, tESTTS)
//{
//	ErrorSat stat;
//	stat.lastReportStamps = timestamp();
//	stat.count = 10;
//	Nub nub;
//	ErrorStatMgr mgr(&nub);
//
//	std::string error = "The send buf is full";
//	ACE_INET_Addr addr(20001, ACE_LOCALHOST);
//	/// if exceeeds the max report period it will be reported otherwise coutn_ will be updated
//	mgr.reportError(addr, "%s, %d", error.c_str(), 12);
//	/// report all errors true report all erros false report id it exceeds max report period
//	mgr.reportPendingExceptions(true);
//	nub.startLoop();
//}

#include "common\Watcher.h"
int hello()
{
	return 12;
}
struct tt
{
	int a;
	int hello()
	{
		return 12;
	}
};
TEST(WatcherTest, watchertests)
{
	int a = 12;
	tt t;
	t.a = 1;

	std::string path1 = "watcher1";
	CRATE_WATCH_OBJECT(path1, a);
	std::cout << ( ( ValueWatcher<int>* )GET_WATCHER_OBJECT(path1).get() )->getValue()
		<< std::endl;

	std::string path2 = "p0/p2/watcher2";
	CRATE_WATCH_OBJECT(path2, t.a);
	std::cout << ( ( ValueWatcher<int>* )GET_WATCHER_OBJECT(path2).get() )->getValue()
		<< std::endl;

	std::string path3 = "path1/path2/p3/watcher3";
	CRATE_WATCH_OBJECT(path3, &hello);
	std::cout << ( ( FunctionWatcher<int>* )GET_WATCHER_OBJECT(path3).get() )->getValue()
		<< std::endl;

	std::string path4 = "path1/path2/p5/p4/watcher4";
	CRATE_WATCH_OBJECT(path4, &t, &tt::hello);
	std::cout << ( ( MethodWatcher<int, tt>* )GET_WATCHER_OBJECT(path4).get() )->
		getValue() << std::endl;

	a = 2;
	t.a = 2;

	std::cout << ( ( ValueWatcher<int>* )GET_WATCHER_OBJECT(path1).get() )->getValue()
		<< std::endl;
	std::cout << ( ( ValueWatcher<int>* )GET_WATCHER_OBJECT(path2).get() )->getValue()
		<< std::endl;
}