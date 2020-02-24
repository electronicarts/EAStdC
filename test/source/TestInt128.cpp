///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////



#include <EABase/eabase.h>
#include <EAStdC/Int128_t.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

static bool CompareSelfTestResult(const char* p1, const char* p2)
{
	return strcmp(p1, p2) == 0;
}

int TestInt128()
{
	using namespace EA::StdC;

	int  nErrorCount(0);
	char array[256];
	bool bResult;

	{  // Test of small string assignment
		EA::StdC::int128_t x("10345");
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "10345"));
	}

	{  // Test of small string assignment
		EA::StdC::uint128_t x("10345");
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "10345"));
	}

	{  // Test of small string assignment
		EA::StdC::int128_t x("-10345");
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-10345"));
	}

	{  // Test of small string assignment
		EA::StdC::int128_t x("0xabcd1234fefe", 16);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00000000000000000000abcd1234fefe"));

		x.Int128ToStr(array, NULL, 16, EA::StdC::int128_t::kLZEnable, EA::StdC::int128_t::kPrefixEnable);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00000000000000000000abcd1234fefe"));

		x.Int128ToStr(array, NULL, 16, EA::StdC::int128_t::kLZDisable, EA::StdC::int128_t::kPrefixDisable);
		EATEST_VERIFY_F(CompareSelfTestResult(array, "abcd1234fefe"), "%s %s", array, "abcd1234fefe");

		x.Int128ToStr(array, NULL, 16,EA::StdC::int128_t:: kLZEnable, EA::StdC::int128_t::kPrefixDisable);
		EATEST_VERIFY(CompareSelfTestResult(array, "00000000000000000000abcd1234fefe"));

		x.Int128ToStr(array, NULL, 16, EA::StdC::int128_t::kLZDisable, EA::StdC::int128_t::kPrefixEnable);
		EATEST_VERIFY_F(CompareSelfTestResult(array, "0xabcd1234fefe"), "%s %s", array, "abcd1234fefe");
	}

	{  // Test of small string assignment
		EA::StdC::uint128_t x("0xabcd1234fefe", 16);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00000000000000000000abcd1234fefe"));

		x.Int128ToStr(array, NULL, 16, EA::StdC::uint128_t::kLZEnable, EA::StdC::uint128_t::kPrefixEnable);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00000000000000000000abcd1234fefe"));

		x.Int128ToStr(array, NULL, 16, EA::StdC::uint128_t::kLZDisable, EA::StdC::uint128_t::kPrefixDisable);
		EATEST_VERIFY_F(CompareSelfTestResult(array, "abcd1234fefe"), "%s %s", array, "abcd1234fefe");

		x.Int128ToStr(array, NULL, 16, EA::StdC::uint128_t::kLZEnable, EA::StdC::uint128_t::kPrefixDisable);
		EATEST_VERIFY(CompareSelfTestResult(array, "00000000000000000000abcd1234fefe"));

		x.Int128ToStr(array, NULL, 16, EA::StdC::uint128_t::kLZDisable, EA::StdC::uint128_t::kPrefixEnable);
		EATEST_VERIFY_F(CompareSelfTestResult(array, "0xabcd1234fefe"), "%s %s", array, "abcd1234fefe");
	}

	{  // Test of small string assignment
		EA::StdC::int128_t x("1010101010", 2);
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "1010101010"));

		x.Int128ToStr(array, NULL, 2, EA::StdC::int128_t::kLZEnable, EA::StdC::uint128_t::kPrefixEnable);
		EATEST_VERIFY(CompareSelfTestResult(array, "0b00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001010101010"));

		x.Int128ToStr(array, NULL, 2, EA::StdC::int128_t::kLZDisable, EA::StdC::uint128_t::kPrefixDisable);
		EATEST_VERIFY(CompareSelfTestResult(array, "1010101010"));

		x.Int128ToStr(array, NULL, 2, EA::StdC::int128_t::kLZEnable, EA::StdC::uint128_t::kPrefixDisable);
		EATEST_VERIFY(CompareSelfTestResult(array, "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001010101010"));

		x.Int128ToStr(array, NULL, 2, EA::StdC::int128_t::kLZDisable, EA::StdC::uint128_t::kPrefixEnable);
		EATEST_VERIFY(CompareSelfTestResult(array, "0b1010101010"));
	}

	{  // Test of small string assignment
		EA::StdC::uint128_t x("1010101010", 2);
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "1010101010"));

		x.Int128ToStr(array, NULL, 2, EA::StdC::uint128_t::kLZEnable, EA::StdC::uint128_t::kPrefixEnable);
		EATEST_VERIFY(CompareSelfTestResult(array, "0b00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001010101010"));

		x.Int128ToStr(array, NULL, 2, EA::StdC::uint128_t::kLZDisable, EA::StdC::uint128_t::kPrefixDisable);
		EATEST_VERIFY(CompareSelfTestResult(array, "1010101010"));

		x.Int128ToStr(array, NULL, 2, EA::StdC::uint128_t::kLZEnable, EA::StdC::uint128_t::kPrefixDisable);
		EATEST_VERIFY(CompareSelfTestResult(array, "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001010101010"));

		x.Int128ToStr(array, NULL, 2, EA::StdC::uint128_t::kLZDisable, EA::StdC::uint128_t::kPrefixEnable);
		EATEST_VERIFY(CompareSelfTestResult(array, "0b1010101010"));
	}

	{  // Test of large string assignment
		EA::StdC::int128_t x("141183460469231731687303715884105728");
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "141183460469231731687303715884105728"));
	}

	{  // Test of large string assignment
		EA::StdC::uint128_t x("141183460469231731687303715884105728");
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "141183460469231731687303715884105728"));
	}

	{  // Test of large string assignment
		EA::StdC::int128_t x("0xabcd1234fefeabcd123abcd1234fef", 16);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00abcd1234fefeabcd123abcd1234fef"));
	}

	{  // Test of large string assignment
		EA::StdC::uint128_t x("0xabcd1234fefeabcd123abcd1234fef", 16);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00abcd1234fefeabcd123abcd1234fef"));
	}

	{  // Test of large string assignment
		EA::StdC::int128_t x("1010101010111010111111111000000000001111111110101010101000100111101001010101", 2);
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "1010101010111010111111111000000000001111111110101010101000100111101001010101"));
	}

	{  // Test of large string assignment
		EA::StdC::uint128_t x("1010101010111010111111111000000000001111111110101010101000100111101001010101", 2);
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "1010101010111010111111111000000000001111111110101010101000100111101001010101"));
	}

	{  //Test of floating point assignment
		EA::StdC::int128_t x(4294967296.f);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "4294967296"));
	}

	{  //Test of floating point assignment
		EA::StdC::int128_t x(-12345672704.f);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-12345672704"));
	}

	{  //Test of floating point assignment
		EA::StdC::uint128_t x(0.0);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "0"));
	}

	{  //Test of floating point assignment
		EA::StdC::uint128_t x(3456345634563456.0);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "3456345634563456"));
	}

	{  //Test of floating point assignment
		EA::StdC::int128_t x(-123456345634563456.0);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-123456345634563456"));
	}

	{  // Test of EASTDC_INT128_MIN
		EA::StdC::int128_t x(EASTDC_INT128_MIN);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-170141183460469231731687303715884105728"));
	}

	{  // Test of EASTDC_INT128_MIN
		EA::StdC::int128_t x(EASTDC_INT128_MIN);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x80000000000000000000000000000000"));
	}

	{  // Test of EASTDC_UINT128_MIN
		EA::StdC::uint128_t x(EASTDC_UINT128_MIN);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "0"));
	}

	{  // Test of EASTDC_INT128_MAX
		EA::StdC::int128_t x(EASTDC_INT128_MAX);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "170141183460469231731687303715884105727"));
	}

	{  // Test of EASTDC_INT128_MAX
		EA::StdC::int128_t x(EASTDC_INT128_MAX);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x7fffffffffffffffffffffffffffffff"));
	}

	{  // Test of EASTDC_UINT128_MAX
		EA::StdC::uint128_t x(EASTDC_UINT128_MAX);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0xffffffffffffffffffffffffffffffff"));
	}

	{  // Test of unary operator-
		EA::StdC::int128_t x("123456781234567812345678");
		x = -x;
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-123456781234567812345678"));
	}

	{  // Test of unary operator-
		EA::StdC::int128_t x("-123456781234567812345678");
		x = -x;
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "123456781234567812345678"));
	}

	{  // Test of unary operator-
		EA::StdC::int128_t x("1234");
		x = -x;
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-1234"));
	}

	{  // Test of unary operator-
		EA::StdC::uint128_t x("-1234");
		x = -x;
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "1234"));
	}

	{  // Test of unary operator+
		EA::StdC::int128_t x("123456781234567812345678");
		x = +x;
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "123456781234567812345678"));
	}

	{  // Test of unary operator+
		EA::StdC::uint128_t x("123456781234567812345678");
		x = +x;
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "123456781234567812345678"));
	}

	{  // Test of unary operator+
		EA::StdC::int128_t x("-123456781234567812345678");
		x = +x;
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-123456781234567812345678"));
	}

	{  // Test of unary operator>>
		EA::StdC::int128_t x("0x77777777000000008888888800000000", 16);
		x >>= 32;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00000000777777770000000088888888"));
	}

	{  // Test of unary operator>>
		EA::StdC::uint128_t x("0x77777777000000008888888800000000", 16);
		x >>= 32;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00000000777777770000000088888888"));
	}

	{  // Test of unary operator>>
		EA::StdC::int128_t x("0x77777777000000008888888800000000", 16);
		x >>= -16;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x77770000000088888888000000000000"));
	}

	{  // Test of unary operator>>
		EA::StdC::uint128_t x("0x77777777000000008888888800000000", 16);
		x >>= -16;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x77770000000088888888000000000000"));
	}

	{  // Test of unary operator<<
		EA::StdC::int128_t x("0x77777777000000008888888800000000", 16);
		x <<= 32;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00000000888888880000000000000000"));
	}

	{  // Test of unary operator<<
		EA::StdC::uint128_t x("0x77777777000000008888888800000000", 16);
		x <<= 32;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00000000888888880000000000000000"));
	}

	{  // Test of unary operator>>
		EA::StdC::int128_t x("0x77777777000000008888888800000000", 16);
		x <<= -16;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00007777777700000000888888880000"));
	}

	{  // Test of unary operator>>
		EA::StdC::uint128_t x("0x77777777000000008888888800000000", 16);
		x <<= -16;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x00007777777700000000888888880000"));
	}

	{  // Test of unary operator!
		EA::StdC::int128_t x("0");
		bResult = !x;
		EATEST_VERIFY(bResult);
	}

	{  // Test of unary operator!
		EA::StdC::uint128_t x("0");
		bResult = !x;
		EATEST_VERIFY(bResult);
	}

	{  // Test of unary operator!
		EA::StdC::int128_t x("-1");
		bResult = !x;
		EATEST_VERIFY(!bResult);
	}

	{  // Test of unary operator!
		EA::StdC::uint128_t x("-1");
		bResult = !x;
		EATEST_VERIFY(!bResult);
	}

	{  // Test of unary operator~
		EA::StdC::int128_t x("0x77777777000000008888888800000000", 16);
		x = ~x;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x88888888ffffffff77777777ffffffff"));
	}

	{  // Test of unary operator~
		EA::StdC::uint128_t x("0x77777777000000008888888800000000", 16);
		x = ~x;
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x88888888ffffffff77777777ffffffff"));
	}

	{  // Test of operator^
		EA::StdC::int128_t x("1010101010101010101010101010101010101010101010101010101010101010", 2);
		EA::StdC::int128_t y("0101010101010101010101010101010101111111111111111111111111111111", 2);
		x = x ^ y;
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "1111111111111111111111111111111111010101010101010101010101010101"));
	}

	{  // Test of operator^
		EA::StdC::uint128_t x("1010101010101010101010101010101010101010101010101010101010101010", 2);
		EA::StdC::uint128_t y("0101010101010101010101010101010101111111111111111111111111111111", 2);
		x = x ^ y;
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "1111111111111111111111111111111111010101010101010101010101010101"));
	}

	{  // Test of operator|
		EA::StdC::int128_t x("1010101010101010101010101010101010101010101010101010101010101010", 2);
		EA::StdC::int128_t y("0101010101010101010101010101010101111111111111111111111111111111", 2);
		x = x | y;
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "1111111111111111111111111111111111111111111111111111111111111111"));
	}

	{  // Test of operator|
		EA::StdC::uint128_t x("1010101010101010101010101010101010101010101010101010101010101010", 2);
		EA::StdC::uint128_t y("0101010101010101010101010101010101111111111111111111111111111111", 2);
		x = x | y;
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "1111111111111111111111111111111111111111111111111111111111111111"));
	}

	{  // Test of operator&
		EA::StdC::int128_t x("1010101010101010101010101010101010101010101010101010101010101010", 2);
		EA::StdC::int128_t y("0101010101010101010101010101010101111111111111111111111111111111", 2);
		x = x & y;
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "101010101010101010101010101010"));
	}

	{  // Test of operator&
		EA::StdC::uint128_t x("1010101010101010101010101010101010101010101010101010101010101010", 2);
		EA::StdC::uint128_t y("0101010101010101010101010101010101111111111111111111111111111111", 2);
		x = x & y;
		x.Int128ToStr(array, NULL, 2);
		EATEST_VERIFY(CompareSelfTestResult(array, "101010101010101010101010101010"));
	}

	{  // Test of operator==
		EA::StdC::int128_t x("123456781234567812345678");
		EA::StdC::int128_t y("123456781234567812345678");
		bResult = (x == y);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator==
		EA::StdC::uint128_t x("123456781234567812345678");
		EA::StdC::uint128_t y("123456781234567812345678");
		bResult = (x == y);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator==
		EA::StdC::int128_t x("123456781234567812345678");
		bResult = (x == 100);
		EATEST_VERIFY(!bResult);
	}

	{  // Test of operator==
		EA::StdC::uint128_t x("123456781234567812345678");
		bResult = (x == 100L);
		EATEST_VERIFY(!bResult);
	}

	{  // Test of operator!=
		EA::StdC::int128_t x("123123123123123123");
		EA::StdC::int128_t y("123123123123123123");
		bResult = (x != y);
		EATEST_VERIFY(!bResult);
	}

	 {  // Test of operator!=
		 EA::StdC::uint128_t x("123123123123123123");
		 EA::StdC::uint128_t y("123123123123123123");
		bResult = (x != y);
		EATEST_VERIFY(!bResult);
	}

	{  // Test of operator!=
		EA::StdC::int128_t x("123456781234567812345678");
		bResult = (x != 0x12341234L);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator!=
		EA::StdC::uint128_t x("123456781234567812345678");
		bResult = (x != 0x12341234L);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>
		EA::StdC::int128_t x("1000");
		EA::StdC::int128_t y("2000");
		bResult = (x > y);
		EATEST_VERIFY(!bResult);
	}

	{  // Test of operator>
		EA::StdC::uint128_t x("1000");
		EA::StdC::uint128_t y("2000");
		bResult = (x > y);
		EATEST_VERIFY(!bResult);
	}

	{  // Test of operator>
		EA::StdC::int128_t x("9999999999999999999999999999999");
		EA::StdC::int128_t y("8888888888888888888888888888888");
		bResult = (x > y);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>
		EA::StdC::uint128_t x("9999999999999999999999999999999");
		EA::StdC::uint128_t y("8888888888888888888888888888888");
		bResult = (x > y);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>
		EA::StdC::int128_t x("-9999999999999999999999999999999");
		EA::StdC::int128_t y("-8888888888888888888888888888888");
		bResult = (x > y);
		EATEST_VERIFY(!bResult);
	}

	{  // Test of operator>
		EA::StdC::int128_t x("-1000");
		EA::StdC::int128_t y("-2000");
		bResult = (x > y);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>
		EA::StdC::int128_t x("1000");
		EA::StdC::int128_t y("-2000");
		bResult = (x > y);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>
		EA::StdC::int128_t x("-1000");
		EA::StdC::int128_t y("2000");
		bResult = (x > y);
		EATEST_VERIFY(!bResult);
	}

	{  // Test of operator>=
		EA::StdC::int128_t x("123456781234567812345678");
		EA::StdC::int128_t y("123456781234567812345678");
		bResult = (x >= y);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>=
		EA::StdC::uint128_t x("123456781234567812345678");
		EA::StdC::uint128_t y("123456781234567812345678");
		bResult = (x >= y);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>=
		EA::StdC::int128_t x("-12345678");
		bResult = ((int32_t)-12345678 >= x);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>=
		EA::StdC::uint128_t x("12345678");
		bResult = ((int32_t)12345679 >= x);
		EATEST_VERIFY(bResult);
	}

	{  // Test of operator>=
		EA::StdC::uint128_t x("12345679");
		bResult = ((int32_t)1234567 >= x);
		EATEST_VERIFY(!bResult);
	}

	{  // Test of AsBool
		EA::StdC::int128_t x("0");
		bResult = (x.AsBool());
		EATEST_VERIFY(!bResult);
	}

	{  // Test of AsBool
		EA::StdC::uint128_t x("0");
		bResult = (x.AsBool());
		EATEST_VERIFY(!bResult);
	}

	{  // Test of AsBool
		EA::StdC::int128_t x("-500");
		bResult = (x.AsBool());
		EATEST_VERIFY(bResult);
	}


	{  // Test of AsInt8
		EA::StdC::int128_t x("20");
		bResult = (x.AsInt8() == int8_t(20));
		EATEST_VERIFY(bResult);
	}

	{  // Test of AsInt8
		EA::StdC::uint128_t x("20");
		bResult = (x.AsInt8() == int8_t(20));
		EATEST_VERIFY(bResult);
	}

	{  // Test of AsInt8
		uint64_t x64 = UINT64_C(0x3333333322222222);
		bResult = ((int8_t)x64 == int8_t(0x22));
		EATEST_VERIFY(bResult);
	}
	{
		uint64_t x64 = UINT64_C(0x9999999922222222);
		bResult = ((int8_t)x64 == int8_t(0x22));
		EATEST_VERIFY(bResult);
	}
	{
		EA::StdC::uint128_t x("0x55555555444444443333333322222222", 16);
		bResult = (x.AsInt8() == int8_t(0x22));
		EATEST_VERIFY(bResult);
	}

	{  // Test of AsInt8
		EA::StdC::int128_t x("-20");
		bResult = (x.AsInt8() == int8_t(-20));
		EATEST_VERIFY(bResult);
	}


	{  // Test of AsInt64
		EA::StdC::int128_t x("18374403898749255808");
		#if !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ >= 4)
			bResult = (x.AsUint64() == UINT64_C(18374403898749255808));
		#else
			bResult = (x.AsUint64() == 0xFEFEFEFE80808080ULL);            
		#endif
		EATEST_VERIFY(bResult);
	}

	{  // Test of AsInt64
		EA::StdC::uint128_t x("18374403898749255808");
		#if !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ >= 4)
			bResult = (x.AsUint64() == UINT64_C(18374403898749255808));
		#else
			bResult = (x.AsUint64() == 0xFEFEFEFE80808080ULL);
		#endif
		EATEST_VERIFY(bResult);
	}

	{  // Test of AsInt64
		EA::StdC::int128_t x("-9223372036854775808");
		bResult = (x.AsInt64() == INT64_MIN);
		EATEST_VERIFY(bResult);
	}

	{  // Test of AsInt64
		EA::StdC::uint128_t x("8374403898749255808");
		#if !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ >= 4)
			bResult = (x.AsInt64() == INT64_C(8374403898749255808));
		#else
			bResult = (x.AsInt64() == (int64_t)0x7437DBF9F6988080LL);
		#endif
		
		EATEST_VERIFY(bResult);
	}

	{  // Test of AsFloat
		EA::StdC::int128_t x("18374403898749255808");
		volatile float actual = x.AsFloat();                // This prevents the FPU from cheating by using higher internal precision.
		#if !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ >= 4)
			volatile float desired = float(UINT64_C(18374403898749255808));
		#else
			volatile float desired = float(0xFEFEFEFE80808080ULL);
		#endif
		bResult = FloatEqual(actual, desired);

		EATEST_VERIFY(bResult);
	}

	{  // Test of AsFloat
		EA::StdC::uint128_t x("18374403898749255808");
		volatile float actual = x.AsFloat();                // This prevents the FPU from cheating by using higher internal precision.
		#if !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ >= 4)
			volatile float desired = float(UINT64_C(18374403898749255808));
		#else
			volatile float desired = float(0xFEFEFEFE80808080ULL);
		#endif
		bResult = FloatEqual(actual, desired);
		EATEST_VERIFY(bResult);
	}

	{  // Test of AsFloat
		EA::StdC::int128_t x("-18374403898749255808");
		volatile float actual = x.AsFloat();                 // This prevents the FPU from cheating by using higher internal precision.
		#if !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ >= 4)
			volatile float desired = -float(UINT64_C(18374403898749255808));
		#else
			volatile float desired = -float(0xFEFEFEFE80808080ULL);
		#endif
		bResult = FloatEqual(actual, desired);
		EATEST_VERIFY(bResult);
	}

	{
		// Test utility functions
		// int      GetBit(int nIndex) const;
		// void     SetBit(int nIndex, int value);
		// uint8_t  GetPartUint8 (int nIndex) const;
		// uint16_t GetPartUint16(int nIndex) const;
		// uint32_t GetPartUint32(int nIndex) const;
		// uint64_t GetPartUint64(int nIndex) const;
		// void     SetPartUint8 (int nIndex, uint8_t  value);
		// void     SetPartUint16(int nIndex, uint16_t value);
		// void     SetPartUint32(int nIndex, uint32_t value);
		// void     SetPartUint64(int nIndex, uint64_t value);
		// bool     IsZero() const;
		// void     SetZero();
		// void     TwosComplement();
		// void     InverseTwosComplement();

		EA::StdC::int128_t x(0);

		// uint8_t  GetPartUint8 (int nIndex) const;
		// void     SetPartUint8 (int nIndex, uint8_t  value);
		for(uint8_t i8 = 0; (uint8_t)(i8 < 16/sizeof(uint8_t)); i8++)
			x.SetPartUint8((int)i8, i8);
		for(uint8_t i8 = 0; (uint8_t)(i8 < 16/sizeof(uint8_t)); i8++)
			EATEST_VERIFY(x.GetPartUint8((int)i8) == i8);

		// uint16_t GetPartUint16(int nIndex) const;
		// void     SetPartUint16(int nIndex, uint16_t value);
		for(uint16_t i16 = 0; i16 < (uint16_t)(16/sizeof(uint16_t)); i16++)
			x.SetPartUint16((int)i16, i16);
		for(uint16_t i16 = 0; i16 < (uint16_t)(16/sizeof(uint16_t)); i16++)
			EATEST_VERIFY(x.GetPartUint16((int)i16) == i16);

		// uint32_t GetPartUint32(int nIndex) const;
		// void     SetPartUint32(int nIndex, uint32_t value);
		for(uint32_t i32 = 0; (uint32_t)(i32 < 16/sizeof(uint32_t)); i32++)
			x.SetPartUint32((int)i32, i32);
		for(uint32_t i32 = 0; (uint32_t)(i32 < 16/sizeof(uint32_t)); i32++)
			EATEST_VERIFY(x.GetPartUint32((int)i32) == i32);

		// uint64_t GetPartUint64(int nIndex) const;
		// void     SetPartUint64(int nIndex, uint64_t value);
		for(uint64_t i64 = 0; i64 < (uint64_t)(16/sizeof(uint64_t)); i64++)
			x.SetPartUint64((int)i64, i64);
		for(uint64_t i64 = 0; i64 < (uint64_t)(16/sizeof(uint64_t)); i64++)
			EATEST_VERIFY(x.GetPartUint64((int)i64) == i64);


		x = EA::StdC::int128_t("0x11223344556677880123456789ABCDEF", 0);

		// uint8_t  GetPartUint8 (int nIndex) const;
		bResult = (x.GetPartUint8(0) == 0xEF);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(1) == 0xCD);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(2) == 0xaB);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(3) == 0x89);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(4) == 0x67);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(5) == 0x45);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(6) == 0x23);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(7) == 0x01);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(8) == 0x88);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(9) == 0x77);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(10) == 0x66);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(11) == 0x55);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(12) == 0x44);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(13) == 0x33);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(14) == 0x22);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint8(15) == 0x11);
		EATEST_VERIFY(bResult);

		// uint16_t GetPartUint16(int nIndex) const;
		bResult = (x.GetPartUint16(0) == 0xCDEF);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint16(1) == 0x89AB);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint16(2) == 0x4567);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint16(3) == 0x0123);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint16(4) == 0x7788);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint16(5) == 0x5566);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint16(6) == 0x3344);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint16(7) == 0x1122);
		EATEST_VERIFY(bResult);

		// uint32_t GetPartUint32(int nIndex) const;
		bResult = (x.GetPartUint32(0) == 0x89ABCDEF);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint32(1) == 0x01234567);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint32(2) == 0x55667788);
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint32(3) == 0x11223344);
		EATEST_VERIFY(bResult);

		// uint64_t GetPartUint64(int nIndex) const;
		bResult = (x.GetPartUint64(0) == UINT64_C(0x0123456789ABCDEF));
		EATEST_VERIFY(bResult);

		bResult = (x.GetPartUint64(1) == UINT64_C(0x1122334455667788));
		EATEST_VERIFY(bResult);

		// bool     IsZero() const;
		// void     SetZero();
		bResult = x.IsZero();
		EATEST_VERIFY(!bResult);

		x.SetZero();
		bResult = x.IsZero();
		EATEST_VERIFY(bResult);
		bResult = (x.GetPartUint64(0) == 0 && x.GetPartUint64(1) == 0);
		EATEST_VERIFY(bResult);


		// int      GetBit(int nIndex) const;
		// void     SetBit(int nIndex, int value);
		x = EA::StdC::int128_t("0b10101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010", 0);

		for(int i = 0; i < 128; i++)
			EATEST_VERIFY(x.GetBit(i) == (i % 2));
		for(int i = 0; i < 128; i++)
			x.SetBit(i, x.GetBit(i) ? 0 : 1);
		for(int i = 0; i < 128; i++)
			EATEST_VERIFY(x.GetBit(i) == ((i + 1) % 2));

		// void     TwosComplement();
		// void     InverseTwosComplement();
		x = EA::StdC::int128_t(1);
		x.TwosComplement();
		EATEST_VERIFY(x == -1);

		x.InverseTwosComplement();
		EATEST_VERIFY(x == 1);
	}

	{  // Test of operator += / -=
		EA::StdC::int128_t y("-10000000000000000");
		y += 3000L;
		y -= 3000.0;
		y += UINT64_C(3000);
		y -= 3000.f;
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-10000000000000000"));
	}

	{  // Test of math
		EA::StdC::int128_t y;
		y = 120L * EA::StdC::int128_t("-10000000000000000");
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-1200000000000000000"));
	}

	{  // Test of math
		EA::StdC::int128_t  x("-10000000000000000");
		EA::StdC::uint128_t y(120L);
		x = x * y;
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-1200000000000000000"));
	}

	{  // Test of math
		EA::StdC::int128_t x((int64_t)INT64_C(-10000000000000000));
		EA::StdC::int128_t y((int64_t)INT64_C(-20000000000000002));
		y *= x;
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "200000000000000020000000000000000"));
	} 

	{  // Test of math
		EA::StdC::uint128_t x((int64_t)INT64_C(-10000000000000000));
		EA::StdC::uint128_t y((int64_t)INT64_C(-20000000000000002));
		y *= x;
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "200000000000000020000000000000000"));
	} 

	{  // Test of math
		EA::StdC::int128_t y;
		y = 0L / EA::StdC::int128_t("-10000000000000000");
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "0"));
	}

	{  // Test of math
		EA::StdC::uint128_t y;
		y = 0L / EA::StdC::uint128_t("10000000000000000");
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "0"));
	}

	{  // Test of math
		EA::StdC::int128_t y;
		y = EA::StdC::int128_t("-10000000000000000") / 10L;
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "-1000000000000000"));
	}

	{  // Test of math
		EA::StdC::uint128_t y;
		y = EA::StdC::int128_t("10000000000000000") / 10L;
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "1000000000000000"));
	}

	{  // Test of math
		EA::StdC::int128_t x;
		EA::StdC::int128_t y(1);
		EA::StdC::int128_t z(2);
		x = (y ^ z) + (y & z) + (y | z);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "6"));
	}

	{  // Test of math
		EA::StdC::uint128_t x;
		EA::StdC::uint128_t y(1L);
		EA::StdC::uint128_t z(2L);
		x = (y ^ z) + (y & z) + (y | z);
		x.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "6"));
	}

	{  // Test of math
		EA::StdC::int128_t x;
		EA::StdC::int128_t y("0x11111111000100001111111100000001", 16);
		EA::StdC::int128_t z("0x22222222000100002222222200000001", 16);
		x = (y ^ z) + (y & z) + (y | z);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x66666666000200006666666600000002"));
	}

	{  // Test of math
		EA::StdC::uint128_t x;
		EA::StdC::uint128_t y("0x11111111000100001111111100000001", 16);
		EA::StdC::uint128_t z("0x22222222000100002222222200000001", 16);
		x = (y ^ z) + (y & z) + (y | z);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x66666666000200006666666600000002"));
	}

	{  // Test of math
		int32_t        a(17);
		int16_t        b(26);
		int32_t        c(45);
		int64_t        d(77);
		uint16_t       e(25);
		EA::StdC::int128_t       x(13L);
		EA::StdC::int128_t       y;

		y = (((x + (a + b) * 37) / c) * x) % (d + e);
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "47"));
	}

	{  // Test of math
		int            a(17);
		short          b(26);
		EA::StdC::int128_t       c(45L);
		int64_t        d(77);
		unsigned short e(25);
		EA::StdC::uint128_t      x(13L);
		EA::StdC::uint128_t      y;

		y = (((x + (a + b) * 37L) / c) * x) % (d + e);
		y.Int128ToStr(array, NULL, 10);
		EATEST_VERIFY(CompareSelfTestResult(array, "47"));
	}

	{  // Test of math
		EA::StdC::int128_t x;
		EA::StdC::int128_t y("0x11111111000100001111111100000001", 16);
		EA::StdC::uint128_t z("0x22222222000100002222222200000001", 16);
		x = (y ^ z) + (y & z) + (y | z);
		x.Int128ToStr(array, NULL, 16);
		EATEST_VERIFY(CompareSelfTestResult(array, "0x66666666000200006666666600000002"));
	}


	{
		//  int128_t  int128_t::StrToInt128(const char*    pValue, char**    ppEnd, int base) const;
		//  int128_t  int128_t::StrToInt128(const wchar_t* pValue, wchar_t** ppEnd, int base) const;
		// EA::StdC::uint128_t EA::StdC::uint128_t::StrToInt128(const char*    pValue, char**    ppEnd, int base) const;
		// EA::StdC::uint128_t EA::StdC::uint128_t::StrToInt128(const wchar_t* pValue, wchar_t** ppEnd, int base) const;

		char*     pEnd;
		EA::StdC::int128_t  i128;
		EA::StdC::uint128_t u128;

		{   // Base 2
			char strBase2[]  = "0b101_";  // Decimal 5

			i128 = EA::StdC::int128_t::StrToInt128(strBase2, &pEnd, 0);
			EATEST_VERIFY((i128.AsInt32() == 5) && (*pEnd == '_'));

			i128 = EA::StdC::int128_t::StrToInt128(strBase2, &pEnd, 2);
			EATEST_VERIFY((i128.AsInt32() == 5) && (*pEnd == '_'));

			i128 = EA::StdC::int128_t::StrToInt128(strBase2 + 2, &pEnd, 2);
			EATEST_VERIFY((i128.AsInt32() == 5) && (*pEnd == '_'));


			u128 = EA::StdC::uint128_t::StrToInt128(strBase2, &pEnd, 0);
			EATEST_VERIFY((u128.AsInt32() == 5) && (*pEnd == '_'));

			u128 = EA::StdC::uint128_t::StrToInt128(strBase2, &pEnd, 2);
			EATEST_VERIFY((u128.AsInt32() == 5) && (*pEnd == '_'));

			u128 = EA::StdC::uint128_t::StrToInt128(strBase2 + 2, &pEnd, 2);
			EATEST_VERIFY((u128.AsInt32() == 5) && (*pEnd == '_'));
		}

		/* Base 8 is not yet supported by StrToInt128. 
		{   // Base 8
			char strBase8[] = "032_";    // Decimal 26

			i128 = int128_t::StrToInt128(strBase8, &pEnd, 0);
			EATEST_VERIFY((i128.AsInt32() == 26) && (*pEnd == '_'));

			i128 = int128_t::StrToInt128(strBase8, &pEnd, 8);
			EATEST_VERIFY((i128.AsInt32() == 26) && (*pEnd == '_'));


			u128 = EA::StdC::uint128_t::StrToInt128(strBase8, &pEnd, 0);
			EATEST_VERIFY((u128.AsInt32() == 26) && (*pEnd == '_'));

			u128 = EA::StdC::uint128_t::StrToInt128(strBase8, &pEnd, 8);
			EATEST_VERIFY((u128.AsInt32() == 26) && (*pEnd == '_'));
		}
		*/

		{   // Base 10
			char strBase10[] = "32_";    // Decimal 32

			i128 = EA::StdC::int128_t::StrToInt128(strBase10, &pEnd, 0);
			EATEST_VERIFY((i128.AsInt32() == 32) && (*pEnd == '_'));

			i128 = EA::StdC::int128_t::StrToInt128(strBase10, &pEnd, 10);
			EATEST_VERIFY((i128.AsInt32() == 32) && (*pEnd == '_'));


			u128 = EA::StdC::uint128_t::StrToInt128(strBase10, &pEnd, 0);
			EATEST_VERIFY((u128.AsInt32() == 32) && (*pEnd == '_'));

			u128 = EA::StdC::uint128_t::StrToInt128(strBase10, &pEnd, 10);
			EATEST_VERIFY((u128.AsInt32() == 32) && (*pEnd == '_'));
		}

		{   // Base 16
			char strBase16[] = "0x32_";    // Decimal 50

			i128 = EA::StdC::int128_t::StrToInt128(strBase16, &pEnd, 0);
			EATEST_VERIFY((i128.AsInt32() == 50) && (*pEnd == '_'));

			i128 = EA::StdC::int128_t::StrToInt128(strBase16, &pEnd, 16);
			EATEST_VERIFY((i128.AsInt32() == 50) && (*pEnd == '_'));

			i128 = EA::StdC::int128_t::StrToInt128(strBase16 + 2, &pEnd, 16);
			EATEST_VERIFY((i128.AsInt32() == 50) && (*pEnd == '_'));


			u128 = EA::StdC::uint128_t::StrToInt128(strBase16, &pEnd, 0);
			EATEST_VERIFY((u128.AsInt32() == 50) && (*pEnd == '_'));

			u128 = EA::StdC::uint128_t::StrToInt128(strBase16, &pEnd, 16);
			EATEST_VERIFY((u128.AsInt32() == 50) && (*pEnd == '_'));

			u128 = EA::StdC::uint128_t::StrToInt128(strBase16 + 2, &pEnd, 16);
			EATEST_VERIFY((u128.AsInt32() == 50) && (*pEnd == '_'));
		}
	}

	return nErrorCount;
}


