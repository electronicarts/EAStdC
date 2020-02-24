///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EASprintf.h>
#include <EAStdC/EASprintfOrdered.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EADateTime.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EASTL/string.h>
#include <EASTL/unique_ptr.h>
#include <EATest/EATest.h>
#include <float.h>
#include <string.h>
#include <stdarg.h>


static void TestCRTVsnprintf(char* pDestination, size_t n, const char* pFormat, ...)
{
	va_list vList;
	va_start(vList, pFormat);
	EA::StdC::Vsnprintf(pDestination, n, pFormat, vList);
	va_end(vList);
}

static void TestCRTVsnprintf(char16_t* pDestination, size_t n, const char16_t* pFormat, ...)
{
	va_list vList;
	va_start(vList, pFormat);
	EA::StdC::Vsnprintf(pDestination, n, pFormat, vList);
	va_end(vList);
}

static void TestCRTVsnprintf(char32_t* pDestination, size_t n, const char32_t* pFormat, ...)
{
	va_list vList;
	va_start(vList, pFormat);
	EA::StdC::Vsnprintf(pDestination, n, pFormat, vList);
	va_end(vList);
}


#if EASTDC_VSNPRINTF8_ENABLED
	static void TestCRTVsnprintf8(char* pDestination, size_t n, const char* pFormat, ...)
	{
		va_list vList;
		va_start(vList, pFormat);
		EA::StdC::Vsnprintf8(pDestination, n, pFormat, vList);
		va_end(vList);
	}

	static void TestCRTVsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, ...)
	{
		va_list vList;
		va_start(vList, pFormat);
		EA::StdC::Vsnprintf16(pDestination, n, pFormat, vList);
		va_end(vList);
	}

	static void TestCRTVsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, ...)
	{
		va_list vList;
		va_start(vList, pFormat);
		EA::StdC::Vsnprintf32(pDestination, n, pFormat, vList);
		va_end(vList);
	}
#endif


static float FloatFromBitRepr(uint32_t bits)
{
	union { float f; uint32_t u; } typepun;
	typepun.u = bits;
	return typepun.f;
}

static double DoubleFromBitRepr(uint64_t bits)
{
	union { double d; uint64_t u; } typepun;
	typepun.u = bits;
	return (double)typepun.d;
}

static int TestSprintf8(int unused = 0, ...)
{
	using namespace EA::StdC;

	int nErrorCount(0);

	// int Snprintf(char_t* pDestination, size_t n, const char_t* pFormat, ...);
	{
		char sn18[128];
		Snprintf(sn18, 128, "%5s%-4d%03i", "abc", -12, 3);
		EATEST_VERIFY(!Strcmp("  abc-12 003", sn18));
		Snprintf(sn18, 128, "%.2f", 3.1415);
		EATEST_VERIFY(!Strcmp("3.14", sn18));
	}

	// int Vsnprintf(char_t* pDestination, size_t n, const char_t* pFormat, ...);
	{
		char sn18[128];
		TestCRTVsnprintf(sn18, 128, "%5s%-5d%04i", "abc", -12, 3);
		EATEST_VERIFY(!Strcmp("  abc-12  0003", sn18));
		TestCRTVsnprintf(sn18, 128, "%.2f", 3.1415);
		EATEST_VERIFY(!Strcmp("3.14", sn18));
	}

	#if EASTDC_VSNPRINTF8_ENABLED
		{
			char sn18[128];
			TestCRTVsnprintf8(sn18, 128, "%5s%-5d%04i", "abc", -12, 3);
			EATEST_VERIFY(!Strcmp("  abc-12  0003", sn18));
			TestCRTVsnprintf8(sn18, 128, "%.2f", 3.1415);
			EATEST_VERIFY(!Strcmp("3.14", sn18));
		}
	#endif


	// int Vscprintf(const char_t* pFormat, va_list arguments);
	{
		va_list arguments;
		va_start(arguments, unused);

		int result = Vscprintf("abc", arguments);       
		EATEST_VERIFY(result == 3);

		va_end(arguments);
	}


	// template <typename String>
	// int StringVcprintf(String& s, const char* EA_RESTRICT pFormat, va_list arguments)
	{
		va_list arguments;
		va_start(arguments, unused);

		eastl::string s8;
		int result = StringVcprintf(s8, "hello", arguments);
		EATEST_VERIFY((result == 5) && (s8 == "hello"));

		va_end(arguments);
	}


	// template <typename String> 
	// int StringPrintf(String& s, const typename String::value_type* EA_RESTRICT pFormat, ...)
	{
		eastl::string s8;
		int result = StringPrintf(s8, "%s", "hello");
		EATEST_VERIFY((result == 5) && (s8 == "hello"));
	}

	{
		char buffer[128];
		Sprintf(buffer, "%Lf", 42.0l);
		EATEST_VERIFY(Strcmp(buffer, "42.000000") == 0);
	}

	{
		// Test for parsing of PRI constants in format strings
		char buffer[128];
		Sprintf(buffer, "%" PRIxPTR, (intptr_t) 0xDEADBEEF);
		EATEST_VERIFY(Strcmp(buffer, "deadbeef") == 0);
	}

	// Sprintf
	{
		char buffer[128];
		const int kHexValue = 0x12;

		Sprintf(buffer, "%.4x", kHexValue);
		EATEST_VERIFY(Strcmp(buffer, "0012") == 0);

		Sprintf(buffer, "%04x", kHexValue);
		EATEST_VERIFY(Strcmp(buffer, "0012") == 0);

		Sprintf(buffer, "%4.4x", kHexValue);
		EATEST_VERIFY(Strcmp(buffer, "0012") == 0);

		Sprintf(buffer, "%04.4x", kHexValue);
		EATEST_VERIFY(Strcmp(buffer, "0012") == 0);

		Sprintf(buffer, "%4.3x", kHexValue);
		EATEST_VERIFY(Strcmp(buffer, " 012") == 0);

		Sprintf(buffer, "%04.3x", kHexValue);
		EATEST_VERIFY(Strcmp(buffer, " 012") == 0);

		Sprintf(buffer, "%.*x", 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, "0012") == 0);

		Sprintf(buffer, "%0*x", 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, "0012") == 0);

		Sprintf(buffer, "%*.*x", 4, 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, "0012") == 0);

		Sprintf(buffer, "%0*.*x", 4, 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, "0012") == 0);
	}


	{
		char buffer[128];

		Sprintf(buffer, "decimal negative: \"%d\"\n", -2345);
		EATEST_VERIFY(Strcmp(buffer, "decimal negative: \"-2345\"\n") == 0);

		Sprintf(buffer, "octal negative: \"%o\"\n", -2345);
		if(sizeof(int) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, "octal negative: \"37777773327\"\n") == 0);
		else if(sizeof(int) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, "octal negative: \"1777777777777777773327\"\n") == 0);

		Sprintf(buffer, "hex negative: \"%x\"\n", -2345);
		if(sizeof(int) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, "hex negative: \"fffff6d7\"\n") == 0);
		else if(sizeof(int) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, "hex negative: \"fffffffffffff6d7\"\n") == 0);

		Sprintf(buffer, "long decimal number: \"%ld\"\n", -123456L);
		EATEST_VERIFY(Strcmp(buffer, "long decimal number: \"-123456\"\n") == 0);

		Sprintf(buffer, "long octal negative: \"%lo\"\n", -2345L);
		if(sizeof(long) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, "long octal negative: \"37777773327\"\n") == 0);
		else if(sizeof(long) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, "long octal negative: \"1777777777777777773327\"\n") == 0);

		Sprintf(buffer, "long unsigned decimal number: \"%lu\"\n", -123456L);
		if(sizeof(long) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, "long unsigned decimal number: \"4294843840\"\n") == 0);
		else if(sizeof(long) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, "long unsigned decimal number: \"18446744073709428160\"\n") == 0);

		Sprintf(buffer, "zero-padded LDN: \"%010ld\"\n", -123456L);
		EATEST_VERIFY(Strcmp(buffer, "zero-padded LDN: \"-000123456\"\n") == 0);

		Sprintf(buffer, "left-adjusted ZLDN: \"%-010ld\"\n", -123456L);
		EATEST_VERIFY(Strcmp(buffer, "left-adjusted ZLDN: \"-123456   \"\n") == 0);

		Sprintf(buffer, "space-padded LDN: \"%10ld\"\n", -123456L);
		EATEST_VERIFY(Strcmp(buffer, "space-padded LDN: \"   -123456\"\n") == 0);

		Sprintf(buffer, "left-adjusted SLDN: \"%-10ld\"\n", -123456L);
		EATEST_VERIFY(Strcmp(buffer, "left-adjusted SLDN: \"-123456   \"\n") == 0);
	}


	{
		char buffer[1024];
		char str1[] = "abc de";
		char str2[] = "abd def ghi jkl mno pqr stu vwz yz.";

		// The C99 standard specifies that leading zeros only put zeroes in front of numerical types. Spaces for others.
		Sprintf(buffer, "zero-padded string: \"%010s\"\n", str1);
		EATEST_VERIFY(Strcmp(buffer, "zero-padded string: \"    abc de\"\n") == 0); // VC++ fails this, as it puts zeroes in front.

		Sprintf(buffer, "left-adjusted Z string: \"%-010s\"\n", str1);
		EATEST_VERIFY(Strcmp(buffer, "left-adjusted Z string: \"abc de    \"\n") == 0);

		Sprintf(buffer, "space-padded string: \"%10s\"\n", str1);
		EATEST_VERIFY(Strcmp(buffer, "space-padded string: \"    abc de\"\n") == 0);

		Sprintf(buffer, "left-adjusted S string: \"%-10s\"\n", str1);
		EATEST_VERIFY(Strcmp(buffer, "left-adjusted S string: \"abc de    \"\n") == 0);

		Sprintf(buffer, "limited string: \"%.22s\"\n", str2);
		EATEST_VERIFY(Strcmp(buffer, "limited string: \"abd def ghi jkl mno pq\"\n") == 0);

		Sprintf(buffer, "null string: \"%s\"\n", (char*)NULL);
		EATEST_VERIFY(Strcmp(buffer, "null string: \"(null)\"\n") == 0);

		Sprintf(buffer, "%10s\n", (char*)NULL);
		EATEST_VERIFY(Strcmp(buffer, "    (null)\n") == 0);

		Sprintf(buffer, "%-10s\n", (char*)NULL);
		EATEST_VERIFY(Strcmp(buffer, "(null)    \n") == 0);

		Sprintf(buffer, "%*s%*s%*s", -1, "one", -20, "two", -30, "three");
		EATEST_VERIFY(Strcmp(buffer, "onetwo                 three                         ") == 0);

		int i;
		memset(buffer, '_', sizeof(buffer));
		Sprintf(buffer, "x%1000s", " ");
		EATEST_VERIFY(buffer[0] == 'x');
		for(i = 0; i < 1000; i++)
		{
			if(buffer[1 + i] != ' ')
				break;
		}
		if(i != 1000)
			EATEST_VERIFY(i == 1000);
		else
			EATEST_VERIFY(buffer[1 + 1000] == 0);
	}


	{   // String tests
		// We accept %hc, %c, %lc, %I8c, %I16c, %I32c (regular, regular, wide, char, char16_t, char32_t)
		// We accept %hC, %C, %lC, %I8C, %I16C, %I32C (regular, wide,    wide, char, char16_t, char32_t)
		// We accept %hs, %s, %ls, %I8s, %I16s, %I32s (regular, regular, wide, char, char16_t, char32_t)
		// We accept %hS, %S, %lS, %I8s, %I16s, %I32s (regular, wide,    wide, char, char16_t, char32_t)

		char  buffer[32];
		char  dStr8[2]  = { 'd', 0 };
		char16_t eStr16[2] = { 'e', 0 };
		char32_t fStr32[2] = { 'f', 0 };

		Sprintf(buffer, "%hc %c %lc %I8c %I16c %I32c", 'a', 'b',           EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
		EATEST_VERIFY(Strcmp(buffer, "a b c d e f") == 0);

		Sprintf(buffer, "%hC %C %lC %I8C %I16C %I32C", 'a', EA_WCHAR('b'), EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
		EATEST_VERIFY(Strcmp(buffer, "a b c d e f") == 0);

		Sprintf(buffer, "%hs %s %ls %I8s %I16s %I32s", "a", "b",           EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
		EATEST_VERIFY(Strcmp(buffer, "a b c d e f") == 0);

		Sprintf(buffer, "%hS %S %lS %I8S %I16S %I32S", "a", EA_WCHAR("b"), EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
		EATEST_VERIFY(Strcmp(buffer, "a b c d e f") == 0);
	}


	{ // NaN/Inf functionality tests
		char buffer[256];

		const float     kFloat32PositiveInfinity     = FloatFromBitRepr(UINT32_C(0x7f800000));
		const float     kFloat32NegativeInfinity     = FloatFromBitRepr(UINT32_C(0xff800000));
		const double    kFloat64PositiveInfinity     = DoubleFromBitRepr(UINT64_C(0x7ff0000000000000));
		const double    kFloat64NegativeInfinity     = DoubleFromBitRepr(UINT64_C(0xfff0000000000000));

		const float     kFloat32PositiveNaN     = FloatFromBitRepr(UINT32_C(0x7fffffff));
		const float     kFloat32NegativeNaN     = FloatFromBitRepr(UINT32_C(0xffffffff));
		const double    kFloat64PositiveNaN     = DoubleFromBitRepr(UINT64_C(0x7fffffffffffffff));
		const double    kFloat64NegativeNaN     = DoubleFromBitRepr(UINT64_C(0xffffffffffffffff));
		
		Sprintf(buffer, "%e %f %g", kFloat32PositiveInfinity, kFloat32PositiveInfinity, kFloat32PositiveInfinity);
		EATEST_VERIFY(Strcmp(buffer, "inf inf inf") == 0);

		Sprintf(buffer, "%e %f %g", kFloat32NegativeInfinity, kFloat32NegativeInfinity, kFloat32NegativeInfinity);
		EATEST_VERIFY(Strcmp(buffer, "-inf -inf -inf") == 0);

		Sprintf(buffer, "%e %f %g", kFloat32PositiveNaN, kFloat32PositiveNaN, kFloat32PositiveNaN);
		EATEST_VERIFY(Strcmp(buffer, "nan nan nan") == 0);

		// The ARM instruction fcvtds (convert single to double point precision) does not maintain the sign of NaN.
		// (Float is always promoted to double in variable length arguments)
		// This seems to work on the Android
		// To consider: Disable this test altogether, as it seems to be too FPU-specific to spend our time with.
		#if defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64) || defined(EA_PROCESSOR_POWERPC)
			Sprintf(buffer, "%e %f %g", kFloat32NegativeNaN, kFloat32NegativeNaN, kFloat32NegativeNaN);
			EATEST_VERIFY(Strcmp(buffer, "-nan -nan -nan") == 0);
		#else
			EA_UNUSED(kFloat32NegativeNaN);
		#endif

		Sprintf(buffer, "%e %f %g", kFloat64PositiveInfinity, kFloat64PositiveInfinity, kFloat64PositiveInfinity);
		EATEST_VERIFY(Strcmp(buffer, "inf inf inf") == 0);

		Sprintf(buffer, "%e %f %g", kFloat64NegativeInfinity, kFloat64NegativeInfinity, kFloat64NegativeInfinity);
		EATEST_VERIFY(Strcmp(buffer, "-inf -inf -inf") == 0);

		Sprintf(buffer, "%e %f %g", kFloat64PositiveNaN, kFloat64PositiveNaN, kFloat64PositiveNaN);
		EATEST_VERIFY(Strcmp(buffer, "nan nan nan") == 0);

		Sprintf(buffer, "%e %f %g", kFloat64NegativeNaN, kFloat64NegativeNaN, kFloat64NegativeNaN);
		EATEST_VERIFY(Strcmp(buffer, "-nan -nan -nan") == 0);

		#if !defined(EA_PLATFORM_XBOXONE)
			// This test should theoretically work on Capilano.  But currently it must be disabled or it
			// will cause the runtime to assert.  A bug has been logged with MS so hopefully the issue
			// will be resolved in the future.  We should try to enable the test later if it is resolved.
			//
			// Link to issue;
			//   https://forums.xboxlive.com/questions/48008/is-there-a-bug-in-string-formatting-functions-in-t.html
			const double kSmallestDoubleNum = DoubleFromBitRepr(0x0000000000000001ull);
			Sprintf(buffer, "%f", kSmallestDoubleNum);
			EATEST_VERIFY_F(Strcmp(buffer, "0.000000") == 0, "Result was %s", buffer);
		#endif
	}

	{ // Extended functionality tests

		char buffer[256];

		Sprintf(buffer, "%08x %032b", 0xaaaaaaaa, 0xaaaaaaaa);
		EATEST_VERIFY(Strcmp(buffer, "aaaaaaaa 10101010101010101010101010101010") == 0);

		Sprintf(buffer, "%I8u %I8d %I16u %I16d %I32u %I32d %I64u %I64d", 0xff, 0xff, 0xffff, 0xffff, 0xffffffff, 0xffffffff, UINT64_C(0xffffffffffffffff), UINT64_C(0xffffffffffffffff));
		EATEST_VERIFY(Strcmp(buffer, "255 -1 65535 -1 4294967295 -1 18446744073709551615 -1") == 0);

		Sprintf(buffer, "%s %10s", NULL, NULL);
		EATEST_VERIFY(Strcmp(buffer, "(null)     (null)") == 0);
	}


	{
		char buffer[1024];
		int i;

		Sprintf(buffer, "e-style >= 1: \"%e\"\n", 12.34);
		EATEST_VERIFY(Strcmp(buffer, "e-style >= 1: \"1.234000e+01\"\n") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, "e-style >= .1: \"%e\"\n", 0.1234);
		EATEST_VERIFY(Strcmp(buffer, "e-style >= .1: \"1.234000e-01\"\n") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, "e-style < .1: \"%e\"\n", 0.001234);
		EATEST_VERIFY(Strcmp(buffer, "e-style < .1: \"1.234000e-03\"\n") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, "e-style big: \"%.60e\"\n", 1e20);
		EATEST_VERIFY(Strcmp(buffer, "e-style big: \"1.000000000000000000000000000000000000000000000000000000000000e+20\"\n") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, "e-style == .1: \"%e\"\n", 0.1);
		EATEST_VERIFY(Strcmp(buffer, "e-style == .1: \"1.000000e-01\"\n") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, "f-style >= 1: \"%f\"\n", 12.34);
		EATEST_VERIFY(Strcmp(buffer, "f-style >= 1: \"12.340000\"\n") == 0);

		Sprintf(buffer, "f-style >= 1: \"%.3f\"\n", 12.34);
		EATEST_VERIFY(Strcmp(buffer, "f-style >= 1: \"12.340\"\n") == 0);

		Sprintf(buffer, "f-style >= .1: \"%f\"\n", 0.1234);
		EATEST_VERIFY(Strcmp(buffer, "f-style >= .1: \"0.123400\"\n") == 0);

		Sprintf(buffer, "f-style < .1: \"%f\"\n", 0.001234);
		EATEST_VERIFY(Strcmp(buffer, "f-style < .1: \"0.001234\"\n") == 0);

		Sprintf(buffer, "g-style >= 1: \"%.0g\"\n", 1.234);
		EATEST_VERIFY(Strcmp(buffer, "g-style >= 1: \"1.2\"\n") == 0);//%g takes precision to be 1, even if set to be 0

		Sprintf(buffer, "g-style >= 1: \"%.1g\"\n", 1.234);
		EATEST_VERIFY(Strcmp(buffer, "g-style >= 1: \"1.2\"\n") == 0);

		Sprintf(buffer, "g-style >= 1: \"%.2g\"\n", 1.234);
		EATEST_VERIFY(Strcmp(buffer, "g-style >= 1: \"1.23\"\n") == 0);

		Sprintf(buffer, "g-style >= 1: \"%g\"\n", 12.34);
		EATEST_VERIFY(Strcmp(buffer, "g-style >= 1: \"12.34\"\n") == 0);

		Sprintf(buffer, "g-style >= .1: \"%g\"\n", 0.1234);
		EATEST_VERIFY(Strcmp(buffer, "g-style >= .1: \"0.1234\"\n") == 0);

		Sprintf(buffer, "g-style < .1: \"%g\"\n", 0.001234);
		EATEST_VERIFY(Strcmp(buffer, "g-style < .1: \"0.001234\"\n") == 0);

		Sprintf(buffer, "g-style < .1: \"%g\"\n", 0.001234678);
		EATEST_VERIFY(Strcmp(buffer, "g-style < .1: \"0.001235\"\n") == 0);

		Sprintf(buffer, "g-style big: \"%.60g\"\n", 1e20);
		EATEST_VERIFY(Strcmp(buffer, "g-style big: \"100000000000000000000\"\n") == 0);

		//Sprintf(buffer, "%#.4g\n", 0.0); // The C99 committee has decided in a defect analysis that this is how it should work.
		//EATEST_VERIFY(Strcmp(buffer, "0\n") == 0);

		Sprintf(buffer, " %6.5f\n", .099999999860301614);
		EATEST_VERIFY(Strcmp(buffer, " 0.10000\n") == 0);

		Sprintf(buffer, " %6.5f\n", .1);
		EATEST_VERIFY(Strcmp(buffer, " 0.10000\n") == 0);

		Sprintf(buffer, "x%5.4fx\n", .5);
		EATEST_VERIFY(Strcmp(buffer, "x0.5000x\n") == 0);

		Sprintf(buffer, "%#03x\n", 1);
		EATEST_VERIFY(Strcmp(buffer, "0x1\n") == 0);


		memset(buffer, '_', sizeof(buffer));
		Sprintf(buffer, "%.300f", 1.0);
		EATEST_VERIFY((buffer[0] == '1') && (buffer[1] == '.'));
		for(i = 0; i < 300; i++)
		{
			if(buffer[2 + i] != '0')
				break;
		}
		if(i != 300)
			EATEST_VERIFY(i == 300);
		else
			EATEST_VERIFY(buffer[2 + 300] == 0);


		double d = static_cast<double>(FLT_MIN);    // We are intentionally using FLT_MIN and not DBL_MIN.
		d /= 2.0;
		Sprintf(buffer, "%.17e", d);                // It should be something like 5.87747175411143___e-39 where count and values of the _ digits vary by hardware.
		buffer[16] = buffer[17] = buffer[18] = '_'; // Replace the uncertain digits with '_' characters, as they are system-dependent.

		EATEST_VERIFY(Strcmp(buffer, "5.87747175411143___e-39") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.


		Sprintf(buffer, "%15.5e\n", 4.9406564584124654e-307);
		EATEST_VERIFY(Strcmp(buffer, "   4.94066e-307\n") == 0);


		// Exercise bug report on PS3 platform which results in a crash when printing DBL_MAX.
		i = Sprintf(buffer, "%15.8g", DBL_MAX);
		EATEST_VERIFY(i > 0);

		i = Sprintf(buffer, "%15.8g", FLT_MAX);
		EATEST_VERIFY(i > 0);
	}


	{
		char        buffer[256];
		const char* pExpected;

		// VC++ sprintf would fail these tests, as the Standard says to print no more 
		// than 2 unless necessary, yet VC++ sprintf prints 3 digits exponents.

		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", 0.0, 0.0, 0.0);
		EATEST_VERIFY(Strcmp(buffer, "|      0.0000|  0.0000e+00|           0|") == 0);

		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", 1.0, 1.0, 1.0);
		EATEST_VERIFY(Strcmp(buffer, "|      1.0000|  1.0000e+00|           1|") == 0);

		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", -1.0, -1.0, -1.0);
		EATEST_VERIFY(Strcmp(buffer, "|     -1.0000| -1.0000e+00|          -1|") == 0);

		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", 100.0, 100.0, 100.0);
		EATEST_VERIFY(Strcmp(buffer, "|    100.0000|  1.0000e+02|         100|") == 0);

		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", 1000.0, 1000.0, 1000.0);
		EATEST_VERIFY(Strcmp(buffer, "|   1000.0000|  1.0000e+03|        1000|") == 0);

		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", 10000.0, 10000.0, 10000.0);
		EATEST_VERIFY(Strcmp(buffer, "|  10000.0000|  1.0000e+04|       1e+04|") == 0);

		// %g picks one of %f or %e, though uses precision differently.
		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", 12346.0, 12346.0, 12346.0);
		pExpected = "|  12346.0000|  1.2346e+04|   1.235e+04|";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", 100000.0, 100000.0, 100000.0);
		pExpected = "| 100000.0000|  1.0000e+05|       1e+05|";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "|%12.4f|%12.4e|%12.4g|", 123467.0, 123467.0, 123467.0);
		pExpected = "| 123467.0000|  1.2347e+05|   1.235e+05|";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer);
	}


	{
		char buffer[256];

		// Verify that snprintf follows the C99 convention of returning the number of characters
		// required. This is as opposed to the non-standard way that some libraries just return 
		// -1 if the buffer isn't big enough.

		const int kBuf1Capacity = 20;
		char   buf1[kBuf1Capacity];
		int       n1 = Snprintf(buf1, kBuf1Capacity, "%30s", "foo");
		Sprintf(buffer, "snprintf(\"%%30s\", \"foo\") == %d, \"%.*s\"\n", n1, kBuf1Capacity, buf1);
		EATEST_VERIFY(Strcmp(buffer, "snprintf(\"%30s\", \"foo\") == 30, \"                   \"\n") == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		const int kBuf2Capacity = 512;
		char   buf2[kBuf2Capacity];
		int       n2 = Snprintf(buf2, kBuf2Capacity, "%.1000u", 10);
		Sprintf(buffer, "snprintf(\"%%.1000u\", 10) == %d\n", n2);
		EATEST_VERIFY(Strcmp(buffer, "snprintf(\"%.1000u\", 10) == 1000\n") == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		const int kBuf3Capacity = 512;
		char   buf3[kBuf3Capacity];
		char*  pString = new char[100000];
		memset(pString, '_', 100000 * sizeof(char));
		pString[100000 - 1] = 0;
		int n3 = Snprintf(buf3, kBuf2Capacity, "%s", pString);
		Sprintf(buffer, "snprintf(\"%%s\", pString) == %d\n", n3);
		EATEST_VERIFY(Strcmp(buffer, "snprintf(\"%s\", pString) == 99999\n") == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.
		delete[] pString;

		int n4 = Snprintf(NULL, 0, "%s", "abc");
		Sprintf(buffer, "snprintf(NULL, \"abc\") == %d\n", n4);
		EATEST_VERIFY(Strcmp(buffer, "snprintf(NULL, \"abc\") == 3\n") == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		int n5 = Snprintf(NULL, 100, "%s", "abc");
		Sprintf(buffer, "snprintf(NULL, \"abc\") == %d\n", n5);
		EATEST_VERIFY(Strcmp(buffer, "snprintf(NULL, \"abc\") == 3\n") == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.
	}


	{
		char buffer[16][256];

		int n = 0, i, j, k, m;

		for(i = 0; i < 2; i++)
		{
			for(j = 0; j < 2; j++)
			{
				for(k = 0; k < 2; k++)
				{
					for(m = 0; m < 2; m++)
					{
						char prefix[7];
						char format[128];

						Strcpy(prefix, "%");
						if(i == 0)
							Strcat(prefix, "-");
						if(j == 0)
							Strcat(prefix, "+");
						if(k == 0)
							Strcat(prefix, "#");
						if(m == 0)
							Strcat(prefix, "0");

						#define DEC -123
						#define INT 255
						#define UNS (~0)

						Sprintf(format, "%%5s |%s6d |%s6o |%s6x |%s6X |%s6u |", prefix, prefix, prefix, prefix, prefix);
						Sprintf(buffer[n], format, prefix, DEC, INT, INT, INT, UNS);
						n++;
					}
				}
			}
		}

		EATEST_VERIFY(Strcmp(buffer[ 0], "%-+#0 |-123   |0377   |0xff   |0XFF   |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 1], " %-+# |-123   |0377   |0xff   |0XFF   |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 2], " %-+0 |-123   |377    |ff     |FF     |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 3], "  %-+ |-123   |377    |ff     |FF     |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 4], " %-#0 |-123   |0377   |0xff   |0XFF   |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 5], "  %-# |-123   |0377   |0xff   |0XFF   |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 6], "  %-0 |-123   |377    |ff     |FF     |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 7], "   %- |-123   |377    |ff     |FF     |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 8], " %+#0 |-00123 |000377 |0x00ff |0X00FF |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[ 9], "  %+# |  -123 |  0377 |  0xff |  0XFF |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[10], "  %+0 |-00123 |000377 |0000ff |0000FF |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[11], "   %+ |  -123 |   377 |    ff |    FF |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[12], "  %#0 |-00123 |000377 |0x00ff |0X00FF |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[13], "   %# |  -123 |  0377 |  0xff |  0XFF |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[14], "   %0 |-00123 |000377 |0000ff |0000FF |4294967295 |") == 0);
		EATEST_VERIFY(Strcmp(buffer[15], "    % |  -123 |   377 |    ff |    FF |4294967295 |") == 0);

	}


	{
		char        buffer[256];
		const char* pExpected;

		Sprintf(buffer, "%e", 1234567.8); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = "1.234568e+06";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%f", 1234567.8);
		pExpected = "1234567.800000";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%g", 1234567.8); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = "1.23457e+06";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%g", 123.456);
		pExpected = "123.456";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%g", 1000000.0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = "1e+06";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%g", 10.0);
		pExpected = "10";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%g", 0.02);
		pExpected = "0.02";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%.1f", 0.09523f);
		pExpected = "0.1";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%.1f", 0.9523f);
		pExpected = "1.0";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%.1f", -0.9523f);
		pExpected = "-1.0";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%.1e", 0.9523f);
		pExpected = "9.5e-01";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		#if EASTDC_NATIVE_FCVT && defined(_MSC_VER)
			Sprintf(buffer, "%.0e", 0.9523f);
			pExpected = "1e+00";
			EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 
		#else
			// Note that this test result is incorrect! It should be "1e+00" and not "1.0e00".
			// We have a bug and it needs to be fixed. At least the numerical value is still correct.
			// The problem is that GCC's standard library fcvt and our custom fcvt generate a string
			// of "10" instead of "1" for the case of calling fcvt(0.9523, 0, ...).
			Sprintf(buffer, "%.0e", 0.9523f);
			pExpected = "1.0e+00";
			EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 
		#endif
	}


	{   // Test the ' extension, which cases numbers to be printed with a thousands separator.
		char        buffer[64];
		const char* pExpected;

		Sprintf(buffer, "%'u", 123456789);
		EATEST_VERIFY(Strcmp(buffer, "123,456,789") == 0);

		Sprintf(buffer, "%'d", -123456789);
		EATEST_VERIFY(Strcmp(buffer, "-123,456,789") == 0);

		Sprintf(buffer, "%'I8u", 123);
		EATEST_VERIFY(Strcmp(buffer, "123") == 0);

		Sprintf(buffer, "%'I16u", 12345);
		EATEST_VERIFY(Strcmp(buffer, "12,345") == 0);

		Sprintf(buffer, "%'I16d", -12345);
		EATEST_VERIFY(Strcmp(buffer, "-12,345") == 0);

		Sprintf(buffer, "%'I32u", 12345678);
		EATEST_VERIFY(Strcmp(buffer, "12,345,678") == 0);

		Sprintf(buffer, "%'I32d", -12345678);
		EATEST_VERIFY(Strcmp(buffer, "-12,345,678") == 0);

		Sprintf(buffer, "%20I32d", -12345678);
		EATEST_VERIFY(Strcmp(buffer, "           -12345678") == 0);

		Sprintf(buffer, "%'20I32d", -12345678); // Verify that the , chars count towards the field width.
		EATEST_VERIFY(Strcmp(buffer, "         -12,345,678") == 0);

		Sprintf(buffer, "%'I32x", 0x12345678);  // ' has no effect on hex formatting.
		EATEST_VERIFY(Strcmp(buffer, "12345678") == 0);

		Sprintf(buffer, "%'I64u", UINT64_C(1234999995678));
		EATEST_VERIFY(Strcmp(buffer, "1,234,999,995,678") == 0);

		Sprintf(buffer, "%'I64d", INT64_C(-1234599999678));
		EATEST_VERIFY(Strcmp(buffer, "-1,234,599,999,678") == 0);

		Sprintf(buffer, "%'I64x", UINT64_C(0x1234567812345678));  // ' has no effect on hex formatting.
		EATEST_VERIFY(Strcmp(buffer, "1234567812345678") == 0);

		Sprintf(buffer, "%'f", 123456.234);
		EATEST_VERIFY(Strcmp(buffer, "123,456.234000") == 0);

		Sprintf(buffer, "%'e", 1234567.8);  // ' has no effect on %e formatting.
		EATEST_VERIFY(Strcmp(buffer, "1.234568e+06") == 0);

		Sprintf(buffer, "%'g", 1234.54);   // In some cases %g acts like %f.
		pExpected = "1,234.54";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 

		Sprintf(buffer, "%'g", 1234567.8);   // In some cases %g acts like %f.
		pExpected = "1.23457e+06";
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I8s\n   Actual:   %I8s", pExpected, buffer); 
	}


	{
		char buffer[256];

		Sprintf(buffer, "%hhu", UCHAR_MAX + 2);
		EATEST_VERIFY(Strcmp(buffer, "1") == 0); // VC++ fails this, as it doesn't implement the C99 standard %hh modifier.

		Sprintf(buffer, "%hu", USHRT_MAX + 2);
		EATEST_VERIFY(Strcmp(buffer, "1") == 0);
	}


	{
		char buffer[128];

		Sprintf(buffer, "%5.s", "xyz");
		EATEST_VERIFY(Strcmp(buffer, "     ") == 0);

		Sprintf(buffer, "%5.f", 33.3);
		EATEST_VERIFY(Strcmp(buffer, "   33") == 0);

		Sprintf(buffer, "%8.e", 33.3e7);
		EATEST_VERIFY(Strcmp(buffer, "   3e+08") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, "%8.E", 33.3e7);
		EATEST_VERIFY(Strcmp(buffer, "   3E+08") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, "%.g", 33.3);
		EATEST_VERIFY(Strcmp(buffer, "3e+01") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, "%.G", 33.3);
		EATEST_VERIFY(Strcmp(buffer, "3E+01") == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
	}


	{
		char buffer[128];
		int     precision;
		//for %g, precision 0 is not valid, if specified 0, it is taken as 1
		precision = 0;
		Sprintf(buffer, "%.*g", precision, 3.3);
		EATEST_VERIFY(Strcmp(buffer, "3.3") == 0);

		precision = 0;
		Sprintf(buffer, "%.*G", precision, 3.3);
		EATEST_VERIFY(Strcmp(buffer, "3.3") == 0);

		precision = 0;
		Sprintf(buffer, "%7.*G", precision, 3.33);
		EATEST_VERIFY(Strcmp(buffer, "    3.3") == 0);

		precision = 3;
		Sprintf(buffer, "%04.*o", precision, 33);
		EATEST_VERIFY(Strcmp(buffer, " 041") == 0);

		precision = 7;
		Sprintf(buffer, "%09.*u", precision, 33);
		EATEST_VERIFY(Strcmp(buffer, "  0000033") == 0);

		precision = 3;
		Sprintf(buffer, "%04.*x", precision, 33);
		EATEST_VERIFY(Strcmp(buffer, " 021") == 0);

		precision = 3;
		Sprintf(buffer, "%04.*X", precision, 33);
		EATEST_VERIFY(Strcmp(buffer, " 021") == 0);
	}


	{   // EAC tests

		char dest1[1024], dest3[1024];

		// Test #1
		Sprintf(dest1, "Hello.  Test.");
		EATEST_VERIFY(Strcmp(dest1, "Hello.  Test.") == 0);

		// Test #2
		Sprintf(dest1,"Hello.\\ \\  a\n\n\tTest.");
		EATEST_VERIFY(Strcmp(dest1, "Hello.\\ \\  a\n\n\tTest.") == 0);

		// Test #3
		Sprintf(dest1, "Int. %d %d %d blah%d%d", -1, 1, 0, 392832993, -32903298);
		EATEST_VERIFY(Strcmp(dest1, "Int. -1 1 0 blah392832993-32903298") == 0);

		// Test #4
		Sprintf(dest1,"Float.  %f %f %f\n", -0.01f, 10.0f, 0.0f);
		EATEST_VERIFY(Strcmp(dest1, "Float.  -0.010000 10.000000 0.000000\n") == 0);

		// Test #5
		Sprintf(dest1, "Str/char: %s %c %c", "test", 'b', 0341);
		EATEST_VERIFY(Strcmp(dest1, "Str/char: test b \341") == 0);

		// Test #6
		Sprintf(dest1,"Hex: %x %X",3829,-392);
		EATEST_VERIFY(Strcmp(dest1, "Hex: ef5 FFFFFE78") == 0);

		// Test #7
		//for(int32_t i = 0; i < 7; i++)    We'd need to modify the tests below to deal with this.
		//{
			Sprintf(dest3, "Precision: %%.%df", 0); 
			EATEST_VERIFY(Strcmp(dest3, "Precision: %.0f") == 0);
			Sprintf(dest1, dest3, 34.56f);
			EATEST_VERIFY(Strcmp(dest1, "Precision: 35") == 0);
		//}

		// Test #8
		Sprintf(dest1, "Align. %9s %8d %5s %10.2f", "test", 10293, "testtesttest", 10.8f);
		EATEST_VERIFY(Strcmp(dest1, "Align.      test    10293 testtesttest      10.80") == 0);

		// Test #9
		Sprintf(dest1, "Align. %-9s %-8d %-5s %-10.2f", "test", 10293, "testtesttest", 10.8f);
		EATEST_VERIFY(Strcmp(dest1, "Align. test      10293    testtesttest 10.80     ") == 0);

		// Test #10
		Sprintf(dest1, "Str: %S", EA_WCHAR("test"));
		EATEST_VERIFY(Strcmp(dest1, "Str: test") == 0);

		// Test #11
		Sprintf(dest1, "Str: %9S", EA_WCHAR("test"));
		EATEST_VERIFY(Strcmp(dest1, "Str:      test") == 0);

		// Test #12
		Sprintf(dest1, "Str: %-9S", EA_WCHAR("test"));
		EATEST_VERIFY(Strcmp(dest1, "Str: test     ") == 0);

		// Test #13
		Sprintf(dest1, "0x%08x", 0x876543);
		EATEST_VERIFY(Strcmp(dest1, "0x00876543") == 0);

		// Test #14
		Sprintf(dest1, "%08d", -15);
		EATEST_VERIFY(Strcmp(dest1, "-0000015") == 0);

		// Test #15
		Sprintf(dest1, "Int. %i %i %i blah%i%i", -1, 1, 0, 392832993, -32903298);
		EATEST_VERIFY(Strcmp(dest1, "Int. -1 1 0 blah392832993-32903298") == 0);

		// Test #16
		Sprintf(dest1, "%p", 0x38e340);
		EATEST_VERIFY(Strcmp(dest1, "38e340") == 0);

		// Test #17
		Sprintf(dest1, "%u", -372834);
		EATEST_VERIFY(Strcmp(dest1, "4294594462") == 0);

		// Test #18
		int tv1, tv2;

		Sprintf(dest1, "Testing%n%dTests%nabc", &tv1, 4738, &tv2);
		EATEST_VERIFY(Strcmp(dest1, "Testing4738Testsabc") == 0);
		EATEST_VERIFY((tv1 == 7) && (tv2 == 16));

		// Test #19
		Sprintf(dest1, "%o", -372834);
		EATEST_VERIFY(Strcmp(dest1, "37776447636") == 0);

		// Test #20
		Sprintf(dest1, "%%%s", "Hello");
		EATEST_VERIFY(Strcmp(dest1, "%Hello") == 0);

		// Test #21
		Sprintf(dest1, "%I64d", INT64_C(-12345678901234));
		EATEST_VERIFY(Strcmp(dest1, "-12345678901234") == 0);

		// Test #22
		Sprintf(dest1, "%I64u", INT64_C(-12345678901234));
		EATEST_VERIFY(Strcmp(dest1, "18446731728030650382") == 0);

		// Test #23
		Sprintf(dest1, "%I64x", INT64_C(0x1234567890123a));
		EATEST_VERIFY(Strcmp(dest1, "1234567890123a") == 0);

		// Test #24
		Sprintf(dest1, "%I64X", INT64_C(0x1234567890123A));
		EATEST_VERIFY(Strcmp(dest1, "1234567890123A") == 0);
	}

	{ // EAC FPU tests

		const float list1f[]={0.832f,0.00832f,8.32f,0.0f,-0.15f};

		const char* list1[]={"0.832000","0.008320","8.320000","0.000000","-0.150000"};

		const float list2f[]={0.0000000000123f,0.000000000123f,0.00000000123f,0.0000000123f
								,0.000000123f,0.00000123f,0.0000123f,0.000123f,0.00123f,0.0123f,0.123f,1.23f
								,12.3f,123.0f,1230.0f,12300.0f,123000.0f,1230000.0f,12300000.0f,123000000.0f,12300000000.0f};

		const char* list2[]={"0.000000","0.000000","0.000000","0.000000"
								,"0.000000","0.000001","0.000012","0.000123","0.001230","0.012300","0.123000","1.230000"
								,"12.300000","123.000000","1230.000000","12300.000000","123000.000000","1230000.000000",
								"12300000.000000","123000000.000000","12300000000.000000"};

		const char* list22D[]={"0.00","0.00","0.00","0.00"
								,"0.00","0.00","0.00","0.00","0.00","0.01","0.12","1.23"
								,"12.30","123.00","1230.00","12300.00","123000.00","1230000.00","12300000.00",
								"123000000.00","12300000000.00"};

		const char* list23DSL[]={"1.230e-11","1.230e-10","1.230e-09","1.230e-08"
								,"1.230e-07","1.230e-06","1.230e-05","1.230e-04","1.230e-03","1.230e-02","1.230e-01","1.230e+00"
								,"1.230e+01","1.230e+02","1.230e+03","1.230e+04","1.230e+05","1.230e+06","1.230e+07","1.230e+08"
								,"1.230e+10"};

		const char* list24DSL[]={"1.230E-11","1.230E-10","1.230E-09","1.230E-08"
								,"1.230E-07","1.230E-06","1.230E-05","1.230E-04","1.230E-03","1.230E-02","1.230E-01","1.230E+00"
								,"1.230E+01","1.230E+02","1.230E+03","1.230E+04","1.230E+05","1.230E+06","1.230E+07","1.230E+08"
								,"1.230E+10"};

		const char* list31DSL[]={"1.23e-11","1.23e-10","1.23e-09","1.23e-08"
								,"1.23e-07","1.23e-06","1.23e-05","0","0.001","0.012","0.123","1.23"
								,"12.3","123","1.23e+03","1.23e+04","1.23e+05","1.23e+06","1.23e+07","1.23e+08"
								,"1.23e+10"};

		const char* list32DSL[]={"1.23E-11","1.23E-10","1.23E-09","1.23E-08"
								,"1.23E-07","1.23E-06","1.23E-05","0","0.001","0.012","0.123","1.23"
								,"12.3","123","1.23E+03","1.23E+04","1.23E+05","1.23E+06","1.23E+07","1.23E+08"
								,"1.23E+10"};

		int32_t list1size = sizeof(list1f)/sizeof(list1f[0]);
		int32_t list2size = sizeof(list2f)/sizeof(list2f[0]);
		int32_t i, error;
		char str[256];

		error = 0;
		for(i = 0; i < list1size; i++)
		{
			Sprintf(str, "%f", list1f[i]);
			if(Strcmp(str, list1[i]) != 0)
			{
				error++;
				EA::UnitTest::Report("\nMismatch(A): %s/%s (Halt if > 2/section A)", str, list1[i]);
			}
		}

		EATEST_VERIFY(error <= 2);
		error = 0;
		for(i = 0; i < list2size - 4; i++)     // purposely take out the last 4 due to precision of float
		{
			Sprintf(str, "%f", list2f[i]);
			if(Strcmp(str, list2[i]) != 0)
			{
				error++;
				EA::UnitTest::Report("\nMismatch(B): %s/%s (Halt if > 3/section B)", str, list2[i]);
			}
		}

		EATEST_VERIFY(error <= 3);
		error = 0;
		for(i = 0; i < list2size - 2; i++)     // purposely take out the last 2 due to precision of float
		{
			Sprintf(str, "%.2f", list2f[i]);
			if(Strcmp(str, list22D[i]) != 0)
			{
				error++;
				EA::UnitTest::Report("\nMismatch(C): %s/%s (Halt if > 3/section C)", str, list22D[i]);
			}
		}

		EATEST_VERIFY(error <= 3);
		error = 0;
		for(i=0; i < list2size; i++)
		{
			Sprintf(str, "%.3e", list2f[i]);
			if(Strcmp(str, list23DSL[i]) != 0)
			{
				error++;
				EA::UnitTest::Report("\nMismatch(D): %s/%s (Halt if > 3/section D)", str, list23DSL[i]);
			}
		}

		EATEST_VERIFY(error <= 2);
		error = 0;
		for(i = 0; i < list2size; i++)
		{
			Sprintf(str,"%.3E", list2f[i]);
			if(Strcmp(str, list24DSL[i]) != 0)
			{
				error++;
				EA::UnitTest::Report("\nMismatch(E): %s/%s (Halt if > 3/section E)", str, list24DSL[i]);
			}
		}

		EATEST_VERIFY(error <= 2);
		error = 0;
		for(i = 0; i < list2size; i++)
		{
			Sprintf(str,"%.3g", list2f[i]);
			if(Strcmp(str, list31DSL[i]) != 0)
			{
				error++;
				EA::UnitTest::Report("\nMismatch(F): %s/%s (Halt if > 0/section F)", str, list23DSL[i]);
			}
		}

		EATEST_VERIFY(error == 0);
		error=0;
		for (i=0;i<list2size;i++)
		{
			Sprintf(str, "%.3G", list2f[i]);
			if(Strcmp(str, list32DSL[i]) != 0)
			{
				error++;
				EA::UnitTest::Report("\nMismatch(G): %s/%s (Halt if > 0/section G)", str, list24DSL[i]);
			}
		}
		EATEST_VERIFY(error == 0);
	}


	{ // Regression for user-reported bug.
		const size_t kBufferSize = 2048;

		char format[16];
		EA::StdC::Snprintf(format, sizeof(format), "%%.%us%%c123", kBufferSize - 1);
		EATEST_VERIFY(Strcmp(format, "%.2047s%c123") == 0);

		auto unique_buffer = eastl::make_unique<char[]>(kBufferSize);
		auto unique_expect = eastl::make_unique<char[]>(kBufferSize + 16);
		auto unique_actual = eastl::make_unique<char[]>(kBufferSize + 16);

		char* buffer = unique_buffer.get();
		char* expectedOutput = unique_expect.get();
		char* actualOutput = unique_actual.get();

		memset(buffer, '?', kBufferSize);
		buffer[kBufferSize - 1] = 0;
		char c = 'A';

		EA::StdC::Snprintf(actualOutput, kBufferSize + 16, format, buffer, c);
		memset(expectedOutput, '?', 2047);
		expectedOutput[2047] = 0;
		strcat(expectedOutput, "A123");
		EATEST_VERIFY(Strcmp(actualOutput, expectedOutput) == 0);
	}

	{
		static const int kSourceSize = 1024 * 5;
		static const int kOutputSize = kSourceSize + 100;
		char16_t value[kSourceSize];
		char destination[kOutputSize];
		char comparison[kOutputSize];

		for(int i = 0; i < kSourceSize - 1; ++i)
		{
			value[i] = '0' + (i % 10);
			comparison[i] = '0' + (i % 10);
		}
		value[kSourceSize - 1] = 0;
		comparison[kSourceSize - 1] = 0;

		EA::StdC::Snprintf(destination, kOutputSize, "%I16s", value);
		EATEST_VERIFY(Strcmp(destination, comparison) == 0);

		EA::StdC::Snprintf(destination, kOutputSize, "%.10I16s", value);
		EATEST_VERIFY(Strcmp(destination, "0123456789") == 0);
	}

	/* Copied from rwstdc but not completed.
	// Compare with Sprintf
	{
		char  buffer1[128];
		char  buffer2[128];
		char* test = "test %d %5.2f   %s \n \t\t\\\\  %X";

		Sprintf(buffer1, test, -3923, 0.38293, "test", 4568);
		Snprintf(buffer2, 128, test, -3923, 0.38293, "test", 4568);
		EATEST_VERIFY(strcmp(buffer1, buffer2) == 0);
	}


	// Compare with standard sprintf
	{
		char buffer1[128];
		char buffer2[128];
		char*   test = "test %d %5.2f   %s \n \t\t\\\\  %X";

		sprintf(buffer1, test, -3923, 0.38293, "test", 4568);
		Snprintf(buffer2, 128, test, -3923, 0.38293, "test", 4568);
		EATEST_VERIFY(strcmp(buffer1, buffer2) == 0);
	}


	{
		float sum    = 1035.32432f;
		float ave    = 34.3f;
		float min    = 343.34f;
		float max    = 576.3f;
		char  test[] = "NumberEvents: %5d, TotalDuration: %12.4f ms\nAve: %12.4f ms = %6.2f Hz\nMin: %12.4f ms = %6.2f Hz\nMax: %12.4f ms = %6.2f Hz\n";

		int res = Snprintf(snprintfString, bufferLength, 
						   "NumberEvents: %5d, TotalDuration: %12.4f ms\nAve: %12.4f ms = %6.2f Hz\nMin: %12.4f ms = %6.2f Hz\nMax: %12.4f ms = %6.2f Hz\n",
						   bufferLength, sum, ave, 1000.0f/ave, min, 1000.0f/min, max, 1000.0f/max);

		// Full String
		char testString[384];
		sprintf(testString, test, 384, sum, ave, 1000.0f/ave, min, 1000.0f/min, max, 1000.0f/max);
		EATEST_VERIFY(strcmp(snprintfString, testString) == 0);

		// Truncated string.
		const size_t origStringSize = strlen(snprintfString);
		for(size_t i = 0; i < origStringSize; i++)
		{
			res = Snprintf(snprintfString, i, test, 384, sum, ave, 1000.0f/ave, min, 1000.0f/min, max, 1000.0f/max);
			EATEST_VERIFY(res == origStringSize);
		}
	}


	{
		int   testData = 123;
		char* test     = "'%d' '%10d' '%-10d'";

		sprintf(expectedString, test, testData, testData, testData);
		Snprintf(snprintfString, bufferLength, test, testData, testData, testData);
		EATEST_VERIFY(strcmp(expectedString, snprintfString) == 0);
	}
	*/

	return nErrorCount;
}


static int TestSprintf16(int unused = 0, ...)
{
	using namespace EA::StdC;

	int nErrorCount(0);

	// int Snprintf(char_t* pDestination, size_t n, const char_t* pFormat, ...);
	{
		char16_t sn18[128];
		Snprintf(sn18, 128, EA_CHAR16("%5s%-4d%03i"), EA_WCHAR("abc"), -12, 3);
		EATEST_VERIFY(!Strcmp(EA_CHAR16("  abc-12 003"), sn18));
		Snprintf(sn18, 128, EA_CHAR16("%.2f"), 3.1415);
		EATEST_VERIFY(!Strcmp(EA_CHAR16("3.14"), sn18));
	}

	// int Vsnprintf(char_t* pDestination, size_t n, const char_t* pFormat, ...);
	{
		char16_t sn18[128];
		TestCRTVsnprintf(sn18, 128, EA_CHAR16("%5s%-5d%04i"), EA_WCHAR("abc"), -12, 3);
		EATEST_VERIFY(!Strcmp(EA_CHAR16("  abc-12  0003"), sn18));
		TestCRTVsnprintf(sn18, 128, EA_CHAR16("%.2f"), 3.1415);
		EATEST_VERIFY(!Strcmp(EA_CHAR16("3.14"), sn18));
	}

	#if EASTDC_VSNPRINTF8_ENABLED
		{
			char16_t sn18[128];
			TestCRTVsnprintf16(sn18, 128, EA_CHAR16("%5s%-5d%04i"), EA_WCHAR("abc"), -12, 3);
			EATEST_VERIFY(!Strcmp(EA_CHAR16("  abc-12  0003"), sn18));
			TestCRTVsnprintf16(sn18, 128, EA_CHAR16("%.2f"), 3.1415);
			EATEST_VERIFY(!Strcmp(EA_CHAR16("3.14"), sn18));
		}
	#endif

	// int Vscprintf(const char_t* pFormat, va_list arguments);
	{
		va_list arguments;
		va_start(arguments, unused);

		int result = Vscprintf(EA_CHAR16("abc"), arguments);       
		EATEST_VERIFY(result == 3);

		va_end(arguments);
	}

	// template <typename String>
	// int StringVcprintf(String& s, const char* EA_RESTRICT pFormat, va_list arguments)
	{
		va_list arguments;
		va_start(arguments, unused);

		eastl::string16 s16;
		int result = StringVcprintf(s16, EA_CHAR16("hello"), arguments);
		EATEST_VERIFY((result == 5) && (s16 == EA_CHAR16("hello")));

		va_end(arguments);
	}

	// template <typename String> 
	// int StringPrintf(String& s, const typename String::value_type* EA_RESTRICT pFormat, ...)
	{
		eastl::string16 s16;
		int result = StringPrintf(s16, EA_CHAR16("%s"), EA_WCHAR("hello"));
		EATEST_VERIFY((result == 5) && (s16 == EA_CHAR16("hello")));
	}

	{
		char16_t buffer[128];
		const int kHexValue = 0x12;

		Sprintf(buffer, EA_CHAR16("%.4x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0012")) == 0);

		Sprintf(buffer, EA_CHAR16("%04x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0012")) == 0);

		Sprintf(buffer, EA_CHAR16("%4.4x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0012")) == 0);

		Sprintf(buffer, EA_CHAR16("%04.4x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0012")) == 0);

		Sprintf(buffer, EA_CHAR16("%4.3x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16(" 012")) == 0);

		Sprintf(buffer, EA_CHAR16("%04.3x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16(" 012")) == 0);

		Sprintf(buffer, EA_CHAR16("%.*x"), 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0012")) == 0);

		Sprintf(buffer, EA_CHAR16("%0*x"), 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0012")) == 0);

		Sprintf(buffer, EA_CHAR16("%*.*x"), 4, 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0012")) == 0);

		Sprintf(buffer, EA_CHAR16("%0*.*x"), 4, 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0012")) == 0);
	}


	{
		char16_t buffer[128];

		Sprintf(buffer, EA_CHAR16("decimal negative: \"%d\"\n"), -2345);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("decimal negative: \"-2345\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("octal negative: \"%o\"\n"), -2345);
		if(sizeof(int) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("octal negative: \"37777773327\"\n")) == 0);
		else if(sizeof(int) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("octal negative: \"1777777777777777773327\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("hex negative: \"%x\"\n"), -2345);
		if(sizeof(int) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("hex negative: \"fffff6d7\"\n")) == 0);
		else if(sizeof(int) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("hex negative: \"fffffffffffff6d7\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("long decimal number: \"%ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("long decimal number: \"-123456\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("long octal negative: \"%lo\"\n"), -2345L);
		if(sizeof(long) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("long octal negative: \"37777773327\"\n")) == 0);
		else if(sizeof(long) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("long octal negative: \"1777777777777777773327\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("long unsigned decimal number: \"%lu\"\n"), -123456L);
		if(sizeof(long) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("long unsigned decimal number: \"4294843840\"\n")) == 0);
		else if(sizeof(long) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("long unsigned decimal number: \"18446744073709428160\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("zero-padded LDN: \"%010ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("zero-padded LDN: \"-000123456\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("left-adjusted ZLDN: \"%-010ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("left-adjusted ZLDN: \"-123456   \"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("space-padded LDN: \"%10ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("space-padded LDN: \"   -123456\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("left-adjusted SLDN: \"%-10ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("left-adjusted SLDN: \"-123456   \"\n")) == 0);
	}


	{
		char16_t  buffer[1024];
		wchar_t   str1[64]; Strlcpy(str1, EA_WCHAR("abc de"), EAArrayCount(str1)); // Can't do str1[64] = EA_CHAR16("abc de") because some compilers don't support 16 bit string literals.
		wchar_t   str2[64]; Strlcpy(str2, EA_WCHAR("abd def ghi jkl mno pqr stu vwz yz."), EAArrayCount(str2));

		// The C99 standard specifies that leading zeros only put zeroes in front of numerical types. Spaces for others.
		Sprintf(buffer, EA_CHAR16("zero-padded string: \"%010s\"\n"), str1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("zero-padded string: \"    abc de\"\n")) == 0); // VC++ fails this, as it puts zeroes in front.

		Sprintf(buffer, EA_CHAR16("left-adjusted Z string: \"%-010s\"\n"), str1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("left-adjusted Z string: \"abc de    \"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("space-padded string: \"%10s\"\n"), str1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("space-padded string: \"    abc de\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("left-adjusted S string: \"%-10s\"\n"), str1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("left-adjusted S string: \"abc de    \"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("limited string: \"%.22s\"\n"), str2);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("limited string: \"abd def ghi jkl mno pq\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("null string: \"%s\"\n"), (wchar_t*)NULL);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("null string: \"(null)\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("%10s\n"), (wchar_t*)NULL);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("    (null)\n")) == 0);

		Sprintf(buffer, EA_CHAR16("%-10s\n"), (wchar_t*)NULL);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("(null)    \n")) == 0);

		Sprintf(buffer, EA_CHAR16("%*s%*s%*s"), -1, EA_WCHAR("one"), -20, EA_WCHAR("two"), -30, EA_WCHAR("three"));
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("onetwo                 three                         ")) == 0);

		int i;
		memset(buffer, '_', sizeof(buffer));
		Sprintf(buffer, EA_CHAR16("x%1000ls"), EA_WCHAR(" "));
		EATEST_VERIFY(buffer[0] == 'x');
		for(i = 0; i < 1000; i++)
		{
			if(buffer[1 + i] != ' ')
				break;
		}
		if(i != 1000)
			EATEST_VERIFY(i == 1000);
		else
			EATEST_VERIFY(buffer[1 + 1000] == EA_CHAR16('\0'));
	}


	{   // String tests
		// We accept %hc, %c, %lc, %I8c, %I16c, %I32c (regular, regular, wide, char, char16_t, char32_t)
		// We accept %hC, %C, %lC, %I8C, %I16C, %I32C (regular, wide,    wide, char, char16_t, char32_t)
		// We accept %hs, %s, %ls, %I8s, %I16s, %I32s (regular, regular, wide, char, char16_t, char32_t)
		// We accept %hS, %S, %lS, %I8s, %I16s, %I32s (regular, wide,    wide, char, char16_t, char32_t)

		char16_t buffer[32];
		char  dStr8[2]  = { 'd', 0 };
		char16_t eStr16[2] = { 'e', 0 };
		char32_t fStr32[2] = { 'f', 0 };

		#if EASPRINTF_MS_STYLE_S_FORMAT // Microsoft style means that the meanings of S and s are reversed for non-char Sprintf.
			Sprintf(buffer, EA_CHAR16("%hc %c %lc %I8c %I16c %I32c"), 'a', EA_WCHAR('b'), EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR16("%hC %C %lC %I8C %I16C %I32C"), 'a', 'b',           EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR16("%hs %s %ls %I8s %I16s %I32s"), "a", EA_WCHAR("b"), EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR16("%hS %S %lS %I8S %I16S %I32S"), "a", "b",           EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("a b c d e f")) == 0);
		#else
			Sprintf(buffer, EA_CHAR16("%hc %c %lc %I8c %I16c %I32c"), 'a', 'b',           EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR16("%hC %C %lC %I8C %I16C %I32C"), 'a', EA_WCHAR('b'), EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("a b c d e f)") == 0);

			Sprintf(buffer, EA_CHAR16("%hs %s %ls %I8s %I16s %I32s"), "a", "b",           EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR16("%hS %S %lS %I8S %I16S %I32S"), "a", EA_WCHAR("b"), EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("a b c d e f")) == 0);
		#endif
	}

	{
		char16_t buffer[1024];
		int i;

		Sprintf(buffer, EA_CHAR16("e-style >= 1: \"%e\"\n"), 12.34);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("e-style >= 1: \"1.234000e+01\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR16("e-style >= .1: \"%e\"\n"), 0.1234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("e-style >= .1: \"1.234000e-01\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR16("e-style < .1: \"%e\"\n"), 0.001234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("e-style < .1: \"1.234000e-03\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR16("e-style big: \"%.60e\"\n"), 1e20);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("e-style big: \"1.000000000000000000000000000000000000000000000000000000000000e+20\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR16("e-style == .1: \"%e\"\n"), 0.1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("e-style == .1: \"1.000000e-01\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR16("f-style >= 1: \"%f\"\n"), 12.34);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("f-style >= 1: \"12.340000\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("f-style >= .1: \"%f\"\n"), 0.1234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("f-style >= .1: \"0.123400\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("f-style < .1: \"%f\"\n"), 0.001234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("f-style < .1: \"0.001234\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("g-style >= 1: \"%g\"\n"), 12.34);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("g-style >= 1: \"12.34\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("g-style >= .1: \"%g\"\n"), 0.1234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("g-style >= .1: \"0.1234\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("g-style < .1: \"%g\"\n"), 0.001234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("g-style < .1: \"0.001234\"\n")) == 0);

		Sprintf(buffer, EA_CHAR16("g-style big: \"%.60g\"\n"), 1e20);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("g-style big: \"100000000000000000000\"\n")) == 0);

		//Sprintf(buffer, EA_CHAR16("%#.4g\n"), 0.0); // The C99 committee has decided in a defect analysis that this is how it should work.
		//EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0\n")) == 0);

		Sprintf(buffer, EA_CHAR16(" %6.5f\n"), .099999999860301614);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16(" 0.10000\n")) == 0);

		Sprintf(buffer, EA_CHAR16(" %6.5f\n"), .1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16(" 0.10000\n")) == 0);

		Sprintf(buffer, EA_CHAR16("x%5.4fx\n"), .5);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("x0.5000x\n")) == 0);

		Sprintf(buffer, EA_CHAR16("%#03x\n"), 1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("0x1\n")) == 0);


		memset(buffer, '_', sizeof(buffer));
		Sprintf(buffer, EA_CHAR16("%.300f"), 1.0);
		EATEST_VERIFY((buffer[0] == '1') && (buffer[1] == '.'));
		for(i = 0; i < 300; i++)
		{
			if(buffer[2 + i] != '0')
				break;
		}
		if(i != 300)
			EATEST_VERIFY(i == 300);
		else
			EATEST_VERIFY(buffer[2 + 300] == EA_CHAR16('\0'));


		// Operations on FLT_MIN are undefined on Neon
		double d = static_cast<double>(FLT_MIN);    // We are intentionally using FLT_MIN and not DBL_MIN.
		d /= 2.0;
		Sprintf(buffer, EA_CHAR16("%.17e"), d);     // It should be something like 5.87747175411143___e-39 where count and values of the _ digits vary by hardware.
		buffer[16] = buffer[17] = buffer[18] = '_'; // Replace the uncertain digits with '_' characters, as they are system-dependent.
		
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("5.87747175411143___e-39")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.


		Sprintf(buffer, EA_CHAR16("%15.5e\n"), 4.9406564584124654e-307);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("   4.94066e-307\n")) == 0);
	}


	{
		char16_t        buffer[256];
		const char16_t* pExpected;

		// VC++ sprintf would fail these tests, as the Standard says to print no more 
		// than 2 unless necessary, yet VC++ sprintf prints 3 digits exponents.

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), 0.0, 0.0, 0.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("|      0.0000|  0.0000e+00|           0|")) == 0);

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), 1.0, 1.0, 1.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("|      1.0000|  1.0000e+00|           1|")) == 0);

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), -1.0, -1.0, -1.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("|     -1.0000| -1.0000e+00|          -1|")) == 0);

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), 100.0, 100.0, 100.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("|    100.0000|  1.0000e+02|         100|")) == 0);

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), 1000.0, 1000.0, 1000.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("|   1000.0000|  1.0000e+03|        1000|")) == 0);

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), 10000.0, 10000.0, 10000.0);
		EATEST_VERIFY(Strcmp(buffer,EA_CHAR16("|  10000.0000|  1.0000e+04|       1e+04|")) == 0);

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), 12346.0, 12346.0, 12346.0);
		pExpected = EA_CHAR16("|  12346.0000|  1.2346e+04|   1.235e+04|");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), 100000.0, 100000.0, 100000.0);
		pExpected = EA_CHAR16("| 100000.0000|  1.0000e+05|       1e+05|");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("|%12.4f|%12.4e|%12.4g|"), 123467.0, 123467.0, 123467.0);
		pExpected = EA_CHAR16("| 123467.0000|  1.2347e+05|   1.235e+05|");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 
	}


	{
		char16_t buffer[256];

		// Verify that snprintf follows the C99 convention of returning the number of characters
		// required. This is as opposed to the non-standard way that some libraries just return 
		// -1 if the buffer isn't big enough.

		const int kBuf1Capacity = 20;
		wchar_t   buf1[kBuf1Capacity];
		int       n1 = Snprintf(buf1, kBuf1Capacity, EA_WCHAR("%30I16s"), EA_CHAR16("foo"));
		Sprintf(buffer, EA_CHAR16("snprintf(\"%%30s\", \"foo\") == %d, \"%.*s\"\n"), n1, kBuf1Capacity, buf1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("snprintf(\"%30s\", \"foo\") == 30, \"                   \"\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		const int kBuf2Capacity = 512;
		char16_t  buf2[kBuf2Capacity];
		int       n2 = Snprintf(buf2, kBuf2Capacity, EA_CHAR16("%.1000u"), 10);
		Sprintf(buffer, EA_CHAR16("snprintf(\"%%.1000u\", 10) == %d\n"), n2);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("snprintf(\"%.1000u\", 10) == 1000\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		const int kBuf3Capacity = 512;
		char16_t  buf3[kBuf3Capacity];
		char16_t* pString = new char16_t[100000];
		memset(pString, '_', 100000 * sizeof(char16_t));
		pString[100000 - 1] = 0;
		int n3 = Snprintf(buf3, kBuf2Capacity, EA_CHAR16("%I16s"), pString);
		Sprintf(buffer, EA_CHAR16("snprintf(\"%%s\", pString) == %d\n"), n3);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("snprintf(\"%s\", pString) == 99999\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.
		delete[] pString;

		int n4 = Snprintf(NULL, 0, EA_CHAR16("%I16s"), EA_CHAR16("abc"));
		Sprintf(buffer, EA_CHAR16("snprintf(NULL, \"abc\") == %d\n"), n4);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("snprintf(NULL, \"abc\") == 3\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		int n5 = Snprintf(NULL, 100, EA_CHAR16("%I16s"), EA_CHAR16("abc"));
		Sprintf(buffer, EA_CHAR16("snprintf(NULL, \"abc\") == %d\n"), n5);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("snprintf(NULL, \"abc\") == 3\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.
	}


	{
		char16_t buffer[16][256];

		int n = 0, i, j, k, m;

		for(i = 0; i < 2; i++)
		{
			for(j = 0; j < 2; j++)
			{
				for(k = 0; k < 2; k++)
				{
					for(m = 0; m < 2; m++)
					{
						wchar_t  prefix[7];
						char16_t format[128];

						Strcpy(prefix, EA_WCHAR("%"));
						if(i == 0)
							Strcat(prefix, EA_WCHAR("-"));
						if(j == 0)
							Strcat(prefix, EA_WCHAR("+"));
						if(k == 0)
							Strcat(prefix, EA_WCHAR("#"));
						if(m == 0)
							Strcat(prefix, EA_WCHAR("0"));

						#define DEC -123
						#define INT  255
						#define UNS (~0)

						Sprintf(format, EA_CHAR16("%%5s |%s6d |%s6o |%s6x |%s6X |%s6u |"), prefix, prefix, prefix, prefix, prefix);
						Sprintf(buffer[n], format, prefix, DEC, INT, INT, INT, UNS);
						n++;
					}
				}
			}
		}

		EATEST_VERIFY(Strcmp(buffer[ 0], EA_CHAR16("%-+#0 |-123   |0377   |0xff   |0XFF   |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 1], EA_CHAR16(" %-+# |-123   |0377   |0xff   |0XFF   |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 2], EA_CHAR16(" %-+0 |-123   |377    |ff     |FF     |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 3], EA_CHAR16("  %-+ |-123   |377    |ff     |FF     |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 4], EA_CHAR16(" %-#0 |-123   |0377   |0xff   |0XFF   |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 5], EA_CHAR16("  %-# |-123   |0377   |0xff   |0XFF   |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 6], EA_CHAR16("  %-0 |-123   |377    |ff     |FF     |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 7], EA_CHAR16("   %- |-123   |377    |ff     |FF     |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 8], EA_CHAR16(" %+#0 |-00123 |000377 |0x00ff |0X00FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 9], EA_CHAR16("  %+# |  -123 |  0377 |  0xff |  0XFF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[10], EA_CHAR16("  %+0 |-00123 |000377 |0000ff |0000FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[11], EA_CHAR16("   %+ |  -123 |   377 |    ff |    FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[12], EA_CHAR16("  %#0 |-00123 |000377 |0x00ff |0X00FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[13], EA_CHAR16("   %# |  -123 |  0377 |  0xff |  0XFF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[14], EA_CHAR16("   %0 |-00123 |000377 |0000ff |0000FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[15], EA_CHAR16("    % |  -123 |   377 |    ff |    FF |4294967295 |")) == 0);

	}


	{
		char16_t        buffer[256];
		const char16_t* pExpected;

		Sprintf(buffer, EA_CHAR16("%e"), 1234567.8); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR16("1.234568e+06"); 
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("%f"), 1234567.8);
		pExpected = EA_CHAR16("1234567.800000");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("%g"), 1234567.8); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR16("1.23457e+06");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("%g"), 123.456);
		pExpected = EA_CHAR16("123.456");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("%g"), 1000000.0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR16("1e+06"); 
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("%g"), 10.0);
		pExpected = EA_CHAR16("10");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("%g"), 0.02);
		pExpected = EA_CHAR16("0.02");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 
	}


	{   // Test the ' extension, that prints numbers with a thousands separator.
		char16_t        buffer[64];
		const char16_t* pExpected;

		Sprintf(buffer, EA_CHAR16("%'u"), 123456789);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("123,456,789")) == 0);

		Sprintf(buffer, EA_CHAR16("%'d"), -123456789);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("-123,456,789")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I8u"), 123);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("123")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I16u"), 12345);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("12,345")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I16d"), -12345);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("-12,345")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I32u"), 12345678);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("12,345,678")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I32d"), -12345678);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("-12,345,678")) == 0);

		Sprintf(buffer, EA_CHAR16("%20I32d"), -12345678);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("           -12345678")) == 0);

		Sprintf(buffer, EA_CHAR16("%'20I32d"), -12345678); // Verify that the , chars count towards the field width.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("         -12,345,678")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I32x"), 0x12345678);  // ' has no effect on hex formatting.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("12345678")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I64u"), UINT64_C(1234999995678));
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("1,234,999,995,678")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I64d"), INT64_C(-1234599999678));
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("-1,234,599,999,678")) == 0);

		Sprintf(buffer, EA_CHAR16("%'I64x"), UINT64_C(0x1234567812345678));  // ' has no effect on hex formatting.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("1234567812345678")) == 0);

		Sprintf(buffer, EA_CHAR16("%'f"), 123456.234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("123,456.234000")) == 0);

		Sprintf(buffer, EA_CHAR16("%'e"), 1234567.8);  // ' has no effect on %e formatting.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("1.234568e+06")) == 0);

		Sprintf(buffer, EA_CHAR16("%'g"), 1234.54);   // In some cases %g acts like %f.
		pExpected = EA_CHAR16("1,234.54");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("%'g"), 1234567.8);   // In some cases %g acts like %f.
		pExpected = EA_CHAR16("1.23457e+06");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 
	}

	{
		char16_t buffer[256];

		Sprintf(buffer, EA_CHAR16("%hhu"), UCHAR_MAX + 2);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("1")) == 0); // VC++ fails this, as it doesn't implement the C99 standard %hh modifier.

		Sprintf(buffer, EA_CHAR16("%hu"), USHRT_MAX + 2);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("1")) == 0);
	}


	{
		char16_t        buffer[128];
		const char16_t* pExpected;

		Sprintf(buffer, EA_CHAR16("%5.s"), EA_WCHAR("xyz"));
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("     ")) == 0);

		Sprintf(buffer, EA_CHAR16("%5.f"), 33.3);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("   33")) == 0);

		Sprintf(buffer, EA_CHAR16("%8.e"), 33.3e7);                 // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("   3e+08")) == 0);

		Sprintf(buffer, EA_CHAR16("%8.E"), 33.3e7);                 // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("   3E+08")) == 0); 

		Sprintf(buffer, EA_CHAR16("%.g"), 33.3);                    // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR16("3e+01"); 
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR16("%.G"), 33.3);                    // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR16("3E+01"); 
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I16s\n   Actual:   %I16s", pExpected, buffer); 
	}


	{
		char16_t buffer[128];
		int      precision;

		precision = 0;
		Sprintf(buffer, EA_CHAR16("%.*g"), precision, 3.3);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("3")) == 0);

		precision = 0;
		Sprintf(buffer, EA_CHAR16("%.*G"), precision, 3.3);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("3")) == 0);

		precision = 0;
		Sprintf(buffer, EA_CHAR16("%7.*G"), precision, 3.33);
		EATEST_VERIFY(Strcmp(buffer,EA_CHAR16("      3")) == 0);

		precision = 3;
		Sprintf(buffer, EA_CHAR16("%04.*o"), precision, 33);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16(" 041")) == 0);

		precision = 7;
		Sprintf(buffer, EA_CHAR16("%09.*u"), precision, 33);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16("  0000033")) == 0);

		precision = 3;
		Sprintf(buffer, EA_CHAR16("%04.*x"), precision, 33);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16(" 021")) == 0);

		precision = 3;
		Sprintf(buffer, EA_CHAR16("%04.*X"), precision, 33);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR16(" 021")) == 0);
	}

	{
		static const int kSourceSize = 1024 * 5;
		static const int kOutputSize = kSourceSize + 100;
		char value[kSourceSize];
		char16_t destination[kOutputSize];
		char16_t comparison[kOutputSize];

		for(int i = 0; i < kSourceSize - 1; ++i)
		{
			value[i] = '0' + (i % 10);
			comparison[i] = '0' + (i % 10);
		}
		value[kSourceSize - 1] = 0;
		comparison[kSourceSize - 1] = 0;

		EA::StdC::Snprintf(destination, kOutputSize, EA_CHAR16("%I8s"), value);
		EATEST_VERIFY(Strcmp(destination, comparison) == 0);

		EA::StdC::Snprintf(destination, kOutputSize, EA_CHAR16("%.10I8s"), value);
		EATEST_VERIFY(Strcmp(destination, EA_CHAR16("0123456789")) == 0);
	}

	return nErrorCount;
}


static int TestSprintf32(int unused = 0, ...)
{
	using namespace EA::StdC;

	int nErrorCount(0);

	// int Snprintf(char_t* pDestination, size_t n, const char_t* pFormat, ...);
	{
		char32_t sn18[32];
		Snprintf(sn18, EAArrayCount(sn18), EA_CHAR32("%5s%-4d%03i"), EA_WCHAR("abc"), -12, 3);
		EATEST_VERIFY(!Strcmp(EA_CHAR32("  abc-12 003"), sn18));
		Snprintf(sn18, EAArrayCount(sn18), EA_CHAR32("%.2f"), 3.1415);
		EATEST_VERIFY(!Strcmp(EA_CHAR32("3.14"), sn18));
	}

	// int Vsnprintf(char_t* pDestination, size_t n, const char_t* pFormat, ...);
	{
		char32_t sn18[32];
		TestCRTVsnprintf(sn18, EAArrayCount(sn18), EA_CHAR32("%5s%-5d%04i"), EA_WCHAR("abc"), -12, 3);
		EATEST_VERIFY(!Strcmp(EA_CHAR32("  abc-12  0003"), sn18));
		TestCRTVsnprintf(sn18, EAArrayCount(sn18), EA_CHAR32("%.2f"), 3.1415);
		EATEST_VERIFY(!Strcmp(EA_CHAR32("3.14"), sn18));
	}

	#if EASTDC_VSNPRINTF8_ENABLED
		{
			char32_t sn18[32];
			TestCRTVsnprintf32(sn18, EAArrayCount(sn18), EA_CHAR32("%5s%-5d%04i"), EA_WCHAR("abc"), -12, 3);
			EATEST_VERIFY(!Strcmp(EA_CHAR32("  abc-12  0003"), sn18));
			TestCRTVsnprintf32(sn18, EAArrayCount(sn18), EA_CHAR32("%.2f"), 3.1415);
			EATEST_VERIFY(!Strcmp(EA_CHAR32("3.14"), sn18));
		}
	#endif

	// int Vscprintf(const char_t* pFormat, va_list arguments);
	{
		va_list arguments;
		va_start(arguments, unused);

		int result = Vscprintf(EA_CHAR32("abc"), arguments);       
		EATEST_VERIFY(result == 3);

		va_end(arguments);
	}

	// template <typename String>
	// int StringVcprintf(String& s, const char* EA_RESTRICT pFormat, va_list arguments)
	{
		va_list arguments;
		va_start(arguments, unused);

		eastl::string32 s32;
		int result = StringVcprintf(s32, EA_CHAR32("hello"), arguments);
		EATEST_VERIFY((result == 5) && (s32 == EA_CHAR32("hello")));

		va_end(arguments);
	}

	// template <typename String> 
	// int StringPrintf(String& s, const typename String::value_type* EA_RESTRICT pFormat, ...)
	{
		eastl::string32 s32;
		int result = StringPrintf(s32, EA_CHAR32("%s"), EA_WCHAR("hello"));
		EATEST_VERIFY((result == 5) && (s32 == EA_CHAR32("hello")));
	}

	{
		char32_t buffer[32];
		const int kHexValue = 0x12;

		Sprintf(buffer, EA_CHAR32("%.4x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0012")) == 0);

		Sprintf(buffer, EA_CHAR32("%04x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0012")) == 0);

		Sprintf(buffer, EA_CHAR32("%4.4x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0012")) == 0);

		Sprintf(buffer, EA_CHAR32("%04.4x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0012")) == 0);

		Sprintf(buffer, EA_CHAR32("%4.3x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32(" 012")) == 0);

		Sprintf(buffer, EA_CHAR32("%04.3x"), kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32(" 012")) == 0);

		Sprintf(buffer, EA_CHAR32("%.*x"), 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0012")) == 0);

		Sprintf(buffer, EA_CHAR32("%0*x"), 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0012")) == 0);

		Sprintf(buffer, EA_CHAR32("%*.*x"), 4, 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0012")) == 0);

		Sprintf(buffer, EA_CHAR32("%0*.*x"), 4, 4, kHexValue);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0012")) == 0);
	}


	{
		char32_t buffer[96];

		Sprintf(buffer, EA_CHAR32("decimal negative: \"%d\"\n"), -2345);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("decimal negative: \"-2345\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("octal negative: \"%o\"\n"), -2345);
		if(sizeof(int) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("octal negative: \"37777773327\"\n")) == 0);
		else if(sizeof(int) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("octal negative: \"1777777777777777773327\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("hex negative: \"%x\"\n"), -2345);
		if(sizeof(int) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("hex negative: \"fffff6d7\"\n")) == 0);
		else if(sizeof(int) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("hex negative: \"fffffffffffff6d7\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("long decimal number: \"%ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("long decimal number: \"-123456\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("long octal negative: \"%lo\"\n"), -2345L);
		if(sizeof(long) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("long octal negative: \"37777773327\"\n")) == 0);
		else if(sizeof(long) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("long octal negative: \"1777777777777777773327\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("long unsigned decimal number: \"%lu\"\n"), -123456L);
		if(sizeof(long) == (4 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("long unsigned decimal number: \"4294843840\"\n")) == 0);
		else if(sizeof(long) == (8 + (__FILE__[0] / 100000))) // Trickery here to avoid compiler warnings.
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("long unsigned decimal number: \"18446744073709428160\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("zero-padded LDN: \"%010ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("zero-padded LDN: \"-000123456\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("left-adjusted ZLDN: \"%-010ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("left-adjusted ZLDN: \"-123456   \"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("space-padded LDN: \"%10ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("space-padded LDN: \"   -123456\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("left-adjusted SLDN: \"%-10ld\"\n"), -123456L);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("left-adjusted SLDN: \"-123456   \"\n")) == 0);
	}


	{
		char32_t  buffer[1024];
		wchar_t   str1[64]; Strlcpy(str1, EA_WCHAR("abc de"), EAArrayCount(str1)); // Can't do str1[64] = EA_CHAR32("abc de") because some compilers don't support 32 bit string literals.
		wchar_t   str2[64]; Strlcpy(str2, EA_WCHAR("abd def ghi jkl mno pqr stu vwz yz."), EAArrayCount(str2));

		// The C99 standard specifies that leading zeros only put zeroes in front of numerical types. Spaces for others.
		Sprintf(buffer, EA_CHAR32("zero-padded string: \"%010s\"\n"), str1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("zero-padded string: \"    abc de\"\n")) == 0); // VC++ fails this, as it puts zeroes in front.

		Sprintf(buffer, EA_CHAR32("left-adjusted Z string: \"%-010s\"\n"), str1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("left-adjusted Z string: \"abc de    \"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("space-padded string: \"%10s\"\n"), str1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("space-padded string: \"    abc de\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("left-adjusted S string: \"%-10s\"\n"), str1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("left-adjusted S string: \"abc de    \"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("limited string: \"%.22s\"\n"), str2);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("limited string: \"abd def ghi jkl mno pq\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("null string: \"%s\"\n"), (wchar_t*)NULL);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("null string: \"(null)\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("%10s\n"), (wchar_t*)NULL);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("    (null)\n")) == 0);

		Sprintf(buffer, EA_CHAR32("%-10s\n"), (wchar_t*)NULL);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("(null)    \n")) == 0);

		Sprintf(buffer, EA_CHAR32("%*s%*s%*s"), -1, EA_WCHAR("one"), -20, EA_WCHAR("two"), -30, EA_WCHAR("three"));
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("onetwo                 three                         ")) == 0);

		int i;
		memset(buffer, '_', sizeof(buffer));
		Sprintf(buffer, EA_CHAR32("x%1000ls"), EA_WCHAR(" "));
		EATEST_VERIFY(buffer[0] == 'x');
		for(i = 0; i < 1000; i++)
		{
			if(buffer[1 + i] != ' ')
				break;
		}
		if(i != 1000)
			EATEST_VERIFY(i == 1000);
		else
			EATEST_VERIFY(buffer[1 + 1000] == 0);
	}


	{   // String tests
		// We accept %hc, %c, %lc, %I8c, %I16c, %I32c (regular, regular, wide, char, char16_t, char32_t)
		// We accept %hC, %C, %lC, %I8C, %I16C, %I32C (regular, wide,    wide, char, char16_t, char32_t)
		// We accept %hs, %s, %ls, %I8s, %I16s, %I32s (regular, regular, wide, char, char16_t, char32_t)
		// We accept %hS, %S, %lS, %I8s, %I16s, %I32s (regular, wide,    wide, char, char16_t, char32_t)

		char32_t buffer[32];
		char  dStr8[2]  = { 'd', 0 };
		char16_t eStr16[2] = { 'e', 0 };
		char32_t fStr32[2] = { 'f', 0 };

		#if EASPRINTF_MS_STYLE_S_FORMAT // Microsoft style means that the meanings of S and s are reversed for non-char Sprintf.
			Sprintf(buffer, EA_CHAR32("%hc %c %lc %I8c %I16c %I32c"), 'a', EA_WCHAR('b'), EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR32("%hC %C %lC %I8C %I16C %I32C"), 'a', 'b',           EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR32("%hs %s %ls %I8s %I16s %I32s"), "a", EA_WCHAR("b"), EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR32("%hS %S %lS %I8S %I16S %I32S"), "a", "b",           EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("a b c d e f")) == 0);
		#else
			Sprintf(buffer, EA_CHAR32("%hc %c %lc %I8c %I16c %I32c"), 'a', 'b',           EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR32("%hC %C %lC %I8C %I16C %I32C"), 'a', EA_WCHAR('b'), EA_WCHAR('c'), (char)'d', (char16_t)'e', (char32_t)'f');
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("a b c d e f)") == 0);

			Sprintf(buffer, EA_CHAR32("%hs %s %ls %I8s %I16s %I32s"), "a", "b",           EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("a b c d e f")) == 0);

			Sprintf(buffer, EA_CHAR32("%hS %S %lS %I8S %I16S %I32S"), "a", EA_WCHAR("b"), EA_WCHAR("c"), dStr8,        eStr16,         fStr32);
			EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("a b c d e f")) == 0);
		#endif
	}

	{
		char32_t buffer[384];
		int i;

		Sprintf(buffer, EA_CHAR32("e-style >= 1: \"%e\"\n"), 12.34);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("e-style >= 1: \"1.234000e+01\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR32("e-style >= .1: \"%e\"\n"), 0.1234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("e-style >= .1: \"1.234000e-01\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR32("e-style < .1: \"%e\"\n"), 0.001234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("e-style < .1: \"1.234000e-03\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR32("e-style big: \"%.60e\"\n"), 1e20);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("e-style big: \"1.000000000000000000000000000000000000000000000000000000000000e+20\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR32("e-style == .1: \"%e\"\n"), 0.1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("e-style == .1: \"1.000000e-01\"\n")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR32("f-style >= 1: \"%f\"\n"), 12.34);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("f-style >= 1: \"12.340000\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("f-style >= .1: \"%f\"\n"), 0.1234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("f-style >= .1: \"0.123400\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("f-style < .1: \"%f\"\n"), 0.001234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("f-style < .1: \"0.001234\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("g-style >= 1: \"%g\"\n"), 12.34);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("g-style >= 1: \"12.34\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("g-style >= .1: \"%g\"\n"), 0.1234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("g-style >= .1: \"0.1234\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("g-style < .1: \"%g\"\n"), 0.001234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("g-style < .1: \"0.001234\"\n")) == 0);

		Sprintf(buffer, EA_CHAR32("g-style big: \"%.60g\"\n"), 1e20);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("g-style big: \"100000000000000000000\"\n")) == 0);

		//Sprintf(buffer, EA_CHAR32("%#.4g\n"), 0.0); // The C99 committee has decided in a defect analysis that this is how it should work.
		//EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0\n")) == 0);

		Sprintf(buffer, EA_CHAR32(" %6.5f\n"), .099999999860301614);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32(" 0.10000\n")) == 0);

		Sprintf(buffer, EA_CHAR32(" %6.5f\n"), .1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32(" 0.10000\n")) == 0);

		Sprintf(buffer, EA_CHAR32("x%5.4fx\n"), .5);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("x0.5000x\n")) == 0);

		Sprintf(buffer, EA_CHAR32("%#03x\n"), 1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("0x1\n")) == 0);


		memset(buffer, '_', sizeof(buffer));
		Sprintf(buffer, EA_CHAR32("%.300f"), 1.0);
		EATEST_VERIFY((buffer[0] == '1') && (buffer[1] == '.'));
		for(i = 0; i < 300; i++)
		{
			if(buffer[2 + i] != '0')
				break;
		}
		if(i != 300)
			EATEST_VERIFY(i == 300);
		else
			EATEST_VERIFY(buffer[2 + 300] == 0);


		double d = static_cast<double>(FLT_MIN);    // We are intentionally using FLT_MIN and not DBL_MIN.
		d /= 2.0;
		Sprintf(buffer, EA_CHAR32("%.17e"), d);     // It should be something like 5.87747175411143___e-39 where count and values of the _ digits vary by hardware.
		buffer[16] = buffer[17] = buffer[18] = '_'; // Replace the uncertain digits with '_' characters, as they are system-dependent.
		
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("5.87747175411143___e-39")) == 0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.

		Sprintf(buffer, EA_CHAR32("%15.5e\n"), 4.9406564584124654e-307);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("   4.94066e-307\n")) == 0);
	}


	{
		char32_t        buffer[96];
		const char32_t* pExpected;

		// VC++ sprintf would fail these tests, as the Standard says to print no more 
		// than 2 unless necessary, yet VC++ sprintf prints 3 digits exponents.

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), 0.0, 0.0, 0.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("|      0.0000|  0.0000e+00|           0|")) == 0);

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), 1.0, 1.0, 1.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("|      1.0000|  1.0000e+00|           1|")) == 0);

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), -1.0, -1.0, -1.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("|     -1.0000| -1.0000e+00|          -1|")) == 0);

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), 100.0, 100.0, 100.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("|    100.0000|  1.0000e+02|         100|")) == 0);

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), 1000.0, 1000.0, 1000.0);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("|   1000.0000|  1.0000e+03|        1000|")) == 0);

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), 10000.0, 10000.0, 10000.0);
		EATEST_VERIFY(Strcmp(buffer,EA_CHAR32("|  10000.0000|  1.0000e+04|       1e+04|")) == 0);

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), 12346.0, 12346.0, 12346.0);
		pExpected = EA_CHAR32("|  12346.0000|  1.2346e+04|   1.235e+04|");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), 100000.0, 100000.0, 100000.0);
		pExpected = EA_CHAR32("| 100000.0000|  1.0000e+05|       1e+05|");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("|%12.4f|%12.4e|%12.4g|"), 123467.0, 123467.0, 123467.0);
		pExpected = EA_CHAR32("| 123467.0000|  1.2347e+05|   1.235e+05|");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 
	}


	{
		char32_t buffer[96];

		// Verify that snprintf follows the C99 convention of returning the number of characters
		// required. This is as opposed to the non-standard way that some libraries just return 
		// -1 if the buffer isn't big enough.

		const int kBuf1Capacity = 20;
		wchar_t   buf1[kBuf1Capacity];
		int       n1 = Snprintf(buf1, kBuf1Capacity, EA_WCHAR("%30I32s"), EA_CHAR32("foo"));
		Sprintf(buffer, EA_CHAR32("snprintf(\"%%30s\", \"foo\") == %d, \"%.*s\"\n"), n1, kBuf1Capacity, buf1);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("snprintf(\"%30s\", \"foo\") == 30, \"                   \"\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		const int kBuf2Capacity = 512;
		char32_t  buf2[kBuf2Capacity];
		int       n2 = Snprintf(buf2, kBuf2Capacity, EA_CHAR32("%.1000u"), 10);
		Sprintf(buffer, EA_CHAR32("snprintf(\"%%.1000u\", 10) == %d\n"), n2);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("snprintf(\"%.1000u\", 10) == 1000\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		const int kBuf3Capacity = 512;
		char32_t  buf3[kBuf3Capacity];
		char32_t* pString = new char32_t[100000];
		memset(pString, '_', 100000 * sizeof(char32_t));
		pString[100000 - 1] = 0;
		int n3 = Snprintf(buf3, kBuf2Capacity, EA_CHAR32("%I32s"), pString);
		Sprintf(buffer, EA_CHAR32("snprintf(\"%%s\", pString) == %d\n"), n3);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("snprintf(\"%s\", pString) == 99999\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.
		delete[] pString;

		int n4 = Snprintf(NULL, 0, EA_CHAR32("%I32s"), EA_CHAR32("abc"));
		Sprintf(buffer, EA_CHAR32("snprintf(NULL, \"abc\") == %d\n"), n4);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("snprintf(NULL, \"abc\") == 3\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.

		int n5 = Snprintf(NULL, 100, EA_CHAR32("%I32s"), EA_CHAR32("abc"));
		Sprintf(buffer, EA_CHAR32("snprintf(NULL, \"abc\") == %d\n"), n5);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("snprintf(NULL, \"abc\") == 3\n")) == 0); // VC++ fails this, as it's version of snprintf doesn't use C99 standard snprintf return value conventions.
	}


	{
		char32_t buffer[16][64];

		int n = 0, i, j, k, m;

		for(i = 0; i < 2; i++)
		{
			for(j = 0; j < 2; j++)
			{
				for(k = 0; k < 2; k++)
				{
					for(m = 0; m < 2; m++)
					{
						wchar_t  prefix[7];
						char32_t format[96];

						Strcpy(prefix, EA_WCHAR("%"));
						if(i == 0)
							Strcat(prefix, EA_WCHAR("-"));
						if(j == 0)
							Strcat(prefix, EA_WCHAR("+"));
						if(k == 0)
							Strcat(prefix, EA_WCHAR("#"));
						if(m == 0)
							Strcat(prefix, EA_WCHAR("0"));

						#define DEC -123
						#define INT  255
						#define UNS (~0)

						Sprintf(format, EA_CHAR32("%%5s |%s6d |%s6o |%s6x |%s6X |%s6u |"), prefix, prefix, prefix, prefix, prefix);
						Sprintf(buffer[n], format, prefix, DEC, INT, INT, INT, UNS);
						n++;
					}
				}
			}
		}

		EATEST_VERIFY(Strcmp(buffer[ 0], EA_CHAR32("%-+#0 |-123   |0377   |0xff   |0XFF   |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 1], EA_CHAR32(" %-+# |-123   |0377   |0xff   |0XFF   |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 2], EA_CHAR32(" %-+0 |-123   |377    |ff     |FF     |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 3], EA_CHAR32("  %-+ |-123   |377    |ff     |FF     |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 4], EA_CHAR32(" %-#0 |-123   |0377   |0xff   |0XFF   |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 5], EA_CHAR32("  %-# |-123   |0377   |0xff   |0XFF   |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 6], EA_CHAR32("  %-0 |-123   |377    |ff     |FF     |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 7], EA_CHAR32("   %- |-123   |377    |ff     |FF     |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 8], EA_CHAR32(" %+#0 |-00123 |000377 |0x00ff |0X00FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[ 9], EA_CHAR32("  %+# |  -123 |  0377 |  0xff |  0XFF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[10], EA_CHAR32("  %+0 |-00123 |000377 |0000ff |0000FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[11], EA_CHAR32("   %+ |  -123 |   377 |    ff |    FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[12], EA_CHAR32("  %#0 |-00123 |000377 |0x00ff |0X00FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[13], EA_CHAR32("   %# |  -123 |  0377 |  0xff |  0XFF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[14], EA_CHAR32("   %0 |-00123 |000377 |0000ff |0000FF |4294967295 |")) == 0);
		EATEST_VERIFY(Strcmp(buffer[15], EA_CHAR32("    % |  -123 |   377 |    ff |    FF |4294967295 |")) == 0);

	}


	{
		char32_t        buffer[256];
		const char32_t* pExpected;

		Sprintf(buffer, EA_CHAR32("%e"), 1234567.8); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR32("1.234568e+06"); 
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%f"), 1234567.8);
		pExpected = EA_CHAR32("1234567.800000");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%g"), 1234567.8); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR32("1.23457e+06");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%g"), 123.456);
		pExpected = EA_CHAR32("123.456");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%g"), 1000000.0); // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR32("1e+06"); 
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%g"), 10.0);
		pExpected = EA_CHAR32("10");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%g"), 0.02);
		pExpected = EA_CHAR32("0.02");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 
	}


	{   // Test the ' extension, which cases numbers to be printed with a thousands separator.
		char32_t        buffer[64];
		const char32_t* pExpected;

		Sprintf(buffer, EA_CHAR32("%'u"), 123456789);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("123,456,789")) == 0);

		Sprintf(buffer, EA_CHAR32("%'d"), -123456789);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("-123,456,789")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I8u"), 123);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("123")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I16u"), 12345);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("12,345")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I16d"), -12345);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("-12,345")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I32u"), 12345678);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("12,345,678")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I32d"), -12345678);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("-12,345,678")) == 0);

		Sprintf(buffer, EA_CHAR32("%20I32d"), -12345678);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("           -12345678")) == 0);

		Sprintf(buffer, EA_CHAR32("%'20I32d"), -12345678); // Verify that the , chars count towards the field width.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("         -12,345,678")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I32x"), 0x12345678);  // ' has no effect on hex formatting.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("12345678")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I64u"), UINT64_C(1234999995678));
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("1,234,999,995,678")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I64d"), INT64_C(-1234599999678));
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("-1,234,599,999,678")) == 0);

		Sprintf(buffer, EA_CHAR32("%'I64x"), UINT64_C(0x1234567812345678));  // ' has no effect on hex formatting.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("1234567812345678")) == 0);

		Sprintf(buffer, EA_CHAR32("%'f"), 123456.234);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("123,456.234000")) == 0);

		Sprintf(buffer, EA_CHAR32("%'e"), 1234567.8);   // ' has no effect on %e formatting.
		pExpected = EA_CHAR32("1.234568e+06");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%'g"), 1234.54);     // In some cases %g acts like %f.
		pExpected = EA_CHAR32("1,234.54");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%'g"), 1234567.8);   // In some cases %g acts like %f.
		pExpected = EA_CHAR32("1.23457e+06");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 
	}

	{
		char32_t buffer[256];

		Sprintf(buffer, EA_CHAR32("%hhu"), UCHAR_MAX + 2);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("1")) == 0); // VC++ fails this, as it doesn't implement the C99 standard %hh modifier.

		Sprintf(buffer, EA_CHAR32("%hu"), USHRT_MAX + 2);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("1")) == 0);
	}


	{
		char32_t        buffer[128];
		const char32_t* pExpected;

		Sprintf(buffer, EA_CHAR32("%5.s"), EA_WCHAR("xyz"));
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("     ")) == 0);

		Sprintf(buffer, EA_CHAR32("%5.f"), 33.3);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("   33")) == 0);

		Sprintf(buffer, EA_CHAR32("%8.e"), 33.3e7);                 // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("   3e+08")) == 0); 

		Sprintf(buffer, EA_CHAR32("%8.E"), 33.3e7);                 // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("   3E+08")) == 0);

		Sprintf(buffer, EA_CHAR32("%.g"), 33.3);                    // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR32("3e+01");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 

		Sprintf(buffer, EA_CHAR32("%.G"), 33.3);                    // VC++ sprintf would fail this, as it uses 3 exponent digits, but the Standard says to print no more than 2 unless necessary.
		pExpected = EA_CHAR32("3E+01");
		EATEST_VERIFY_F(Strcmp(buffer, pExpected) == 0, "\n   Expected: %I32s\n   Actual:   %I32s", pExpected, buffer); 
	}


	{
		char32_t buffer[128];
		int      precision;

		precision = 0;
		Sprintf(buffer, EA_CHAR32("%.*g"), precision, 3.3);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("3")) == 0);

		precision = 0;
		Sprintf(buffer, EA_CHAR32("%.*G"), precision, 3.3);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("3")) == 0);

		precision = 0;
		Sprintf(buffer, EA_CHAR32("%7.*G"), precision, 3.33);
		EATEST_VERIFY(Strcmp(buffer,EA_CHAR32("      3")) == 0);

		precision = 3;
		Sprintf(buffer, EA_CHAR32("%04.*o"), precision, 33);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32(" 041")) == 0);

		precision = 7;
		Sprintf(buffer, EA_CHAR32("%09.*u"), precision, 33);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32("  0000033")) == 0);

		precision = 3;
		Sprintf(buffer, EA_CHAR32("%04.*x"), precision, 33);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32(" 021")) == 0);

		precision = 3;
		Sprintf(buffer, EA_CHAR32("%04.*X"), precision, 33);
		EATEST_VERIFY(Strcmp(buffer, EA_CHAR32(" 021")) == 0);
	}

	{
		static const int kSourceSize = 1024 * 5;
		static const int kOutputSize = kSourceSize + 100;
		char value[kSourceSize];
		char32_t destination[kOutputSize];
		char32_t comparison[kOutputSize];

		for(int i = 0; i < kSourceSize - 1; ++i)
		{
			value[i] = '0' + (i % 10);
			comparison[i] = '0' + (i % 10);
		}
		value[kSourceSize - 1] = 0;
		comparison[kSourceSize - 1] = 0;

		EA::StdC::Snprintf(destination, kOutputSize, EA_CHAR32("%I8s"), value);
		EATEST_VERIFY(Strcmp(destination, comparison) == 0);

		EA::StdC::Snprintf(destination, kOutputSize, EA_CHAR32("%.10I8s"), value);
		EATEST_VERIFY(Strcmp(destination, EA_CHAR32("0123456789")) == 0);
	}

	return nErrorCount;
}


static int TestDrintf8()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{
		int result;

		Dprintf("Begin Dprintf (debug output printf) testing...\n");

		// EASTDC_API int Vdprintf(const char* EA_RESTRICT pFormat, va_list arguments);
		// EASTDC_API int Dprintf(const char* EA_RESTRICT pFormat, ...);
		eastl::string8 sBuffer;
		for(eastl_size_t i = 0; i < 1024; i++) // This size should be > than the size of the buffer(s) used in PlatformLogWriter8, though those buffer sizes aren't publicly exposed.
			sBuffer.push_back('a' + (char)(i % 26));

		EA::UnitTest::Rand rand((uint32_t)EA::StdC::GetTime());

		for(eastl_size_t i = 0; i < 1024; i += rand.RandRange(1, 100))
		{
			char formatBuffer[32]; // Something like "%.52s"
			Sprintf(formatBuffer, "%%.%ds\n", i); // Use \n because some debug output systems might otherwise get overwhelmed.
			result = Dprintf(formatBuffer, sBuffer.c_str());
			EATEST_VERIFY(result > 0);
		}

		#if EASTDC_PRINTF_DEBUG_ENABLED
			// EASTDC_API int Fprintf(FILE* EA_RESTRICT pFile, const char* EA_RESTRICT pFormat, ...);
			// EASTDC_API int Printf(const char* EA_RESTRICT pFormat, ...);
			// EASTDC_API int Vfprintf(FILE* EA_RESTRICT pFile, const char* EA_RESTRICT pFormat, va_list arguments);
			// EASTDC_API int Vprintf(const char* EA_RESTRICT pFormat, va_list arguments);
			result = Printf("%s", "Printf test (EASTDC_PRINTF_DEBUG_ENABLED).\n");
			EATEST_VERIFY(result > 0);

			result = Fprintf(stdout, "%s", "Printf test (EASTDC_PRINTF_DEBUG_ENABLED).\n");
			EATEST_VERIFY(result > 0);
		#endif

		Dprintf("End Dprintf (debug output printf) testing.\n");
	}

	return nErrorCount;
}


static int TestOsprintf8()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{
		char buffer[32];
		int     result;

		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, "%0:d", (int)0);
		EATEST_VERIFY((result == 1) && Strcmp(buffer, "0") == 0);

		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, "%2:1.0f %3:d %1:c", (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && Strcmp(buffer, "1 2 3") == 0);

		// Test 0-based ordering
		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, "%1:1.0f %2:d %0:c", (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && Strcmp(buffer, "1 2 3") == 0);

		// Test format limit (currently 21 spans)
		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, " %0:d %1:d %2:d %3:d %4:d %5:d %6:d %7:d %8:d %9:d        ", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
		EATEST_VERIFY((result == 28) && Strcmp(buffer, " 0 1 2 3 4 5 6 7 8 9        " ) == 0);

		// Test format overflow
		// Tests below are disabled by default because they trigger runtime asserts which break auto-testing.
		// They can be enabled by running these tests in interactive mode, in which case you'll have to manually
		// dismiss assertion failures for these.
		if(EA::UnitTest::GetInteractive())
		{
			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, " %0:d %1:d %2:d %3:d %4:d %5:d %6:d %7:d %8:d %9:d %9:d ", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, "%00000000000000000000:f", 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, "%0:000000000000000000f", 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, "%0:.000000000000000000f", 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, "%000000000000000000:000000000000000000.000000000000000000f", 0.f);
			EATEST_VERIFY(result == -1);
		}

		// Test OVsnprintf capacity limits
		memset(buffer, 0, sizeof(buffer));
		result = OSnprintf(buffer, 0, "%2:1.0f %3:d %1:c", (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && (buffer[0] == 0));
	}

	return nErrorCount;
}


static int TestOsprintf16()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{
		char16_t buffer[128];
		int      result;

		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, EA_CHAR16("%0:d"), (int)0);
		EATEST_VERIFY((result == 1) && Strcmp(buffer, EA_CHAR16("0")) == 0);

		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, EA_CHAR16("%2:1.0f %3:d %1:c"), (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && Strcmp(buffer, EA_CHAR16("1 2 3")) == 0);

		// Test 0-based ordering
		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, EA_CHAR16("%1:1.0f %2:d %0:c"), (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && Strcmp(buffer, EA_CHAR16("1 2 3")) == 0);

		// Test format limit (currently 21 spans)
		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, EA_CHAR16(" %0:d %1:d %2:d %3:d %4:d %5:d %6:d %7:d %8:d %9:d        "), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
		EATEST_VERIFY((result == 28) && Strcmp(buffer, EA_CHAR16(" 0 1 2 3 4 5 6 7 8 9        ")) == 0);

		// Test format overflow
		// Tests below are disabled by default because they trigger runtime asserts which break auto-testing.
		// They can be enabled by running these tests in interactive mode, in which case you'll have to manually
		// dismiss assertion failures for these.
		if(EA::UnitTest::GetInteractive())
		{
			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR16(" %0:d %1:d %2:d %3:d %4:d %5:d %6:d %7:d %8:d %9:d %9:d "), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR16("%00000000000000000000:f"), 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR16("%0:000000000000000000f"), 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR16("%0:.000000000000000000f"), 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR16("%000000000000000000:000000000000000000.000000000000000000f"), 0.f);
			EATEST_VERIFY(result == -1);
		}

		// Test OVsnprintf capacity limits
		memset(buffer, 0, sizeof(buffer));
		result = OSnprintf(buffer, 0, EA_CHAR16("%2:1.0f %3:d %1:c"), (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && (buffer[0] == 0));
	}

	return nErrorCount;
}


static int TestOsprintf32()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{
		char32_t buffer[128];
		int      result;

		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, EA_CHAR32("%0:d"), (int)0);
		EATEST_VERIFY((result == 1) && Strcmp(buffer, EA_CHAR32("0")) == 0);

		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, EA_CHAR32("%2:1.0f %3:d %1:c"), (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && Strcmp(buffer, EA_CHAR32("1 2 3")) == 0);

		// Test 0-based ordering
		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, EA_CHAR32("%1:1.0f %2:d %0:c"), (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && Strcmp(buffer, EA_CHAR32("1 2 3")) == 0);

		// Test format limit (currently 21 spans)
		memset(buffer, 0, sizeof(buffer));
		result = OSprintf(buffer, EA_CHAR32(" %0:d %1:d %2:d %3:d %4:d %5:d %6:d %7:d %8:d %9:d        "), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
		EATEST_VERIFY((result == 28) && Strcmp(buffer, EA_CHAR32(" 0 1 2 3 4 5 6 7 8 9        ")) == 0);

		// Test format overflow
		// Tests below are disabled by default because they trigger runtime asserts which break auto-testing.
		// They can be enabled by running these tests in interactive mode, in which case you'll have to manually
		// dismiss assertion failures for these.
		if(EA::UnitTest::GetInteractive())
		{
			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR32(" %0:d %1:d %2:d %3:d %4:d %5:d %6:d %7:d %8:d %9:d %9:d "), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR32("%00000000000000000000:f"), 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR32("%0:000000000000000000f"), 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR32("%0:.000000000000000000f"), 0.f);
			EATEST_VERIFY(result == -1);

			memset(buffer, 0, sizeof(buffer));
			result = OSprintf(buffer, EA_CHAR32("%000000000000000000:000000000000000000.000000000000000000f"), 0.f);
			EATEST_VERIFY(result == -1);
		}

		// Test OVsnprintf capacity limits
		memset(buffer, 0, sizeof(buffer));
		result = OSnprintf(buffer, 0, EA_CHAR32("%2:1.0f %3:d %1:c"), (char)'3', (float)1.f, (int)2);
		EATEST_VERIFY((result == 5) && (buffer[0] == 0));
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestSprintf
///////////////////////////////////////////////////////////////////////////////

int TestSprintf()
{
	int nErrorCount(0);

	// Regular sprintf
	nErrorCount += TestSprintf8();
	nErrorCount += TestSprintf16();
	nErrorCount += TestSprintf32();

	// Dprintf
	nErrorCount += TestDrintf8();

	// Ordered sprintf
	nErrorCount += TestOsprintf8();
	nErrorCount += TestOsprintf16();
	nErrorCount += TestOsprintf32();

	return nErrorCount;
}


