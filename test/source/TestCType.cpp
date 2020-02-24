///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EACType.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>


int TestCType()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// Character categorization
	// To do: Change this from a macro to a template.
	#define ISCHAR_CATEGORY_TEST(type, category, c_from, c_to, positive) \
	{\
		for(uint32_t c = (uint32_t)(unsigned char)(c_from); c <= (uint32_t)(unsigned char)(c_to); ++c)\
			EATEST_VERIFY((Is##category((char##type##_t)c) != 0) == positive);\
	}


	// Make sure all chars are accepted
	int32_t  i;
	char  n8;
	char16_t n16;
	int      n32;
	char  c8;
	char16_t c16;

	for(c8 = INT8_MIN, i = INT8_MIN; i <= INT8_MAX; ++c8, ++i)
	{
		n32 = Isalnum(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Isalpha(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Isdigit(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Isxdigit(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Isgraph(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Islower(c8);
		EATEST_VERIFY(n32 != -1);
		n8 = Tolower(c8);
		EATEST_VERIFY(n32 != 99 || (n8 == 0));
		n32 = Isupper(c8);
		EATEST_VERIFY(n32 != -1);
		n8 = Toupper(c8);
		EATEST_VERIFY(n32 != 99 || (n8 == 0));
		n32 = Isprint(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Ispunct(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Isspace(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Iscntrl(c8);
		EATEST_VERIFY(n32 != -1);
		n32 = Isascii(c8);
		EATEST_VERIFY(n32 != -1);
	}

	// Make sure all chars are accepted
	for(c16 = 0, i = 0; i <= INT16_MAX; ++c16, ++i)
	{
		n32 = Isalnum(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Isalpha(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Isdigit(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Isxdigit(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Isgraph(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Islower(c16);
		EATEST_VERIFY(n32 != -1);
		n16 = Tolower(c16);
		EATEST_VERIFY(n32 != 99 || (n16 == 0));
		n32 = Isupper(c16);
		EATEST_VERIFY(n32 != -1);
		n16 = Toupper(c16);
		EATEST_VERIFY(n32 != 99 || (n16 == 0));
		n32 = Isprint(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Ispunct(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Isspace(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Iscntrl(c16);
		EATEST_VERIFY(n32 != -1);
		n32 = Isascii(c16);
		EATEST_VERIFY(n32 != -1);
	}

	// ********************* alnum
	// ISCHAR_CATEGORY_TEST(8,  alnum, '0', '9', true);
	// ISCHAR_CATEGORY_TEST(8,  alnum, 'a', 'z', true);
	// ISCHAR_CATEGORY_TEST(8,  alnum, 'A', 'Z', true);
	// ISCHAR_CATEGORY_TEST(8,  alnum, '\x0', '\x15', false);

	ISCHAR_CATEGORY_TEST(16, alnum, '0', '9', true);
	ISCHAR_CATEGORY_TEST(16, alnum, 'a', 'z', true);
	ISCHAR_CATEGORY_TEST(16, alnum, 'A', 'Z', true);

	// ********************* alpha
	// ISCHAR_CATEGORY_TEST(8,  alpha, '0', '9', false);
	// ISCHAR_CATEGORY_TEST(8,  alpha, 'a', 'z', true);
	// ISCHAR_CATEGORY_TEST(8,  alpha, 'A', 'Z', true);
	// ISCHAR_CATEGORY_TEST(8,  alpha, '\x0', '\x15', false);

	ISCHAR_CATEGORY_TEST(16, alpha, '0', '9', false);
	ISCHAR_CATEGORY_TEST(16, alpha, 'a', 'z', true);
	ISCHAR_CATEGORY_TEST(16, alpha, 'A', 'Z', true);
	ISCHAR_CATEGORY_TEST(16, alpha, '(', '('+10, false);

	// digit
	// ISCHAR_CATEGORY_TEST(8,  digit, '0', '9', true);
	// ISCHAR_CATEGORY_TEST(8,  digit, 'a', 'z', false);
	// ISCHAR_CATEGORY_TEST(8,  digit, 'A', 'Z', false);
	// ISCHAR_CATEGORY_TEST(8,  digit, '\x0', '\x15', false);

	ISCHAR_CATEGORY_TEST(16, digit, '0', '9', true);
	ISCHAR_CATEGORY_TEST(16, digit, 'a', 'z', false);
	ISCHAR_CATEGORY_TEST(16, digit, 'A', 'Z', false);
	ISCHAR_CATEGORY_TEST(16, digit, '\x70', '\x71', false);

	// xdigit
	// ISCHAR_CATEGORY_TEST(8,  xdigit, '0', '9', true);
	// ISCHAR_CATEGORY_TEST(8,  xdigit, 'a', 'f', true);
	// ISCHAR_CATEGORY_TEST(8,  xdigit, 'A', 'F', true);
	// ISCHAR_CATEGORY_TEST(8,  xdigit, '\x0', '\x15', false);

	ISCHAR_CATEGORY_TEST(16, xdigit, '0', '9', true);
	ISCHAR_CATEGORY_TEST(16, xdigit, 'a', 'f', true);
	ISCHAR_CATEGORY_TEST(16, xdigit, 'A', 'F', true);
	ISCHAR_CATEGORY_TEST(16, xdigit, 'z'+1, 'z'+60, false);

	// graph
	// ISCHAR_CATEGORY_TEST(8,  graph, '\x21','\x7e', true);
	// ISCHAR_CATEGORY_TEST(8,  graph, '\x0', '\x20', false);

	ISCHAR_CATEGORY_TEST(16, graph, '!', '!'+14, true);
	ISCHAR_CATEGORY_TEST(16,  graph, '0', 'z', true);
	ISCHAR_CATEGORY_TEST(16,  graph, '\x0', '\x20', false);

	// lower
	// ISCHAR_CATEGORY_TEST(8,  lower, '0', '9', false);
	// ISCHAR_CATEGORY_TEST(8,  lower, 'a', 'z', true);
	// ISCHAR_CATEGORY_TEST(8,  lower, 'A', 'Z', false);
	// ISCHAR_CATEGORY_TEST(8,  lower, '\x0', '\x15', false);

	ISCHAR_CATEGORY_TEST(16, lower, '0', '9', false);
	ISCHAR_CATEGORY_TEST(16, lower, 'a', 'z', true);
	ISCHAR_CATEGORY_TEST(16, lower, 'A', 'Z', false);
	ISCHAR_CATEGORY_TEST(16, lower, '!', '!'+14, false);

	// upper
	// ISCHAR_CATEGORY_TEST(8,  upper, '0', '9', false);
	// ISCHAR_CATEGORY_TEST(8,  upper, 'a', 'z', false);
	// ISCHAR_CATEGORY_TEST(8,  upper, 'A', 'Z', true);
	// ISCHAR_CATEGORY_TEST(8,  upper, '\x0', '\x15', false);

	ISCHAR_CATEGORY_TEST(16, upper, '0', '9', false);
	ISCHAR_CATEGORY_TEST(16, upper, 'a', 'z', false);
	ISCHAR_CATEGORY_TEST(16, upper, 'A', 'Z', true);
	ISCHAR_CATEGORY_TEST(16, upper, '!', '!'+14, false);

	// punct
	// ISCHAR_CATEGORY_TEST(8,  punct, '0', '9', false);
	// ISCHAR_CATEGORY_TEST(8,  punct, 'a', 'z', false);
	// ISCHAR_CATEGORY_TEST(8,  punct, 'A', 'Z', false);
	// ISCHAR_CATEGORY_TEST(8,  punct, '\x0', '\x15', false);
	// ISCHAR_CATEGORY_TEST(8, punct, '!', '!', true);
	// ISCHAR_CATEGORY_TEST(8, punct, '?', '?', true);
	// ISCHAR_CATEGORY_TEST(8, punct, '.', '.', true);
	// ISCHAR_CATEGORY_TEST(8, punct, ',', ',', true);

	ISCHAR_CATEGORY_TEST(16, punct, '0', '9', false);
	ISCHAR_CATEGORY_TEST(16, punct, 'a', 'z', false);
	ISCHAR_CATEGORY_TEST(16, punct, 'A', 'Z', false);
	ISCHAR_CATEGORY_TEST(16, punct, '!', '!', true);
	ISCHAR_CATEGORY_TEST(16, punct, '?', '?', true);
	ISCHAR_CATEGORY_TEST(16, punct, '.', '.', true);
	ISCHAR_CATEGORY_TEST(16, punct, ',', ',', true);
	ISCHAR_CATEGORY_TEST(16, punct, ':', ';', true);

	// space
	// ISCHAR_CATEGORY_TEST(8,  space, '0', '9', false);
	// ISCHAR_CATEGORY_TEST(8,  space, ' ', ' ', true);
	// ISCHAR_CATEGORY_TEST(8,  space, '\x0a', '\x0a', true);

	ISCHAR_CATEGORY_TEST(16,  space, 'a', 'z', false);
	ISCHAR_CATEGORY_TEST(16,  space, '\x09', '\x09', true);
	ISCHAR_CATEGORY_TEST(16,  space, '\x0d', '\x0d', true);

	// cntrl
	// ISCHAR_CATEGORY_TEST(8,  cntrl, 0, '\x1f', true);
	// ISCHAR_CATEGORY_TEST(8,  cntrl, '\x7f', '\x7f', true);

	ISCHAR_CATEGORY_TEST(16,  cntrl, 0, '\x1f', true);
	ISCHAR_CATEGORY_TEST(16,  cntrl, '\x7f', '\x7f', true);

	// ascii
	// ISCHAR_CATEGORY_TEST(8,   ascii, '\x0', '\x7f', true);
	// ISCHAR_CATEGORY_TEST(8,   ascii, '\x80', '\xff', false);

	ISCHAR_CATEGORY_TEST(16,  ascii, '\x0', '\x7f', true);
	ISCHAR_CATEGORY_TEST(16,  ascii, '\x80', '\xff', false);


	// Toupper8 / Tolower8 / Toupper16 Tolower16
	for (int c = 'a'; c < 'z'; ++c)
	{
		EATEST_VERIFY(Toupper((char)c) == 'A' + c - 'a');
		EATEST_VERIFY(Tolower((char)('A' + c - 'a')) == (char)c);

		EATEST_VERIFY(Toupper((char16_t)c) == char16_t('A' + c - 'a'));
		EATEST_VERIFY(Tolower((char16_t)('A' + c - 'a')) == char16_t((char16_t)c));
	}

	return nErrorCount;
}






