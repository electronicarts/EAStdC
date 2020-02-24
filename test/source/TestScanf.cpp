///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EAScanf.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAMathHelp.h>
#include <EAStdC/Int128_t.h>
#include <EAStdC/internal/Config.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>
#include <EAAssert/eaassert.h>
#include <limits.h>
#include <float.h>
#include <string.h>




#if !defined(LLONG_MAX) // Some C++ standard libraries don't define LLONG_MAX, etc.
	#if defined(__LONG_LONG_MAX__) // GCC
		#define  LLONG_MIN  (-LLONG_MAX-1)
		#define  LLONG_MAX  __LONG_LONG_MAX__
		#define ULLONG_MIN  0
		#define ULLONG_MAX  (LLONG_MAX * 2ULL + 1)
	#else
		#define  LLONG_MIN  INT64_MIN
		#define  LLONG_MAX  INT64_MAX
		#define ULLONG_MIN  0
		#define ULLONG_MAX  UINT64_MAX
	#endif
#endif

#ifdef EA_ASSERT_ENABLED
	#define VERIFY_ASSERTCOUNT(count) EATEST_VERIFY(v.assertCount_ == count)
#else
	#define VERIFY_ASSERTCOUNT(count)
#endif


struct Values
{
	// char_ is defined explicitly as 'signed char' rather than 'char' (which is determined signed/unsigned by the compiler) on purpose.
	// If char were interpreted as unsigned char then there will be no type promotion for negative values which our tests assume (we want 0xFF to be promoted to 0xFFFFFFFF). 
	// Why work around the unsigned/signed char issue? There would be little gain duplicating test code per platform+compiler, particularly when a user 
	// can force the signedness to be one way or the other regardless. Additionally, we already test our suite for explicit signed and unsigned char types.
	signed char         char_[8];
	signed char         schar_[8];
	unsigned char       uchar_[8];
	wchar_t             wchar_[8];
	short               short_[8];
	unsigned short      ushort_[8];
	int                 int_[8];
	unsigned int        uint_[8];
	long                long_[8];
	unsigned long       ulong_[8];
	long long           longlong_[8];
	unsigned long long  ulonglong_[8];
	int8_t              int8_[8];
	uint8_t             uint8_[8];
	int16_t             int16_[8];
	uint16_t            uint16_[8];
	int32_t             int32_[8];
	uint32_t            uint32_[8];
	int64_t             int64_[8];
	uint64_t            uint64_[8];
  //EA::StdC::int128_t  int128_[8];     // int128_t has constructors and so is not a POD and cannot be part of the Values union.
  //EA::StdC::uint128_t uint128_[8];
	char             char8_[8];
	char16_t            char16_[8];
	char32_t            char32_[8];
	char             str8_[8][64];
	char16_t            str16_[8][64];
	char32_t            str32_[8][64];
	wchar_t             strw_[8][64];
	float               float_[8];
	double              double_[8];
	size_t              size_[8];
	ptrdiff_t           ptrdiff_[8];
	intptr_t            intptr_[8];
	uintptr_t           uintptr_[8];
	intmax_t            intmax_[8];
	uintmax_t           uintmax_[8];
	void*               voidptr_[8];
	static uint32_t     assertCount_; //Static to allow for use from lambda with empty capture set.  Implicitly not thread safe.

	int Clear()
	{
		memset(this, 0xff, sizeof(*this));
		assertCount_ = 0;
		return 0;
	}
};

uint32_t Values::assertCount_ = 0;

static int TestCRTVsscanf(const char* pBuffer, const char* pFormat, ...)
{
	va_list vList;
	va_start(vList, pFormat);
	int n = EA::StdC::Vsscanf(pBuffer, pFormat, vList);
	va_end(vList);
	return n;
}

static int TestCRTVsscanf(const char16_t* pBuffer, const char16_t* pFormat, ...)
{
	va_list vList;
	va_start(vList, pFormat);
	int n = EA::StdC::Vsscanf(pBuffer, pFormat, vList);
	va_end(vList);
	return n;
}

static int TestCRTVsscanf(const char32_t* pBuffer, const char32_t* pFormat, ...)
{
	va_list vList;
	va_start(vList, pFormat);
	int n = EA::StdC::Vsscanf(pBuffer, pFormat, vList);
	va_end(vList);
	return n;
}

///////////////////////////////////////////////////////////////////////////////
// TestScanfLimits
//
static int TestScanfLimits()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// Until our custom hand-implemented scanf is some day completed, we don't
	// have a portable version of the scanf family of functions. In the meantime
	// we can only support it on Microsoft platforms as opposed to GCC.
	int    n;
	Values v;

	{ // Test limits

		// char
		v.Clear();
		n = Sscanf("-128 127", "%hhd %hhd", &v.schar_[0], &v.schar_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.schar_[0] == SCHAR_MIN);
		EATEST_VERIFY(v.schar_[1] == SCHAR_MAX);

		v.Clear();
		n = Sscanf("0x80 0x7f", "%hhx %hhx", &v.schar_[0], &v.schar_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.schar_[0] == SCHAR_MIN);
		EATEST_VERIFY(v.schar_[1] == SCHAR_MAX);


		// unsigned char
		v.Clear();
		n = Sscanf("-0 255", "%hhu %hhu", &v.uchar_[0], &v.uchar_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.uchar_[0] == 0);
		EATEST_VERIFY(v.uchar_[1] == UCHAR_MAX);

		v.Clear();
		n = Sscanf("0 0xff", "%hhx %hhx", &v.uchar_[0], &v.uchar_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.uchar_[0] == 0);
		EATEST_VERIFY(v.uchar_[1] == UCHAR_MAX);


		// short
		v.Clear();
		n = Sscanf("-32768 32767", "%hd %hd", &v.short_[0], &v.short_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.short_[0] == SHRT_MIN);
		EATEST_VERIFY(v.short_[1] == SHRT_MAX);

		v.Clear();
		n = Sscanf("0x8000 0x7fff", "%hx %hx", &v.short_[0], &v.short_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.short_[0] == SHRT_MIN);
		EATEST_VERIFY(v.short_[1] == SHRT_MAX);


		// unsigned short
		v.Clear();
		n = Sscanf("-0 65535", "%hu %hu", &v.ushort_[0], &v.ushort_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.ushort_[0] == 0);
		EATEST_VERIFY(v.ushort_[1] == USHRT_MAX);

		v.Clear();
		n = Sscanf("0 0xffff", "%hx %hx", &v.ushort_[0], &v.ushort_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.ushort_[0] == 0);
		EATEST_VERIFY(v.ushort_[1] == USHRT_MAX);


		// int
		v.Clear();
		n = Sscanf("-0 +0 -2147483648 +2147483647", "%d %d %d %d", &v.int_[0], &v.int_[1], &v.int_[2], &v.int_[3]);
		EATEST_VERIFY(n == 4);
		EATEST_VERIFY(v.int_[0] == 0);
		EATEST_VERIFY(v.int_[1] == 0);
		EATEST_VERIFY(v.int_[2] == INT32_MIN); 
		EATEST_VERIFY(v.int_[3] == INT32_MAX);

		v.Clear();
		n = Sscanf("-0 +0 0x80000000 0x7fffffff", "%x %x %x %x", &v.int_[0], &v.int_[1], &v.int_[2], &v.int_[3]);
		EATEST_VERIFY(n == 4);
		EATEST_VERIFY(v.int_[0] == 0);
		EATEST_VERIFY(v.int_[1] == 0);
		EATEST_VERIFY(v.int_[2] == INT32_MIN); 
		EATEST_VERIFY(v.int_[3] == INT32_MAX);


		// unsigned int
		v.Clear();
		n = Sscanf("0 4294967295", "%u %u", &v.uint_[0], &v.uint_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.uint_[0] == 0);
		EATEST_VERIFY(v.uint_[1] == UINT32_MAX);

		v.Clear();
		n = Sscanf("0000000 0xffffffff", "%x %x", &v.uint_[0], &v.uint_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.uint_[0] == 0);
		EATEST_VERIFY(v.uint_[1] == UINT32_MAX);


		// long
		#if LONG_MAX == INT32_MAX
			v.Clear();
			n = Sscanf("-2147483648 +2147483647", "%ld %ld", &v.long_[0], &v.long_[1]);
		#elif LONG_MAX == INT64_MAX
			v.Clear();
			n = Sscanf("-9223372036854775808 9223372036854775807", "%ld %ld", &v.long_[0], &v.long_[1]);
		#endif
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.long_[0] == LONG_MIN);
		EATEST_VERIFY(v.long_[1] == LONG_MAX);

		#if LONG_MAX == INT32_MAX
			v.Clear();
			n = Sscanf("0x80000000 0x7fffffff", "%lx %lx", &v.long_[0], &v.long_[1]);
		#elif LONG_MAX == INT64_MAX
			v.Clear();
			n = Sscanf("0x8000000000000000 0x7fffffffffffffff", "%lx %lx", &v.long_[0], &v.long_[1]);
		#endif
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.long_[0] == LONG_MIN);
		EATEST_VERIFY(v.long_[1] == LONG_MAX);


		// ulong
		#if ULONG_MAX == UINT_MAX
			v.Clear();
			n = Sscanf("-0 4294967295", "%ld %ld", &v.ulong_[0], &v.ulong_[1]);
		#elif ULONG_MAX > UINT_MAX
			v.Clear();
			n = Sscanf("-0 18446744073709551615", "%ld %ld", &v.ulong_[0], &v.ulong_[1]);
		#endif
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.ulong_[0] == 0);
		EATEST_VERIFY(v.ulong_[1] == ULONG_MAX);

		#if ULONG_MAX == UINT_MAX
			v.Clear();
			n = Sscanf("0 0xffffffff", "%lx %lx", &v.ulong_[0], &v.ulong_[1]);
		#elif ULONG_MAX > UINT_MAX
			v.Clear();
			n = Sscanf("-0 0xffffffffffffffff", "%lx %lx", &v.ulong_[0], &v.ulong_[1]);
		#endif
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.ulong_[0] == 0);
		EATEST_VERIFY(v.ulong_[1] == ULONG_MAX);


		// long long
		#if LLONG_MAX == INT64_MAX
			v.Clear();
			n = Sscanf("-9223372036854775808 9223372036854775807", "%lld %lld", &v.longlong_[0], &v.longlong_[1]);
		#else
			v.Clear();
			n = Sscanf("-170141183460469231731687303715884105728 170141183460469231731687303715884105727", "%lld %lld", &v.longlong_[0], &v.longlong_[1]);
		#endif
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.longlong_[0] == LLONG_MIN);
		EATEST_VERIFY(v.longlong_[1] == LLONG_MAX);

		#if LLONG_MAX == INT64_MAX
			v.Clear();
			n = Sscanf("0x8000000000000000 0x7fffffffffffffff", "%llx %llx", &v.longlong_[0], &v.longlong_[1]);
		#else
			v.Clear();
			n = Sscanf("0x80000000000000000000000000000000 0x7fffffffffffffffffffffffffffffff", "%llx %llx", &v.longlong_[0], &v.longlong_[1]);
		#endif
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.longlong_[0] == LLONG_MIN);
		EATEST_VERIFY(v.longlong_[1] == LLONG_MAX);

		// unsigned long long
		#if ULLONG_MAX == UINT64_MAX
			v.Clear();
			n = Sscanf("0 18446744073709551615", "%lld %lld", &v.ulonglong_[0], &v.ulonglong_[1]);
		#else
			v.Clear();
			n = Sscanf("0 170141183460469231731687303715884105727", "%lld %lld", &v.ulonglong_[0], &v.ulonglong_[1]);
		#endif
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.ulonglong_[0] == 0);
		EATEST_VERIFY(v.ulonglong_[1] == ULLONG_MAX);

		#if ULLONG_MAX == UINT64_MAX
			v.Clear();
			n = Sscanf("0x0000000000000000 0xffffffffffffffff", "%llx %llx", &v.ulonglong_[0], &v.ulonglong_[1]);
		#else
			v.Clear();
			n = Sscanf("0x00000000000000000000000000000000 0xffffffffffffffffffffffffffffffff", "%llx %llx", &v.ulonglong_[0], &v.ulonglong_[1]);
		#endif
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(v.ulonglong_[0] == 0);
		EATEST_VERIFY(v.ulonglong_[1] == ULLONG_MAX);


		// float
		v.Clear();
		n = Sscanf("1.175494351e-38 3.402823466e+38", "%f %f", &v.float_[0], &v.float_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(FloatEqual(v.float_[0], 1.1754944e-38f)); // This is actually not a good enough test.
		EATEST_VERIFY(FloatEqual(v.float_[1], 3.4028234e+38f));


		// double
		v.Clear();
		n = Sscanf("2.2250738585072014e-308 1.7976931348623158e+308", "%lf %lf", &v.double_[0], &v.double_[1]);
		EATEST_VERIFY(n == 2);
		EATEST_VERIFY(DoubleEqual(v.double_[0], 2.2250738585072014e-308)); // This is actually not a good enough test.
		#if defined(EA_PLATFORM_WIN32)
			EATEST_VERIFY(DoubleEqual(v.double_[1], 1.7976931348623158e+308));
		#endif
	}

	// Regression of a user-reported bug:
	{
		float f;
		Sscanf("0.0001100f", "%f", &f);
		EATEST_VERIFY(FloatEqual(f, 0.0001100f));
		if(!FloatEqual(f, 0.0001100f))
			EA::UnitTest::Report("%f\n", f);
	}

	// Test fractions
	{
		const int nErrorCountSaved = nErrorCount; // We use this to exit the loop below upon the first found error.
		char buffer[10] = { '0', '.', '0', '0', '0', '0', '0', '0' };
		
		for(int i = 0; (i < 1000000) && (nErrorCount == nErrorCountSaved); i += 23)
		{
			buffer[2] = (char)('0' + (i / 100000 % 10));
			buffer[3] = (char)('0' + (i / 10000 % 10));
			buffer[4] = (char)('0' + (i / 1000 % 10));
			buffer[5] = (char)('0' + (i / 100 % 10));
			buffer[6] = (char)('0' + (i / 10 % 10));
			buffer[7] = (char)('0' + (i / 1 % 10));

			n = Sscanf(buffer, "%f", &v.float_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(FloatEqual(v.float_[0], (float)i / 1000000));
			if(!FloatEqual(v.float_[0], (float)i / 1000000))
				EA::UnitTest::Report("%f %f\n", v.float_[0], (float)i / 1000000);

			char buffer2[32];
			sprintf(buffer2, "%f", v.float_[0]);
			EATEST_VERIFY(Strcmp(buffer2, buffer) == 0);
			if(Strcmp(buffer2, buffer) != 0)
				EA::UnitTest::Report("%s %s\n", buffer2, buffer);
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestScanfMisc
//
static int TestScanfMisc()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	int    n;
	Values v;

	{   // Test of sscanf calls culled from some EA code.
		{
			v.Clear();
			n = Sscanf("", "");
			EATEST_VERIFY(n == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16(""), EA_CHAR16(""));
			EATEST_VERIFY(n == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32(""), EA_CHAR32(""));
			EATEST_VERIFY(n == 0);
		}

		{
			v.Clear();
			n = Sscanf("a", "%s", v.str8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("a"), EA_CHAR16("%s"), v.strw_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("a")) == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("a"), EA_CHAR32("%s"), v.strw_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("a")) == 0);
		}

		{   // String tests
			// We accept %hc, %c, %lc, %I8c, %I16c, %I32c (regular, regular, wide, char, char16_t, char32_t)
			// We accept %hC, %C, %lC, %I8C, %I16C, %I32C (regular, wide,    wide, char, char16_t, char32_t)
			// We accept %hs, %s, %ls, %I8s, %I16s, %I32s (regular, regular, wide, char, char16_t, char32_t)
			// We accept %hS, %S, %lS, %I8s, %I16s, %I32s (regular, wide,    wide, char, char16_t, char32_t)

			{   // char
				v.Clear();
				n = Sscanf("a b c d e f", "%hc %c %lc %I8c %I16c %I32c", &v.char_[0], &v.char_[1], &v.wchar_[0], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
				EATEST_VERIFY(n == 6);
				EATEST_VERIFY((v.char_[0] == 'a') && (v.char_[1] == 'b') && (v.wchar_[0] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));

				v.Clear();
				n = Sscanf("a b c d e f", "%hC %C %lC %I8C %I16C %I32C", &v.char_[0], &v.wchar_[0], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
				EATEST_VERIFY(n == 6);
				EATEST_VERIFY((v.char_[0] == 'a') && (v.wchar_[0] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));

				v.Clear();
				n = Sscanf("a b c d e f", "%hs %s %ls %I8s %I16s %I32s", &v.str8_[0], &v.str8_[1], &v.strw_[0], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
				EATEST_VERIFY(n == 6);
				EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
				EATEST_VERIFY(Strcmp(v.str8_[1], "b") == 0);
				EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("c")) == 0);
				EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
				EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
				EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);

				v.Clear();
				n = Sscanf("a b c d e f", "%hS %S %lS %I8S %I16S %I32S", &v.str8_[0], &v.strw_[0], &v.strw_[1], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
				EATEST_VERIFY(n == 6);
				EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
				EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("b")) == 0);
				EATEST_VERIFY(Strcmp(v.strw_[1], EA_WCHAR("c")) == 0);
				EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
				EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
				EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
			}

			{   // char16_t
				v.Clear();
				#if EASCANF_MS_STYLE_S_FORMAT // Microsoft style means that the meanings of S/C and s/c are reversed for non-char Sprintf.
					n = Sscanf(EA_CHAR16("a b c d e f"), EA_CHAR16("%hc %c %lc %I8c %I16c %I32c"), &v.char_[0], &v.wchar_[0], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY((v.char_[0] == 'a') && (v.wchar_[0] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));
				#else
					n = Sscanf(EA_CHAR16("a b c d e f"), EA_CHAR16("%hc %c %lc %I8c %I16c %I32c"), &v.char_[0], &v.char_[1], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY((v.char_[0] == 'a') && (v.char_[1] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));
				#endif

				v.Clear();
				#if EASCANF_MS_STYLE_S_FORMAT
					n = Sscanf(EA_CHAR16("a b c d e f"), EA_CHAR16("%hC %C %lC %I8C %I16C %I32C"), &v.char_[0], &v.char_[1], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY((v.char_[0] == 'a') && (v.char_[1] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));
				#else
					n = Sscanf(EA_CHAR16("a b c d e f"), EA_CHAR16("%hC %C %lC %I8C %I16C %I32C"), &v.char_[0], &v.wchar_[0], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY((v.char_[0] == 'a') && (v.wchar_[0] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));
				#endif

				v.Clear();
				#if EASCANF_MS_STYLE_S_FORMAT
					n = Sscanf(EA_CHAR16("a b c d e f"), EA_CHAR16("%hs %s %ls %I8s %I16s %I32s"), &v.str8_[0], &v.strw_[0], &v.strw_[1], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
					EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("b")) == 0);
					EATEST_VERIFY(Strcmp(v.strw_[1], EA_WCHAR("c")) == 0);
					EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
					EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
					EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
				#else
					n = Sscanf(EA_CHAR16("a b c d e f"), EA_CHAR16("%hs %s %ls %I8s %I16s %I32s"), &v.str8_[0], &v.str8_[1], &v.strw_[0], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
					EATEST_VERIFY(Strcmp(v.str8_[1], "b") == 0);
					EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("c")) == 0);
					EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
					EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
					EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
				#endif

				v.Clear();
				#if EASCANF_MS_STYLE_S_FORMAT
					n = Sscanf(EA_CHAR16("a b c d e f"), EA_CHAR16("%hS %S %lS %I8S %I16S %I32S"), &v.str8_[0], &v.str8_[1], &v.strw_[0], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
					EATEST_VERIFY(Strcmp(v.str8_[1], "b") == 0);
					EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("c")) == 0);
					EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
					EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
					EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
				#else
					n = Sscanf(EA_CHAR16("a b c d e f"), EA_CHAR16("%hS %S %lS %I8S %I16S %I32S"), &v.str8_[0], &v.strw_[0], &v.strw_[1], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
					EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("b")) == 0);
					EATEST_VERIFY(Strcmp(v.strw_[1], EA_WCHAR("c")) == 0);
					EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
					EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
					EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
				#endif
			}

			{   // char32_t
				v.Clear();
				#if EASCANF_MS_STYLE_S_FORMAT // Microsoft style means that the meanings of S/C and s/c are reversed for non-char Sprintf.
					n = Sscanf(EA_CHAR32("a b c d e f"), EA_CHAR32("%hc %c %lc %I8c %I16c %I32c"), &v.char_[0], &v.wchar_[0], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY((v.char_[0] == 'a') && (v.wchar_[0] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));
				#else
					n = Sscanf(EA_CHAR32("a b c d e f"), EA_CHAR32("%hc %c %lc %I8c %I16c %I32c"), &v.char_[0], &v.char_[1], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY((v.char_[0] == 'a') && (v.char_[1] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));
				#endif

				v.Clear();
				#if EASCANF_MS_STYLE_S_FORMAT
					n = Sscanf(EA_CHAR32("a b c d e f"), EA_CHAR32("%hC %C %lC %I8C %I16C %I32C"), &v.char_[0], &v.char_[1], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY((v.char_[0] == 'a') && (v.char_[1] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));
				#else
					n = Sscanf(EA_CHAR32("a b c d e f"), EA_CHAR32("%hC %C %lC %I8C %I16C %I32C"), &v.char_[0], &v.wchar_[0], &v.wchar_[1], &v.char8_[0], &v.char16_[0], &v.char32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY((v.char_[0] == 'a') && (v.wchar_[0] == 'b') && (v.wchar_[1] == 'c') && (v.char8_[0] == 'd') && (v.char16_[0] == 'e') && (v.char32_[0] == 'f'));
				#endif

				v.Clear();
				#if EASCANF_MS_STYLE_S_FORMAT
					n = Sscanf(EA_CHAR32("a b c d e f"), EA_CHAR32("%hs %s %ls %I8s %I16s %I32s"), &v.str8_[0], &v.strw_[0], &v.strw_[1], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
					EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("b")) == 0);
					EATEST_VERIFY(Strcmp(v.strw_[1], EA_WCHAR("c")) == 0);
					EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
					EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
					EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
				#else
					n = Sscanf(EA_CHAR32("a b c d e f"), EA_CHAR32("%hs %s %ls %I8s %I16s %I32s"), &v.str8_[0], &v.str8_[1], &v.strw_[0], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
					EATEST_VERIFY(Strcmp(v.str8_[1], "b") == 0);
					EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("c")) == 0);
					EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
					EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
					EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
				#endif

				v.Clear();
				#if EASCANF_MS_STYLE_S_FORMAT
					n = Sscanf(EA_CHAR32("a b c d e f"), EA_CHAR32("%hS %S %lS %I8S %I16S %I32S"), &v.str8_[0], &v.str8_[1], &v.strw_[0], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
					EATEST_VERIFY(Strcmp(v.str8_[1], "b") == 0);
					EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("c")) == 0);
					EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
					EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
					EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
				#else
					n = Sscanf(EA_CHAR32("a b c d e f"), EA_CHAR32("%hS %S %lS %I8S %I16S %I32S"), &v.str8_[0], &v.strw_[0], &v.strw_[1], &v.str8_[2], &v.str16_[0], &v.str32_[0]);
					EATEST_VERIFY(n == 6);
					EATEST_VERIFY(Strcmp(v.str8_[0], "a") == 0);
					EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("b")) == 0);
					EATEST_VERIFY(Strcmp(v.strw_[1], EA_WCHAR("c")) == 0);
					EATEST_VERIFY(Strcmp(v.str8_[2], "d") == 0);
					EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("e")) == 0);
					EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("f")) == 0);
				#endif
			}
		}

		{
			v.Clear();
			n = Sscanf("abc 123", "%I8s\t%i", v.str8_[0], &v.int_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(Strcmp(v.str8_[0], "abc") == 0);
			EATEST_VERIFY(v.int_[0] == 123);

			v.Clear();
			n = Sscanf(EA_CHAR16("abc 123"), EA_CHAR16("%I16s\t%i"), v.str16_[0], &v.int_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("abc")) == 0);
			EATEST_VERIFY(v.int_[0] == 123);

			v.Clear();
			n = Sscanf(EA_CHAR32("abc 123"), EA_CHAR32("%I32s\t%i"), v.str32_[0], &v.int_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("abc")) == 0);
			EATEST_VERIFY(v.int_[0] == 123);
		}

		{
			v.Clear();
			n = Sscanf("0", "%lf", &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.double_[0] == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("0"), EA_CHAR16("%lf"), &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.double_[0] == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("0"), EA_CHAR32("%lf"), &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.double_[0] == 0);
		}

		{
			v.Clear();
			n = Sscanf("-123.456", "%f", &v.float_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(FloatEqual(v.float_[0], -123.456f));

			v.Clear();
			n = Sscanf(EA_CHAR16("-123.456"), EA_CHAR16("%f"), &v.float_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(FloatEqual(v.float_[0], -123.456f));

			v.Clear();
			n = Sscanf(EA_CHAR32("-123.456"), EA_CHAR32("%f"), &v.float_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(FloatEqual(v.float_[0], -123.456f));
		}

		{
			v.Clear();
			n = Sscanf("-123.456", "%lf", &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(DoubleEqual(v.double_[0], -123.456));

			v.Clear();
			n = Sscanf(EA_CHAR16("-123.456"), EA_CHAR16("%lf"), &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(DoubleEqual(v.double_[0], -123.456));

			v.Clear();
			n = Sscanf(EA_CHAR32("-123.456"), EA_CHAR32("%lf"), &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(DoubleEqual(v.double_[0], -123.456));
		}

		{
			v.Clear();
			n = Sscanf("-123.456e4", "%lf", &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.double_[0] == -123.456e4);

			v.Clear();
			n = Sscanf(EA_CHAR16("-123.456e4"), EA_CHAR16("%lf"), &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.double_[0] == -123.456e4);

			v.Clear();
			n = Sscanf(EA_CHAR32("-123.456e4"), EA_CHAR32("%lf"), &v.double_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.double_[0] == -123.456e4);
		}

		{
			v.Clear();
			n = Sscanf("12 abcdef", "%4u%n", &v.uint_[0], &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.uint_[0] == 12);
			EATEST_VERIFY(v.int_[0] == 2);

			v.Clear();
			n = Sscanf(EA_CHAR16("12 abcdef"), EA_CHAR16("%4u%n"), &v.uint_[0], &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.uint_[0] == 12);
			EATEST_VERIFY(v.int_[0] == 2);

			v.Clear();
			n = Sscanf(EA_CHAR32("12 abcdef"), EA_CHAR32("%4u%n"), &v.uint_[0], &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.uint_[0] == 12);
			EATEST_VERIFY(v.int_[0] == 2);
		}

		{
			v.Clear();
			n = Sscanf("Test, more text -999 1.345e22 0x12345678", "%s %s %*s %i %lf 0x%08x", v.str8_[0], v.str8_[1], &v.int_[0], &v.double_[0], &v.uint_[1]);
			EATEST_VERIFY(n == 5);
			EATEST_VERIFY(!Strcmp("Test,", v.str8_[0]));
			EATEST_VERIFY(!Strcmp("more", v.str8_[1]));
			EATEST_VERIFY(v.int_[0] == -999);
			EATEST_VERIFY(DoubleEqual(v.double_[0], 1.345e22));
			EATEST_VERIFY(v.uint_[1] == 0x12345678);

			v.Clear();
			n = Sscanf(EA_CHAR16("Test, more text -999 1.345e22 0x12345678"), EA_CHAR16("%I16s %I16s %*s %i %lf 0x%08x"), v.str16_[0], v.str16_[1], &v.int_[0], &v.double_[0], &v.uint_[1]);
			EATEST_VERIFY(n == 5);
			EATEST_VERIFY(!Strcmp(EA_CHAR16("Test,"), v.str16_[0]));
			EATEST_VERIFY(!Strcmp(EA_CHAR16("more"), v.str16_[1]));
			EATEST_VERIFY(v.int_[0] == -999);
			EATEST_VERIFY(DoubleEqual(v.double_[0], 1.345e22));
			EATEST_VERIFY(v.uint_[1] == 0x12345678);

			v.Clear();
			n = Sscanf(EA_CHAR32("Test, more text -999 1.345e22 0x12345678"), EA_CHAR32("%I32s %I32s %*s %i %lf 0x%08x"), v.str32_[0], v.str32_[1], &v.int_[0], &v.double_[0], &v.uint_[1]);
			EATEST_VERIFY(n == 5);
			EATEST_VERIFY(!Strcmp(EA_CHAR32("Test,"), v.str32_[0]));
			EATEST_VERIFY(!Strcmp(EA_CHAR32("more"), v.str32_[1]));
			EATEST_VERIFY(v.int_[0] == -999);
			EATEST_VERIFY(DoubleEqual(v.double_[0], 1.345e22));
			EATEST_VERIFY(v.uint_[1] == 0x12345678);
		}

		#if (EA_PLATFORM_PTR_SIZE == 8)
		{
			v.Clear();
			n = Sscanf("1234567843434343", "%p", &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == UINT64_C(0x1234567843434343));

			v.Clear();
			n = Sscanf("ffffffffffffffff", "%p", &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == UINT64_C(0xffffffffffffffff));

			v.Clear();
			n = Sscanf("0xffffffffffffffff", "%p", &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == UINT64_C(0xffffffffffffffff));
		}
		#else
		{
			v.Clear();
			n = Sscanf("12345678", "%p", &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0x12345678);

			v.Clear();
			n = Sscanf("ffffffff", "%p", &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0xffffffff);

			v.Clear();
			n = Sscanf("0xffffffff", "%p", &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0xffffffff);
		}
		#endif
	}


	// UTF8 / UCS2 conversions
	{   
		{
			v.Clear();
			n = Sscanf("a " "\xC2" "\xA9" "\xE2" "\x89" "\xA0", "%ls %ls", v.strw_[0], v.strw_[1]); // U+00A9 => 0xC2 0xA9. U+2260 => 0xE2 0x89 0xA0
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.strw_[0][0] == 'a');
			EATEST_VERIFY(v.strw_[0][1] == 0);
			EATEST_VERIFY(v.strw_[1][0] == 0x00A9);
			EATEST_VERIFY(v.strw_[1][1] == 0x2260);
			EATEST_VERIFY(v.strw_[1][2] == 0);

			v.Clear();
			n = Sscanf("a " "\xC2" "\xA9" "\xE2" "\x89" "\xA0", "%I16s %I16s", v.str16_[0], v.str16_[1]); // U+00A9 => 0xC2 0xA9. U+2260 => 0xE2 0x89 0xA0
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.str16_[0][0] == 'a');
			EATEST_VERIFY(v.str16_[0][1] == 0);
			EATEST_VERIFY(v.str16_[1][0] == 0x00A9);
			EATEST_VERIFY(v.str16_[1][1] == 0x2260);
			EATEST_VERIFY(v.str16_[1][2] == 0);

			v.Clear();
			n = Sscanf("a " "\xC2" "\xA9" "\xE2" "\x89" "\xA0", "%I32s %I32s", v.str32_[0], v.str32_[1]); // U+00A9 => 0xC2 0xA9. U+2260 => 0xE2 0x89 0xA0
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.str32_[0][0] == 'a');
			EATEST_VERIFY(v.str32_[0][1] == 0);
			EATEST_VERIFY(v.str32_[1][0] == 0x00A9);
			EATEST_VERIFY(v.str32_[1][1] == 0x2260);
			EATEST_VERIFY(v.str32_[1][2] == 0);
		}

		{
			v.Clear();
			n = Sscanf(EA_CHAR16("a \x00A9\x2260"), EA_CHAR16("%hs %hs"), v.str8_[0], v.str8_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY((uint8_t)v.str8_[0][0] == 'a');
			EATEST_VERIFY((uint8_t)v.str8_[0][1] == 0);
			EATEST_VERIFY((uint8_t)v.str8_[1][0] == 0xC2);
			EATEST_VERIFY((uint8_t)v.str8_[1][1] == 0xA9);
			EATEST_VERIFY((uint8_t)v.str8_[1][2] == 0xE2);
			EATEST_VERIFY((uint8_t)v.str8_[1][3] == 0x89);
			EATEST_VERIFY((uint8_t)v.str8_[1][4] == 0xA0);
			EATEST_VERIFY((uint8_t)v.str8_[1][5] == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("a \x00A9\x2260"), EA_CHAR32("%hs %hs"), v.str8_[0], v.str8_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY((uint8_t)v.str8_[0][0] == 'a');
			EATEST_VERIFY((uint8_t)v.str8_[0][1] == 0);
			EATEST_VERIFY((uint8_t)v.str8_[1][0] == 0xC2);
			EATEST_VERIFY((uint8_t)v.str8_[1][1] == 0xA9);
			EATEST_VERIFY((uint8_t)v.str8_[1][2] == 0xE2);
			EATEST_VERIFY((uint8_t)v.str8_[1][3] == 0x89);
			EATEST_VERIFY((uint8_t)v.str8_[1][4] == 0xA0);
			EATEST_VERIFY((uint8_t)v.str8_[1][5] == 0);
		}
	}


	// The following are examples taken from some EA code.
	{
		{
			v.Clear();
			n = Sscanf("12a", "%ld%c", &v.long_[0], &v.char_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.long_[0] == 12);
			EATEST_VERIFY(v.char_[0] == 'a');

			v.Clear();
			n = Sscanf(EA_CHAR16("12a"), EA_CHAR16("%ld%c"), &v.long_[0], &v.wchar_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.long_[0] == 12);
			EATEST_VERIFY(v.wchar_[0] == 'a');

			v.Clear();
			n = Sscanf(EA_CHAR32("12a"), EA_CHAR32("%ld%c"), &v.long_[0], &v.wchar_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.long_[0] == 12);
			EATEST_VERIFY(v.wchar_[0] == 'a');
		}

		{
			v.Clear();
			n = Sscanf("#define width 640 #define height 480", "#define %*s %d #define %*s %d", &v.int_[0], &v.int_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 640);
			EATEST_VERIFY(v.int_[1] == 480);

			v.Clear();
			n = Sscanf(EA_CHAR16("#define width 640 #define height 480"), EA_CHAR16("#define %*s %d #define %*s %d"), &v.int_[0], &v.int_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 640);
			EATEST_VERIFY(v.int_[1] == 480);

			v.Clear();
			n = Sscanf(EA_CHAR32("#define width 640 #define height 480"), EA_CHAR32("#define %*s %d #define %*s %d"), &v.int_[0], &v.int_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 640);
			EATEST_VERIFY(v.int_[1] == 480);
		}

		{
			v.Clear();
			n = Sscanf("00010101", "%04d%02d%02d", &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.int_[0] == 1);
			EATEST_VERIFY(v.int_[1] == 1);
			EATEST_VERIFY(v.int_[2] == 1);

			v.Clear();
			n = Sscanf(EA_CHAR16("00010101"), EA_CHAR16("%04d%02d%02d"), &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.int_[0] == 1);
			EATEST_VERIFY(v.int_[1] == 1);
			EATEST_VERIFY(v.int_[2] == 1);

			v.Clear();
			n = Sscanf(EA_CHAR32("00010101"), EA_CHAR32("%04d%02d%02d"), &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.int_[0] == 1);
			EATEST_VERIFY(v.int_[1] == 1);
			EATEST_VERIFY(v.int_[2] == 1);
		}

		{
			v.Clear();
			n = Sscanf("0xfafbfcfd", "%lx", &v.long_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uint32_t)v.long_[0] == 0xfafbfcfd);

			v.Clear();
			n = Sscanf(EA_CHAR16("0xfafbfcfd"), EA_CHAR16("%lx"), &v.long_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uint32_t)v.long_[0] == 0xfafbfcfd);

			v.Clear();
			n = Sscanf(EA_CHAR32("0xfafbfcfd"), EA_CHAR32("%lx"), &v.long_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uint32_t)v.long_[0] == 0xfafbfcfd);
		}

		{
			v.Clear();
			n = Sscanf("127.255.3.0", "%u.%u.%u.%u", &v.uint_[0], &v.uint_[1], &v.uint_[2], &v.uint_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.uint_[0] == 127);
			EATEST_VERIFY(v.uint_[1] == 255);
			EATEST_VERIFY(v.uint_[2] ==   3);
			EATEST_VERIFY(v.uint_[3] ==   0);

			v.Clear();
			n = Sscanf(EA_CHAR16("127.255.3.0"), EA_CHAR16("%u.%u.%u.%u"), &v.uint_[0], &v.uint_[1], &v.uint_[2], &v.uint_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.uint_[0] == 127);
			EATEST_VERIFY(v.uint_[1] == 255);
			EATEST_VERIFY(v.uint_[2] ==   3);
			EATEST_VERIFY(v.uint_[3] ==   0);

			v.Clear();
			n = Sscanf(EA_CHAR32("127.255.3.0"), EA_CHAR32("%u.%u.%u.%u"), &v.uint_[0], &v.uint_[1], &v.uint_[2], &v.uint_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.uint_[0] == 127);
			EATEST_VERIFY(v.uint_[1] == 255);
			EATEST_VERIFY(v.uint_[2] ==   3);
			EATEST_VERIFY(v.uint_[3] ==   0);
		}

		{
			v.Clear();
			n = Sscanf("0.255.3.127", "%d.%d.%d.%d", &v.int_[0], &v.int_[1], &v.int_[2], &v.int_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.int_[0] ==   0);
			EATEST_VERIFY(v.int_[1] == 255);
			EATEST_VERIFY(v.int_[2] ==   3);
			EATEST_VERIFY(v.int_[3] == 127);

			v.Clear();
			n = Sscanf(EA_CHAR16("0.255.3.127"), EA_CHAR16("%d.%d.%d.%d"), &v.int_[0], &v.int_[1], &v.int_[2], &v.int_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.int_[0] ==   0);
			EATEST_VERIFY(v.int_[1] == 255);
			EATEST_VERIFY(v.int_[2] ==   3);
			EATEST_VERIFY(v.int_[3] == 127);

			v.Clear();
			n = Sscanf(EA_CHAR32("0.255.3.127"), EA_CHAR32("%d.%d.%d.%d"), &v.int_[0], &v.int_[1], &v.int_[2], &v.int_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.int_[0] ==   0);
			EATEST_VERIFY(v.int_[1] == 255);
			EATEST_VERIFY(v.int_[2] ==   3);
			EATEST_VERIFY(v.int_[3] == 127);
		}

		{
			v.Clear();
			n = Sscanf("ff12ee", "%2x%2x%2x", &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.int_[0] == 0xff);
			EATEST_VERIFY(v.int_[1] == 0x12);
			EATEST_VERIFY(v.int_[2] == 0xee);

			v.Clear();
			n = Sscanf(EA_CHAR16("ff12ee"), EA_CHAR16("%2x%2x%2x"), &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.int_[0] == 0xff);
			EATEST_VERIFY(v.int_[1] == 0x12);
			EATEST_VERIFY(v.int_[2] == 0xee);

			v.Clear();
			n = Sscanf(EA_CHAR32("ff12ee"), EA_CHAR32("%2x%2x%2x"), &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.int_[0] == 0xff);
			EATEST_VERIFY(v.int_[1] == 0x12);
			EATEST_VERIFY(v.int_[2] == 0xee);
		}

		{
			v.Clear();
			n = Sscanf("f2e", "%1hhx%1hhx%1hhx", &v.uint8_[0], &v.uint8_[1], &v.uint8_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.uint8_[0] == 0xf);
			EATEST_VERIFY(v.uint8_[1] == 0x2);
			EATEST_VERIFY(v.uint8_[2] == 0xe);

			v.Clear();
			n = Sscanf(EA_CHAR16("f2e"), EA_CHAR16("%1hhx%1hhx%1hhx"), &v.uint8_[0], &v.uint8_[1], &v.uint8_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.uint8_[0] == 0xf);
			EATEST_VERIFY(v.uint8_[1] == 0x2);
			EATEST_VERIFY(v.uint8_[2] == 0xe);

			v.Clear();
			n = Sscanf(EA_CHAR32("f2e"), EA_CHAR32("%1hhx%1hhx%1hhx"), &v.uint8_[0], &v.uint8_[1], &v.uint8_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(v.uint8_[0] == 0xf);
			EATEST_VERIFY(v.uint8_[1] == 0x2);
			EATEST_VERIFY(v.uint8_[2] == 0xe);
		}

		{
			v.Clear();
			n = Sscanf("Test/123.4 567", "%4s/%5f %3d", v.str8_[0], &v.float_[0], &v.int_[0]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(!Strcmp(v.str8_[0], "Test"));
			EATEST_VERIFY(FloatEqual(v.float_[0], 123.4f));
			EATEST_VERIFY(v.int_[0] == 567);

			v.Clear();
			n = Sscanf(EA_CHAR16("Test/123.4 567"), EA_CHAR16("%4I16s/%5f %3d"), v.str16_[0], &v.float_[0], &v.int_[0]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(!Strcmp(v.str16_[0], EA_CHAR16("Test")));
			EATEST_VERIFY(FloatEqual(v.float_[0], 123.4f));
			EATEST_VERIFY(v.int_[0] == 567);

			v.Clear();
			n = Sscanf(EA_CHAR32("Test/123.4 567"), EA_CHAR32("%4I32s/%5f %3d"), v.str32_[0], &v.float_[0], &v.int_[0]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY(!Strcmp(v.str32_[0], EA_CHAR32("Test")));
			EATEST_VERIFY(FloatEqual(v.float_[0], 123.4f));
			EATEST_VERIFY(v.int_[0] == 567);
		}

		{
			v.Clear();
			n = Sscanf("abdefg-hijk-a-mnopqrstu\n", "%32[^-]-%32[^-]-%32[^-]-%32[^\n\r]", v.str8_[0], v.str8_[1], v.str8_[2], v.str8_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(!Strcmp(v.str8_[0], "abdefg"));
			EATEST_VERIFY(!Strcmp(v.str8_[1], "hijk"));
			EATEST_VERIFY(!Strcmp(v.str8_[2], "a"));
			EATEST_VERIFY(!Strcmp(v.str8_[3], "mnopqrstu"));

			v.Clear();
			n = Sscanf(EA_CHAR16("abdefg-hijk-a-mnopqrstu\n"), EA_CHAR16("%32I16[^-]-%32I16[^-]-%32I16[^-]-%32I16[^\n\r]"), v.str16_[0], v.str16_[1], v.str16_[2], v.str16_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(!Strcmp(v.str16_[0], EA_CHAR16("abdefg")));
			EATEST_VERIFY(!Strcmp(v.str16_[1], EA_CHAR16("hijk")));
			EATEST_VERIFY(!Strcmp(v.str16_[2], EA_CHAR16("a")));
			EATEST_VERIFY(!Strcmp(v.str16_[3], EA_CHAR16("mnopqrstu")));

			v.Clear();
			n = Sscanf(EA_CHAR32("abdefg-hijk-a-mnopqrstu\n"), EA_CHAR32("%32I32[^-]-%32I32[^-]-%32I32[^-]-%32I32[^\n\r]"), v.str32_[0], v.str32_[1], v.str32_[2], v.str32_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(!Strcmp(v.str32_[0], EA_CHAR32("abdefg")));
			EATEST_VERIFY(!Strcmp(v.str32_[1], EA_CHAR32("hijk")));
			EATEST_VERIFY(!Strcmp(v.str32_[2], EA_CHAR32("a")));
			EATEST_VERIFY(!Strcmp(v.str32_[3], EA_CHAR32("mnopqrstu")));
		}

		{
			v.Clear();
			n = Sscanf("[17;-18R", "[%hu;%huR", &v.short_[0], &v.short_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.short_[0] == 17);
			EATEST_VERIFY(v.short_[1] == -18);

			v.Clear();
			n = Sscanf(EA_CHAR16("[17;-18R"), EA_CHAR16("[%hu;%huR"), &v.short_[0], &v.short_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.short_[0] == 17);
			EATEST_VERIFY(v.short_[1] == -18);

			v.Clear();
			n = Sscanf(EA_CHAR32("[17;-18R"), EA_CHAR32("[%hu;%huR"), &v.short_[0], &v.short_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.short_[0] == 17);
			EATEST_VERIFY(v.short_[1] == -18);
		}
	
		{
			v.Clear();
			n = Sscanf("0x01-0x02-0304", "%08x-%04x-%02x%02x", &v.int_[0], &v.int_[1], &v.int_[2], &v.int_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.int_[0] == 0x01);
			EATEST_VERIFY(v.int_[1] == 0x02);
			EATEST_VERIFY(v.int_[2] == 0x03);
			EATEST_VERIFY(v.int_[3] == 0x04);

			v.Clear();
			n = Sscanf(EA_CHAR16("0x01-0x02-0304"), EA_CHAR16("%08x-%04x-%02x%02x"), &v.int_[0], &v.int_[1], &v.int_[2], &v.int_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.int_[0] == 0x01);
			EATEST_VERIFY(v.int_[1] == 0x02);
			EATEST_VERIFY(v.int_[2] == 0x03);
			EATEST_VERIFY(v.int_[3] == 0x04);

			v.Clear();
			n = Sscanf(EA_CHAR32("0x01-0x02-0304"), EA_CHAR32("%08x-%04x-%02x%02x"), &v.int_[0], &v.int_[1], &v.int_[2], &v.int_[3]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(v.int_[0] == 0x01);
			EATEST_VERIFY(v.int_[1] == 0x02);
			EATEST_VERIFY(v.int_[2] == 0x03);
			EATEST_VERIFY(v.int_[3] == 0x04);
		}

		{
			v.Clear();
			n = Sscanf("-3.14 .2 48 1a", "%f %f %x %x", &v.float_[0], &v.float_[1], &v.int_[0], &v.int_[1]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(FloatEqual(v.float_[0], -3.14f));
			EATEST_VERIFY(FloatEqual(v.float_[1], .2f));
			EATEST_VERIFY(v.int_[0] == 0x48);
			EATEST_VERIFY(v.int_[1] == 0x1a);

			v.Clear();
			n = Sscanf(EA_CHAR16("-3.14 .2 48 1a"), EA_CHAR16("%f %f %x %x"), &v.float_[0], &v.float_[1], &v.int_[0], &v.int_[1]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(FloatEqual(v.float_[0], -3.14f));
			EATEST_VERIFY(FloatEqual(v.float_[1], .2f));
			EATEST_VERIFY(v.int_[0] == 0x48);
			EATEST_VERIFY(v.int_[1] == 0x1a);

			v.Clear();
			n = Sscanf(EA_CHAR32("-3.14 .2 48 1a"), EA_CHAR32("%f %f %x %x"), &v.float_[0], &v.float_[1], &v.int_[0], &v.int_[1]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(FloatEqual(v.float_[0], -3.14f));
			EATEST_VERIFY(FloatEqual(v.float_[1], .2f));
			EATEST_VERIFY(v.int_[0] == 0x48);
			EATEST_VERIFY(v.int_[1] == 0x1a);
		}

		{   // Regression for %x '_' bug.
			{
				v.Clear();
				n = EA::StdC::Sscanf("_s", "%x_%c", &v.int_[0], &v.char_[1]);
				EATEST_VERIFY(n == 0);
				EATEST_VERIFY(v.int_[0] == -1);
				EATEST_VERIFY(v.char_[1] == -1);

				v.Clear();
				n = EA::StdC::Sscanf("1a01_", "%x_%c", &v.int_[0], &v.char_[1]);  // EAStdC was mistakenly reading this as 0x1a018 instead of 0x1a01
				EATEST_VERIFY(n == 1);
				EATEST_VERIFY(v.int_[0] == 0x1a01);
				EATEST_VERIFY(v.char_[1] == -1);

				v.Clear();
				n = EA::StdC::Sscanf("1a01_s", "%x_%c", &v.int_[0], &v.char_[1]);
				EATEST_VERIFY(n == 2);
				EATEST_VERIFY(v.int_[0] == 0x1a01);
				EATEST_VERIFY(v.char_[1] == 's');
			}

			{
				v.Clear();
				n = EA::StdC::Sscanf(EA_CHAR16("_s"), EA_CHAR16("%x_%I16c"), &v.int_[0], &v.char16_[1]);
				EATEST_VERIFY(n == 0);
				EATEST_VERIFY(v.int_[0] == -1);
				EATEST_VERIFY((uint16_t)v.char16_[1] == 0xffff);

				v.Clear();
				n = EA::StdC::Sscanf(EA_CHAR16("1a01_"), EA_CHAR16("%x_%I16c"), &v.int_[0], &v.char16_[1]);  // EAStdC was mistakenly reading this as 0x1a018 instead of 0x1a01
				EATEST_VERIFY(n == 1);
				EATEST_VERIFY(v.int_[0] == 0x1a01);
				EATEST_VERIFY((uint16_t)v.char16_[1] == 0xffff);

				v.Clear();
				n = EA::StdC::Sscanf(EA_CHAR16("1a01_s"), EA_CHAR16("%x_%I16c"), &v.int_[0], &v.char16_[1]);
				EATEST_VERIFY(n == 2);
				EATEST_VERIFY(v.int_[0] == 0x1a01);
				EATEST_VERIFY(v.char16_[1] == 's');
			}

			{
				v.Clear();
				n = EA::StdC::Sscanf(EA_CHAR32("_s"), EA_CHAR32("%x_%I32c"), &v.int_[0], &v.char32_[1]);
				EATEST_VERIFY(n == 0);
				EATEST_VERIFY(v.int_[0] == -1);
				EATEST_VERIFY((uint32_t)v.char32_[1] == 0xffffffff);

				v.Clear();
				n = EA::StdC::Sscanf(EA_CHAR32("1a01_"), EA_CHAR32("%x_%I32c"), &v.int_[0], &v.char32_[1]);  // EAStdC was mistakenly reading this as 0x1a018 instead of 0x1a01
				EATEST_VERIFY(n == 1);
				EATEST_VERIFY(v.int_[0] == 0x1a01);
				EATEST_VERIFY((uint32_t)v.char32_[1] == 0xffffffff);

				v.Clear();
				n = EA::StdC::Sscanf(EA_CHAR32("1a01_s"), EA_CHAR32("%x_%I32c"), &v.int_[0], &v.char32_[1]);
				EATEST_VERIFY(n == 2);
				EATEST_VERIFY(v.int_[0] == 0x1a01);
				EATEST_VERIFY(v.char32_[1] == 's');
			}
		}

		{
			v.Clear();
			n = Sscanf(" for (int x = 0; x < 17; ++x)", " for %*[^(](%n%*[^)]%n%c\n", &v.int_[0], &v.int_[1], &v.char8_[0]);
			EATEST_VERIFY(n == 1);  // %n doesn't count towards the return value, so the expected result is 1 and not 3.
			EATEST_VERIFY(v.int_[0] == 6);
			EATEST_VERIFY(v.int_[1] == 28);
			EATEST_VERIFY(v.char8_[0] == ')');

			v.Clear();
			n = Sscanf(EA_CHAR16(" for (int x = 0; x < 17; ++x)"), EA_CHAR16(" for %*[^(](%n%*[^)]%n%I16c\n"), &v.int_[0], &v.int_[1], &v.char16_[0]);
			EATEST_VERIFY(n == 1);  // %n doesn't count towards the return value, so the expected result is 1 and not 3.
			EATEST_VERIFY(v.int_[0] == 6);
			EATEST_VERIFY(v.int_[1] == 28);
			EATEST_VERIFY(v.char16_[0] == ')');

			v.Clear();
			n = Sscanf(EA_CHAR32(" for (int x = 0; x < 17; ++x)"), EA_CHAR32(" for %*[^(](%n%*[^)]%n%I32c\n"), &v.int_[0], &v.int_[1], &v.char32_[0]);
			EATEST_VERIFY(n == 1);  // %n doesn't count towards the return value, so the expected result is 1 and not 3.
			EATEST_VERIFY(v.int_[0] == 6);
			EATEST_VERIFY(v.int_[1] == 28);
			EATEST_VERIFY(v.char32_[0] == ')');
		}

		// To do: Implement some of these:
		/*
		v.Clear();
		//n = Sscanf(mpStreamPosition, " %p %u %d", &pResult, &nSize, &nAllocationFlags);

		v.Clear();
		//n = Sscanf(line.c_str(), "IMADDI %[^,], %[^,], %[^,], %d", register1, register2, register3, &immediate1)

		v.Clear();
		//n = Sscanf(buffer, "%s %[A-Za-z0-9][%d][%d] %s %s", currentState->stateName,currentState->elementType, &currentState->numColumns, &currentState->numRows,currentState->uiType, currentState->globalStateType);

		v.Clear();
		//n = Sscanf(argv[i], "-i%x", &id);

		v.Clear();
		//n = Sscanf(argv[i], "-p%s", prefix);

		v.Clear();
		//n = Sscanf(argv[i], "-o%s", outputPath);

		v.Clear();
		//n = Sscanf(argv[i], "-h%s", stateFile);

		v.Clear();
		//n = Sscanf(argv[i], "-r%s", registerFile);

		v.Clear();
		//n = Sscanf(fp,"%128s",effectName)

		v.Clear();
		//n = Sscanf(linebuf.c_str(), "%x:%x %xH %*s %31s", &g.mSegment, &g.mOffset, &g.mLength, className))

		v.Clear();
		//n = Sscanf(linebuf.c_str(), "%x:%x %s %x %*c %s", &seg, &offset, symbolMangled, &rva, source))

		v.Clear();
		//n = Sscanf(&linebuf[9], " load address is %x", &address))

		v.Clear();
		//n = Sscanf(dfile,"%[^\n\r]", packageName);

		v.Clear();
		//n = Sscanf("127 -127 177 255 ff", "%"SCNd8 " %"SCNi8 " %"SCNo8 " %"SCNu8 " %"SCNx8, &d8, &i8, &o8, &u8, &x8);

		v.Clear();
		//n = Sscanf("32767 -32768 77777 65535 ffff", "%"SCNd16 " %"SCNi16 " %"SCNo16 " %"SCNu16 " %"SCNx16, &d16, &i16, &o16, &u16, &x16);

		v.Clear();
		//n = Sscanf("2147483647 -2147483648 17777777777 4294967295 ffffffff", "%"SCNd32 " %"SCNi32 " %"SCNo32 " %"SCNu32 " %"SCNx32, &d32, &i32, &o32, &u32, &x32);

		v.Clear();
		//n = Sscanf("9223372036854775807 -9223372036854775808 777777777777777777777 18446744073709551615 ffffffffffffffff", "%"SCNd64 " %"SCNi64 " %"SCNo64 " %"SCNu64 " %"SCNx64, &d64, &i64, &o64, &u64, &x64);

		v.Clear();
		//n = Sscanf("2147483647 -2147483648 17777777777 4294967295 ffffffff", "%"SCNdPTR " %"SCNiPTR " %"SCNoPTR " %"SCNuPTR " %"SCNxPTR, &dPtr, &iPtr, &oPtr, &uPtr, &xPtr);

		v.Clear();
		//n = Sscanf("2147483647 -2147483648 17777777777 18446744073709551615 ffffffffffffffff", "%"SCNdPTR " %"SCNiPTR " %"SCNoPTR " %"SCNuPTR " %"SCNxPTR, &dPtr, &iPtr, &oPtr, &uPtr, &xPtr);

		v.Clear();
		//n = Sscanf( sValue, EA_CHAR16( "%g,%g,%g,%g" ), &rf.mLeft, &rf.mTop, &rf.mRight, &rf.mBottom ) != 4)

		v.Clear();
		//n = Sscanf(sctheader+4,"%14lf%14lf%12lf%12lf",&faspectx,&faspecty,&fheight,&fwidth);
		*/
	}

	{
		{
			v.Clear();
			n = Sscanf("%", "%%");
			EATEST_VERIFY(n == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("%"), EA_CHAR16("%%"));
			EATEST_VERIFY(n == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("%"), EA_CHAR32("%%"));
			EATEST_VERIFY(n == 0);
		}

		{
			v.Clear();
			n = Sscanf("0 1 2 3 4 5 6 7 8 9 10 11 a 13 14", "%*d%*i%*o%*u%*x%*X%*e%*E%*f%*g%*G%*s%*[abc]%*c%*p");
			EATEST_VERIFY(n == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("0 1 2 3 4 5 6 7 8 9 10 11 a 13 14"), EA_CHAR16("%*d%*i%*o%*u%*x%*X%*e%*E%*f%*g%*G%*s%*[abc]%*c%*p"));
			EATEST_VERIFY(n == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("0 1 2 3 4 5 6 7 8 9 10 11 a 13 14"), EA_CHAR32("%*d%*i%*o%*u%*x%*X%*e%*E%*f%*g%*G%*s%*[abc]%*c%*p"));
			EATEST_VERIFY(n == 0);
		}

		{
			v.Clear();
			n = Sscanf("123", "%0d", &v.int_[0]);      // We should just ignore the field, as if it was %*d.
			EATEST_VERIFY(n == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("123"), EA_CHAR16("%0d"), &v.int_[0]);      // We should just ignore the field, as if it was %*d.
			EATEST_VERIFY(n == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("123"), EA_CHAR32("%0d"), &v.int_[0]);      // We should just ignore the field, as if it was %*d.
			EATEST_VERIFY(n == 0);
		}

		{
			v.Clear();
			n = Sscanf("-0.0 -0.0 -0.0 -0.0 -0.0 -0.0 -0.0 -0.0", "%e %E %f %F %g %G %f %f", &v.float_[0], &v.float_[1], &v.float_[2], &v.float_[3], &v.float_[4], &v.float_[5], &v.float_[6], &v.float_[7]);
			EATEST_VERIFY(n == 8);
			EATEST_VERIFY(v.float_[0] == -0.f);
			EATEST_VERIFY(v.float_[1] == -0.f);
			EATEST_VERIFY(v.float_[2] == -0.f);
			EATEST_VERIFY(v.float_[3] == -0.f);
			EATEST_VERIFY(v.float_[4] == -0.f);
			EATEST_VERIFY(v.float_[5] == -0.f);
			EATEST_VERIFY(v.float_[6] == -0.f);
			EATEST_VERIFY(v.float_[7] == -0.f);

			v.Clear();
			n = Sscanf(EA_CHAR16("-0.0 -0.0 -0.0 -0.0 -0.0 -0.0 -0.0 -0.0"), EA_CHAR16("%e %E %f %F %g %G %f %f"), &v.float_[0], &v.float_[1], &v.float_[2], &v.float_[3], &v.float_[4], &v.float_[5], &v.float_[6], &v.float_[7]);
			EATEST_VERIFY(n == 8);
			EATEST_VERIFY(v.float_[0] == -0.f);
			EATEST_VERIFY(v.float_[1] == -0.f);
			EATEST_VERIFY(v.float_[2] == -0.f);
			EATEST_VERIFY(v.float_[3] == -0.f);
			EATEST_VERIFY(v.float_[4] == -0.f);
			EATEST_VERIFY(v.float_[5] == -0.f);
			EATEST_VERIFY(v.float_[6] == -0.f);
			EATEST_VERIFY(v.float_[7] == -0.f);

			v.Clear();
			n = Sscanf(EA_CHAR32("-0.0 -0.0 -0.0 -0.0 -0.0 -0.0 -0.0 -0.0"), EA_CHAR32("%e %E %f %F %g %G %f %f"), &v.float_[0], &v.float_[1], &v.float_[2], &v.float_[3], &v.float_[4], &v.float_[5], &v.float_[6], &v.float_[7]);
			EATEST_VERIFY(n == 8);
			EATEST_VERIFY(v.float_[0] == -0.f);
			EATEST_VERIFY(v.float_[1] == -0.f);
			EATEST_VERIFY(v.float_[2] == -0.f);
			EATEST_VERIFY(v.float_[3] == -0.f);
			EATEST_VERIFY(v.float_[4] == -0.f);
			EATEST_VERIFY(v.float_[5] == -0.f);
			EATEST_VERIFY(v.float_[6] == -0.f);
			EATEST_VERIFY(v.float_[7] == -0.f);
		}

		{
			v.Clear();
			n = Sscanf("inF -inf nan -Nan INF -inf nan -NaN", "%e %E %f %F %g %G %f %f", &v.float_[0], &v.float_[1], &v.float_[2], &v.float_[3], &v.float_[4], &v.float_[5], &v.float_[6], &v.float_[7]);
			EATEST_VERIFY(n == 8);
			EATEST_VERIFY(IsInfinite(v.float_[0]));
			EATEST_VERIFY(IsInfinite(v.float_[1]));
			EATEST_VERIFY(IsNAN(v.float_[2]));
			EATEST_VERIFY(IsNAN(v.float_[3]));
			EATEST_VERIFY(IsInfinite(v.float_[4]));
			EATEST_VERIFY(IsInfinite(v.float_[5]));
			EATEST_VERIFY(IsNAN(v.float_[6]));
			EATEST_VERIFY(IsNAN(v.float_[7]));

			v.Clear();
			n = Sscanf(EA_CHAR16("inF -inf nan -Nan INF -inf nan -NaN"), EA_CHAR16("%e %E %f %F %g %G %f %f"), &v.float_[0], &v.float_[1], &v.float_[2], &v.float_[3], &v.float_[4], &v.float_[5], &v.float_[6], &v.float_[7]);
			EATEST_VERIFY(n == 8);
			EATEST_VERIFY(IsInfinite(v.float_[0]));
			EATEST_VERIFY(IsInfinite(v.float_[1]));
			EATEST_VERIFY(IsNAN(v.float_[2]));
			EATEST_VERIFY(IsNAN(v.float_[3]));
			EATEST_VERIFY(IsInfinite(v.float_[4]));
			EATEST_VERIFY(IsInfinite(v.float_[5]));
			EATEST_VERIFY(IsNAN(v.float_[6]));
			EATEST_VERIFY(IsNAN(v.float_[7]));

			v.Clear();
			n = Sscanf(EA_CHAR32("inF -inf nan -Nan INF -inf nan -NaN"), EA_CHAR32("%e %E %f %F %g %G %f %f"), &v.float_[0], &v.float_[1], &v.float_[2], &v.float_[3], &v.float_[4], &v.float_[5], &v.float_[6], &v.float_[7]);
			EATEST_VERIFY(n == 8);
			EATEST_VERIFY(IsInfinite(v.float_[0]));
			EATEST_VERIFY(IsInfinite(v.float_[1]));
			EATEST_VERIFY(IsNAN(v.float_[2]));
			EATEST_VERIFY(IsNAN(v.float_[3]));
			EATEST_VERIFY(IsInfinite(v.float_[4]));
			EATEST_VERIFY(IsInfinite(v.float_[5]));
			EATEST_VERIFY(IsNAN(v.float_[6]));
			EATEST_VERIFY(IsNAN(v.float_[7]));
		}

		// To do: Implement some of these:
		//n = Sscanf8_("%d", ip);
		//n = Sscanf8_("%*d");
		//n = Sscanf8_("%3d", ip);
		//n = Sscanf8_("%hd", hp);
		//n = Sscanf8_("%3ld", lp);
		//n = Sscanf8_("%*3d");
		//n = Sscanf8_("%d %ld", ip, lp);
		//n = Sscanf8_("%d%i%o%u%x%X%a%A%e%E%f%F%g%G%s%[abc]%c%p%n%%", ip, ip, uip, uip, uip,  uip, fp, fp, fp, fp, fp, fp, fp, fp, s, s, s, pp, n);
		//n = Sscanf8_("%*d%*i%*o%*u%*x%*X%*e%*E%*f%*g%*G%*s%*[abc]%*c%*p");
		//n = Sscanf8_("%*2d%*8s%*3c");
		//n = Sscanf8_("%*n", n);         // "suppress" "suppression of %n"
		//n = Sscanf8_("%*hd");           // "together" "suppression with length"
		//n = Sscanf8_("%le%lE%lf%lg%lG", dp, dp, dp, dp, dp);
		//n = Sscanf8_("%Le%LE%Lf%Lg%LG", ldp, ldp, ldp, ldp, ldp);
		//n = Sscanf8_("%d%i%o%u%x%X%e%E%f%g%G%s%[abc]%c%p%n%%", ip, ip, uip, uip, uip, uip, fp, fp, fp, fp, fp, s, s, s, pp, n);
		//n = Sscanf8_("%2s%3s%4c%5c%6[abc]%7[abc]", ss, us, ss, us, ss, us);
		//n = Sscanf8_("%[%d]%d", s, ip);       // This is valid. It searches for % and d.
		//n = Sscanf8_("%[^%d]%d", s, ip);
		//n = Sscanf8_("%[]%d]%d", s, ip);
		//n = Sscanf8_("%[^]%d]%d", s, ip);
		//n = Sscanf8_("%hhd", hhp);
		//n = Sscanf8_("%lld", llp);
		//n = Sscanf8_("%jd", jp);
		//n = Sscanf8_("%zu", zp);
		//n = Sscanf8_("%td", tp);
		//n = Sscanf8_("%F", fp);
		//n = Sscanf8_("%a", fp);
		//n = Sscanf8_("%A", fp);
		//n = Sscanf8_("%lc%ls%l[abc]", ls, ls, ls);
		//n = Sscanf8_("%*d%*i%*o%*u%*x%*X%*a%*A%*e%*E%*f%*F%*g%*G%*s%*[abc]%*c%*p");
		//n = Sscanf8_("%*2d%*8s%*3c"); 
		//n = Sscanf8_("%*n", n);       // We should leave the n parameter as-is.
		//n = Sscanf8_("%*hd");         // We should ignore the h.
		//n = Sscanf8_("%2d%3i%4o%5u%6x%7X%8a%9A%10e%11E%12f%13F%14g%15G%16s%3[abc]%4c%5p", ip, ip, uip, uip, uip, uip, fp, fp, fp, fp, fp, fp, fp, fp, s, s, s, pp);
		//n = Sscanf8_("%0d", ip);      // We should just ignore the field, as if it was %*d.
		//n = Sscanf8_("%3n", n);       // We should just ignore the 3. Possibly assert about it.

		// Modifiers
		{
			v.Clear();
			n = Sscanf("0 1 012 55555 0x4ff 0x5FF x", "%hd%hi%ho%hu%hx%hX%hn", &v.short_[0], &v.short_[1], &v.short_[2], &v.ushort_[3], &v.short_[4], &v.short_[5], &v.short_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.short_[0] == 0);
			EATEST_VERIFY(v.short_[1] == 1);
			EATEST_VERIFY(v.short_[2] == 012);
			EATEST_VERIFY(v.ushort_[3] == 55555);
			EATEST_VERIFY(v.short_[4] == 0x4ff);
			EATEST_VERIFY(v.short_[5] == 0x5FF);
			EATEST_VERIFY(v.short_[6] == 25);

			v.Clear();
			n = Sscanf(EA_CHAR16("0 1 012 55555 0x4ff 0x5FF x"), EA_CHAR16("%hd%hi%ho%hu%hx%hX%hn"), &v.short_[0], &v.short_[1], &v.short_[2], &v.ushort_[3], &v.short_[4], &v.short_[5], &v.short_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.short_[0] == 0);
			EATEST_VERIFY(v.short_[1] == 1);
			EATEST_VERIFY(v.short_[2] == 012);
			EATEST_VERIFY(v.ushort_[3] == 55555);
			EATEST_VERIFY(v.short_[4] == 0x4ff);
			EATEST_VERIFY(v.short_[5] == 0x5FF);
			EATEST_VERIFY(v.short_[6] == 25);

			v.Clear();
			n = Sscanf(EA_CHAR32("0 1 012 55555 0x4ff 0x5FF x"), EA_CHAR32("%hd%hi%ho%hu%hx%hX%hn"), &v.short_[0], &v.short_[1], &v.short_[2], &v.ushort_[3], &v.short_[4], &v.short_[5], &v.short_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.short_[0] == 0);
			EATEST_VERIFY(v.short_[1] == 1);
			EATEST_VERIFY(v.short_[2] == 012);
			EATEST_VERIFY(v.ushort_[3] == 55555);
			EATEST_VERIFY(v.short_[4] == 0x4ff);
			EATEST_VERIFY(v.short_[5] == 0x5FF);
			EATEST_VERIFY(v.short_[6] == 25);
		}

		{
			v.Clear();
			n = Sscanf("0 1 2 3 4 5 x", "%hhd%hhi%hho%hhu%hhx%hhX%hhn", &v.int8_[0], &v.int8_[1], &v.int8_[2], &v.int8_[3], &v.int8_[4], &v.int8_[5], &v.int8_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.int8_[0] == 0);
			EATEST_VERIFY(v.int8_[1] == 1);
			EATEST_VERIFY(v.int8_[2] == 2);
			EATEST_VERIFY(v.int8_[3] == 3);
			EATEST_VERIFY(v.int8_[4] == 4);
			EATEST_VERIFY(v.int8_[5] == 5);
			EATEST_VERIFY(v.int8_[6] == 11);

			v.Clear();
			n = Sscanf(EA_CHAR16("0 1 2 3 4 5 x"), EA_CHAR16("%hhd%hhi%hho%hhu%hhx%hhX%hhn"), &v.int8_[0], &v.int8_[1], &v.int8_[2], &v.int8_[3], &v.int8_[4], &v.int8_[5], &v.int8_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.int8_[0] == 0);
			EATEST_VERIFY(v.int8_[1] == 1);
			EATEST_VERIFY(v.int8_[2] == 2);
			EATEST_VERIFY(v.int8_[3] == 3);
			EATEST_VERIFY(v.int8_[4] == 4);
			EATEST_VERIFY(v.int8_[5] == 5);
			EATEST_VERIFY(v.int8_[6] == 11);

			v.Clear();
			n = Sscanf(EA_CHAR32("0 1 2 3 4 5 x"), EA_CHAR32("%hhd%hhi%hho%hhu%hhx%hhX%hhn"), &v.int8_[0], &v.int8_[1], &v.int8_[2], &v.int8_[3], &v.int8_[4], &v.int8_[5], &v.int8_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.int8_[0] == 0);
			EATEST_VERIFY(v.int8_[1] == 1);
			EATEST_VERIFY(v.int8_[2] == 2);
			EATEST_VERIFY(v.int8_[3] == 3);
			EATEST_VERIFY(v.int8_[4] == 4);
			EATEST_VERIFY(v.int8_[5] == 5);
			EATEST_VERIFY(v.int8_[6] == 11);
		}

		{
			v.Clear();
			n = Sscanf("0 1 2 3 4 5 z", "%ld%li%lo%lu%lx%lX%ln", &v.long_[0], &v.long_[1], &v.long_[2], &v.long_[3], &v.long_[4], &v.long_[5], &v.long_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.long_[0] == 0);
			EATEST_VERIFY(v.long_[1] == 1);
			EATEST_VERIFY(v.long_[2] == 2);
			EATEST_VERIFY(v.long_[3] == 3);
			EATEST_VERIFY(v.long_[4] == 4);
			EATEST_VERIFY(v.long_[5] == 5);
			EATEST_VERIFY(v.long_[6] == 11);

			v.Clear();
			n = Sscanf(EA_CHAR16("0 1 2 3 4 5 z"), EA_CHAR16("%ld%li%lo%lu%lx%lX%ln"), &v.long_[0], &v.long_[1], &v.long_[2], &v.long_[3], &v.long_[4], &v.long_[5], &v.long_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.long_[0] == 0);
			EATEST_VERIFY(v.long_[1] == 1);
			EATEST_VERIFY(v.long_[2] == 2);
			EATEST_VERIFY(v.long_[3] == 3);
			EATEST_VERIFY(v.long_[4] == 4);
			EATEST_VERIFY(v.long_[5] == 5);
			EATEST_VERIFY(v.long_[6] == 11);

			v.Clear();
			n = Sscanf(EA_CHAR32("0 1 2 3 4 5 z"), EA_CHAR32("%ld%li%lo%lu%lx%lX%ln"), &v.long_[0], &v.long_[1], &v.long_[2], &v.long_[3], &v.long_[4], &v.long_[5], &v.long_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.long_[0] == 0);
			EATEST_VERIFY(v.long_[1] == 1);
			EATEST_VERIFY(v.long_[2] == 2);
			EATEST_VERIFY(v.long_[3] == 3);
			EATEST_VERIFY(v.long_[4] == 4);
			EATEST_VERIFY(v.long_[5] == 5);
			EATEST_VERIFY(v.long_[6] == 11);
		}

		{
			v.Clear();
			n = Sscanf("1e-1\t 1e-2\t 123.456\t 234.567\t 123.456\t 234.567", "%le %lE %lf %lF %lg %lG", &v.double_[0], &v.double_[1], &v.double_[2], &v.double_[3], &v.double_[4], &v.double_[5]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.double_[0] == 1e-1);
			EATEST_VERIFY(v.double_[1] == 1e-2);
			EATEST_VERIFY(DoubleEqual(v.double_[2], 123.456));
			EATEST_VERIFY(DoubleEqual(v.double_[3], 234.567));
			EATEST_VERIFY(DoubleEqual(v.double_[4], 123.456));
			EATEST_VERIFY(DoubleEqual(v.double_[5], 234.567));

			v.Clear();
			n = Sscanf(EA_CHAR16("1e-1\t 1e-2\t 123.456\t 234.567\t 123.456\t 234.567"), EA_CHAR16("%le %lE %lf %lF %lg %lG"), &v.double_[0], &v.double_[1], &v.double_[2], &v.double_[3], &v.double_[4], &v.double_[5]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.double_[0] == 1e-1);
			EATEST_VERIFY(v.double_[1] == 1e-2);
			EATEST_VERIFY(DoubleEqual(v.double_[2], 123.456));
			EATEST_VERIFY(DoubleEqual(v.double_[3], 234.567));
			EATEST_VERIFY(DoubleEqual(v.double_[4], 123.456));
			EATEST_VERIFY(DoubleEqual(v.double_[5], 234.567));

			v.Clear();
			n = Sscanf(EA_CHAR32("1e-1\t 1e-2\t 123.456\t 234.567\t 123.456\t 234.567"), EA_CHAR32("%le %lE %lf %lF %lg %lG"), &v.double_[0], &v.double_[1], &v.double_[2], &v.double_[3], &v.double_[4], &v.double_[5]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.double_[0] == 1e-1);
			EATEST_VERIFY(v.double_[1] == 1e-2);
			EATEST_VERIFY(DoubleEqual(v.double_[2], 123.456));
			EATEST_VERIFY(DoubleEqual(v.double_[3], 234.567));
			EATEST_VERIFY(DoubleEqual(v.double_[4], 123.456));
			EATEST_VERIFY(DoubleEqual(v.double_[5], 234.567));
		}

		{
			// Test conversion of UTF8 sequences to wide strings.
			// This works under the Standard C library scanf only if it supports UTF8 and you set it to UTF8.
			v.Clear();
			n = Sscanf("a\xed\x9f\xbf_ \xed\x9f\xbf", "%I16s %I32s", v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.str16_[0][0] == 'a');
			EATEST_VERIFY(v.str16_[0][1] == 0xd7ff);
			EATEST_VERIFY(v.str16_[0][2] == '_');
			EATEST_VERIFY(v.str16_[0][3] == 0);
			EATEST_VERIFY(v.str32_[0][0] == 0xd7ff);
			EATEST_VERIFY(v.str32_[0][1] == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("a\x1234_ \x5678"), EA_CHAR16("%I16s %I32s"), v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.str16_[0][0] == 'a');
			EATEST_VERIFY(v.str16_[0][1] == 0x1234);
			EATEST_VERIFY(v.str16_[0][2] == '_');
			EATEST_VERIFY(v.str16_[0][3] == 0);
			EATEST_VERIFY(v.str32_[0][0] == 0x5678);
			EATEST_VERIFY(v.str32_[0][1] == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("a\x1234_ \x5678"), EA_CHAR32("%I16s %I32s"), v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.str16_[0][0] == 'a');
			EATEST_VERIFY(v.str16_[0][1] == 0x1234);
			EATEST_VERIFY(v.str16_[0][2] == '_');
			EATEST_VERIFY(v.str16_[0][3] == 0);
			EATEST_VERIFY(v.str32_[0][0] == 0x5678);
			EATEST_VERIFY(v.str32_[0][1] == 0);
		}

		{ // Test %[] 
			v.Clear();
			n = Sscanf("abcdefghij", "%h[ab]%l[cd]%I16[ef]%I32[gh]", v.str8_[0], v.strw_[0], v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(Strcmp(v.str8_[0],  "ab") == 0);
			EATEST_VERIFY(Strcmp(v.strw_[0],  EA_WCHAR("cd")) == 0);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("ef")) == 0);
			EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("gh")) == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("abcdefghij"), EA_CHAR16("%h[ab]%l[cd]%I16[ef]%I32[gh]"), v.str8_[0], v.strw_[0], v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(Strcmp(v.str8_[0],  "ab") == 0);
			EATEST_VERIFY(Strcmp(v.strw_[0],  EA_WCHAR("cd")) == 0);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("ef")) == 0);
			EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("gh")) == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("abcdefghij"), EA_CHAR32("%h[ab]%l[cd]%I16[ef]%I32[gh]"), v.str8_[0], v.strw_[0], v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY(Strcmp(v.str8_[0],  "ab") == 0);
			EATEST_VERIFY(Strcmp(v.strw_[0],  EA_WCHAR("cd")) == 0);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("ef")) == 0);
			EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("gh")) == 0);
		}

		{ // Test %[^] 
			v.Clear();
			n = Sscanf("abcdefghij", "%h[^cd]%l[^ef]%I16[^gh]%I32[^ij]", v.str8_[0], v.strw_[0], v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(Strcmp(v.str8_[0],  "ab") == 0);
			EATEST_VERIFY(Strcmp(v.strw_[0],  EA_WCHAR("cd")) == 0);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("ef")) == 0);
			EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("gh")) == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("abcdefghij"), EA_CHAR16("%h[^cd]%l[^ef]%I16[^gh]%I32[^ij]"), v.str8_[0], v.strw_[0], v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(Strcmp(v.str8_[0],  "ab") == 0);
			EATEST_VERIFY(Strcmp(v.strw_[0],  EA_WCHAR("cd")) == 0);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("ef")) == 0);
			EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("gh")) == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("abcdefghij"), EA_CHAR32("%h[^cd]%l[^ef]%I16[^gh]%I32[^ij]"), v.str8_[0], v.strw_[0], v.str16_[0], v.str32_[0]);
			EATEST_VERIFY(Strcmp(v.str8_[0],  "ab") == 0);
			EATEST_VERIFY(Strcmp(v.strw_[0],  EA_WCHAR("cd")) == 0);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("ef")) == 0);
			EATEST_VERIFY(Strcmp(v.str32_[0], EA_CHAR32("gh")) == 0);
		}

		{
			// Test conversion of UTF8 sequences to wide chars.
			// This works under the Standard C library only if you set it to UTF8.
			// As of this writing, we don't have support for this in our Scanf, and it is a 'to do' item.
			//v.Clear();
			//n = Sscanf("\xed\x9f\xbf\xed\x9f\xbf", "%I16c %I32c", &v.char16_[0], &v.char32_[0]);
			//EATEST_VERIFY(n == 2);
			//EATEST_VERIFY(v.char16_[0] == 0xd7ff);
			//EATEST_VERIFY(v.char32_[0] == 0xd7ff);

			//v.Clear();
			//n = Sscanf(EA_CHAR16("\xed\x9f\xbf\xed\x9f\xbf"), EA_CHAR16("%I16c %I32c"), &v.char16_[0], &v.char32_[0]);
			//EATEST_VERIFY(n == 2);
			//EATEST_VERIFY(v.char16_[0] == 0xd7ff);
			//EATEST_VERIFY(v.char32_[0] == 0xd7ff);

			//v.Clear();
			//n = Sscanf(EA_CHAR32("\xed\x9f\xbf\xed\x9f\xbf"), EA_CHAR32("%I16c %I32c"), &v.char16_[0], &v.char32_[0]);
			//EATEST_VERIFY(n == 2);
			//EATEST_VERIFY(v.char16_[0] == 0xd7ff);
			//EATEST_VERIFY(v.char32_[0] == 0xd7ff);
		}

		{
			v.Clear();
			n = Sscanf("-0 -1 012 3 10 20", "%lld%lli%llo%llu%llx%llX%lln", &v.longlong_[0], &v.longlong_[1], &v.longlong_[2], &v.longlong_[3], &v.longlong_[4], &v.longlong_[5], &v.longlong_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.longlong_[0] == -0);
			EATEST_VERIFY(v.longlong_[1] == -1);
			EATEST_VERIFY(v.longlong_[2] == 012);
			EATEST_VERIFY(v.longlong_[3] == 3);
			EATEST_VERIFY(v.longlong_[4] == 0x10);
			EATEST_VERIFY(v.longlong_[5] == 0x20);
			EATEST_VERIFY(v.longlong_[6] == 17);

			v.Clear();
			n = Sscanf(EA_CHAR16("-0 -1 012 3 10 20"), EA_CHAR16("%lld%lli%llo%llu%llx%llX%lln"), &v.longlong_[0], &v.longlong_[1], &v.longlong_[2], &v.longlong_[3], &v.longlong_[4], &v.longlong_[5], &v.longlong_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.longlong_[0] == -0);
			EATEST_VERIFY(v.longlong_[1] == -1);
			EATEST_VERIFY(v.longlong_[2] == 012);
			EATEST_VERIFY(v.longlong_[3] == 3);
			EATEST_VERIFY(v.longlong_[4] == 0x10);
			EATEST_VERIFY(v.longlong_[5] == 0x20);
			EATEST_VERIFY(v.longlong_[6] == 17);

			v.Clear();
			n = Sscanf(EA_CHAR32("-0 -1 012 3 10 20"), EA_CHAR32("%lld%lli%llo%llu%llx%llX%lln"), &v.longlong_[0], &v.longlong_[1], &v.longlong_[2], &v.longlong_[3], &v.longlong_[4], &v.longlong_[5], &v.longlong_[6]);
			EATEST_VERIFY(n == 6);
			EATEST_VERIFY(v.longlong_[0] == -0);
			EATEST_VERIFY(v.longlong_[1] == -1);
			EATEST_VERIFY(v.longlong_[2] == 012);
			EATEST_VERIFY(v.longlong_[3] == 3);
			EATEST_VERIFY(v.longlong_[4] == 0x10);
			EATEST_VERIFY(v.longlong_[5] == 0x20);
			EATEST_VERIFY(v.longlong_[6] == 17);
		}

		{
			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf("", "%d", &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);

			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf(EA_CHAR16(""), EA_CHAR16("%d"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);

			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf(EA_CHAR32(""), EA_CHAR32("%d"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
		}

		{
			v.Clear();
			n = Sscanf("0x519", "%x", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0x519);

			v.Clear();
			n = Sscanf(EA_CHAR16("0x519"), EA_CHAR16("%x"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0x519);

			v.Clear();
			n = Sscanf(EA_CHAR32("0x519"), EA_CHAR32("%x"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0x519);
		}

		{
			v.Clear();
			n = Sscanf("0x51ag", "%x", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0x51a);

			v.Clear();
			n = Sscanf(EA_CHAR16("0x51ag"), EA_CHAR16("%x"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0x51a);

			v.Clear();
			n = Sscanf(EA_CHAR32("0x51ag"), EA_CHAR32("%x"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0x51a);
		}

		{
			v.Clear();
			n = Sscanf("-1", "%x", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR16("-1"), EA_CHAR16("%x"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR32("-1"), EA_CHAR32("%x"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);
		}

		{
			v.Clear();
			n = Sscanf("\"%12@", "\"%%%d%%", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 12);

			v.Clear();
			n = Sscanf(EA_CHAR16("\"%12@"), EA_CHAR16("\"%%%d%%"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 12);

			v.Clear();
			n = Sscanf(EA_CHAR32("\"%12@"), EA_CHAR32("\"%%%d%%"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 12);
		}

		{
			v.Clear();
			n = Sscanf("1.1\t2.2", "%f\t%f", &v.float_[0], &v.float_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(FloatEqual(v.float_[0], 1.1f));
			EATEST_VERIFY(FloatEqual(v.float_[1], 2.2f));

			v.Clear();
			n = Sscanf(EA_CHAR16("1.1\t2.2"), EA_CHAR16("%f\t%f"), &v.float_[0], &v.float_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(FloatEqual(v.float_[0], 1.1f));
			EATEST_VERIFY(FloatEqual(v.float_[1], 2.2f));

			v.Clear();
			n = Sscanf(EA_CHAR32("1.1\t2.2"), EA_CHAR32("%f\t%f"), &v.float_[0], &v.float_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(FloatEqual(v.float_[0], 1.1f));
			EATEST_VERIFY(FloatEqual(v.float_[1], 2.2f));
		}

		{
			v.Clear(); // Skip one char then read a string that ends with \n (don't write the \n).
			n = Sscanf("  Hello World\n", "%*c%[^\n]", v.str8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.str8_[0], " Hello World") == 0);

			v.Clear();
			#if EASCANF_MS_STYLE_S_FORMAT
				n = Sscanf(EA_CHAR16("  Hello World\n"), EA_CHAR16("%*c%[^\n]"), v.strw_[0]);
				EATEST_VERIFY(n == 1);
				EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR(" Hello World")) == 0);
			#else
				n = Sscanf(EA_CHAR16("  Hello World\n"), EA_CHAR16("%*c%[^\n]"), v.str8_[0]);
				EATEST_VERIFY(n == 1);
				EATEST_VERIFY(Strcmp(v.str8_[0], " Hello World") == 0);
			#endif

			#if EASCANF_MS_STYLE_S_FORMAT
				n = Sscanf(EA_CHAR32("  Hello World\n"), EA_CHAR32("%*c%[^\n]"), v.strw_[0]);
				EATEST_VERIFY(n == 1);
				EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR(" Hello World")) == 0);
			#else
				n = Sscanf(EA_CHAR32("  Hello World\n"), EA_CHAR32("%*c%[^\n]"), v.str8_[0]);
				EATEST_VERIFY(n == 1);
				EATEST_VERIFY(Strcmp(v.str8_[0], " Hello World") == 0);
			#endif
		}

		{
			v.Clear(); // Skip all the chars in the [] range, then read one char.
			n = Sscanf("abcefgdh", "%*[a-cb-g]%c", &v.char8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char8_[0] == 'h');

			v.Clear();
			n = Sscanf(EA_CHAR16("abcefgdh"), EA_CHAR16("%*[a-cb-g]%I16c"), &v.char16_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char16_[0] == 'h');

			v.Clear();
			n = Sscanf(EA_CHAR32("abcefgdh"), EA_CHAR32("%*[a-cb-g]%I32c"), &v.char32_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char32_[0] == 'h');
		}

		{
			v.Clear(); // Skip all the chars in the [] range (exercizing d-d), then read one char.
			n = Sscanf("abcefgdh", "%*[a-cd-de-g]%c", &v.char8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char8_[0] == 'h');

			v.Clear();
			n = Sscanf(EA_CHAR16("abcefgdh"), EA_CHAR16("%*[a-cd-de-g]%I16c"), &v.char16_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char16_[0] == 'h');

			v.Clear();
			n = Sscanf(EA_CHAR32("abcefgdh"), EA_CHAR32("%*[a-cd-de-g]%I32c"), &v.char32_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char32_[0] == 'h');
		}

		{
			v.Clear();
			n = Sscanf("3:5:7", "%d%n:%n", &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 3);
			EATEST_VERIFY(v.int_[1] == 1);
			EATEST_VERIFY(v.int_[2] == 2);

			v.Clear();
			n = Sscanf(EA_CHAR16("3:5:7"), EA_CHAR16("%d%n:%n"), &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 3);
			EATEST_VERIFY(v.int_[1] == 1);
			EATEST_VERIFY(v.int_[2] == 2);

			v.Clear();
			n = Sscanf(EA_CHAR32("3:5:7"), EA_CHAR32("%d%n:%n"), &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 3);
			EATEST_VERIFY(v.int_[1] == 1);
			EATEST_VERIFY(v.int_[2] == 2);
		}

		{
			v.Clear(); // Skip a char, then report how many chars were read so far.
			n = Sscanf("5:7", "%*c%n", &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 1);

			v.Clear();
			n = Sscanf(EA_CHAR16("5:7"), EA_CHAR16("%*c%n"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 1);

			v.Clear();
			n = Sscanf(EA_CHAR32("5:7"), EA_CHAR32("%*c%n"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 1);
		}

		{
			v.Clear();
			n = Sscanf("-1", "%i", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR16("-1"), EA_CHAR16("%i"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR32("-1"), EA_CHAR32("%i"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);
		}

		{
			v.Clear();
			n = Sscanf("0xff", "%i", &v.int_[0]);   // %i differs from %d in that it acts like strtol with a base of 0 (auto-detect) as opposed to a base of 10.
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 255);

			v.Clear();
			n = Sscanf(EA_CHAR16("0xff"), EA_CHAR16("%i"), &v.int_[0]); // %i differs from %d in that it acts like strtol with a base of 0 (auto-detect) as opposed to a base of 10.
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 255);

			v.Clear();
			n = Sscanf(EA_CHAR32("0xff"), EA_CHAR32("%i"), &v.int_[0]); // %i differs from %d in that it acts like strtol with a base of 0 (auto-detect) as opposed to a base of 10.
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 255);
		}

		{
			v.Clear();
			n = Sscanf("017", "%i", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 017);         // See C99 7.20.1.4 p3 and p5, 6.4.4.1

			v.Clear();
			n = Sscanf(EA_CHAR16("017"), EA_CHAR16("%i"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 017);         // See C99 7.20.1.4 p3 and p5, 6.4.4.1

			v.Clear();
			n = Sscanf(EA_CHAR32("017"), EA_CHAR32("%i"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 017);         // See C99 7.20.1.4 p3 and p5, 6.4.4.1
		}

		{
			v.Clear();
			n = Sscanf("-1", "%d", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR16("-1"), EA_CHAR16("%d"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR32("-1"), EA_CHAR32("%d"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);
		}

		{
			v.Clear();
			n = Sscanf("0xff", "%d", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0);          // %d should not treat 0x sequences as hexidecimal.

			v.Clear();
			n = Sscanf(EA_CHAR16("0xff"), EA_CHAR16("%d"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0);          // %d should not treat 0x sequences as hexidecimal.

			v.Clear();
			n = Sscanf(EA_CHAR32("0xff"), EA_CHAR32("%d"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == 0);          // %d should not treat 0x sequences as hexidecimal.
		}

		{
			v.Clear();
			n = Sscanf("-1", "%o", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR16("-1"), EA_CHAR16("%o"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR32("-1"), EA_CHAR32("%o"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);
		}

		{
			v.Clear();
			n = Sscanf("-1", "%u", &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR16("-1"), EA_CHAR16("%u"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);

			v.Clear();
			n = Sscanf(EA_CHAR32("-1"), EA_CHAR32("%u"), &v.int_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.int_[0] == -1);
		}

		{
			v.Clear();
			n = Sscanf("xyzwa", "%hc%lc%I16c%I32c", &v.char8_[0], &v.wchar_[0], &v.char16_[0], &v.char32_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY((v.char8_[0] == 'x') && (v.wchar_[0] == 'y') && (v.char16_[0] == 'z') && (v.char32_[0] == 'w'));

			v.Clear();
			n = Sscanf(EA_CHAR16("xyzwa"), EA_CHAR16("%hc%lc%I16c%I32c"), &v.char8_[0], &v.wchar_[0], &v.char16_[0], &v.char32_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY((v.char8_[0] == 'x') && (v.wchar_[0] == 'y') && (v.char16_[0] == 'z') && (v.char32_[0] == 'w'));

			v.Clear();
			n = Sscanf(EA_CHAR16("xyzwa"), EA_CHAR16("%hc%lc%I16c%I32c"), &v.char8_[0], &v.wchar_[0], &v.char16_[0], &v.char32_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY((v.char8_[0] == 'x') && (v.wchar_[0] == 'y') && (v.char16_[0] == 'z') && (v.char32_[0] == 'w'));
		}

		{
			v.Clear();
			n = Sscanf(" xyz", "%c", &v.char8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char8_[0] == ' ');

			v.Clear();
			n = Sscanf(EA_CHAR16(" xyz"), EA_CHAR16("%I16c"), &v.char16_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char16_[0] == ' ');

			v.Clear();
			n = Sscanf(EA_CHAR32(" xyz"), EA_CHAR32("%I32c"), &v.char32_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char32_[0] == ' ');
		}

		{
			v.Clear();
			v.char8_[0] = (char)(uint8_t)0xdd;
			n = Sscanf("10:11", "%d:%d%c", &v.int_[0], &v.int_[1], &v.char8_[0]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 10);
			EATEST_VERIFY(v.int_[1] == 11);
			EATEST_VERIFY(v.char8_[0] == (char)(uint8_t)0xdd);

			v.Clear();
			#if EASCANF_MS_STYLE_S_FORMAT
				v.wchar_[0] = 0xdd;
				n = Sscanf(EA_CHAR16("10:11"), EA_CHAR16("%d:%d%c"), &v.int_[0], &v.int_[1], &v.wchar_[0]);
				EATEST_VERIFY(v.wchar_[0] == 0xdd);
			#else
				v.char8_[0] = (char)(uint8_t)0xdd;
				n = Sscanf(EA_CHAR16("10:11"), EA_CHAR16("%d:%d%c"), &v.int_[0], &v.int_[1], &v.char8_[0]);
				EATEST_VERIFY(v.char8_[0] == (char)(uint8_t)0xdd);
			#endif
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 10);
			EATEST_VERIFY(v.int_[1] == 11);

			v.Clear();
			#if EASCANF_MS_STYLE_S_FORMAT
				v.wchar_[0] = 0xdd;
				n = Sscanf(EA_CHAR32("10:11"), EA_CHAR32("%d:%d%c"), &v.int_[0], &v.int_[1], &v.wchar_[0]);
				EATEST_VERIFY(v.wchar_[0] == 0xdd);
			#else
				v.char8_[0] = (char)(uint8_t)0xdd;
				n = Sscanf(EA_CHAR32("10:11"), EA_CHAR32("%d:%d%c"), &v.int_[0], &v.int_[1], &v.char8_[0]);
				EATEST_VERIFY(v.char8_[0] == (char)(uint8_t)0xdd);
			#endif
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 10);
			EATEST_VERIFY(v.int_[1] == 11);
		}

		{
			v.Clear();
			n = Sscanf("abc   def", "%s %n%s", v.str8_[0], &v.int_[0], v.str8_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(Strcmp(v.str8_[0], "abc") == 0);
			EATEST_VERIFY(v.int_[0] == 6);
			EATEST_VERIFY(Strcmp(v.str8_[1], "def") == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("abc   def"), EA_CHAR16("%I16s %n%I32s"), v.str16_[0], &v.int_[0], v.str32_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("abc")) == 0);
			EATEST_VERIFY(v.int_[0] == 6);
			EATEST_VERIFY(Strcmp(v.str32_[1], EA_CHAR32("def")) == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("abc   def"), EA_CHAR32("%I16s %n%I32s"), v.str16_[0], &v.int_[0], v.str32_[1]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(Strcmp(v.str16_[0], EA_CHAR16("abc")) == 0);
			EATEST_VERIFY(v.int_[0] == 6);
			EATEST_VERIFY(Strcmp(v.str32_[1], EA_CHAR32("def")) == 0);
		}

		{
			v.Clear();
			n = Sscanf("1:23", "%d:%d%n", &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 1);
			EATEST_VERIFY(v.int_[1] == 23);
			EATEST_VERIFY(v.int_[2] == 4);

			v.Clear();
			n = Sscanf(EA_CHAR16("1:23"), EA_CHAR16("%d:%d%n"), &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 1);
			EATEST_VERIFY(v.int_[1] == 23);
			EATEST_VERIFY(v.int_[2] == 4);

			v.Clear();
			n = Sscanf(EA_CHAR32("1:23"), EA_CHAR32("%d:%d%n"), &v.int_[0], &v.int_[1], &v.int_[2]);
			EATEST_VERIFY(n == 2);
			EATEST_VERIFY(v.int_[0] == 1);
			EATEST_VERIFY(v.int_[1] == 23);
			EATEST_VERIFY(v.int_[2] == 4);
		}

		{
			v.Clear();
			n = Sscanf("_", "_%n%hn%hhn%ln%lln%I8n%I16n%I32n%I64n", &v.int_[0], &v.short_[0], &v.char_[0], &v.long_[0], &v.longlong_[0], &v.int8_[0], &v.int16_[0], &v.int32_[0], &v.int64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0]      == 1);
			EATEST_VERIFY(v.short_[0]    == 1);
			EATEST_VERIFY(v.char_[0]     == 1);
			EATEST_VERIFY(v.long_[0]     == 1);
			EATEST_VERIFY(v.longlong_[0] == 1);
			EATEST_VERIFY(v.int8_[0]     == 1);
			EATEST_VERIFY(v.int32_[0]    == 1);
			EATEST_VERIFY(v.int64_[0]    == 1);

			v.Clear();
			n = Sscanf(EA_CHAR16("_"), EA_CHAR16("_%n%hn%hhn%ln%lln%I8n%I16n%I32n%I64n"), &v.int_[0], &v.short_[0], &v.char_[0], &v.long_[0], &v.longlong_[0], &v.int8_[0], &v.int16_[0], &v.int32_[0], &v.int64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0]      == 1);
			EATEST_VERIFY(v.short_[0]    == 1);
			EATEST_VERIFY(v.char_[0]     == 1);
			EATEST_VERIFY(v.long_[0]     == 1);
			EATEST_VERIFY(v.longlong_[0] == 1);
			EATEST_VERIFY(v.int8_[0]     == 1);
			EATEST_VERIFY(v.int32_[0]    == 1);
			EATEST_VERIFY(v.int64_[0]    == 1);

			v.Clear();
			n = Sscanf(EA_CHAR32("_"), EA_CHAR32("_%n%hn%hhn%ln%lln%I8n%I32n%I32n%I64n"), &v.int_[0], &v.short_[0], &v.char_[0], &v.long_[0], &v.longlong_[0], &v.int8_[0], &v.int32_[0], &v.int32_[0], &v.int64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0]      == 1);
			EATEST_VERIFY(v.short_[0]    == 1);
			EATEST_VERIFY(v.char_[0]     == 1);
			EATEST_VERIFY(v.long_[0]     == 1);
			EATEST_VERIFY(v.longlong_[0] == 1);
			EATEST_VERIFY(v.int8_[0]     == 1);
			EATEST_VERIFY(v.int32_[0]    == 1);
			EATEST_VERIFY(v.int64_[0]    == 1);
		}
	}

	{
		// Bug report regression.
		// What was special about this case was that it represented an exponent of 
		// the power -10 (wouldn't happen with -9 or -11), and the code was mistakenly 
		// doing a test for >10 where it needed to do >=10.

		float a, b, c;
		int count = EA::StdC::Sscanf("0.1797734499 0.1797734499 0.1797734499", "%f %f %f", &a, &b, &c);

		EATEST_VERIFY((count == 3) && FloatEqual(a,  1.79773e-01f));
	}

	{
		float a;

		EA::StdC::Sscanf( ".750", "%f", &a);
		EATEST_VERIFY(FloatEqual(a,  .750f));

		EA::StdC::Sscanf(EA_CHAR16(".750"), EA_CHAR16("%f"), &a);
		EATEST_VERIFY(FloatEqual(a,  .750f));
	}

	{
		// Regresssion for Coverity report that mSigStr[-1] could occur (out of bounds). Actually, mSigStr[-1] was never read though a pointer to it was created.
		float a = 1.f;
		int count = EA::StdC::Sscanf(".", "%f", &a);    // This input caused mSigStr to be negatively indexed. 
		EATEST_VERIFY((count == 0) && (a == 1.f));      // VC++ sscanf returns a count of -1 (EOF), whereas glibc sscanf returns 0. The C11 Standard, section 7.21.6.2, example 3, seems to indicate that 0 is the proper return value.
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestScanfUnusual
//
static int TestScanfUnusual()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{ // Test unusual things

		Values v;
		int    n;

		{
			n = TestCRTVsscanf("", "");
			EATEST_VERIFY(n == 0);

			n = TestCRTVsscanf(EA_CHAR16(""), EA_CHAR16(""));
			EATEST_VERIFY(n == 0);

			n = TestCRTVsscanf(EA_CHAR32(""), EA_CHAR32(""));
			EATEST_VERIFY(n == 0);
		}

		{   
			// testing to see if the 'j' specifier
			// The test below is OK for both 32 and 64 bit pointer platforms.
			v.Clear();
			n = Sscanf("1111", "%jx", &v.uintmax_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.uintmax_[0] == 0x1111);
		}

		{
			// testing to see if the 'z' specifier
			// The test below is OK for both 32 and 64 bit pointer platforms.
			v.Clear();
			n = Sscanf("1111", "%zx", &v.size_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.size_[0] == 0x1111);
		}

		{
			// testing to see if the 't' specifier
			// The test below is OK for both 32 and 64 bit pointer platforms.
			v.Clear();
			n = Sscanf("1111", "%tx", &v.ptrdiff_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.ptrdiff_[0] == 0x1111);
		}

		{   // We should ignore the 'l' in front of 'p'
			// The test below is OK for both 32 and 64 bit pointer platforms.
			v.Clear();
			n = Sscanf("0xffffffff", "%lp", &v.voidptr_[0]);    
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0xffffffff);

			v.Clear();
			n = Sscanf(EA_CHAR16("0xffffffff"), EA_CHAR16("%lp"), &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0xffffffff);

			v.Clear();
			n = Sscanf(EA_CHAR32("0xffffffff"), EA_CHAR32("%lp"), &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0xffffffff);
		}

		{   // We should ignore the 'h' in front of 'p'
			// The test below is OK for both 32 and 64 bit pointer platforms.
			v.Clear();
			n = Sscanf("0xffffffff", "%hp", &v.voidptr_[0]);    
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0xffffffff);

			v.Clear();
			n = Sscanf(EA_CHAR16("0xffffffff"), EA_CHAR16("%hp"), &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0xffffffff);

			v.Clear();
			n = Sscanf(EA_CHAR32("0xffffffff"), EA_CHAR32("%hp"), &v.voidptr_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY((uintptr_t)v.voidptr_[0] == 0xffffffff);
		}

		{   // Make sure we support explicit char strings.
			v.Clear();
			n = Sscanf("2.0", "%hs", v.str8_[0]);    
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.str8_[0], "2.0") == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("2.0"), EA_CHAR16("%hs"), v.str8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.str8_[0], "2.0") == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("2.0"), EA_CHAR32("%hs"), v.str8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.str8_[0], "2.0") == 0);
		}

		{  // Make sure we support explicit wchar_t strings.
			v.Clear();
			n = Sscanf("2.0", "%ls", v.strw_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("2.0")) == 0);

			v.Clear();
			n = Sscanf(EA_CHAR16("2.0"), EA_CHAR16("%ls"), v.strw_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("2.0")) == 0);

			v.Clear();
			n = Sscanf(EA_CHAR32("2.0"), EA_CHAR32("%ls"), v.strw_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(Strcmp(v.strw_[0], EA_WCHAR("2.0")) == 0);
		}

		{   // Make sure we support explicit char chars.
			v.Clear();
			n = Sscanf("2.0", "%hc", &v.char8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char8_[0] == '2');

			v.Clear();
			n = Sscanf(EA_CHAR16("2.0"), EA_CHAR16("%hc"), &v.char8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char8_[0] == '2');

			v.Clear();
			n = Sscanf(EA_CHAR32("2.0"), EA_CHAR32("%hc"), &v.char8_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.char8_[0] == '2');
		}

		{   // Make sure we support explicit char16_t chars.
			v.Clear();
			n = Sscanf("2.0", "%lc", &v.wchar_[0]);    
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.wchar_[0] == '2');

			v.Clear();
			n = Sscanf(EA_CHAR16("2.0"), EA_CHAR16("%lc"), &v.wchar_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.wchar_[0] == '2');

			v.Clear();
			n = Sscanf(EA_CHAR32("2.0"), EA_CHAR32("%lc"), &v.wchar_[0]);
			EATEST_VERIFY(n == 1);
			EATEST_VERIFY(v.wchar_[0] == '2');
		}

		EA::Assert::FailureCallback originalCallback = EA::Assert::GetFailureCallback();
		EA::Assert::SetFailureCallback([](const char* expr, const char* filename, int line, const char* function, const char* msg, va_list args) -> bool { ++Values::assertCount_; return false; });

		// The following are invalid formats. They will result in assertion failures in Sscanf.
		{
			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf("%4", "%*%%d", &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf(EA_CHAR16("%4"), EA_CHAR16("%*%%d"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf(EA_CHAR32("%4"), EA_CHAR32("%*%%d"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);
		}

		{
			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf("%4", "%2%%d", &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf(EA_CHAR16("%4"), EA_CHAR16("%2%%d"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf(EA_CHAR32("%4"), EA_CHAR32("%2%%d"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);
		}

		{
			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf("%4", "%h%%d", &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf(EA_CHAR16("%4"), EA_CHAR16("%h%%d"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.int_[0] = 0xdd;
			n = Sscanf(EA_CHAR32("%4"), EA_CHAR32("%h%%d"), &v.int_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.int_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);
		}

		{
			v.Clear();
			Strcpy(v.str8_[0], "x");
			n = Sscanf("abcd", "%[]abcd", v.str8_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(Strcmp(v.str8_[0], "x") == 0);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			Strcpy(v.str8_[0], "x");
			n = Sscanf(EA_CHAR16("abcd"), EA_CHAR16("%[]abcd"), v.str8_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(Strcmp(v.str8_[0], "x") == 0);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			Strcpy(v.str8_[0], "x");
			n = Sscanf(EA_CHAR32("abcd"), EA_CHAR32("%[]abcd"), v.str8_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(Strcmp(v.str8_[0], "x") == 0);
			VERIFY_ASSERTCOUNT(1);
		}

		{
			v.Clear();
			v.float_[0] = -1.f;
			n = Sscanf("2.0", "%hf", &v.float_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.float_[0] == -1.f);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.float_[0] = -1.f;
			n = Sscanf(EA_CHAR16("2.0"), EA_CHAR16("%hf"), &v.float_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.float_[0] == -1.f);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.float_[0] = -1.f;
			n = Sscanf(EA_CHAR32("2.0"), EA_CHAR32("%hf"), &v.float_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.float_[0] == -1.f);
			VERIFY_ASSERTCOUNT(1);
		}

		/*{ Currently our Scanf supports %h in front of string formats, but it's not required to by the Standard. We should disable this support.
			v.Clear();
			Strcpy(v.str8_[0], "x");
			n = Sscanf("2.0", "%h[2.]", v.str8_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(Strcmp(v.str8_[0], "x") == 0);

			v.Clear();
			Strcpy(v.str8_[0], "x");
			n = Sscanf(EA_CHAR16("2.0"), EA_CHAR16("%h[2.]"), v.str8_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(Strcmp(v.str8_[0], "x") == 0);

			v.Clear();
			Strcpy(v.str8_[0], "x");
			n = Sscanf(EA_CHAR32("2.0"), EA_CHAR32("%h[2.]"), v.str8_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(Strcmp(v.str8_[0], "x") == 0);
		} */

		{
			v.Clear();
			v.uint64_[0] = 0xdd;
			n = Sscanf("abcd", "%h", &v.uint64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.uint64_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.uint64_[0] = 0xdd;
			n = Sscanf(EA_CHAR16("abcd"), EA_CHAR16("%h"), &v.uint64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.uint64_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.uint64_[0] = 0xdd;
			n = Sscanf(EA_CHAR32("abcd"), EA_CHAR32("%h"), &v.uint64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.uint64_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);
		}

		{
			v.Clear();
			v.uint64_[0] = 0xdd;
			n = Sscanf("abcd", "%l.", &v.uint64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.uint64_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.uint64_[0] = 0xdd;
			n = Sscanf(EA_CHAR16("abcd"), EA_CHAR16("%l."), &v.uint64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.uint64_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.uint64_[0] = 0xdd;
			n = Sscanf(EA_CHAR32("abcd"), EA_CHAR32("%l."), &v.uint64_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.uint64_[0] == 0xdd);
			VERIFY_ASSERTCOUNT(1);
		}

		{
			v.Clear();
			v.float_[0] = -1.f;
			n = Sscanf("2.0", "%zf", &v.float_[0]);
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.float_[0] == -1.f);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.float_[0] = -1.f;
			n = Sscanf("2.0", "%ze", &v.float_[0]);  //Invalid use of 'z' modifier with 'f' field specifier
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.float_[0] == -1.f);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.float_[0] = -1.f;
			n = Sscanf("2.0", "%zg", &v.float_[0]);  //Invalid use of 'z' modifier with 'g' field specifier
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.float_[0] == -1.f);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.float_[0] = -1.f;
			n = Sscanf("2.0", "%za", &v.float_[0]);  //Invalid use of 'z' modifier with 'a' field specifier
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.float_[0] == -1.f);
			VERIFY_ASSERTCOUNT(1);

			v.Clear();
			v.char_[0] = 'x';
			n = Sscanf("y", "%zc", &v.char_[0]);  //Invalid use of 'z' modifier with 'c' field specifier
			EATEST_VERIFY(n == 0);
			EATEST_VERIFY(v.char_[0] == 'x');
			VERIFY_ASSERTCOUNT(1);
		}

		EA::Assert::SetFailureCallback(originalCallback);
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestScanfVariants
//
static int TestScanfVariants()
{
	int nErrorCount = 0;

	{ // Test sscanf family variants.

		// To do: Implement some of these:
		/*
		EASTDC_API int Cscanf(ReadFunction8 pReadFunction8, void* pContext, const char* pFormat, ...);
		EASTDC_API int Fscanf(FILE* pFile, const char* pFormat, ...);
		EASTDC_API int Scanf(const char* pFormat, ...);
		EASTDC_API int Sscanf(const char*  pTextBuffer, const char* pFormat, ...);

		EASTDC_API int Cscanf(ReadFunction16 pReadFunction16, void* pContext, const char16_t* pFormat, ...);
		EASTDC_API int Fscanf(FILE* pFile, const char16_t* pFormat, ...);
		EASTDC_API int Scanf(const char16_t* pFormat, ...);
		EASTDC_API int Sscanf(const char16_t* pTextBuffer, const char16_t* pFormat, ...);

		EASTDC_API int Cscanf(ReadFunction32 pReadFunction32, void* pContext, const char32_t* pFormat, ...);
		EASTDC_API int Fscanf(FILE* pFile, const char32_t* pFormat, ...);
		EASTDC_API int Scanf(const char32_t* pFormat, ...);
		EASTDC_API int Sscanf(const char32_t* pTextBuffer, const char32_t* pFormat, ...);


		EASTDC_API int Vcscanf(ReadFunction8 pReadFunction8, void* pContext, const char* pFormat, va_list arguments);
		EASTDC_API int Vfscanf(FILE* pFile, const char* pFormat, va_list arguments);
		EASTDC_API int Vscanf(const char* pFormat, va_list arguments);
		EASTDC_API int Vsscanf(const char* pTextBuffer, const char* pFormat, va_list arguments);

		EASTDC_API int Vcscanf(ReadFunction16 pReadFunction16, void* pContext, const char16_t* pFormat, va_list arguments);
		EASTDC_API int Vfscanf(FILE* pFile, const char16_t* pFormat, va_list arguments);
		EASTDC_API int Vscanf(const char16_t* pFormat, va_list arguments);
		EASTDC_API int Vsscanf(const char16_t* pTextBuffer, const char16_t* pFormat, va_list arguments);

		EASTDC_API int Vcscanf(ReadFunction32 pReadFunction32, void* pContext, const char32_t* pFormat, va_list arguments);
		EASTDC_API int Vfscanf(FILE* pFile, const char32_t* pFormat, va_list arguments);
		EASTDC_API int Vscanf(const char32_t* pFormat, va_list arguments);
		EASTDC_API int Vsscanf(const char32_t* pTextBuffer, const char32_t* pFormat, va_list arguments);
		*/
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestScanfExtensions
//
static int TestScanfExtensions()
{
	int nErrorCount = 0;

	{ // Test sscanf extensions
		using namespace EA::StdC;

		int       n;
		Values    v;

		{
			n = Sscanf("127 -128 255", "%I8d %I8i %I8u", &v.int8_[0], &v.int8_[1], &v.uint8_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int8_[0] == INT8_MAX) && (v.int8_[1] == INT8_MIN) && (v.uint8_[2] == UINT8_MAX));

			n = Sscanf(EA_CHAR16("127 -128 255"), EA_CHAR16("%I8d %I8i %I8u"), &v.int8_[0], &v.int8_[1], &v.uint8_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int8_[0] == INT8_MAX) && (v.int8_[1] == INT8_MIN) && (v.uint8_[2] == UINT8_MAX));

			n = Sscanf(EA_CHAR32("127 -128 255"), EA_CHAR32("%I8d %I8i %I8u"), &v.int8_[0], &v.int8_[1], &v.uint8_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int8_[0] == INT8_MAX) && (v.int8_[1] == INT8_MIN) && (v.uint8_[2] == UINT8_MAX));
		}

		{
			n = Sscanf("32767 -32768 65535", "%I16d %I16i %I16u", &v.int16_[0], &v.int16_[1], &v.uint16_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int16_[0] == INT16_MAX));
			EATEST_VERIFY((v.int16_[1] == INT16_MIN));
			EATEST_VERIFY((v.uint16_[2] == UINT16_MAX));
			// The three lines above were added to replace the line below.  The comparisons are identical, but 
			// the line below fails on iPhone due to an apparent Clang compiler bug.
			//EATEST_VERIFY((v.int16_[0] == INT16_MAX) && (v.int16_[1] == INT16_MIN) && (v.uint16_[2] == UINT16_MAX));

			n = Sscanf(EA_CHAR16("32767 -32768 65535"), EA_CHAR16("%I16d %I16i %I16u"), &v.int16_[0], &v.int16_[1], &v.uint16_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int16_[0] == INT16_MAX));
			EATEST_VERIFY((v.int16_[1] == INT16_MIN));
			EATEST_VERIFY((v.uint16_[2] == UINT16_MAX));
			// The three lines above were added to replace the line below.  The comparisons are identical, but 
			// the line below fails on iPhone due to an apparent Clang compiler bug.
			//EATEST_VERIFY((v.int16_[0] == INT16_MAX) && (v.int16_[1] == INT16_MIN) && (v.uint16_[2] == UINT16_MAX));

			n = Sscanf(EA_CHAR32("32767 -32768 65535"), EA_CHAR32("%I16d %I16i %I16u"), &v.int16_[0], &v.int16_[1], &v.uint16_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int16_[0] == INT16_MAX));
			EATEST_VERIFY((v.int16_[1] == INT16_MIN));
			EATEST_VERIFY((v.uint16_[2] == UINT16_MAX));
			// The three lines above were added to replace the line below.  The comparisons are identical, but 
			// the line below fails on iPhone due to an apparent Clang compiler bug.
			//EATEST_VERIFY((v.int16_[0] == INT16_MAX) && (v.int16_[1] == INT16_MIN) && (v.uint16_[2] == UINT16_MAX));
		}

		{
			n = Sscanf("2147483647 -2147483648 4294967295", "%I32d %I32i %I32u", &v.int32_[0], &v.int32_[1], &v.uint32_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int32_[0] == INT32_MAX) && (v.int32_[1] == INT32_MIN) && (v.uint32_[2] == UINT32_MAX));

			n = Sscanf(EA_CHAR16("2147483647 -2147483648 4294967295"), EA_CHAR16("%I32d %I32i %I32u"), &v.int32_[0], &v.int32_[1], &v.uint32_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int32_[0] == INT32_MAX) && (v.int32_[1] == INT32_MIN) && (v.uint32_[2] == UINT32_MAX));

			n = Sscanf(EA_CHAR32("2147483647 -2147483648 4294967295"), EA_CHAR32("%I32d %I32i %I32u"), &v.int32_[0], &v.int32_[1], &v.uint32_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int32_[0] == INT32_MAX) && (v.int32_[1] == INT32_MIN) && (v.uint32_[2] == UINT32_MAX));
		}

		{
			n = Sscanf("9223372036854775807 -9223372036854775808 18446744073709551615", "%I64d %I64i %I64u", &v.int64_[0], &v.int64_[1], &v.uint64_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int64_[0] == INT64_MAX) && (v.int64_[1] == INT64_MIN) && (v.uint64_[2] == UINT64_MAX));

			n = Sscanf(EA_CHAR16("9223372036854775807 -9223372036854775808 18446744073709551615"), EA_CHAR16("%I64d %I64i %I64u"), &v.int64_[0], &v.int64_[1], &v.uint64_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int64_[0] == INT64_MAX) && (v.int64_[1] == INT64_MIN) && (v.uint64_[2] == UINT64_MAX));

			n = Sscanf(EA_CHAR32("9223372036854775807 -9223372036854775808 18446744073709551615"), EA_CHAR32("%I64d %I64i %I64u"), &v.int64_[0], &v.int64_[1], &v.uint64_[2]);
			EATEST_VERIFY(n == 3);
			EATEST_VERIFY((v.int64_[0] == INT64_MAX) && (v.int64_[1] == INT64_MIN) && (v.uint64_[2] == UINT64_MAX));
		}

		// We don't currently support %I128, though it could be done if needed. 
		// If somebody actually asks for this then it can be implemented. 
		// Otherwise it probably would just bloat the library with something nobody wants.
		// 
		// int128_t  int128_[4]; // int128_t has constructors and so is not a POD and cannot be part of the Values union.
		// uint128_t uint128_[4];
		// 
		// n = Sscanf(EA_CHAR16("170141183460469231731687303715884105727 -170141183460469231731687303715884105728 340282366920938463463374607431768211455"), EA_CHAR16("%I128d %I128i %I128u"), &int128_[0], &int128_[1], &uint128_[2]);
		// EATEST_VERIFY(n == 3);
		// EATEST_VERIFY((int128_[0]  == int128_t("170141183460469231731687303715884105727")) && 
		//               (int128_[1]  == int128_t("-170141183460469231731687303715884105728")) && 
		//               (uint128_[2] == uint128_t("340282366920938463463374607431768211455")));

		{   // Test sized %s
			// We could test %I8s, %I16s, %I32s, %I8c, %I16c, %I32c here, but we already 
			// test these a lot in the other code here.
		}

		{   // Test %b (binary)
			n = Sscanf("11010111 111011000101011 10110110111101100001110001100111 111010011101100000110100011111100111000011100111110011101110011", "%I8b %I16b %I32b %I64b", &v.uint8_[0], &v.uint16_[0], &v.uint32_[0], &v.uint64_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY((v.uint8_[0] == 0xd7) && (v.uint16_[0] == 0x762B) && (v.uint32_[0] == 0xB6F61C67) && (v.uint64_[0] == UINT64_C(0x74EC1A3F3873E773)));

			n = Sscanf(EA_CHAR16("11010111 111011000101011 10110110111101100001110001100111 111010011101100000110100011111100111000011100111110011101110011"), EA_CHAR16("%I8b %I16b %I32b %I64b"), &v.uint8_[0], &v.uint16_[0], &v.uint32_[0], &v.uint64_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY((v.uint8_[0] == 0xd7) && (v.uint16_[0] == 0x762B) && (v.uint32_[0] == 0xB6F61C67) && (v.uint64_[0] == UINT64_C(0x74EC1A3F3873E773)));

			n = Sscanf(EA_CHAR32("11010111 111011000101011 10110110111101100001110001100111 111010011101100000110100011111100111000011100111110011101110011"), EA_CHAR32("%I8b %I16b %I32b %I64b"), &v.uint8_[0], &v.uint16_[0], &v.uint32_[0], &v.uint64_[0]);
			EATEST_VERIFY(n == 4);
			EATEST_VERIFY((v.uint8_[0] == 0xd7) && (v.uint16_[0] == 0x762B) && (v.uint32_[0] == 0xB6F61C67) && (v.uint64_[0] == UINT64_C(0x74EC1A3F3873E773)));
		}

		{   // Test %Ld (same as %lld)
			// To do.
		}

		{   // Test %S handling
			// To do.
		}

		{   // Test %C handling
			// To do.
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestScanfErrors
//
static int TestScanfErrors()
{
	int nErrorCount = 0;

	{ // Test sscanf error situations
		// To do: Implement some of these:

		/*
		// Wrong number of arguments. 
		// Not possible to test at runtime.
		// sscanf("%d%d", ip);
		// sscanf("%d", ip, ip);

		// Various tests of bad argument types.  Some of these are only pedantic warnings.
		sscanf("%d", lp);          // "format" "bad argument types"
		sscanf("%d", uip);         // "format" "bad argument types"
		sscanf("%d", pp);          // "format" "bad argument types"
		sscanf("%p", ppc);         // "format" "bad argument types"
		sscanf("%p", ppv);         // "format" "bad argument types"
		sscanf("%s", n);           // "format" "bad argument types"
		sscanf("%s", p);           // "format" "bad argument types"
		sscanf("%p", p);           // "format" "bad argument types"
		sscanf("%p", sp);          // "format" "bad argument types"

		// Tests for writing into constant values. 
		sscanf("%d", cip);         // "constant" "%d writing into const"
		sscanf("%n", cn);          // "constant" "%n writing into const"
		sscanf("%s", cs);          // "constant" "%s writing into const"
		sscanf("%p", pcp);         // "constant" "%p writing into const"

		sscanf("");                // "zero-length" "warning for empty format"
		sscanf("\0");              // "embedded" "warning for embedded NUL"
		sscanf("%d\0", ip);        // "embedded" "warning for embedded NUL"
		sscanf("%d\0%d", ip, ip);  // "embedded|too many" "warning for embedded NUL"
		sscanf(NULL);              // "null" "null format string warning"
		sscanf("%");               // "trailing" "trailing % warning"
		sscanf("%d", (int *)0);    // "null" "writing into NULL"

		sscanf("%lla", fp); // "length" "bad use of %ll"
		sscanf("%llA", fp); // "length" "bad use of %ll"
		sscanf("%lle", fp); // "length" "bad use of %ll"
		sscanf("%llE", fp); // "length" "bad use of %ll"
		sscanf("%llf", fp); // "length" "bad use of %ll"
		sscanf("%llF", fp); // "length" "bad use of %ll"
		sscanf("%llg", fp); // "length" "bad use of %ll"
		sscanf("%llG", fp); // "length" "bad use of %ll"
		sscanf("%lls", s); // "length" "bad use of %ll"
		sscanf("%ll[ac]", s); // "length" "bad use of %ll"
		sscanf("%llc", s); // "length" "bad use of %ll"
		sscanf("%llp", pp); // "length" "bad use of %ll"
		sscanf("%jd%ji%jo%ju%jx%jX%jn", jp, jp, ujp, ujp, ujp, ujp, jn); /// "length" "bogus %j warning"
		sscanf("%ja", fp); // "length" "bad use of %j"
		sscanf("%jA", fp); // "length" "bad use of %j"
		sscanf("%je", fp); // "length" "bad use of %j"
		sscanf("%jE", fp); // "length" "bad use of %j"
		sscanf("%jf", fp); // "length" "bad use of %j"
		sscanf("%jF", fp); // "length" "bad use of %j"
		sscanf("%jg", fp); // "length" "bad use of %j"
		sscanf("%jG", fp); // "length" "bad use of %j"
		sscanf("%js", s); // "length" "bad use of %j"
		sscanf("%j[ac]", s); // "length" "bad use of %j"
		sscanf("%jc", s); // "length" "bad use of %j"
		sscanf("%jp", pp); // "length" "bad use of %j"
		sscanf("%zd%zi%zo%zu%zx%zX%zn", szp, szp, zp, zp, zp, zp, zn);
		sscanf("%za", fp); // "length" "bad use of %z"
		sscanf("%zA", fp); // "length" "bad use of %z"
		sscanf("%ze", fp); // "length" "bad use of %z"
		sscanf("%zE", fp); // "length" "bad use of %z"
		sscanf("%zf", fp); // "length" "bad use of %z"
		sscanf("%zF", fp); // "length" "bad use of %z"
		sscanf("%zg", fp); // "length" "bad use of %z"
		sscanf("%zG", fp); // "length" "bad use of %z"
		sscanf("%zs", s); // "length" "bad use of %z"
		sscanf("%z[ac]", s); // "length" "bad use of %z"
		sscanf("%zc", s); // "length" "bad use of %z"
		sscanf("%zp", pp); // "length" "bad use of %z"
		sscanf("%td%ti%to%tu%tx%tX%tn", tp, tp, utp, utp, utp, utp, tn);
		sscanf("%ta", fp); // "length" "bad use of %t"
		sscanf("%tA", fp); // "length" "bad use of %t"
		sscanf("%te", fp); // "length" "bad use of %t"
		sscanf("%tE", fp); // "length" "bad use of %t"
		sscanf("%tf", fp); // "length" "bad use of %t"
		sscanf("%tF", fp); // "length" "bad use of %t"
		sscanf("%tg", fp); // "length" "bad use of %t"
		sscanf("%tG", fp); // "length" "bad use of %t"
		sscanf("%ts", s); // "length" "bad use of %t"
		sscanf("%t[ac]", s); // "length" "bad use of %t"
		sscanf("%tc", s); // "length" "bad use of %t"
		sscanf("%tp", pp); // "length" "bad use of %t"
		sscanf("%La%LA%Le%LE%Lf%LF%Lg%LG", ldp, ldp, ldp, ldp, ldp, ldp, ldp, ldp);
		sscanf("%Ld", llp); // "does not support" "bad use of L"
		sscanf("%Li", llp); // "does not support" "bad use of L"
		sscanf("%Lo", ullp); // "does not support" "bad use of L"
		sscanf("%Lu", ullp); // "does not support" "bad use of L"
		sscanf("%Lx", ullp); // "does not support" "bad use of L"
		sscanf("%LX", ullp); // "does not support" "bad use of L"
		sscanf("%Ls", s); // "length" "bad use of L"
		sscanf("%L[ac]", s); // "length" "bad use of L"
		sscanf("%Lc", s); // "length" "bad use of L"
		sscanf("%Lp", pp); // "length" "bad use of L"
		sscanf("%Ln", n); // "length" "bad use of L"

		sscanf("%ha", fp); // "length" "bad use of %h"
		sscanf("%hA", fp); // "length" "bad use of %h"
		sscanf("%he", fp); // "length" "bad use of %h"
		sscanf("%hE", fp); // "length" "bad use of %h"
		sscanf("%hf", fp); // "length" "bad use of %h"
		sscanf("%hF", fp); // "length" "bad use of %h"
		sscanf("%hg", fp); // "length" "bad use of %h"
		sscanf("%hG", fp); // "length" "bad use of %h"
		sscanf("%hs", s); // "length" "bad use of %h"
		sscanf("%h[ac]", s); // "length" "bad use of %h"
		sscanf("%hc", s); // "length" "bad use of %h"
		sscanf("%hp", pp); // "length" "bad use of %h"
		sscanf("%hha", fp); // "length" "bad use of %hh"
		sscanf("%hhA", fp); // "length" "bad use of %hh"
		sscanf("%hhe", fp); // "length" "bad use of %hh"
		sscanf("%hhE", fp); // "length" "bad use of %hh"
		sscanf("%hhf", fp); // "length" "bad use of %hh"
		sscanf("%hhF", fp); // "length" "bad use of %hh"
		sscanf("%hhg", fp); // "length" "bad use of %hh"
		sscanf("%hhG", fp); // "length" "bad use of %hh"
		sscanf("%hhs", s); // "length" "bad use of %hh"
		sscanf("%hh[ac]", s); // "length" "bad use of %hh"
		sscanf("%hhc", s); // "length" "bad use of %hh"
		sscanf("%hhp", pp); // "length" "bad use of %hh"

		sscanf("%lp", pp); // "length" "bad use of %l"
		*/
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestScanf
//
int TestScanf()
{
	int nErrorCount = 0;

	nErrorCount += TestScanfLimits();
	nErrorCount += TestScanfMisc();
	nErrorCount += TestScanfUnusual();
	nErrorCount += TestScanfVariants();
	nErrorCount += TestScanfExtensions();
	nErrorCount += TestScanfErrors();

	return nErrorCount;
}



