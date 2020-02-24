///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EATextUtil.h>
#include <EAStdC/EAString.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>
#include <EASTL/fixed_string.h>
#include <EASTL/string.h>

#include <string.h>

#ifdef _MSC_VER
	#pragma warning(disable: 4310)  // cast truncates constant value.
#endif


typedef eastl::basic_string<char>  String8;
typedef eastl::basic_string<char16_t> String16;
typedef eastl::basic_string<char32_t> String32;



static int TestUTF8()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	bool           bResult;
	const char* pResult;
	char16_t       cResult;
	size_t         nResult;
	char        buffer[32];

	// We would need a lot more strings than this to test this functionality well.
	const uint8_t str1[]    = { 0 };
	const uint8_t str2[]    = { 'a', 'b', 'c', 0 };
	const uint8_t str3[]    = { 0xe8,    0x8f, 0xa4,    0xc8, 0x80,    0x61,    0 }; // 0x83e4, 0x0200, 0x0061
	const uint8_t str4[]    = { 0x7f,    0xc4, 0x80,    0xef, 0xbf,    0xb0,    0 }; // 0x007f, 0x0100, 0xfff0

	const char* strArray[] =
	{ 
		reinterpret_cast<const char*>(str1), 
		reinterpret_cast<const char*>(str2), 
		reinterpret_cast<const char*>(str3), 
		reinterpret_cast<const char*>(str4)
	};

	// These represent bad combinations of bytes that any UTF8 decoder should recognize.
	const uint8_t strBad1[] = { 0xc1, 0 };
	const uint8_t strBad2[] = { 0xfe, 0 };
	const uint8_t strBad3[] = { 0xc2, 0x7f, 0 };
	const uint8_t strBad4[] = { 0xc2, 0xc0, 0 };
	const uint8_t strBad5[] = { 0xe0, 0x9f, 0x80, 0 };
	const uint8_t strBad6[] = { 0xe0, 0x80, 0xff, 0 };

	// The following UTF-8 sequences should be rejected like malformed sequences, because they never represent valid ISO 10646 characters and a UTF-8 decoder that accepts them might introduce security problems comparable to overlong UTF-8 sequences. 
	//const uint8_t strBad7[] = { 0xed, 0xa0, 0x80, 0 };
	//const uint8_t strBad8[] = { 0xed, 0xbf, 0xbf, 0 };
	//const uint8_t strBad9[] = { 0xef, 0xbf, 0xbe, 0 };

	const char* strBadArray[] =
	{
		reinterpret_cast<const char*>(strBad1), 
		reinterpret_cast<const char*>(strBad2), 
		reinterpret_cast<const char*>(strBad3), 
		reinterpret_cast<const char*>(strBad4), 
		reinterpret_cast<const char*>(strBad5), 
		reinterpret_cast<const char*>(strBad6)
	/*, reinterpret_cast<const char*>(strBad7), 
		reinterpret_cast<const char*>(strBad8), 
		reinterpret_cast<const char*>(strBad9) */
	};


	// bool UTF8Validate(const char* p, size_t nLength);
	for(size_t i = 0; i < sizeof(strArray)/sizeof(strArray[0]); ++i)
	{
		bResult = UTF8Validate(strArray[i], strlen(strArray[i]));
		EATEST_VERIFY(bResult);
	}

	for(size_t i = 0; i < sizeof(strBadArray)/sizeof(strBadArray[0]); ++i)
	{
		bResult = UTF8Validate(strBadArray[i], strlen(strBadArray[i]));
		EATEST_VERIFY(!bResult);
	}


	// char* UTF8Increment(const char* p, size_t n);
	// char* UTF8Decrement(const char* p, size_t n);
	pResult = UTF8Increment(strArray[0], 1);
	EATEST_VERIFY(pResult == (strArray[0] + 1));
	pResult = UTF8Decrement(pResult, 1);
	EATEST_VERIFY(pResult == strArray[0]);

	pResult = UTF8Increment(strArray[1], 3);
	EATEST_VERIFY(pResult == (strArray[1] + 3));
	pResult = UTF8Decrement(pResult, 3);
	EATEST_VERIFY(pResult == strArray[1]);

	pResult = UTF8Increment(strArray[2], 3);
	EATEST_VERIFY(pResult == (strArray[2] + 6));
	pResult = UTF8Decrement(pResult, 3);
	EATEST_VERIFY(pResult == strArray[2]);

	pResult = UTF8Increment(strArray[3], 3);
	EATEST_VERIFY(pResult == (strArray[3] + 6));
	pResult = UTF8Decrement(pResult, 3);
	EATEST_VERIFY(pResult == strArray[3]);


	// size_t UTF8Length(const char* p);
	EATEST_VERIFY(UTF8Length("0123456789") == 10);
	EATEST_VERIFY(UTF8Length("") == 0);
	EATEST_VERIFY(UTF8Length("\xc2" "\xa2") == 1);
	EATEST_VERIFY(UTF8Length("\xd7" "\x90") == 1);
	EATEST_VERIFY(UTF8Length("\xe0" "\xbc" "\xa0") == 1);
	EATEST_VERIFY(UTF8Length("\xc2\x80 \xc2\x81 \xdf\xbe \xdf\xbf") == 7);
	EATEST_VERIFY(UTF8Length("\xe0\xa0\x80 \xe0\xa0\x81 \xef\xbf\xbe \xef\xbf\xbf") == 7);
	EATEST_VERIFY(UTF8Length("\xf0\x90\x80\x80 \xf0\x90\x80\x81") == 3);
	EATEST_VERIFY(UTF8Length("\xf4\x8f\xbf\xbe \xf4\x8f\xbf\xbf") == 3);


	// size_t UTF8Length(const char16_t* p);
	EATEST_VERIFY(UTF8Length(EA_CHAR16("0123456789")) == 10);
	EATEST_VERIFY(UTF8Length(EA_CHAR16("")) == 0);
	EATEST_VERIFY(UTF8Length(EA_CHAR16("\x00a0")) == 2);
	EATEST_VERIFY(UTF8Length(EA_CHAR16("\x0400")) == 2);
	EATEST_VERIFY(UTF8Length(EA_CHAR16("\x0800")) == 3);

	// We have to break up the string into multiple sub-strings because the \x escape sequence has limitations in how it works.
	eastl::fixed_string<char16_t, 32> s16; s16 = EA_CHAR16("\xffff"); s16 += EA_CHAR16("\xffff"); // We use a string object because some compilers don't support 16 bit string literals, and thus EA_CHAR16 is a function and doesn't just prepend "L" or "u" to the string.
	EATEST_VERIFY(UTF8Length(s16.c_str()) == 6);

	s16 = EA_CHAR16("\xffff"); s16 += EA_CHAR16("\x0900"); s16 += EA_CHAR16("0"); s16 += EA_CHAR16("\x00a0");
	EATEST_VERIFY(UTF8Length(s16.c_str()) == 9);


	// size_t UTF8Length(const char32_t* p);
	EATEST_VERIFY(UTF8Length(EA_CHAR32("0123456789")) == 10);
	EATEST_VERIFY(UTF8Length(EA_CHAR32("")) == 0);
	EATEST_VERIFY(UTF8Length(EA_CHAR32("\x00a0")) == 2);
	EATEST_VERIFY(UTF8Length(EA_CHAR32("\x0400")) == 2);
	EATEST_VERIFY(UTF8Length(EA_CHAR32("\x0800")) == 3);

	// We have to break up the string into multiple sub-strings because the \x escape sequence has limitations in how it works.
	eastl::fixed_string<char32_t, 32> s32; s32 = EA_CHAR32("\xffff"); s32 += EA_CHAR32("\xffff"); // We use a string object because some compilers don't support 32 bit string literals, and thus EA_CHAR32 is a function and doesn't just prepend "L" or "u" to the string.
	EATEST_VERIFY(UTF8Length(s32.c_str()) == 6);

	s32 = EA_CHAR32("\xffff"); s32 += EA_CHAR32("\x0900"); s32 += EA_CHAR32("0"); s32 += EA_CHAR32("\x00a0");
	EATEST_VERIFY(UTF8Length(s32.c_str()) == 9);


	// size_t UTF8CharSize(const char* p);
	EATEST_VERIFY(UTF8CharSize(strArray[0]) == 1);
	EATEST_VERIFY(UTF8CharSize(strArray[1]) == 1);
	EATEST_VERIFY(UTF8CharSize(strArray[2] + 0) == 3);
	EATEST_VERIFY(UTF8CharSize(strArray[2] + 3) == 2);
	EATEST_VERIFY(UTF8CharSize(strArray[3] + 1) == 2);
	EATEST_VERIFY(UTF8CharSize(strArray[3] + 3) == 3);


	// size_t UTF8CharSize(char16_t c);
	nResult = UTF8CharSize((char16_t)0x0001);
	EATEST_VERIFY(nResult == 1);

	nResult = UTF8CharSize((char16_t)0x007f);
	EATEST_VERIFY(nResult == 1);

	nResult = UTF8CharSize((char16_t)0x0080);
	EATEST_VERIFY(nResult == 2);

	nResult = UTF8CharSize((char16_t)0x07ff);
	EATEST_VERIFY(nResult == 2);

	nResult = UTF8CharSize((char16_t)0x0800);
	EATEST_VERIFY(nResult == 3);

	nResult = UTF8CharSize((char16_t)0xfffd);
	EATEST_VERIFY(nResult == 3);


	// size_t UTF8CharSize(char32_t c);
	nResult = UTF8CharSize((char32_t)0x0001);
	EATEST_VERIFY(nResult == 1);

	nResult = UTF8CharSize((char32_t)0x007f);
	EATEST_VERIFY(nResult == 1);

	nResult = UTF8CharSize((char32_t)0x0080);
	EATEST_VERIFY(nResult == 2);

	nResult = UTF8CharSize((char32_t)0x07ff);
	EATEST_VERIFY(nResult == 2);

	nResult = UTF8CharSize((char32_t)0x0800);
	EATEST_VERIFY(nResult == 3);

	nResult = UTF8CharSize((char32_t)0xfffd);
	EATEST_VERIFY(nResult == 3);


	// char16_t UTF8ReadChar(const char* p, const char** ppEnd = NULL);
	pResult = strArray[0];
	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 0x0000) && (pResult == strArray[0] + 1));

	pResult = strArray[1];
	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 'a') && (pResult == strArray[1] + 1));

	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 'b') && (pResult == strArray[1] + 2));

	pResult = strArray[2];
	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 0x83e4) && (pResult == strArray[2] + 3));

	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 0x0200) && (pResult == strArray[2] + 5));

	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 0x0061) && (pResult == strArray[2] + 6));

	pResult = strArray[3];
	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 0x007f) && (pResult == strArray[3] + 1));

	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 0x0100) && (pResult == strArray[3] + 3));

	cResult = UTF8ReadChar(pResult, &pResult);
	EATEST_VERIFY((cResult == 0xfff0) && (pResult == strArray[3] + 6));


	// char* UTF8WriteChar(char* p, char16_t c);
	pResult = buffer;
	pResult = UTF8WriteChar((char*)pResult, (char16_t)0x83e4);
	pResult = UTF8WriteChar((char*)pResult, (char16_t)0x0200);
	pResult = UTF8WriteChar((char*)pResult, (char16_t)0x0061);
	pResult = UTF8WriteChar((char*)pResult, (char16_t)0x0000);
	EATEST_VERIFY((strcmp(buffer, strArray[2]) == 0) && (pResult == buffer + 7));


	// char* UTF8WriteChar(char* p, char32_t c);
	pResult = buffer;
	pResult = UTF8WriteChar((char*)pResult, (char32_t)0x83e4);
	pResult = UTF8WriteChar((char*)pResult, (char32_t)0x0200);
	pResult = UTF8WriteChar((char*)pResult, (char32_t)0x0061);
	pResult = UTF8WriteChar((char*)pResult, (char32_t)0x0000);
	EATEST_VERIFY((strcmp(buffer, strArray[2]) == 0) && (pResult == buffer + 7));


	// bool UTF8IsSoloByte(char c);
	EATEST_VERIFY( UTF8IsSoloByte('\0'));
	EATEST_VERIFY( UTF8IsSoloByte('\n'));
	EATEST_VERIFY( UTF8IsSoloByte('a'));
	EATEST_VERIFY( UTF8IsSoloByte((char)0x7f));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0x80));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xfd));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xfe));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xff));


	// bool UTF8IsLeadByte(char c);
	EATEST_VERIFY( UTF8IsSoloByte('\0'));
	EATEST_VERIFY( UTF8IsSoloByte('\n'));
	EATEST_VERIFY( UTF8IsSoloByte('a'));
	EATEST_VERIFY( UTF8IsSoloByte((char)0x7f));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xc0));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xd1));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xe4));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xf0));       // This assumes that we don't support 4, 5, 6 byte sequences.
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xfe));
	EATEST_VERIFY(!UTF8IsSoloByte((char)0xff));


	// bool UTF8IsFollowByte(char c);
	EATEST_VERIFY(!UTF8IsFollowByte((char)0x00));
	EATEST_VERIFY(!UTF8IsFollowByte((char)0x7f));
	EATEST_VERIFY( UTF8IsFollowByte((char)0x80));
	EATEST_VERIFY( UTF8IsFollowByte((char)0xbf));
	EATEST_VERIFY(!UTF8IsFollowByte((char)0xc0));
	EATEST_VERIFY(!UTF8IsFollowByte((char)0xff));

	// char* UTF8ReplaceInvalidChar(char* pIn, size_t nLength, char replaceWith);
	{
		{
			char8_t outBuffer[256] = {0};

			auto* pBad  = EA_CHAR8("foofoobaazong");
			auto* pGood = EA_CHAR8("foofoobaazong");

			auto* pOut = UTF8ReplaceInvalidChar(pBad, UTF8Length(pBad), outBuffer, '?');

			EATEST_VERIFY(UTF8Length(pBad) == size_t(pOut - outBuffer));
			EATEST_VERIFY(Strcmp(pGood, outBuffer) == 0);
		}

		{
			char outBuffer[256] = {0};

			auto* pBad = "foofoo\xfa" "baazong";
			auto* pGood = "foofoo?baazong";

			auto* pOut = UTF8ReplaceInvalidChar(pBad, UTF8Length(pBad), outBuffer, '?');

			EATEST_VERIFY(UTF8Length(pBad) == size_t(pOut - outBuffer));
			EATEST_VERIFY(Strcmp(pGood, outBuffer) == 0);
		}


		{
			char outBuffer[256] = {0};

			auto* pBad = "foofoo\xfa\xfa\xfa\xfa\xfa" "baazong";
			auto* pGood = "foofoo?????baazong";

			auto* pOut = UTF8ReplaceInvalidChar(pBad, UTF8Length(pBad), outBuffer, '?');

			EATEST_VERIFY(UTF8Length(pBad) == size_t(pOut - outBuffer));
			EATEST_VERIFY(Strcmp(pGood, outBuffer) == 0);
		}

		{
			char outBuffer[256] = {0};

			auto* pBad = "foo\xfa" "foo\xfa\xfa" "b\xfa" "a\xfa" "a\xfa" "z\xfa" "o\xfa" "n\xfa" "g\xfa";
			auto* pGood = "foo?foo??b?a?a?z?o?n?g?";

			auto* pOut = UTF8ReplaceInvalidChar(pBad, UTF8Length(pBad), outBuffer, '?');

			EATEST_VERIFY(UTF8Length(pBad) == size_t(pOut - outBuffer));
			EATEST_VERIFY(Strcmp(pGood, outBuffer) == 0);
		}
	}

	return nErrorCount;
}



int TestTextUtil()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	nErrorCount += TestUTF8();

	// WildcardMatch
	{
		// char
		EATEST_VERIFY(WildcardMatch("abcde", "*e",     false) == true);
		EATEST_VERIFY(WildcardMatch("abcde", "*f",     false) == false);
		EATEST_VERIFY(WildcardMatch("abcde", "???de",  false) == true);
		EATEST_VERIFY(WildcardMatch("abcde", "????g",  false) == false);
		EATEST_VERIFY(WildcardMatch("abcde", "*c??",   false) == true);
		EATEST_VERIFY(WildcardMatch("abcde", "*e??",   false) == false);
		EATEST_VERIFY(WildcardMatch("abcde", "*????",  false) == true);
		EATEST_VERIFY(WildcardMatch("abcde", "bcdef",  false) == false);
		EATEST_VERIFY(WildcardMatch("abcde", "*?????", false) == true);

		EATEST_VERIFY(WildcardMatch("abcdE", "*E",     true) == true);
		EATEST_VERIFY(WildcardMatch("abcde", "*f",     true) == false);
		EATEST_VERIFY(WildcardMatch("abcDE", "???de",  true) == false);
		EATEST_VERIFY(WildcardMatch("abcde", "????g",  true) == false);
		EATEST_VERIFY(WildcardMatch("abCde", "*c??",   true) == false);
		EATEST_VERIFY(WildcardMatch("abcde", "*e??",   true) == false);
		EATEST_VERIFY(WildcardMatch("abcde", "*????",  true) == true);
		EATEST_VERIFY(WildcardMatch("abcde", "bcdef",  true) == false);
		EATEST_VERIFY(WildcardMatch("abcde", "*?????", true) == true);

		// char16_t
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*e"),     false) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*f"),     false) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("???de"),  false) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("????g"),  false) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*c??"),   false) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*e??"),   false) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*????"),  false) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("bcdef"),  false) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*?????"), false) == true);

		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcdE"), EA_CHAR16("*E"),     true) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*f"),     true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcDE"), EA_CHAR16("???de"),  true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("????g"),  true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abCde"), EA_CHAR16("*c??"),   true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*e??"),   true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*????"),  true) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("bcdef"),  true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR16("abcde"), EA_CHAR16("*?????"), true) == true);

		// char32_t
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*e"),     false) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*f"),     false) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("???de"),  false) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("????g"),  false) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*c??"),   false) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*e??"),   false) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*????"),  false) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("bcdef"),  false) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*?????"), false) == true);

		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcdE"), EA_CHAR32("*E"),     true) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*f"),     true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcDE"), EA_CHAR32("???de"),  true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("????g"),  true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abCde"), EA_CHAR32("*c??"),   true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*e??"),   true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*????"),  true) == true);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("bcdef"),  true) == false);
		EATEST_VERIFY(WildcardMatch(EA_CHAR32("abcde"), EA_CHAR32("*?????"), true) == true);
	}


	// EASTDC_API bool ParseDelimitedText(const char* pText, const char* pTextEnd, char cDelimiter, 
	//                                    const char*& pToken, const char*& pTokenEnd, const char** ppNewText);
	// EASTDC_API bool ParseDelimitedText(const char16_t* pText, const char16_t* pTextEnd, char16_t cDelimiter, 
	//                                    const char16_t*& pToken, const char16_t*& pTokenEnd, const char16_t** ppNewText);
	// EASTDC_API bool ParseDelimitedText(const char32_t* pText, const char32_t* pTextEnd, char32_t cDelimiter, 
	//                                    const char32_t*& pToken, const char32_t*& pTokenEnd, const char32_t** ppNewText);
	{
		/// Example behavior:
		///   1  Input String                                             MaxResults        Delimiter    Return Value     OutputArray Size  Output Array
		///   2  "";                                                      -1                ' '          0                0                 ""
		///   3  "000 111";                                               -1                ' '          2                2                 "000"  "111"
		///   4  "000 111   222   333 444 \"555 555\" 666";               -1                ' '          7                7                 "000"  "111"  "222"  "333"  "444"  "555 555"  "666"
		///   5  "     000 111 222         333                ";          -1                ' '          4                4                 "000"  "111"  "222"  "333"
		///   6  "     000 111 222         333                ";           2                ' '          2                2                 "000"  "111"
		///   7  "";                                                      -1                ','          0                0                 ""
		///   8  "000,111";                                               -1                ','          2                2                 "000"  "111"
		///   9  "000,  111 , 222   333 ,444 \"555,  555  \" 666";        -1                ','          4                4                 "000"  "111"  "222   333"  "444 \"555,  555  \" 666"
		///  10  "  ,, 000 ,111, 222,         333          ,     ";       -1                ','          6                6                 ""   ""   "000"   "111"   "222"   "333"
		///  11  "  ,, 000 ,111, 222,         333          ,     ";        2                ','          0                0                 ""   ""

		const char16_t* pTest2  = EA_CHAR16("");
		const char16_t* pTest3  = EA_CHAR16("000 111");
		const char16_t* pTest4  = EA_CHAR16("  \"555 555\"   ");
	  //const char16_t* pTest5  = EA_CHAR16("     000 111 222         333                ");
	  //const char16_t* pTest6  = EA_CHAR16("     000 111 222         333                ");
	  //const char16_t* pTest7  = EA_CHAR16("     000 111 222         333                ");
	  //const char16_t* pTest8  = EA_CHAR16("000,111"));
	  //const char16_t* pTest9  = EA_CHAR16("000,  111 , 222   333 ,444 \"555,  555  \" 666");
	  //const char16_t* pTest10 = EA_CHAR16("  ,, 000 ,111, 222,         333          ,     ");
	  //const char16_t* pTest11 = EA_CHAR16("  ,, 000 ,111, 222,         333          ,     ");

		bool            result;
		const char16_t* pToken;
		const char16_t* pTokenEnd;
		const char16_t* pNewText;

		// Test2
		result = ParseDelimitedText(pTest2, pTest2 + Strlen(pTest2), ' ', pToken, pTokenEnd, &pNewText);
		EATEST_VERIFY(!result && (pToken == pTest2) && (pTokenEnd == pTest2) && (pNewText == pTest2));

		// Test3
		result = ParseDelimitedText(pTest3, pTest3 + Strlen(pTest3), ' ', pToken, pTokenEnd, &pNewText);
		EATEST_VERIFY(result && (pToken == pTest3) && (pTokenEnd == pTest3 + 3) && (pNewText == pTest3 + 3));

		result = ParseDelimitedText(pTest3 + 3, pTest3 + Strlen(pTest3), ' ', pToken, pTokenEnd, &pNewText);
		EATEST_VERIFY(result && (pToken == pTest3 + 4) && (pTokenEnd == pTest3 + 7) && (pNewText == pTest3 + 7));

		// Test4
		result = ParseDelimitedText(pTest4, pTest4 + Strlen(pTest4), ' ', pToken, pTokenEnd, &pNewText);
		EATEST_VERIFY(result && (pToken == pTest4 + 3) && (pTokenEnd == pTest4 + 10) && (pNewText == pTest4 + 11));
	}
	{
		const char32_t* pTest2  = EA_CHAR32("");
		const char32_t* pTest3  = EA_CHAR32("000 111");
		const char32_t* pTest4  = EA_CHAR32("  \"555 555\"   ");
	  //const char32_t* pTest5  = EA_CHAR32("     000 111 222         333                ");
	  //const char32_t* pTest6  = EA_CHAR32("     000 111 222         333                ");
	  //const char32_t* pTest7  = EA_CHAR32("     000 111 222         333                ");
	  //const char32_t* pTest8  = EA_CHAR32("000,111"));
	  //const char32_t* pTest9  = EA_CHAR32("000,  111 , 222   333 ,444 \"555,  555  \" 666");
	  //const char32_t* pTest10 = EA_CHAR32("  ,, 000 ,111, 222,         333          ,     ");
	  //const char32_t* pTest11 = EA_CHAR32("  ,, 000 ,111, 222,         333          ,     ");

		bool            result;
		const char32_t* pToken;
		const char32_t* pTokenEnd;
		const char32_t* pNewText;

		// Test2
		result = ParseDelimitedText(pTest2, pTest2 + Strlen(pTest2), ' ', pToken, pTokenEnd, &pNewText);
		EATEST_VERIFY(!result && (pToken == pTest2) && (pTokenEnd == pTest2) && (pNewText == pTest2));

		// Test3
		result = ParseDelimitedText(pTest3, pTest3 + Strlen(pTest3), ' ', pToken, pTokenEnd, &pNewText);
		EATEST_VERIFY(result && (pToken == pTest3) && (pTokenEnd == pTest3 + 3) && (pNewText == pTest3 + 3));

		result = ParseDelimitedText(pTest3 + 3, pTest3 + Strlen(pTest3), ' ', pToken, pTokenEnd, &pNewText);
		EATEST_VERIFY(result && (pToken == pTest3 + 4) && (pTokenEnd == pTest3 + 7) && (pNewText == pTest3 + 7));

		// Test4
		result = ParseDelimitedText(pTest4, pTest4 + Strlen(pTest4), ' ', pToken, pTokenEnd, &pNewText);
		EATEST_VERIFY(result && (pToken == pTest4 + 3) && (pTokenEnd == pTest4 + 10) && (pNewText == pTest4 + 11));
	}


	// ConvertBinaryDataToASCIIArray / ConvertASCIIArrayToBinaryData
	{
		uint8_t  data[4] = { 0x12, 0x34, 0x56, 0x78 };
		char  result8[32];
		char16_t result16[32];
		char32_t result32[32];

		ConvertBinaryDataToASCIIArray(&data, 4, result8);
		EATEST_VERIFY(Strcmp(result8, "12345678") == 0);

		ConvertBinaryDataToASCIIArray(&data, 4, result16);
		EATEST_VERIFY(Strcmp(result16, EA_CHAR16("12345678")) == 0);

		ConvertBinaryDataToASCIIArray(&data, 4, result32);
		EATEST_VERIFY(Strcmp(result32, EA_CHAR32("12345678")) == 0);


		memset(data, 0, sizeof(data));
		EATEST_VERIFY(ConvertASCIIArrayToBinaryData(result8, 8, data) == true);
		EATEST_VERIFY((data[0] == 0x12) && (data[3] == 0x78));

		memset(data, 0, sizeof(data));
		EATEST_VERIFY(ConvertASCIIArrayToBinaryData(result16, 8, data) == true);
		EATEST_VERIFY((data[0] == 0x12) && (data[3] == 0x78));

		memset(data, 0, sizeof(data));
		EATEST_VERIFY(ConvertASCIIArrayToBinaryData(result32, 8, data) == true);
		EATEST_VERIFY((data[0] == 0x12) && (data[3] == 0x78));


		memset(data, 0, sizeof(data));
		result8[0] = '_'; // Set it to an invalid hex character.
		EATEST_VERIFY(ConvertASCIIArrayToBinaryData(result8, 8, data) == false);
		EATEST_VERIFY((data[0] == 0x02) && (data[3] == 0x78));

		memset(data, 0, sizeof(data));
		result16[0] = '_'; // Set it to an invalid hex character.
		EATEST_VERIFY(ConvertASCIIArrayToBinaryData(result16, 8, data) == false);
		EATEST_VERIFY((data[0] == 0x02) && (data[3] == 0x78));

		memset(data, 0, sizeof(data));
		result32[0] = '_'; // Set it to an invalid hex character.
		EATEST_VERIFY(ConvertASCIIArrayToBinaryData(result32, 8, data) == false);
		EATEST_VERIFY((data[0] == 0x02) && (data[3] == 0x78));
	}


	// EASTDC_API const char*  GetTextLine(const char* pText, const char* pTextEnd, const char** ppNewText);
	{
		const char* p1 = "";
		const char* p2 = "\n";
		const char* p3 = "\r\n";
		const char* p4 = "\r\n\n";
		const char* p5 = "\n\r\r";
		const char* p6 = "aaa\nbbb\rccc\r\nddd";
		const char* p7 = "aaa\nddd\n";
		const char* p8 = "aaa\nddd\r\n";

		const char* pLine;
		const char* pLineEnd;
		const char* pLineNext;

		pLine = p1;
		pLineEnd = GetTextLine(pLine, p1 + Strlen(p1), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == pLine);

		pLine = p2;
		pLineEnd = GetTextLine(pLine, p2 + Strlen(p2), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p2 + Strlen(p2));

		pLine = p3;
		pLineEnd = GetTextLine(pLine, p3 + Strlen(p3), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p3 + Strlen(p3));

		pLine = p4;
		pLineEnd = GetTextLine(pLine, p4 + Strlen(p4), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p4 + 2);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p4 + Strlen(p4), &pLineNext);
		EATEST_VERIFY(pLineEnd == pLine);
		EATEST_VERIFY(pLineNext == p4 + Strlen(p4));

		pLine = p5;
		pLineEnd = GetTextLine(pLine, p5 + Strlen(p5), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p5 + 2);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p5 + Strlen(p5), &pLineNext);
		EATEST_VERIFY(pLineEnd == pLine);
		EATEST_VERIFY(pLineNext == p5 + Strlen(p5));

		pLine = p6;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 3);
		EATEST_VERIFY(pLineNext == p6 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 7);
		EATEST_VERIFY(pLineNext == p6 + 8);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 11);
		EATEST_VERIFY(pLineNext == p6 + 13);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + Strlen(p6));
		EATEST_VERIFY(pLineNext == p6 + Strlen(p6));

		pLine = p7;
		pLineEnd = GetTextLine(pLine, p7 + Strlen(p7), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p7 + 3);
		EATEST_VERIFY(pLineNext == p7 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p7 + Strlen(p7), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p7 + 7);
		EATEST_VERIFY(pLineNext == p7 + 8);

		pLine = p8;
		pLineEnd = GetTextLine(pLine, p8 + Strlen(p8), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p8 + 3);
		EATEST_VERIFY(pLineNext == p8 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p8 + Strlen(p8), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p8 + 7);
		EATEST_VERIFY(pLineNext == p8 + 9);
	}



	// EASTDC_API const char16_t* GetTextLine(const char16_t* pText, const char16_t* pTextEnd, const char16_t** ppNewText);
	{
		const char16_t* p1 = EA_CHAR16("");
		const char16_t* p2 = EA_CHAR16("\n");
		const char16_t* p3 = EA_CHAR16("\r\n");
		const char16_t* p4 = EA_CHAR16("\r\n\n");
		const char16_t* p5 = EA_CHAR16("\n\r\r");
		const char16_t* p6 = EA_CHAR16("aaa\nbbb\rccc\r\nddd");
		const char16_t* p7 = EA_CHAR16("aaa\nddd\n");
		const char16_t* p8 = EA_CHAR16("aaa\nddd\r\n");

		const char16_t* pLine;
		const char16_t* pLineEnd;
		const char16_t* pLineNext;

		pLine = p1;
		pLineEnd = GetTextLine(pLine, p1 + Strlen(p1), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == pLine);

		pLine = p2;
		pLineEnd = GetTextLine(pLine, p2 + Strlen(p2), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p2 + Strlen(p2));

		pLine = p3;
		pLineEnd = GetTextLine(pLine, p3 + Strlen(p3), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p3 + Strlen(p3));

		pLine = p4;
		pLineEnd = GetTextLine(pLine, p4 + Strlen(p4), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p4 + 2);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p4 + Strlen(p4), &pLineNext);
		EATEST_VERIFY(pLineEnd == pLine);
		EATEST_VERIFY(pLineNext == p4 + Strlen(p4));

		pLine = p5;
		pLineEnd = GetTextLine(pLine, p5 + Strlen(p5), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p5 + 2);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p5 + Strlen(p5), &pLineNext);
		EATEST_VERIFY(pLineEnd == pLine);
		EATEST_VERIFY(pLineNext == p5 + Strlen(p5));

		pLine = p6;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 3);
		EATEST_VERIFY(pLineNext == p6 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 7);
		EATEST_VERIFY(pLineNext == p6 + 8);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 11);
		EATEST_VERIFY(pLineNext == p6 + 13);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + Strlen(p6));
		EATEST_VERIFY(pLineNext == p6 + Strlen(p6));

		pLine = p7;
		pLineEnd = GetTextLine(pLine, p7 + Strlen(p7), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p7 + 3);
		EATEST_VERIFY(pLineNext == p7 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p7 + Strlen(p7), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p7 + 7);
		EATEST_VERIFY(pLineNext == p7 + 8);

		pLine = p8;
		pLineEnd = GetTextLine(pLine, p8 + Strlen(p8), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p8 + 3);
		EATEST_VERIFY(pLineNext == p8 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p8 + Strlen(p8), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p8 + 7);
		EATEST_VERIFY(pLineNext == p8 + 9);
	}

	// EASTDC_API const char32_t* GetTextLine(const char32_t* pText, const char32_t* pTextEnd, const char32_t** ppNewText);
	{
		const char32_t* p1 = EA_CHAR32("");
		const char32_t* p2 = EA_CHAR32("\n");
		const char32_t* p3 = EA_CHAR32("\r\n");
		const char32_t* p4 = EA_CHAR32("\r\n\n");
		const char32_t* p5 = EA_CHAR32("\n\r\r");
		const char32_t* p6 = EA_CHAR32("aaa\nbbb\rccc\r\nddd");
		const char32_t* p7 = EA_CHAR32("aaa\nddd\n");
		const char32_t* p8 = EA_CHAR32("aaa\nddd\r\n");

		const char32_t* pLine;
		const char32_t* pLineEnd;
		const char32_t* pLineNext;

		pLine = p1;
		pLineEnd = GetTextLine(pLine, p1 + Strlen(p1), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == pLine);

		pLine = p2;
		pLineEnd = GetTextLine(pLine, p2 + Strlen(p2), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p2 + Strlen(p2));

		pLine = p3;
		pLineEnd = GetTextLine(pLine, p3 + Strlen(p3), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p3 + Strlen(p3));

		pLine = p4;
		pLineEnd = GetTextLine(pLine, p4 + Strlen(p4), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p4 + 2);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p4 + Strlen(p4), &pLineNext);
		EATEST_VERIFY(pLineEnd == pLine);
		EATEST_VERIFY(pLineNext == p4 + Strlen(p4));

		pLine = p5;
		pLineEnd = GetTextLine(pLine, p5 + Strlen(p5), &pLineNext);
		EATEST_VERIFY(pLineEnd  == pLine);
		EATEST_VERIFY(pLineNext == p5 + 2);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p5 + Strlen(p5), &pLineNext);
		EATEST_VERIFY(pLineEnd == pLine);
		EATEST_VERIFY(pLineNext == p5 + Strlen(p5));

		pLine = p6;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 3);
		EATEST_VERIFY(pLineNext == p6 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 7);
		EATEST_VERIFY(pLineNext == p6 + 8);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + 11);
		EATEST_VERIFY(pLineNext == p6 + 13);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p6 + Strlen(p6), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p6 + Strlen(p6));
		EATEST_VERIFY(pLineNext == p6 + Strlen(p6));

		pLine = p7;
		pLineEnd = GetTextLine(pLine, p7 + Strlen(p7), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p7 + 3);
		EATEST_VERIFY(pLineNext == p7 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p7 + Strlen(p7), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p7 + 7);
		EATEST_VERIFY(pLineNext == p7 + 8);

		pLine = p8;
		pLineEnd = GetTextLine(pLine, p8 + Strlen(p8), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p8 + 3);
		EATEST_VERIFY(pLineNext == p8 + 4);
		pLine = pLineNext;
		pLineEnd = GetTextLine(pLine, p8 + Strlen(p8), &pLineNext);
		EATEST_VERIFY(pLineEnd  == p8 + 7);
		EATEST_VERIFY(pLineNext == p8 + 9);
	}


	// template<typename String>
	//  bool GetTextLine(String& sSource, String* pLine);
	{
		// To do
	}



	// EASTDC_API bool SplitTokenDelimited(const char*  pSource, size_t nSourceLength, char  cDelimiter, char*  pToken, size_t nTokenLength, const char**  ppNewSource = NULL);
	{
		// To do
	}

	// EASTDC_API bool SplitTokenDelimited(const char16_t* pSource, size_t nSourceLength, char16_t cDelimiter, char16_t* pToken, size_t nTokenLength, const char16_t** ppNewSource = NULL);
	{
		const char16_t* pSource;
		char16_t  pToken[8];

		///    source    token    new source   return
		///    ---------------------------------------
		///    "a,b"     "a"      "b"          true

		pSource = EA_CHAR16("a,b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    "ab,b"    "ab"     "b"          true

		pSource = EA_CHAR16("ab,b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("ab"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ",a,b"    ""       "a,b"        true

		pSource = EA_CHAR16(",a,b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("a,b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ",b"      ""       "b"          true

		pSource = EA_CHAR16(",b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ",,b"     ""       ",b"         true

		pSource = EA_CHAR16(",,b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(",b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ",a,"     ""       "a,"         true

		pSource = EA_CHAR16(",a,");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("a,"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    "a,"      "a"      ""           true

		pSource = EA_CHAR16("a,");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ","       ""       ""           true

		pSource = EA_CHAR16(",");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		///    source    token    new source   return
		///    ---------------------------------------
		///    "a"       "a"      ""           false

		pSource = EA_CHAR16("a");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ""        ""       ""           false

		pSource = EA_CHAR16("");

		EATEST_VERIFY(!SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		// various additional tests
		pSource = EA_CHAR16("testing,long,tokens");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("testing"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("long,tokens"));

		pSource = EA_CHAR16("tokentoolarge,test");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("tokento"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("test"));

		pSource = EA_CHAR16("test,source,length");

		EATEST_VERIFY(!SplitTokenDelimited(pSource, 0, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("test,source,length"));

		EATEST_VERIFY(SplitTokenDelimited(pSource, 2, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("te"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("st,source,length"));

		EATEST_VERIFY(SplitTokenDelimited(pSource, 3, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("st"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("source,length"));
	}

	// EASTDC_API bool SplitTokenDelimited(const char32_t* pSource, size_t nSourceLength, char32_t cDelimiter, char32_t* pToken, size_t nTokenLength, const char32_t** ppNewSource = NULL);
	{
		const char32_t* pSource;
		char32_t  pToken[8];

		///    source    token    new source   return
		///    ---------------------------------------
		///    "a,b"     "a"      "b"          true

		pSource = EA_CHAR32("a,b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    "ab,b"    "ab"     "b"          true

		pSource = EA_CHAR32("ab,b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("ab"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ",a,b"    ""       "a,b"        true

		pSource = EA_CHAR32(",a,b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("a,b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ",b"      ""       "b"          true

		pSource = EA_CHAR32(",b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ",,b"     ""       ",b"         true

		pSource = EA_CHAR32(",,b");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(",b"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ",a,"     ""       "a,"         true

		pSource = EA_CHAR32(",a,");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("a,"));


		///    source    token    new source   return
		///    ---------------------------------------
		///    "a,"      "a"      ""           true

		pSource = EA_CHAR32("a,");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ","       ""       ""           true

		pSource = EA_CHAR32(",");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		///    source    token    new source   return
		///    ---------------------------------------
		///    "a"       "a"      ""           false

		pSource = EA_CHAR32("a");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		///    source    token    new source   return
		///    ---------------------------------------
		///    ""        ""       ""           false

		pSource = EA_CHAR32("");

		EATEST_VERIFY(!SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		// various additional tests
		pSource = EA_CHAR32("testing,long,tokens");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("testing"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("long,tokens"));

		pSource = EA_CHAR32("tokentoolarge,test");

		EATEST_VERIFY(SplitTokenDelimited(pSource, kLengthNull, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("tokento"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("test"));

		pSource = EA_CHAR32("test,source,length");

		EATEST_VERIFY(!SplitTokenDelimited(pSource, 0, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("test,source,length"));

		EATEST_VERIFY(SplitTokenDelimited(pSource, 2, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("te"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("st,source,length"));

		EATEST_VERIFY(SplitTokenDelimited(pSource, 3, ',', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("st"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("source,length"));
	}


	// template<typename String, typename Char>
	// bool SplitTokenDelimited(String& sSource, Char cDelimiter, String* pToken);
	{
		// To do
	}


	// EASTDC_API bool SplitTokenSeparated(const char*  pSource, size_t nSourceLength, char  cDelimiter, char*  pToken, size_t nTokenLength, const char**  ppNewSource = NULL);
	{
		// To do
	}

	// EASTDC_API bool SplitTokenSeparated(const char16_t* pSource, size_t nSourceLength, char16_t cDelimiter, char16_t* pToken, size_t nTokenLength, const char16_t** ppNewSource = NULL);
	{
		String16        sSource, sToken;
		const char16_t* pSource;
		char16_t        pToken[8];

		///   source    token    new source   return
		///   ---------------------------------------
		///    "a"       "a"      ""           true

		pSource = EA_CHAR16("a");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    "a b"     "a"      "b"          true

		pSource = EA_CHAR16("a b");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("b"));


		///   source    token    new source   return
		///   ---------------------------------------
		///    "a  b"    "a"      "b"          true

		pSource = EA_CHAR16("a  b");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("b"));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " a b"    "a"      "b"          true

		pSource = EA_CHAR16(" a b");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull,' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("b"));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " a b "   "a"      "b "         true

		pSource = EA_CHAR16(" a b ");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("b "));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " a "     "a"      ""           true

		pSource = EA_CHAR16(" a ");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " a  "    "a"      ""           true

		pSource = EA_CHAR16(" a  ");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("a"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    ""        ""       ""           false

		pSource = EA_CHAR16("");

		EATEST_VERIFY(!SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " "       ""       ""           false

		pSource = EA_CHAR16(" ");

		EATEST_VERIFY(!SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " "       ""       ""           false

		pSource = NULL;

		EATEST_VERIFY(!SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16(""));
		EATEST_VERIFY(pSource == NULL);


		// various additional tests
		pSource = EA_CHAR16("testing;;;source;;;length");

		EATEST_VERIFY(SplitTokenSeparated(pSource, 4, ';', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("test"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("ing;;;source;;;length"));

		EATEST_VERIFY(SplitTokenSeparated(pSource, 5, ';', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("ing"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16(";source;;;length"));

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ';', pToken, 8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("source"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("length"));

		pSource = EA_CHAR16(";;;;;;;123 456;;;;;a");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ';', pToken,8, &pSource));
		EATEST_VERIFY(String16(pToken) == EA_CHAR16("123 456"));
		EATEST_VERIFY(String16(pSource) == EA_CHAR16("a"));
	}

	// EASTDC_API bool SplitTokenSeparated(const char32_t* pSource, size_t nSourceLength, char32_t cDelimiter, char32_t* pToken, size_t nTokenLength, const char32_t** ppNewSource = NULL);
	{
		String32        sSource, sToken;
		const char32_t* pSource;
		char32_t        pToken[8];

		///   source    token    new source   return
		///   ---------------------------------------
		///    "a"       "a"      ""           true

		pSource = EA_CHAR32("a");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    "a b"     "a"      "b"          true

		pSource = EA_CHAR32("a b");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("b"));


		///   source    token    new source   return
		///   ---------------------------------------
		///    "a  b"    "a"      "b"          true

		pSource = EA_CHAR32("a  b");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("b"));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " a b"    "a"      "b"          true

		pSource = EA_CHAR32(" a b");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull,' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("b"));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " a b "   "a"      "b "         true

		pSource = EA_CHAR32(" a b ");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("b "));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " a "     "a"      ""           true

		pSource = EA_CHAR32(" a ");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " a  "    "a"      ""           true

		pSource = EA_CHAR32(" a  ");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("a"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    ""        ""       ""           false

		pSource = EA_CHAR32("");

		EATEST_VERIFY(!SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " "       ""       ""           false

		pSource = EA_CHAR32(" ");

		EATEST_VERIFY(!SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(""));


		///   source    token    new source   return
		///   ---------------------------------------
		///    " "       ""       ""           false

		pSource = NULL;

		EATEST_VERIFY(!SplitTokenSeparated(pSource, kLengthNull, ' ', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32(""));
		EATEST_VERIFY(pSource == NULL);


		// various additional tests
		pSource = EA_CHAR32("testing;;;source;;;length");

		EATEST_VERIFY(SplitTokenSeparated(pSource, 4, ';', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("test"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("ing;;;source;;;length"));

		EATEST_VERIFY(SplitTokenSeparated(pSource, 5, ';', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("ing"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32(";source;;;length"));

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ';', pToken, 8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("source"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("length"));

		pSource = EA_CHAR32(";;;;;;;123 456;;;;;a");

		EATEST_VERIFY(SplitTokenSeparated(pSource, kLengthNull, ';', pToken,8, &pSource));
		EATEST_VERIFY(String32(pToken) == EA_CHAR32("123 456"));
		EATEST_VERIFY(String32(pSource) == EA_CHAR32("a"));
	}


	// template<typename String, typename Char>
	// bool SplitTokenSeparated(String& sSource, Char c, String* pToken);
	{
		// To do
	}

	return nErrorCount;
}











