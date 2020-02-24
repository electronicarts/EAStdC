///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EAStdC.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EARandom.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4996) // Function is deprecated.
#endif


static inline bool TestString_DoubleEqual(double x1, double x2)
{
	double difference = fabs(x1 - x2);
	double relative   = fabs(difference / x1);

	if (relative < 0.001)
		return true;
	else
	{
		EA::UnitTest::Report("TestString_DoubleEqual Error: %f, %f\n", x1, x2);
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////
// CharTraits 
//
// Simple template structure to assist with initializing buffers 
// and knowing the two other character types that aren't the given type
//

template <typename CharT>
struct CharTraits
{};

template <>
struct CharTraits<char>
{
	static char fill_value() { return 0x33; }
	static void assign(char* buffer, size_t size, char value) { EA::StdC::Memset8(buffer, value, size); }
	typedef char16_t char1_t;
	typedef char32_t char2_t;
};

template <>
struct CharTraits<char16_t>
{
	static char16_t fill_value() { return 0x3344; }
	static void assign(char16_t* buffer, size_t size, char16_t value) { EA::StdC::Memset16(buffer, value, size); }
	typedef char char1_t;
	typedef char32_t char2_t;
};

template <>
struct CharTraits<char32_t>
{
	static char32_t fill_value() { return 0x33445566; }
	static void assign(char32_t* buffer, size_t size, char32_t value) { EA::StdC::Memset32(buffer, value, size); }
	typedef char char1_t;
	typedef char16_t char2_t;
};

static const size_t kStrlcpyTestOutSize = 100;

///////////////////////////////////////////////////////////////////////////////
// StrlcpyTestPartial 
//
// Test the Strlcpy version that supports returning the number of read/written
// code units.
//
template <typename InCharT, typename OutCharT>
int StrlcpyTestPartial(OutCharT *pOutput, const InCharT* pInput, size_t nOutSize, size_t nInCodeUnits, 
	size_t nExpectedCodeUnits, bool bWillError)
{
	int nErrorCount = 0;

	CharTraits<OutCharT>::assign(pOutput, nOutSize + 1, CharTraits<OutCharT>::fill_value()); // We assume that there is always one extra code unit available
	size_t nOutCodeUnitsUsed;
	size_t nInCodeUnitsUsed;
	bool result = EA::StdC::Strlcpy(pOutput, pInput, nOutSize, nInCodeUnits, nOutCodeUnitsUsed, nInCodeUnitsUsed);
	if(!result)
	{
		EATEST_VERIFY(bWillError);
		return nErrorCount;
	}
	EATEST_VERIFY(!bWillError);

	// If the output has no space, then we can only verify that the fill value trashed
	if(nOutSize == 0)
	{
		EATEST_VERIFY(pOutput[0] == CharTraits<OutCharT>::fill_value());
		return nErrorCount;
	}

	// For simple strings, we can do a direct code unit compare.  If the number of code units match,
	// then assume this is a simple string.
	if(nInCodeUnitsUsed == nOutCodeUnitsUsed)
	{
		for(size_t i = 0; i < nOutCodeUnitsUsed; ++i)
		{
			EATEST_VERIFY(static_cast<uint32_t>(pOutput[i]) == static_cast<uint32_t>(pInput[i]));
		}
	}

	// Validate null and that fill value wasn't trashed past the end
	EATEST_VERIFY(pOutput[nOutCodeUnitsUsed] == 0);
	EATEST_VERIFY(pOutput[nOutCodeUnitsUsed + 1] == CharTraits<OutCharT>::fill_value());
	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// StrlcpyTestWithCount 
//
// Test the Strlcpy version supports the number of input code units being passed pInput.
//
template <typename InCharT, typename OutCharT>
int StrlcpyTestWithCount(OutCharT *pOutput, const InCharT* pInput, size_t nOutSize, size_t nInCodeUnits, size_t nExpectedCodeUnits, bool bWillError)
{
	int nErrorCount = 0;

	// Invoke the partial version if we have a count
	if(nInCodeUnits != EA::StdC::kSizeTypeUnset)
	{
		nErrorCount += StrlcpyTestPartial(pOutput, pInput, nOutSize, nInCodeUnits, nExpectedCodeUnits, bWillError);
	}

	CharTraits<OutCharT>::assign(pOutput, nOutSize + 1, CharTraits<OutCharT>::fill_value()); // We assume that there is always one extra code unit available
	int intResult = EA::StdC::Strlcpy(pOutput, pInput, nOutSize, nInCodeUnits);
	if(intResult < 0)
	{
		EATEST_VERIFY(bWillError);
		return nErrorCount;
	}
	EATEST_VERIFY(!bWillError);
	EATEST_VERIFY(intResult >= 0 && static_cast<size_t>(intResult) == nExpectedCodeUnits);

	// If the output has no space, then we can only verify that the fill value trashed
	if(nOutSize == 0)
	{
		EATEST_VERIFY(pOutput[0] == CharTraits<OutCharT>::fill_value());
		return nErrorCount;
	}

	// Get the actual number of code units supplied
	if(nInCodeUnits == EA::StdC::kSizeTypeUnset)
		nInCodeUnits = EA::StdC::Strlen(pInput);

	// The number of output code units doesn't have much to do with the return value when there is
	// an overrun.
	size_t outCodeUnits = static_cast<size_t>(intResult);
	if(outCodeUnits >= nOutSize)
		outCodeUnits = nOutSize - 1;

	// For simple strings, we can do a direct code unit compare.  If the number of code units match,
	// then assume this is a simple string.
	if(nInCodeUnits == nExpectedCodeUnits)
	{
		for(size_t i = 0; i < outCodeUnits; ++i)
		{
			EATEST_VERIFY(static_cast<uint32_t>(pOutput[i]) == static_cast<uint32_t>(pInput[i]));
		}
	}

	// Validate null and that fill value wasn't trashed past the end
	EATEST_VERIFY(pOutput[outCodeUnits] == 0);
	EATEST_VERIFY(pOutput[outCodeUnits + 1] == CharTraits<OutCharT>::fill_value());
	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// StrlcpyTestIWithoutCount 
//
// Test the Strlcpy version has no input code unit count
//
template <typename InCharT, typename OutCharT>
int StrlcpyTestIWithoutCount(OutCharT *pOutput, const InCharT* pInput, size_t nOutSize, size_t nExpectedCodeUnits, bool bWillError)
{
	int nErrorCount = 0;

	CharTraits<OutCharT>::assign(pOutput, nOutSize, CharTraits<OutCharT>::fill_value());
	int intResult = EA::StdC::Strlcpy(pOutput, pInput, nOutSize);
	if(bWillError)
	{
		EATEST_VERIFY(intResult == -1);
		return nErrorCount;
	}

	EATEST_VERIFY(intResult >= 0 && static_cast<size_t>(intResult) == nExpectedCodeUnits);
	EATEST_VERIFY(static_cast<size_t>(intResult) < nOutSize - 1); // To allow for termination check and overrun check

	size_t nInCodeUnits = EA::StdC::Strlen(pInput);

	if(nInCodeUnits == nExpectedCodeUnits)
	{
		for(size_t i = 0; i < nExpectedCodeUnits; ++i)
		{
			EATEST_VERIFY(static_cast<uint32_t>(pOutput[i]) == static_cast<uint32_t>(pInput[i]));
		}
	}
	EATEST_VERIFY(pOutput[nExpectedCodeUnits] == 0);
	EATEST_VERIFY(pOutput[nExpectedCodeUnits + 1] == CharTraits<OutCharT>::fill_value());
	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// StrlcpyTestConversion 
//
// Perform all the basic Strlcpy tests.
//
template <typename InCharT, typename OutCharT>
static int StrlcpyTestConversion(OutCharT *output, const InCharT* input, size_t nOutSize, size_t nInCodeUnits, size_t outCodeUnits, bool bWillError)
{
	EA_ASSERT(nOutSize <= kStrlcpyTestOutSize);
	int nErrorCount = 0;
	OutCharT tempBuff[kStrlcpyTestOutSize+1];

	// Verify that specifying the length works
	nErrorCount += StrlcpyTestWithCount(output, input, nOutSize, nInCodeUnits, outCodeUnits, bWillError);

	// These tests can only be performed if we aren't working with a short string
	if(input[nInCodeUnits] == 0)
	{

		// Verify that not specifying the length works
		nErrorCount += StrlcpyTestWithCount(tempBuff, input, nOutSize, EA::StdC::kSizeTypeUnset, outCodeUnits, bWillError);
		EATEST_VERIFY(EA::StdC::Strcmp(output, tempBuff) == 0);

		// Verify that version of the routine that takes no length works
		nErrorCount += StrlcpyTestIWithoutCount(tempBuff, input, nOutSize, outCodeUnits, bWillError);
		EATEST_VERIFY(EA::StdC::Strcmp(output, tempBuff) == 0);
	}
	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// StrlcpyTestConversionCompare 
//
// Given two types of strings, convert from one to the other and compare against the expected results
//
template <typename InCharT, typename OutCharT>
static int StrlcpyTestConversionCompare(const OutCharT* pInput, size_t nInCodeUnits, const InCharT* match, size_t matchCodeUnits)
{
	EA_ASSERT(nInCodeUnits <= kStrlcpyTestOutSize);
	int nErrorCount = 0;
	InCharT tempBuff[kStrlcpyTestOutSize+1];

	nErrorCount += StrlcpyTestConversion(tempBuff, pInput, kStrlcpyTestOutSize, nInCodeUnits, matchCodeUnits, false);

	EATEST_VERIFY(EA::StdC::Strncmp(match, tempBuff, matchCodeUnits) == 0);

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// StrlcpyTestBasics 
//
// Perform a basic series of conversion tests to make sure that the edge cases work
//
template <typename InCharT, typename OutCharT>
static int StrlcpyTestBasics(const InCharT* pInput, size_t nInCodeUnits)
{
	int nErrorCount = 0;
	OutCharT tempBuff[kStrlcpyTestOutSize+1];

	// Test including the null in the conversion.
	nErrorCount += StrlcpyTestWithCount(tempBuff, pInput, kStrlcpyTestOutSize, nInCodeUnits + 1, nInCodeUnits, false);

	// Test not including the last character
	if(nInCodeUnits > 0)
	{
		nErrorCount += StrlcpyTestWithCount(tempBuff, pInput, kStrlcpyTestOutSize, nInCodeUnits - 1, nInCodeUnits - 1, false);
	}

	// Test including no string at all
	nErrorCount += StrlcpyTestWithCount(tempBuff, pInput, kStrlcpyTestOutSize, 0, 0, false);

	// Test including a short buffer
	if (nInCodeUnits > 0)
	{
		nErrorCount += StrlcpyTestWithCount(tempBuff, pInput, nInCodeUnits - 1, nInCodeUnits, nInCodeUnits, false);
	}
	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// StrlcpyTestBasics 
//
// Given an input string of the given type, perform all the conversion tests 
// between all of the different type combinations.
//
template <typename InCharT>
static int StrlcpyTest(const InCharT* input, size_t nExpectedCodeUnits = EA::StdC::kSizeTypeUnset, bool bWillError = false, size_t nInCodeUnits = EA::StdC::kSizeTypeUnset)
{
	typedef typename CharTraits<InCharT>::char1_t char1_t;
	typedef typename CharTraits<InCharT>::char2_t char2_t;

	int nErrorCount = 0;

	size_t nInActualCodeUnits = EA::StdC::Strlen(input);
	if (nInCodeUnits == EA::StdC::kSizeTypeUnset)
		nInCodeUnits = nInActualCodeUnits;
	else if (nInCodeUnits < nInActualCodeUnits)
		nInActualCodeUnits = nInCodeUnits;
	if (nExpectedCodeUnits == EA::StdC::kSizeTypeUnset)
		nExpectedCodeUnits = nInCodeUnits;

	// PHASE 1: Create a utf8, ucs2 and ucs4 version of the string.

	// Convert to the first type
	char1_t char1Buff[kStrlcpyTestOutSize+1];
	nErrorCount += StrlcpyTestConversion(char1Buff, input, kStrlcpyTestOutSize, nInCodeUnits, nExpectedCodeUnits, bWillError);

	// Convert to the second type
	char2_t char2Buff[kStrlcpyTestOutSize+1];
	nErrorCount += StrlcpyTestConversion(char2Buff, input, kStrlcpyTestOutSize, nInCodeUnits, nExpectedCodeUnits, bWillError);

	// If errors are expected, we can't continue
	if(bWillError)
	{
		return nErrorCount;
	}

	// PHASE 2: Perform basic tests 
	if(nExpectedCodeUnits == nInActualCodeUnits)
	{
		nErrorCount += StrlcpyTestBasics<InCharT, char1_t>(input, nInActualCodeUnits);
		nErrorCount += StrlcpyTestBasics<InCharT, char2_t>(input, nInActualCodeUnits);
		nErrorCount += StrlcpyTestBasics<char1_t, InCharT>(char1Buff, nInActualCodeUnits);
		nErrorCount += StrlcpyTestBasics<char2_t, InCharT>(char2Buff, nInActualCodeUnits);
		nErrorCount += StrlcpyTestBasics<char1_t, char2_t>(char1Buff, nInActualCodeUnits);
		nErrorCount += StrlcpyTestBasics<char2_t, char1_t>(char2Buff, nInActualCodeUnits);
	}

	// PHASE 3: Make sure we can convert back to the original (this can fail if there are code points without 1-1 mapping in the string
	nErrorCount += StrlcpyTestConversionCompare(char1Buff, nExpectedCodeUnits, input, nInActualCodeUnits);
	nErrorCount += StrlcpyTestConversionCompare(char2Buff, nExpectedCodeUnits, input, nInActualCodeUnits);
	nErrorCount += StrlcpyTestConversionCompare(char1Buff, nExpectedCodeUnits, char2Buff, nExpectedCodeUnits);
	nErrorCount += StrlcpyTestConversionCompare(char2Buff, nExpectedCodeUnits, char1Buff, nExpectedCodeUnits);
	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestStringCore
//
EA_DISABLE_VC_WARNING(6262) // Function uses 'XXXX' bytes of stack space:  exceeds /analyze:stacksize '16384'
static int TestStringCore()
{
	using namespace EA::StdC;

	int    nErrorCount = 0;
	size_t sizeResult;

	//{ // Trigger crash intentionally.
	//    EA::StdC::AtofEnglish((char*)NULL); 
	//}

	{ // Test user report of inconsistency between FtoaEnglish and Snprintf.
		const char* stringValue = "1.2345";
		float floatValue = (float)EA::StdC::AtofEnglish(stringValue); 
		// floatValue = 1.2345000505447387 in FPU register.

		char tmp1[64];
		FtoaEnglish(floatValue, tmp1, sizeof(tmp1), 16, false);
		// tmp1 = "1.23450005"

		char tmp2[64];
		Snprintf(tmp2, sizeof(tmp2), "%.16f", floatValue);
		// tmp1 = "1.2345000505447388"

		EATEST_VERIFY(Strcmp(tmp1, tmp2) == 0);
	}


	// char*  Strcat(char*  pDestination, const char*  pSource);
	// char16_t* Strcat(char16_t* pDestination, const char16_t* pSource);
	// char32_t* Strcat(char32_t* pDestination, const char32_t* pSource);
	{
		char s_to[] = "hello\x0           ";
		const char* s_from = " world";

		EATEST_VERIFY(Strcat(s_to, s_from) == s_to);
		EATEST_VERIFY(Strcmp(s_to, "hello world") == 0);
	}
	{
		char16_t s_to[64]; Strlcpy(s_to, EA_CHAR16("hello\x0           "), EAArrayCount(s_to)); // Can't do s_to[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16(" world");

		EATEST_VERIFY(Strcat(s_to, s_from) == s_to);
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR16("hello world")) == 0);
	}
	{
		char32_t s_to[64]; Strlcpy(s_to, EA_CHAR32("hello\x0           "), EAArrayCount(s_to)); // Can't do s_to[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32(" world");

		EATEST_VERIFY(Strcat(s_to, s_from) == s_to);
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR32("hello world")) == 0);
	}


	// char*  Strncat(char*  pDestination, const char*  pSource, size_t n);
	// char16_t* Strncat(char16_t* pDestination, const char16_t* pSource, size_t n);
	// char32_t* Strncat(char32_t* pDestination, const char32_t* pSource, size_t n);
	{
		char s_to[] = "0123\x0......";
		const char* s_from = "456789";

		EATEST_VERIFY(Strncat(s_to, s_from, 5) == s_to);
		EATEST_VERIFY(Strcmp(s_to, "012345678") == 0);
		EATEST_VERIFY(s_to[9] == char(0));
	}
	{
		char16_t s_to[24]; Strlcpy(s_to, EA_CHAR16("0123\x0......"), EAArrayCount(s_to)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16("456789");

		EATEST_VERIFY(Strncat(s_to, s_from, 5) == s_to);
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR16("012345678")) == 0);
		EATEST_VERIFY(s_to[9] == char16_t(0));
	}
	{
		char32_t s_to[24]; Strlcpy(s_to, EA_CHAR32("0123\x0......"), EAArrayCount(s_to)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32("456789");

		EATEST_VERIFY(Strncat(s_to, s_from, 5) == s_to);
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR32("012345678")) == 0);
		EATEST_VERIFY(s_to[9] == char32_t(0));
	}


	// char*  StringnCat(char*  pDestination, const char*  pSource, size_t n);
	// char16_t* StringnCat(char16_t* pDestination, const char16_t* pSource, size_t n);
	// char32_t* StringnCat(char32_t* pDestination, const char32_t* pSource, size_t n);
	//
	// The StringnCopy and StringnCat functions appear to be broken. But since we 
	// are providing them in this library only for backward compatibility, 
	// we verify that their behaviour is the same as the existing rwstdc package, 
	// including any broken behaviour.
	{
		char s_to[] = "0123\x0......";
		const char* s_from = "456789";

		EATEST_VERIFY(StringnCat(s_to, s_from, 5) == s_to);
		//EATEST_VERIFY(Strcmp(s_to, "012345678..") == 0); Disabled while we try to clarify what the expected behaviour is.

		Memset8(s_to, (char)'.', Strlen(s_to));
		EATEST_VERIFY(StringnCat(s_to, s_from, 0) == s_to);
		EATEST_VERIFY(s_to[0] == '.');
	}
	{
		char16_t s_to[24]; Strlcpy(s_to, EA_CHAR16("0123\x0......"), EAArrayCount(s_to)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16("456789");

		EATEST_VERIFY(StringnCat(s_to, s_from, 5) == s_to);
		//EATEST_VERIFY(Strcmp(s_to, EA_CHAR16("012345678..")) == 0); Disabled while we try to clarify what the expected behaviour is.

		Memset16(s_to, (char16_t)'.', Strlen(s_to));
		EATEST_VERIFY(StringnCat(s_to, s_from, 0) == s_to);
		EATEST_VERIFY(s_to[0] == '.');
	}
	{
		char32_t s_to[24]; Strlcpy(s_to, EA_CHAR32("0123\x0......"), EAArrayCount(s_to)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32("456789");

		EATEST_VERIFY(StringnCat(s_to, s_from, 5) == s_to);
		//EATEST_VERIFY(Strcmp(s_to, EA_CHAR32("012345678..")) == 0); Disabled while we try to clarify what the expected behaviour is.

		Memset32(s_to, (char32_t)'.', Strlen(s_to));
		EATEST_VERIFY(StringnCat(s_to, s_from, 0) == s_to);
		EATEST_VERIFY(s_to[0] == '.');
	}


	// size_t Strlcat(char*  pDestination, const char*  pSource, size_t nDestCapacity);
	// size_t Strlcat(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity);
	// size_t Strlcat(char32_t* pDestination, const char32_t* pSource, size_t nDestCapacity);
	{
		char s_to[8] = "0123\x0..";
		const char* s_from = "456789";

		sizeResult = Strlcat(s_to, s_from, EAArrayCount(s_to));
		EATEST_VERIFY(sizeResult == Strlen("0123") + Strlen("456789"));
		EATEST_VERIFY(Strcmp(s_to, "0123456") == 0);
	}
	{
		char16_t s_to[8]; Strlcpy(s_to, EA_CHAR16("0123\x0.."), EAArrayCount(s_to)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16("456789");

		sizeResult = Strlcat(s_to, s_from, EAArrayCount(s_to));
		EATEST_VERIFY(sizeResult == Strlen("0123") + Strlen("456789"));
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR16("0123456")) == 0);
	}
	{
		char32_t s_to[8]; Strlcpy(s_to, EA_CHAR32("0123\x0.."), EAArrayCount(s_to)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32("456789");

		sizeResult = Strlcat(s_to, s_from, EAArrayCount(s_to));
		EATEST_VERIFY(sizeResult == Strlen("0123") + Strlen("456789"));
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR32("0123456")) == 0);
	}


	{
		char s_to[] = "01\x0........";
		const char* s_from = "23456";

		sizeResult = Strlcat(s_to, s_from, EAArrayCount(s_to));
		EATEST_VERIFY(sizeResult == Strlen("0123456"));
		EATEST_VERIFY(Strcmp(s_to, "0123456") == 0);
	}
	{
		char16_t s_to[16]; Strlcpy(s_to, EA_CHAR16("01\x0........"), EAArrayCount(s_to)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16("23456");

		sizeResult = Strlcat(s_to, s_from, EAArrayCount(s_to));
		EATEST_VERIFY(sizeResult == Strlen("0123456"));
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR16("0123456")) == 0);
	}
	{
		char32_t s_to[16]; Strlcpy(s_to, EA_CHAR32("01\x0........"), EAArrayCount(s_to)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32("23456");

		sizeResult = Strlcat(s_to, s_from, EAArrayCount(s_to));
		EATEST_VERIFY(sizeResult == Strlen("0123456"));
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR32("0123456")) == 0);
	}
	
	/// Regression testing for reported wchar_t array problem on 32 bits wchar_t machine.
	{
		char16_t array16[60] = { 'a', 0 };
		char32_t array32[60] = { 'b', 0 };

		sizeResult = Strlcat(array16, array32 , EAArrayCount(array16));
		EATEST_VERIFY(sizeResult == 2);
		EATEST_VERIFY((array16[0] == 'a') && (array16[1] == 'b') && (array16[2] == 0));
	}
	{
		char16_t array16[60] = { 'a', 0 };
		char32_t array32[60] = { 'b', 0 };

		sizeResult = Strlcat(array32, array16, EAArrayCount(array32));
		EATEST_VERIFY(sizeResult == 2);
		EATEST_VERIFY((array32[0] == 'b') && (array32[1] == 'a') && (array32[2] == 0));
	}


	/// char*  Strcpy(char*  pDestination, const char*  pSource);
	/// char16_t* Strcpy(char16_t* pDestination, const char16_t* pSource);
	/// char32_t* Strcpy(char32_t* pDestination, const char32_t* pSource);
	{
		char s_to[] = "0123456789";
		const char* s_from = "1234567890";

		EATEST_VERIFY(Strcpy(s_to, s_from) == s_to);
		EATEST_VERIFY(Strcmp(s_to, "1234567890") == 0);
	}
	{
		char16_t s_to[16]; Strlcpy(s_to, EA_CHAR16("0123456789"), EAArrayCount(s_to)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16("1234567890");

		EATEST_VERIFY(Strcpy(s_to, s_from) == s_to);
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR16("1234567890")) == 0);
	}
	{
		char32_t s_to[16]; Strlcpy(s_to, EA_CHAR32("0123456789"), EAArrayCount(s_to)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32("1234567890");

		EATEST_VERIFY(Strcpy(s_to, s_from) == s_to);
		EATEST_VERIFY(Strcmp(s_to, EA_CHAR32("1234567890")) == 0);
	}


	// char*  Strncpy(char*  pDestination, const char*  pSource, size_t n);
	// char16_t* Strncpy(char16_t* pDestination, const char16_t* pSource, size_t n);
	// char32_t* Strncpy(char32_t* pDestination, const char32_t* pSource, size_t n);
	{
		char  s_to[] = "...........................";
		const char* s_from = "l;kajjsdf;q4w3rrpoiu113<>)(";

		EATEST_VERIFY(Strcpy(s_to, s_from) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from, Strlen(s_from) * sizeof(char)) == 0);

		Memset8(s_to, (char)'.', Strlen(s_to));

		EATEST_VERIFY(Strncpy(s_to, s_from+14, 5) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from+14, 5 * sizeof(char)) == 0);
		EATEST_VERIFY(s_to[5] == (char)'.' );
	}
	{
		char16_t s_to[32]; Strlcpy(s_to, EA_CHAR16("..........................."), EAArrayCount(s_to)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16("l;kajjsdf;q4w3rrpoiu113<>)(");

		EATEST_VERIFY(Strcpy(s_to, s_from) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from, Strlen(s_from) * sizeof(char16_t)) == 0);

		Memset16(s_to, (char16_t)'.', Strlen(s_to));

		EATEST_VERIFY(Strncpy(s_to, s_from+14, 5) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from+14, 5 * sizeof(char16_t)) == 0);
		EATEST_VERIFY(s_to[5] == (char16_t)'.' );
	}
	{
		char32_t s_to[32]; Strlcpy(s_to, EA_CHAR32("..........................."), EAArrayCount(s_to)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32("l;kajjsdf;q4w3rrpoiu113<>)(");

		EATEST_VERIFY(Strcpy(s_to, s_from) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from, Strlen(s_from) * sizeof(char32_t)) == 0);

		Memset32(s_to, (char32_t)'.', Strlen(s_to));

		EATEST_VERIFY(Strncpy(s_to, s_from+14, 5) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from+14, 5 * sizeof(char32_t)) == 0);
		EATEST_VERIFY(s_to[5] == (char32_t)'.' );
	}


	// char*  StringnCopy(char*  pDestination, const char*  pSource, size_t n);
	// char16_t* StringnCopy(char16_t* pDestination, const char16_t* pSource, size_t n);
	// char32_t* StringnCopy(char32_t* pDestination, const char32_t* pSource, size_t n);
	{
		char  s_to[] = "...........................";
		const char* s_from = "l;kajjsdf;q4w3rrpoiu113<>)(";

		EATEST_VERIFY(StringnCopy(s_to, s_from+14, 5) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from+14, 5 * sizeof(char)) == 0);
		EATEST_VERIFY(s_to[5] == (char)'.' );

		// Test copying nothing.
		Memset8(s_to, (char)'.', Strlen(s_to));
		EATEST_VERIFY(StringnCopy(s_to, s_from+14, 0) == s_to);
		EATEST_VERIFY(s_to[0] == '.');
	}
	{
		char16_t s_to[32]; Strlcpy(s_to, EA_CHAR16(".........................."), EAArrayCount(s_to)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16("l;kajjsdf;q4w3rrpoiu113<>)(");

		EATEST_VERIFY(StringnCopy(s_to, s_from+14, 5) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from+14, 5 * sizeof(char16_t)) == 0);
		EATEST_VERIFY(s_to[5] == (char)'.' );

		// Test copying nothing.
		Memset16(s_to, (char16_t)'.', Strlen(s_to));
		EATEST_VERIFY(StringnCopy(s_to, s_from+14, 0) == s_to);
		EATEST_VERIFY(s_to[0] == '.');
	}
	{
		char32_t s_to[32]; Strlcpy(s_to, EA_CHAR32(".........................."), EAArrayCount(s_to)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32("l;kajjsdf;q4w3rrpoiu113<>)(");

		EATEST_VERIFY(StringnCopy(s_to, s_from+14, 5) == s_to);
		EATEST_VERIFY(Memcmp(s_to, s_from+14, 5 * sizeof(char32_t)) == 0);
		EATEST_VERIFY(s_to[5] == (char)'.' );

		// Test copying nothing.
		Memset32(s_to, (char32_t)'.', Strlen(s_to));
		EATEST_VERIFY(StringnCopy(s_to, s_from+14, 0) == s_to);
		EATEST_VERIFY(s_to[0] == '.');
	}

	// size_t Strlcpy(char*  pDestination, const char*  pSource, size_t nDestCapacity);
	// size_t Strlcpy(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity);
	// size_t Strlcpy(char32_t* pDestination, const char32_t* pSource, size_t nDestCapacity);
	// int Strlcpy(char*  pDestination, const char16_t* pSource, size_t nDestCapacity, size_t nSourceLength = (size_t)~0);
	// int Strlcpy(char16_t* pDestination, const char*  pSource, size_t nDestCapacity, size_t nSourceLength = (size_t)~0);
	// int Strlcpy(char*  pDestination, const char32_t* pSource, size_t nDestCapacity, size_t nSourceLength = (size_t)~0);
	// int Strlcpy(char32_t* pDestination, const char*  pSource, size_t nDestCapacity, size_t nSourceLength = (size_t)~0);
	// int Strlcpy(char16_t* pDestination, const char32_t* pSource, size_t nDestCapacity, size_t nSourceLength = (size_t)~0);
	// int Strlcpy(char32_t* pDestination, const char16_t* pSource, size_t nDestCapacity, size_t nSourceLength = (size_t)~0);
	{
		nErrorCount += StrlcpyTest("l;kajjsdf;q4w3rrpoiu113____");
		nErrorCount += StrlcpyTest("Foo!");
		nErrorCount += StrlcpyTest("a");
		nErrorCount += StrlcpyTest("");
		nErrorCount += StrlcpyTest("\x43\x3A\x5C\x45\x6C\x65\x63\x74\x72\x6F\x6E\x69\x63\x20\x41\x72\x74\x73\x5C\xE3\x82\xB6\xEF\xBD\xA5\xE3\x82\xB7\xE3\x83\xA0\xE3\x82\xBA\xEF\xBC\x93", 25);

		const char* kInvalidUTF8StringArray[] = {
			"\xc2",       // 1 byte of a 2 byte sequence
			"\xc2\x20",   // 1 byte of a 2 byte sequence
			"\xe0",       // 1 byte of a 3 byte sequence
			"\xe0\x20",   // 1 byte of a 3 byte sequence
			"\xe0",       // 1 byte of a 3 byte sequence
			"\xf0",       // 1 byte of a 4 byte sequence
			"\xf0\x20",   // 1 byte of a 4 byte sequence
			"\xf8",       // 1 byte of a 5 byte sequence
			"\xf8\x20",   // 1 byte of a 5 byte sequence
			"\xfc",       // 1 byte of a 6 byte sequence
			"\xfc\x20",   // 1 byte of a 6 byte sequence
			"\xfe"        // 1 byte of an invalid sequence
		};

		for (size_t u = 0; u < EAArrayCount(kInvalidUTF8StringArray); u++)
		{
			nErrorCount += StrlcpyTest(kInvalidUTF8StringArray[u], 0, true);
		}
	}

	/// size_t Strlen(const char*  pString);
	/// size_t Strlen(const char16_t* pString);
	/// size_t Strlen(const char32_t* pString);
	{
		// We test many combinations of data and alignments.
		RandomFast   random;
		const size_t kBufferSize(4096 + sizeof(void*));
		uint8_t      buffer[kBufferSize];
		Stopwatch    stopwatch(Stopwatch::kUnitsCycles, false);

		for(size_t i = 0; i < kBufferSize; ++i)
		{
			const uint8_t c = (uint8_t)(random.RandomUint32Uniform() >> 8);
			buffer[i] = c ? c : (uint8_t)0x20;
		}
		buffer[kBufferSize - 1] = 0;

		stopwatch.Start();
		for(size_t j = 0; j < kBufferSize; ++j)
		{
			size_t n = Strlen((char*)buffer + j);

			EATEST_VERIFY(n == ((kBufferSize - 1) - j));
			//if(n != ((kBufferSize - 1) - j))
			//    ++j;
		}
		stopwatch.Stop();
		//EA::UnitTest::Report("Time %I64u\n", stopwatch.GetElapsedTime());
	}
	{
		// We test many combinations of data and alignments.
		RandomFast   random;
		const size_t kBufferSize(2048 + sizeof(void*));
		uint16_t     buffer[kBufferSize];
		Stopwatch    stopwatch(Stopwatch::kUnitsCycles, false);

		for(size_t i = 0; i < kBufferSize; ++i)
		{
			const uint16_t c = (uint16_t)(random.RandomUint32Uniform() >> 8);
			buffer[i] = c ? c : (uint16_t)0x0020;
		}
		buffer[kBufferSize - 1] = 0;

		stopwatch.Start();
		for(size_t j = 0; j < kBufferSize; ++j)
		{
			size_t n = Strlen((char16_t*)buffer + j);

			EATEST_VERIFY(n == ((kBufferSize - 1) - j));
			//if(n != ((kBufferSize - 1) - j))
			//    ++j;
		}
		stopwatch.Stop();
		//EA::UnitTest::Report("Time %I64u\n", stopwatch.GetElapsedTime());
	}
	{
		// We test many combinations of data and alignments.
		RandomFast   random;
		const size_t kBufferSize(2048 + sizeof(void*));
		uint32_t     buffer[kBufferSize];
		Stopwatch    stopwatch(Stopwatch::kUnitsCycles, false);

		for(size_t i = 0; i < kBufferSize; ++i)
		{
			const uint32_t c = (uint32_t)(random.RandomUint32Uniform() >> 8);
			buffer[i] = c ? c : (uint32_t)0x0020;
		}
		buffer[kBufferSize - 1] = 0;

		stopwatch.Start();
		for(size_t j = 0; j < kBufferSize; ++j)
		{
			size_t n = Strlen((char32_t*)buffer + j);

			EATEST_VERIFY(n == ((kBufferSize - 1) - j));
			//if(n != ((kBufferSize - 1) - j))
			//    ++j;
		}
		stopwatch.Stop();
		//EA::UnitTest::Report("Time %I64u\n", stopwatch.GetElapsedTime());
	}



	/// size_t StrlenUTF8Decoded(const char* pString);
	/// size_t StrlenUTF8Encoded(const char16_t* pString);
	/// size_t StrlenUTF8Encoded(const char32_t* pString);
	{
		EATEST_VERIFY(StrlenUTF8Decoded("0123456789") == 10);
		EATEST_VERIFY(StrlenUTF8Decoded("") == 0);
		EATEST_VERIFY(StrlenUTF8Decoded("\xc2" "\xa2") == 1);
		EATEST_VERIFY(StrlenUTF8Decoded("\xd7" "\x90") == 1);
		EATEST_VERIFY(StrlenUTF8Decoded("\xe0" "\xbc" "\xa0") == 1);
		EATEST_VERIFY(StrlenUTF8Decoded("\xc2\x80 \xc2\x81 \xdf\xbe \xdf\xbf") == 7);
		EATEST_VERIFY(StrlenUTF8Decoded("\xe0\xa0\x80 \xe0\xa0\x81 \xef\xbf\xbe \xef\xbf\xbf") == 7);
		EATEST_VERIFY(StrlenUTF8Decoded("\xf0\x90\x80\x80 \xf0\x90\x80\x81") == 3);
		EATEST_VERIFY(StrlenUTF8Decoded("\xf4\x8f\xbf\xbe \xf4\x8f\xbf\xbf") == 3);
	}
	{
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR16("0123456789")) == 10);
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR16("")) == 0);
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR16("\x00a0")) == 2);
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR16("\x0400")) == 2);
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR16("\x0800")) == 3);

		// We have to break up the string into multiple sub-strings because the \x escape sequence has limitations in how it works.
		eastl::fixed_string<char16_t, 32> s16; s16 = EA_CHAR16("\xffff"); s16 += EA_CHAR16("\xffff"); // We use a string object because some compilers don't support 16 bit string literals, and thus EA_CHAR16 is a function and doesn't just prepend "L" or "u" to the string.
		EATEST_VERIFY(StrlenUTF8Encoded(s16.c_str()) == 6);

		s16 = EA_CHAR16("\xffff"); s16 += EA_CHAR16("\x0900"); s16 += EA_CHAR16("0"); s16 += EA_CHAR16("\x00a0");
		EATEST_VERIFY(StrlenUTF8Encoded(s16.c_str()) == 9);
	}
	{
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR32("0123456789")) == 10);
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR32("")) == 0);
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR32("\x00a0")) == 2);
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR32("\x0400")) == 2);
		EATEST_VERIFY(StrlenUTF8Encoded(EA_CHAR32("\x0800")) == 3);

		// We have to break up the string into multiple sub-strings because the \x escape sequence has limitations in how it works.
		eastl::fixed_string<char32_t, 32> s32; s32 = EA_CHAR32("\xffff"); s32 += EA_CHAR32("\xffff"); // We use a string object because some compilers don't support 32 bit string literals, and thus EA_CHAR32 is a function and doesn't just prepend "L" or "u" to the string.
		EATEST_VERIFY(StrlenUTF8Encoded(s32.c_str()) == 6);

		s32 = EA_CHAR32("\xffff"); s32 += EA_CHAR32("\x0900"); s32 += EA_CHAR32("0"); s32 += EA_CHAR32("\x00a0");
		EATEST_VERIFY(StrlenUTF8Encoded(s32.c_str()) == 9);
	}


	/// char*  Strend(const char*  pString);
	/// char16_t* Strend(const char16_t* pString);
	/// char32_t* Strend(const char32_t* pString);
	{
		const char* pString = "0123456789";
		EATEST_VERIFY(Strend(pString) == (pString + Strlen(pString)));
	}
	{
		const char16_t* pString = EA_CHAR16("0123456789");
		EATEST_VERIFY(Strend(pString) == (pString + Strlen(pString)));
	}
	{
		const char32_t* pString = EA_CHAR32("0123456789");
		EATEST_VERIFY(Strend(pString) == (pString + Strlen(pString)));
	}


	// size_t Strxfrm(char*  pDest, const char*  pSource, size_t n);
	// size_t Strxfrm(char16_t* pDest, const char16_t* pSource, size_t n);
	// size_t Strxfrm(char32_t* pDest, const char32_t* pSource, size_t n);
	{
		// To do: Make a better test.
		char  s_to[] = "...........................";
		const char* s_from = "l;kajjsdf;q4w3rrpoiu113<>)(";

		const size_t n = Strxfrm(s_to, s_from, Strlen(s_from) + 1);
		EATEST_VERIFY(Strcmp(s_to, s_from) == 0);
		EATEST_VERIFY(n == Strlen(s_from));
	}
	{
		// To do: Make a better test.
		char16_t s_to[32]; Strlcpy(s_to, EA_CHAR16("..........................."), EAArrayCount(s_to)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		const char16_t* s_from = EA_CHAR16("l;kajjsdf;q4w3rrpoiu113<>)(");

		const size_t n = Strxfrm(s_to, s_from, Strlen(s_from) + 1);
		EATEST_VERIFY(Strcmp(s_to, s_from) == 0);
		EATEST_VERIFY(n == Strlen(s_from));
	}
	{
		// To do: Make a better test.
		char32_t s_to[32]; Strlcpy(s_to, EA_CHAR32("..........................."), EAArrayCount(s_to)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		const char32_t* s_from = EA_CHAR32("l;kajjsdf;q4w3rrpoiu113<>)(");

		const size_t n = Strxfrm(s_to, s_from, Strlen(s_from) + 1);
		EATEST_VERIFY(Strcmp(s_to, s_from) == 0);
		EATEST_VERIFY(n == Strlen(s_from));
	}


	// char*  Strdup(const char*  pString);
	// char16_t* Strdup(const char16_t* pString);
	// char32_t* Strdup(const char32_t* pString);
	// void      Strdel(char*  pString);
	// void      Strdel(char16_t* pString);
	// void      Strdel(char32_t* pString);
	{
		typedef char test_type;
		test_type  s_from[]  = "...........................";
		test_type* s_new = NULL;

		EATEST_VERIFY((s_new = Strdup(s_from)) != NULL);
		EATEST_VERIFY(Strcmp(s_from, s_new) == 0);

		Strdel(s_new);
	}
	{
		typedef char16_t test_type;
		char16_t   s_from[32]; Strlcpy(s_from, EA_CHAR16("..........................."), EAArrayCount(s_from)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		test_type* s_new = NULL;

		EATEST_VERIFY((s_new = Strdup(s_from)) != NULL);
		EATEST_VERIFY(Strcmp(s_from, s_new) == 0);

		Strdel(s_new);
	}
	{
		typedef char32_t test_type;
		char32_t   s_from[32]; Strlcpy(s_from, EA_CHAR32("..........................."), EAArrayCount(s_from)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		test_type* s_new = NULL;

		EATEST_VERIFY((s_new = Strdup(s_from)) != NULL);
		EATEST_VERIFY(Strcmp(s_from, s_new) == 0);

		Strdel(s_new);
	}


	// char*  Strupr(char*  pString);
	// char16_t* Strupr(char16_t* pString);
	// char32_t* Strupr(char32_t* pString);
	{
		char s8[] = "hello world";

		EATEST_VERIFY(Strupr(s8) == s8);
		EATEST_VERIFY(Memcmp(s8, "HELLO WORLD", EAArrayCount(s8)) == 0);
	}
	{
		char16_t s16[16]; Strlcpy(s16, EA_CHAR16("hello world"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strupr(s16) == s16);
		EATEST_VERIFY(Memcmp(s16, EA_CHAR16("HELLO WORLD"), EAArrayCount(s16)) == 0);
	}
	{
		char32_t s32[32]; Strlcpy(s32, EA_CHAR32("hello world"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strupr(s32) == s32);
		EATEST_VERIFY(Memcmp(s32, EA_CHAR32("HELLO WORLD"), EAArrayCount(s32)) == 0);
	}


	// char*  Strlwr(char*  pString);
	// char16_t* Strlwr(char16_t* pString);
	// char32_t* Strlwr(char32_t* pString);
	{
		char s8[] = "HELLO WORLD";

		EATEST_VERIFY(Strlwr(s8) == s8);
		EATEST_VERIFY(Memcmp(s8, "hello world", EAArrayCount(s8)) == 0);
	}
	{
		char16_t s16[16]; Strlcpy(s16, EA_CHAR16("HELLO WORLD"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strlwr(s16) == s16);
		EATEST_VERIFY(Memcmp(s16, EA_CHAR16("hello world"), EAArrayCount(s16)) == 0);
	}
	{
		char32_t s32[32]; Strlcpy(s32, EA_CHAR32("HELLO WORLD"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strlwr(s32) == s32);
		EATEST_VERIFY(Memcmp(s32, EA_CHAR32("hello world"), EAArrayCount(s32)) == 0);
	}


	// char*  Strchr(const char*  pString, int c);
	// char16_t* Strchr(const char16_t* pString, char16_t c);
	// char32_t* Strchr(const char32_t* pString, char32_t c);
	{
		char s8[] = "012a456789abc2ef";

		EATEST_VERIFY(Strrchr(s8, 'a')   == &s8[0xa]);
		EATEST_VERIFY(Strrchr(s8, '2')   == &s8[0xd]);
		EATEST_VERIFY(Strrchr(s8, '0')   == &s8[0x0]);
		EATEST_VERIFY(Strrchr(s8, '\x0') == &s8[0x10]);
	}
	{
		char16_t s16[24]; Strlcpy(s16, EA_CHAR16("012a456789abc2ef"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strrchr(s16, 'a')   == &s16[0xa]);
		EATEST_VERIFY(Strrchr(s16, '2')   == &s16[0xd]);
		EATEST_VERIFY(Strrchr(s16, '0')   == &s16[0x0]);
		EATEST_VERIFY(Strrchr(s16, '\x0') == &s16[0x10]);
	}
	{
		char32_t s32[24]; Strlcpy(s32, EA_CHAR32("012a456789abc2ef"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strrchr(s32, 'a')   == &s32[0xa]);
		EATEST_VERIFY(Strrchr(s32, '2')   == &s32[0xd]);
		EATEST_VERIFY(Strrchr(s32, '0')   == &s32[0x0]);
		EATEST_VERIFY(Strrchr(s32, '\x0') == &s32[0x10]);
	}


	// size_t Strcspn(const char*  pString1, const char*  pString2);
	// size_t Strcspn(const char16_t* pString1, const char16_t* pString2);
	// size_t Strcspn(const char32_t* pString1, const char32_t* pString2);
	{
		char s8[] = "0123456789abcdef";

		EATEST_VERIFY(Strcspn(s8, "@fa")    == 0xa);
		EATEST_VERIFY(Strcspn(s8, "5.a,f")  == 0x5);
		EATEST_VERIFY(Strcspn(s8, ":/1")    == 0x1);
		EATEST_VERIFY(Strcspn(s8, ";/\x0!") == 0x10);
	}
	{
		char16_t s16[24]; Strlcpy(s16, EA_CHAR16("0123456789abcdef"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strcspn(s16, EA_CHAR16("@fa"))    == 0xa);
		EATEST_VERIFY(Strcspn(s16, EA_CHAR16("5.a,f"))  == 0x5);
		EATEST_VERIFY(Strcspn(s16, EA_CHAR16(":/1"))    == 0x1);
		EATEST_VERIFY(Strcspn(s16, EA_CHAR16(";/\x0!")) == 0x10);
	}
	{
		char32_t s32[24]; Strlcpy(s32, EA_CHAR32("0123456789abcdef"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strcspn(s32, EA_CHAR32("@fa"))    == 0xa);
		EATEST_VERIFY(Strcspn(s32, EA_CHAR32("5.a,f"))  == 0x5);
		EATEST_VERIFY(Strcspn(s32, EA_CHAR32(":/1"))    == 0x1);
		EATEST_VERIFY(Strcspn(s32, EA_CHAR32(";/\x0!")) == 0x10);
	}


	// char*  Strpbrk(const char*  pString1, const char*  pString2);
	// char16_t* Strpbrk(const char16_t* pString1, const char16_t* pString2);
	// char32_t* Strpbrk(const char32_t* pString1, const char32_t* pString2);
	{
		char s8[] = "0123456789abcdef";

		EATEST_VERIFY(Strpbrk(s8, "@fa")    == &s8[0xa]);
		EATEST_VERIFY(Strpbrk(s8, "5.a,f")  == &s8[0x5]);
		EATEST_VERIFY(Strpbrk(s8, ":/1")    == &s8[0x1]);
		EATEST_VERIFY(Strpbrk(s8, ";/\x0!") == NULL);
	}
	{
		char16_t s16[24]; Strlcpy(s16, EA_CHAR16("0123456789abcdef"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strpbrk(s16, EA_CHAR16("@fa"))    == &s16[0xa]);
		EATEST_VERIFY(Strpbrk(s16, EA_CHAR16("5.a,f"))  == &s16[0x5]);
		EATEST_VERIFY(Strpbrk(s16, EA_CHAR16(":/1"))    == &s16[0x1]);
		EATEST_VERIFY(Strpbrk(s16, EA_CHAR16(";/\x0!")) == NULL);
	}
	{
		char32_t s32[24]; Strlcpy(s32, EA_CHAR32("0123456789abcdef"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strpbrk(s32, EA_CHAR32("@fa"))    == &s32[0xa]);
		EATEST_VERIFY(Strpbrk(s32, EA_CHAR32("5.a,f"))  == &s32[0x5]);
		EATEST_VERIFY(Strpbrk(s32, EA_CHAR32(":/1"))    == &s32[0x1]);
		EATEST_VERIFY(Strpbrk(s32, EA_CHAR32(";/\x0!")) == NULL);
	}


	// char*  Strrchr(const char*  pString, int      c);
	// char16_t* Strrchr(const char16_t* pString, char16_t c);
	// char32_t* Strrchr(const char32_t* pString, char32_t c);
	{
		char s8[] = "0123456789abcdef";

		EATEST_VERIFY(Strchr(s8, 'z')   == NULL);
		EATEST_VERIFY(Strchr(s8, 'a')   == &s8[0xa]);
		EATEST_VERIFY(Strchr(s8, '0')   == &s8[0x0]);
		EATEST_VERIFY(Strchr(s8, '\x0') == &s8[0x10]);
	}
	{
		char16_t s16[24]; Strlcpy(s16, EA_CHAR16("0123456789abcdef"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strchr(s16, 'z')   == NULL);
		EATEST_VERIFY(Strchr(s16, 'a')   == &s16[0xa]);
		EATEST_VERIFY(Strchr(s16, '0')   == &s16[0x0]);
		EATEST_VERIFY(Strchr(s16, '\x0') == &s16[0x10]);
	}
	{
		char32_t s32[24]; Strlcpy(s32, EA_CHAR32("0123456789abcdef"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strchr(s32, 'z')   == NULL);
		EATEST_VERIFY(Strchr(s32, 'a')   == &s32[0xa]);
		EATEST_VERIFY(Strchr(s32, '0')   == &s32[0x0]);
		EATEST_VERIFY(Strchr(s32, '\x0') == &s32[0x10]);
	}

		{
		char s8[] = "0123456789abcdef";

		EATEST_VERIFY(Strnchr(s8, 'z', EAArrayCount(s8))   == NULL);
		EATEST_VERIFY(Strnchr(s8, 'a', EAArrayCount(s8))   == &s8[0xa]);
		EATEST_VERIFY(Strnchr(s8, '5', 0) == NULL);
		EATEST_VERIFY(Strnchr(s8, '5', 1) == NULL);
		EATEST_VERIFY(Strnchr(s8, '5', 2) == NULL);
		EATEST_VERIFY(Strnchr(s8, '5', 5) == NULL);
		EATEST_VERIFY(Strnchr(s8, '5', 6) == &s8[0x5]);
		EATEST_VERIFY(Strnchr(s8, '5', 7) == &s8[0x5]);
		EATEST_VERIFY(Strnchr(s8, '5', 9) == &s8[0x5]);
		EATEST_VERIFY(Strnchr(s8, '5', EAArrayCount(s8)) == &s8[0x5]);
		EATEST_VERIFY(Strnchr(s8, '0', 0) == NULL);
		EATEST_VERIFY(Strnchr(s8, '0', 1) == &s8[0x0]);
		EATEST_VERIFY(Strnchr(s8, '0', 2) == &s8[0x0]);
		EATEST_VERIFY(Strnchr(s8, '0', 9) == &s8[0x0]);
		EATEST_VERIFY(Strnchr(s8, '0', EAArrayCount(s8)) == &s8[0x0]);
		EATEST_VERIFY(Strnchr(s8, 'f', 0) == NULL);
		EATEST_VERIFY(Strnchr(s8, 'f', 1) == NULL);
		EATEST_VERIFY(Strnchr(s8, 'f', 5) == NULL);
		EATEST_VERIFY(Strnchr(s8, 'f', EA::StdC::Strlen(s8)-1) == NULL);
		EATEST_VERIFY(Strnchr(s8, 'f', EA::StdC::Strlen(s8)) == &s8[0xf]);
		EATEST_VERIFY(Strnchr(s8, 'f', EA::StdC::Strlen(s8)+1) == &s8[0xf]); //This is the null terminator
		EATEST_VERIFY(Strnchr(s8, '\0', 0) == NULL);
		EATEST_VERIFY(Strnchr(s8, '\0', 1) == NULL);
		EATEST_VERIFY(Strnchr(s8, '\0', 5) == NULL);
		EATEST_VERIFY(Strnchr(s8, '\0', EA::StdC::Strlen(s8)-1) == NULL);
		EATEST_VERIFY(Strnchr(s8, '\0', EA::StdC::Strlen(s8)) == NULL);
		EATEST_VERIFY(Strnchr(s8, '\0', EA::StdC::Strlen(s8)+1) == &s8[0x10]); //This is the null terminator
	}
	{
		char16_t s16[24]; Strlcpy(s16, EA_CHAR16("0123456789abcdef"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strnchr(s16, 'z', EAArrayCount(s16))   == NULL);
		EATEST_VERIFY(Strnchr(s16, 'a', EAArrayCount(s16))   == &s16[0xa]);
		EATEST_VERIFY(Strnchr(s16, '5', 0) == NULL);
		EATEST_VERIFY(Strnchr(s16, '5', 1) == NULL);
		EATEST_VERIFY(Strnchr(s16, '5', 2) == NULL);
		EATEST_VERIFY(Strnchr(s16, '5', 5) == NULL);
		EATEST_VERIFY(Strnchr(s16, '5', 6) == &s16[0x5]);
		EATEST_VERIFY(Strnchr(s16, '5', 7) == &s16[0x5]);
		EATEST_VERIFY(Strnchr(s16, '5', 9) == &s16[0x5]);
		EATEST_VERIFY(Strnchr(s16, '5', EAArrayCount(s16)) == &s16[0x5]);
		EATEST_VERIFY(Strnchr(s16, '0', 0) == NULL);
		EATEST_VERIFY(Strnchr(s16, '0', 1) == &s16[0x0]);
		EATEST_VERIFY(Strnchr(s16, '0', 2) == &s16[0x0]);
		EATEST_VERIFY(Strnchr(s16, '0', 9) == &s16[0x0]);
		EATEST_VERIFY(Strnchr(s16, '0', EAArrayCount(s16)) == &s16[0x0]);
		EATEST_VERIFY(Strnchr(s16, 'f', 0) == NULL);
		EATEST_VERIFY(Strnchr(s16, 'f', 1) == NULL);
		EATEST_VERIFY(Strnchr(s16, 'f', 5) == NULL);
		EATEST_VERIFY(Strnchr(s16, 'f', EA::StdC::Strlen(s16)-1) == NULL);
		EATEST_VERIFY(Strnchr(s16, 'f', EA::StdC::Strlen(s16)) == &s16[0xf]);
		EATEST_VERIFY(Strnchr(s16, 'f', EA::StdC::Strlen(s16)+1) == &s16[0xf]); //This is the null terminator
		EATEST_VERIFY(Strnchr(s16, '\0', 0) == NULL);
		EATEST_VERIFY(Strnchr(s16, '\0', 1) == NULL);
		EATEST_VERIFY(Strnchr(s16, '\0', 5) == NULL);
		EATEST_VERIFY(Strnchr(s16, '\0', EA::StdC::Strlen(s16)-1) == NULL);
		EATEST_VERIFY(Strnchr(s16, '\0', EA::StdC::Strlen(s16)) == NULL);
		EATEST_VERIFY(Strnchr(s16, '\0', EA::StdC::Strlen(s16)+1) == &s16[0x10]); //This is the null terminator
	}
	{
		char32_t s32[24]; Strlcpy(s32, EA_CHAR32("0123456789abcdef"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strnchr(s32, 'z', EAArrayCount(s32))   == NULL);
		EATEST_VERIFY(Strnchr(s32, 'a', EAArrayCount(s32))   == &s32[0xa]);
		EATEST_VERIFY(Strnchr(s32, '5', 0) == NULL);
		EATEST_VERIFY(Strnchr(s32, '5', 1) == NULL);
		EATEST_VERIFY(Strnchr(s32, '5', 2) == NULL);
		EATEST_VERIFY(Strnchr(s32, '5', 5) == NULL);
		EATEST_VERIFY(Strnchr(s32, '5', 6) == &s32[0x5]);
		EATEST_VERIFY(Strnchr(s32, '5', 7) == &s32[0x5]);
		EATEST_VERIFY(Strnchr(s32, '5', 9) == &s32[0x5]);
		EATEST_VERIFY(Strnchr(s32, '5', EAArrayCount(s32)) == &s32[0x5]);
		EATEST_VERIFY(Strnchr(s32, '0', 0) == NULL);
		EATEST_VERIFY(Strnchr(s32, '0', 1) == &s32[0x0]);
		EATEST_VERIFY(Strnchr(s32, '0', 2) == &s32[0x0]);
		EATEST_VERIFY(Strnchr(s32, '0', 9) == &s32[0x0]);
		EATEST_VERIFY(Strnchr(s32, '0', EAArrayCount(s32)) == &s32[0x0]);
		EATEST_VERIFY(Strnchr(s32, 'f', 0) == NULL);
		EATEST_VERIFY(Strnchr(s32, 'f', 1) == NULL);
		EATEST_VERIFY(Strnchr(s32, 'f', 5) == NULL);
		EATEST_VERIFY(Strnchr(s32, 'f', EA::StdC::Strlen(s32)-1) == NULL);
		EATEST_VERIFY(Strnchr(s32, 'f', EA::StdC::Strlen(s32)) == &s32[0xf]);
		EATEST_VERIFY(Strnchr(s32, 'f', EA::StdC::Strlen(s32)+1) == &s32[0xf]); //This is the null terminator
		EATEST_VERIFY(Strnchr(s32, '\0', 0) == NULL);
		EATEST_VERIFY(Strnchr(s32, '\0', 1) == NULL);
		EATEST_VERIFY(Strnchr(s32, '\0', 5) == NULL);
		EATEST_VERIFY(Strnchr(s32, '\0', EA::StdC::Strlen(s32)-1) == NULL);
		EATEST_VERIFY(Strnchr(s32, '\0', EA::StdC::Strlen(s32)) == NULL);
		EATEST_VERIFY(Strnchr(s32, '\0', EA::StdC::Strlen(s32)+1) == &s32[0x10]); //This is the null terminator
	}

	// size_t Strspn(const char*  pString, const char*  pSubString);
	// size_t Strspn(const char16_t* pString, const char16_t* pSubString);
	// size_t Strspn(const char32_t* pString, const char32_t* pSubString);
	{
		char s8[] = "0123456789abcdef";

		EATEST_VERIFY(Strspn(s8,    "2103") == 4);
		EATEST_VERIFY(Strspn(s8+10, "badc") == 4);
		EATEST_VERIFY(Strspn(s8,    ":/1")  == 0);
	}
	{
		char16_t s16[24]; Strlcpy(s16, EA_CHAR16("0123456789abcdef"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strspn(s16,    EA_CHAR16("2103")) == 4);
		EATEST_VERIFY(Strspn(s16+10, EA_CHAR16("badc")) == 4);
		EATEST_VERIFY(Strspn(s16,    EA_CHAR16(":/1"))  == 0);
	}
	{
		char32_t s32[24]; Strlcpy(s32, EA_CHAR32("0123456789abcdef"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strspn(s32,    EA_CHAR32("2103")) == 4);
		EATEST_VERIFY(Strspn(s32+10, EA_CHAR32("badc")) == 4);
		EATEST_VERIFY(Strspn(s32,    EA_CHAR32(":/1"))  == 0);
	}


	// char*  Strstr(const char*  pString, const char*  pSubString);
	// char16_t* Strstr(const char16_t* pString, const char16_t* pSubString);
	// char32_t* Strstr(const char32_t* pString, const char32_t* pSubString);
	{
		char s8[] = "012abcdf89abcdef";

		EATEST_VERIFY(Strstr(s8, "")      == &s8[0]);
		EATEST_VERIFY(Strstr(s8, "012")   == &s8[0]);
		EATEST_VERIFY(Strstr(s8, "abcde") == &s8[10]);
		EATEST_VERIFY(Strstr(s8, ":/1")   == NULL);
		EATEST_VERIFY(Strstr(s8, "abcd")  == &s8[3]);
	}
	{
		char16_t s16[24]; Strlcpy(s16, EA_CHAR16("012abcdf89abcdef"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strstr(s16, EA_CHAR16(""))      == &s16[0]);
		EATEST_VERIFY(Strstr(s16, EA_CHAR16("012"))   == &s16[0]);
		EATEST_VERIFY(Strstr(s16, EA_CHAR16("abcde")) == &s16[10]);
		EATEST_VERIFY(Strstr(s16, EA_CHAR16(":/1"))   == NULL);
		EATEST_VERIFY(Strstr(s16, EA_CHAR16("abcd"))  == &s16[3]);
	}
	{
		char32_t s32[24]; Strlcpy(s32, EA_CHAR32("012abcdf89abcdef"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strstr(s32, EA_CHAR32(""))      == &s32[0]);
		EATEST_VERIFY(Strstr(s32, EA_CHAR32("012"))   == &s32[0]);
		EATEST_VERIFY(Strstr(s32, EA_CHAR32("abcde")) == &s32[10]);
		EATEST_VERIFY(Strstr(s32, EA_CHAR32(":/1"))   == NULL);
		EATEST_VERIFY(Strstr(s32, EA_CHAR32("abcd"))  == &s32[3]);
	}


	// char*  Stristr(const char*  pString, const char*  pSubString);
	// char16_t* Stristr(const char16_t* pString, const char16_t* pSubString);
	// char32_t* Stristr(const char32_t* pString, const char32_t* pSubString);
	{
		char s8[] = "012aBcdf89aBcDEf";

		EATEST_VERIFY(Stristr(s8, "012")   == &s8[0]);
		EATEST_VERIFY(Stristr(s8, "abcde") == &s8[10]);
		EATEST_VERIFY(Stristr(s8, ":/1")   == NULL);
		EATEST_VERIFY(Stristr(s8, "abcd")  == &s8[3]);
		EATEST_VERIFY(Stristr(s8, "")      == s8);
	}
	{
		char16_t s16[24]; Strlcpy(s16, EA_CHAR16("012aBcdf89aBcDEf"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Stristr(s16, EA_CHAR16("012"))   == &s16[0]);
		EATEST_VERIFY(Stristr(s16, EA_CHAR16("abcde")) == &s16[10]);
		EATEST_VERIFY(Stristr(s16, EA_CHAR16(":/1"))   == NULL);
		EATEST_VERIFY(Stristr(s16, EA_CHAR16("abcd"))  == &s16[3]);
		EATEST_VERIFY(Stristr(s16, EA_CHAR16(""))      == s16);
	}
	{
		char32_t s32[24]; Strlcpy(s32, EA_CHAR32("012aBcdf89aBcDEf"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Stristr(s32, EA_CHAR32("012"))   == &s32[0]);
		EATEST_VERIFY(Stristr(s32, EA_CHAR32("abcde")) == &s32[10]);
		EATEST_VERIFY(Stristr(s32, EA_CHAR32(":/1"))   == NULL);
		EATEST_VERIFY(Stristr(s32, EA_CHAR32("abcd"))  == &s32[3]);
		EATEST_VERIFY(Stristr(s32, EA_CHAR32(""))      == s32);
	}


	// char*  Strrstr(const char*  pString, const char*  pSubString);
	// char16_t* Strrstr(const char16_t* pString, const char16_t* pSubString);
	// char32_t* Strrstr(const char32_t* pString, const char32_t* pSubString);
	{
		// To do: Make a real test. For now we merely verify that we can call the function.
		Strrstr("Hello", "world");
		Strrstr(EA_CHAR16("Hello"), EA_CHAR16("world"));
		Strrstr(EA_CHAR32("Hello"), EA_CHAR32("world"));
	}


	// char*  Strirstr(const char*  pString, const char*  pSubString);
	// char16_t* Strirstr(const char16_t* pString, const char16_t* pSubString);
	// char32_t* Strirstr(const char32_t* pString, const char32_t* pSubString);
	{
		// To do: Make a real test. For now we merely verify that we can call the function.
		Strirstr("Hello", "world");
		Strirstr(EA_CHAR16("Hello"), EA_CHAR16("world"));
		Strirstr(EA_CHAR32("Hello"), EA_CHAR32("world"));
	}


	// char*  Strtok(char*  pString, const char*  pDelimiters, char**  pContext);
	// char16_t* Strtok(char16_t* pString, const char16_t* pDelimiters, char16_t** pContext);
	// char32_t* Strtok(char32_t* pString, const char32_t* pDelimiters, char32_t** pContext);
	{
		char  s8[] = ",.:1:2.3,4";
		char* s8ctx = s8;
		EATEST_VERIFY(Strtok(s8, ",.:", &s8ctx) == &s8[3]);

		char   p[] = "-abc-=-def";
		char*  p1;
		char*  r;

		r = Strtok(p,    "-",  &p1);    // r = "abc", p1 = "=-def", p = "abc\0=-def"
		EATEST_VERIFY(r != NULL);

		r = Strtok(NULL, "-=", &p1);    // r = "def", p1 = NULL,    p = "abc\0=-def"
		EATEST_VERIFY(r != NULL);

		r = Strtok(NULL, "=",  &p1);    // r = NULL,  p1 = NULL,    p = "abc\0=-def"
		EATEST_VERIFY(r == NULL);
	}
	{
		char16_t  s16[16]; Strlcpy(s16, EA_CHAR16(",.:1:2.3,4"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t* s16ctx = s16;
		EATEST_VERIFY(Strtok(s16, EA_CHAR16(",.:"), &s16ctx) == &s16[3]);
	}
	{
		char32_t  s32[32]; Strlcpy(s32, EA_CHAR32(",.:1:2.3,4"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t* s32ctx = s32;
		EATEST_VERIFY(Strtok(s32, EA_CHAR32(",.:"), &s32ctx) == &s32[3]);
	}

	{
		// Test bug report by user.
		char  pStr[]   = { 'a', '=', 'b', ';',  0 };
		char* pContext = NULL;
		char* pToken;

		   while((pToken = Strtok(pContext ? NULL : pStr, ";", &pContext)) != NULL)
			{ EATEST_VERIFY(pToken != NULL); }  // This was looping infinitely.
	}


	// const char*  Strtok2(const char*  pString, const char*  pDelimiters, size_t* pResultLength, bool bFirst);
	// const char16_t* Strtok2(const char16_t* pString, const char16_t* pDelimiters, size_t* pResultLength, bool bFirst);
	// const char32_t* Strtok2(const char32_t* pString, const char32_t* pDelimiters, size_t* pResultLength, bool bFirst);
	{
		const char* teststr  = "  Hello /// This/is++a test";
		const char* teststr2 = "  ///  ";
		const char* teststr3 = "  /// Hello ";
		const char* delim    = "/ %";
		const char* token1   = "Hello";
		const char* token2   = "This";
		const char* token3   = "is++a";
		const char* token4   = "test";
		const char* pCurrent;
		size_t         n;

		// Test the first string
		pCurrent = Strtok2(teststr, delim, &n, true);
		EATEST_VERIFY(Strncmp(token1, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token2, pCurrent, 4) == 0);
		EATEST_VERIFY(n == 4);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token3, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token4, pCurrent, 4) == 0);
		EATEST_VERIFY(n == 4);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(pCurrent == NULL);

		// Test the second string
		pCurrent = Strtok2(teststr2, delim, &n, true);
		EATEST_VERIFY(pCurrent == NULL);

		// Test the third string
		pCurrent = Strtok2(teststr3, delim, &n, true);
		EATEST_VERIFY(Strncmp(token1, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(pCurrent == NULL);
	}
	{
		const char16_t* teststr  = EA_CHAR16("  Hello /// This/is++a test");
		const char16_t* teststr2 = EA_CHAR16("  ///  ");
		const char16_t* teststr3 = EA_CHAR16("  /// Hello ");
		const char16_t* delim    = EA_CHAR16("/ %");
		const char16_t* token1   = EA_CHAR16("Hello");
		const char16_t* token2   = EA_CHAR16("This");
		const char16_t* token3   = EA_CHAR16("is++a");
		const char16_t* token4   = EA_CHAR16("test");
		const char16_t* pCurrent;
		size_t          n;

		// Test the first string
		pCurrent = Strtok2(teststr, delim, &n, true);
		EATEST_VERIFY(Strncmp(token1, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token2, pCurrent, 4) == 0);
		EATEST_VERIFY(n == 4);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token3, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token4, pCurrent, 4) == 0);
		EATEST_VERIFY(n == 4);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(pCurrent == NULL);

		// Test the second string
		pCurrent = Strtok2(teststr2, delim, &n, true);
		EATEST_VERIFY(pCurrent == NULL);

		// Test the third string
		pCurrent = Strtok2(teststr3, delim, &n, true);
		EATEST_VERIFY(Strncmp(token1, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(pCurrent == NULL);
	}
	{
		const char32_t* teststr  = EA_CHAR32("  Hello /// This/is++a test");
		const char32_t* teststr2 = EA_CHAR32("  ///  ");
		const char32_t* teststr3 = EA_CHAR32("  /// Hello ");
		const char32_t* delim    = EA_CHAR32("/ %");
		const char32_t* token1   = EA_CHAR32("Hello");
		const char32_t* token2   = EA_CHAR32("This");
		const char32_t* token3   = EA_CHAR32("is++a");
		const char32_t* token4   = EA_CHAR32("test");
		const char32_t* pCurrent;
		size_t          n;

		// Test the first string
		pCurrent = Strtok2(teststr, delim, &n, true);
		EATEST_VERIFY(Strncmp(token1, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token2, pCurrent, 4) == 0);
		EATEST_VERIFY(n == 4);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token3, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(Strncmp(token4, pCurrent, 4) == 0);
		EATEST_VERIFY(n == 4);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(pCurrent == NULL);

		// Test the second string
		pCurrent = Strtok2(teststr2, delim, &n, true);
		EATEST_VERIFY(pCurrent == NULL);

		// Test the third string
		pCurrent = Strtok2(teststr3, delim, &n, true);
		EATEST_VERIFY(Strncmp(token1, pCurrent, 5) == 0);
		EATEST_VERIFY(n == 5);

		pCurrent = Strtok2(pCurrent, delim, &n, false);
		EATEST_VERIFY(pCurrent == NULL);
	}


	// char*  Strset(char*  pString, int      c);
	// char16_t* Strset(char16_t* pString, char16_t c);
	// char32_t* Strset(char32_t* pString, char32_t c);
	{
		char s8[] = ",.:1:2.3,4";

		EATEST_VERIFY(Strset(s8, '^') == s8);
		EATEST_VERIFY(Memcmp(s8, "^^^^^^^^^^", Strlen(s8)) == 0);
	}
	{
		char16_t s16[16]; Strlcpy(s16, EA_CHAR16(",.:1:2.3,4"), EAArrayCount(s16)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strset(s16, '^') == s16);
		EATEST_VERIFY(Memcmp(s16, EA_CHAR16("^^^^^^^^^^"), Strlen(s16)) == 0);
	}
	{
		char32_t s32[32]; Strlcpy(s32, EA_CHAR32(",.:1:2.3,4"), EAArrayCount(s32)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strset(s32, '^') == s32);
		EATEST_VERIFY(Memcmp(s32, EA_CHAR32("^^^^^^^^^^"), Strlen(s32)) == 0);
	}


	// char*  Strnset(char*  pString, int      c, size_t n);
	// char16_t* Strnset(char16_t* pString, char16_t c, size_t n);
	// char32_t* Strnset(char32_t* pString, char32_t c, size_t n);
	{
		// To do: Make a real test. For now we merely verify that we can call the function.
		char buffer8[32] = {};
		Strnset(buffer8, 'a', 4);

		char16_t buffer16[32] = {};
		Strnset(buffer16, 'a', 4);

		char32_t buffer32[32] = {};
		Strnset(buffer32, 'a', 4);
	}


	// char*  Strrev(char*  pString);
	// char16_t* Strrev(char16_t* pString);
	// char32_t* Strrev(char32_t* pString);
	{
		char s8[32];
		char sEmpty[1] = { 0 };

		EATEST_VERIFY(Strlen(Strrev(sEmpty)) == 0);

		Strcpy(s8, "abcdefgh");
		EATEST_VERIFY(Strrev(s8) == s8);
		EATEST_VERIFY(Memcmp(s8, "hgfedcba", Strlen(s8)) == 0);

		Strcpy(s8, "abcdefg");
		EATEST_VERIFY(Strrev(s8) == s8);
		EATEST_VERIFY(Memcmp(s8, "gfedcba", Strlen(s8)) == 0);

		Strcpy(s8, "a");
		EATEST_VERIFY(Strrev(s8) == s8);
		EATEST_VERIFY(Memcmp(s8, "a", Strlen(s8)) == 0);
	}
	{
		char16_t s16[32];
		char16_t sEmpty[1] = { 0 };

		EATEST_VERIFY(Strlen(Strrev(sEmpty)) == 0);

		Strcpy(s16, EA_CHAR16("abcdefgh"));
		EATEST_VERIFY(Strrev(s16) == s16);
		EATEST_VERIFY(Memcmp(s16, EA_CHAR16("hgfedcba"), Strlen(s16)) == 0);

		Strcpy(s16, EA_CHAR16("abcdefg"));
		EATEST_VERIFY(Strrev(s16) == s16);
		EATEST_VERIFY(Memcmp(s16, EA_CHAR16("gfedcba"), Strlen(s16)) == 0);

		Strcpy(s16, EA_CHAR16("a"));
		EATEST_VERIFY(Strrev(s16) == s16);
		EATEST_VERIFY(Memcmp(s16, EA_CHAR16("a"), Strlen(s16)) == 0);
	}
	{
		char32_t s32[32];
		char32_t sEmpty[1] = { 0 };

		EATEST_VERIFY(Strlen(Strrev(sEmpty)) == 0);

		Strcpy(s32, EA_CHAR32("abcdefgh"));
		EATEST_VERIFY(Strrev(s32) == s32);
		EATEST_VERIFY(Memcmp(s32, EA_CHAR32("hgfedcba"), Strlen(s32)) == 0);

		Strcpy(s32, EA_CHAR32("abcdefg"));
		EATEST_VERIFY(Strrev(s32) == s32);
		EATEST_VERIFY(Memcmp(s32, EA_CHAR32("gfedcba"), Strlen(s32)) == 0);

		Strcpy(s32, EA_CHAR32("a"));
		EATEST_VERIFY(Strrev(s32) == s32);
		EATEST_VERIFY(Memcmp(s32, EA_CHAR32("a"), Strlen(s32)) == 0);
	}


	// int Strcmp(const char*  pString1, const char*  pString2);
	// int Strcmp(const char16_t* pString1, const char16_t* pString2);
	// int Strcmp(const char32_t* pString1, const char32_t* pString2);
	{
		char buffer1[] = "01234567a"; 
		char buffer2[] = "01234567b"; 
		char buffer3[] = "01234567c";

		EATEST_VERIFY(Strcmp(buffer1, buffer1) == 0);
		EATEST_VERIFY(Strcmp(buffer2, buffer1) >  0);
		EATEST_VERIFY(Strcmp(buffer3, buffer2) >  0);
		EATEST_VERIFY(Strcmp(buffer2, buffer3) <  0);
		EATEST_VERIFY(Strcmp(buffer1, buffer2) <  0);
	}
	{
		char16_t buffer1[12]; Strlcpy(buffer1, EA_CHAR16("01234567a"), EAArrayCount(buffer1)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t buffer2[12]; Strlcpy(buffer2, EA_CHAR16("01234567b"), EAArrayCount(buffer2)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t buffer3[12]; Strlcpy(buffer3, EA_CHAR16("01234567c"), EAArrayCount(buffer3)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strcmp(buffer1, buffer1) == 0);
		EATEST_VERIFY(Strcmp(buffer2, buffer1) >  0);
		EATEST_VERIFY(Strcmp(buffer3, buffer2) >  0);
		EATEST_VERIFY(Strcmp(buffer2, buffer3) <  0);
		EATEST_VERIFY(Strcmp(buffer1, buffer2) <  0);
	}
	{
		char32_t buffer1[12]; Strlcpy(buffer1, EA_CHAR32("01234567a"), EAArrayCount(buffer1)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t buffer2[12]; Strlcpy(buffer2, EA_CHAR32("01234567b"), EAArrayCount(buffer2)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t buffer3[12]; Strlcpy(buffer3, EA_CHAR32("01234567c"), EAArrayCount(buffer3)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strcmp(buffer1, buffer1) == 0);
		EATEST_VERIFY(Strcmp(buffer2, buffer1) >  0);
		EATEST_VERIFY(Strcmp(buffer3, buffer2) >  0);
		EATEST_VERIFY(Strcmp(buffer2, buffer3) <  0);
		EATEST_VERIFY(Strcmp(buffer1, buffer2) <  0);
	}

	{
		// Extended Strcmp char testing for our optimized version.
		// To do: Use a page-protected heap to test that Strcmp isn't reading beyond a page's boundaries.
		eastl::string     s1;
		eastl::string     s2;
		EA::UnitTest::Rand rand(1234);

		for(int i = 0; i < 20; i++)
		{
			bool         makeEqualLength = (rand.RandLimit(3) == 0); // A third of the time
			bool         makeEqualValue  = (rand.RandLimit(2) == 0); // Half of the time
			eastl_size_t s1Length        = rand.RandLimit(131072);

			s1.resize(s1Length);

			if(makeEqualLength)
				s2.resize(s1Length);
			else
				s2.resize(rand.RandLimit(131072));

			for(eastl_size_t j = 0; j < s1Length; j++)
			{
				s1[j] = (char)rand.RandRange(CHAR_MIN, CHAR_MAX + 1);

				if(s1[j] == 0) // We can't have a 0 char, as that's the C string terminator char.
					s1[j] = 'x';
			}
			s2.assign(s1, 0, eastl::min_alt(s1.size(), s2.size()));

			if(!makeEqualValue)
			{
				eastl_size_t pos = rand.RandLimit(static_cast<uint32_t>(s2.length()));

				s2[pos] = ~s2[pos]; // Change one character randomly in the string.

				if(s2[pos] == 0) // We can't have a 0 char, as that's the C string terminator char.
					s2[pos] = 'x';
			}

			const int eaValue  = Strcmp(s1.c_str(), s2.c_str()); // Our strcmp returns <0, 0, >0.
			const int stdValue = strcmp(s1.c_str(), s2.c_str()); // Some strcmp versions return -1, 0, +1, so our verify below can't just compare values.
			EATEST_VERIFY_F(((eaValue < 0) == (stdValue < 0)) && ((eaValue > 0) == (stdValue > 0)), "Strcmp failure for iteration %d.", i);
		}
	}

	{
		// Extended Strcmp char16_t testing for our optimized version.
		// To do: Use a page-protected heap to test that Strcmp isn't reading beyond a page's boundaries.
		eastl::string16    s1;
		eastl::string16    s2;
		EA::UnitTest::Rand rand(1234);

		for(int i = 0; i < 20; i++)
		{
			bool         makeEqualLength = (rand.RandLimit(3) == 0); // A third of the time
			bool         makeEqualValue  = (rand.RandLimit(2) == 0); // Half of the time
			eastl_size_t s1Length        = rand.RandLimit(131072);

			s1.resize(s1Length);

			if(makeEqualLength)
				s2.resize(s1Length);
			else
				s2.resize(rand.RandLimit(131072));

			for(eastl_size_t j = 0; j < s1Length; j++)
			{
				s1[j] = (char16_t)rand.RandRange(0, 32768); // There is no CHAR16_MIN, so we pick safe portable limits.

				if(s1[j] == 0) // We can't have a 0 char, as that's the C string terminator char.
					s1[j] = 'x';
			}
			s2.assign(s1, 0, eastl::min_alt(s1.size(), s2.size()));

			if(!makeEqualValue)
			{
				eastl_size_t pos = rand.RandLimit(static_cast<uint32_t>(s2.length()));

				s2[pos] = ~s2[pos]; // Change one character randomly in the string.

				if(s2[pos] == 0) // We can't have a 0 char, as that's the C string terminator char.
					s2[pos] = 'x';
			}

			#if defined(EA_PLATFORM_MICROSOFT) // Not all platforms provide a 16 bit wchar_t and wcscmp to go with it.
				const int eaValue  = Strcmp(s1.c_str(), s2.c_str()); // Our strcmp returns <0, 0, >0.
				const int stdValue = wcscmp(reinterpret_cast<const wchar_t*>(s1.c_str()), reinterpret_cast<const wchar_t*>(s2.c_str())); // Some strcmp versions return -1, 0, +1, so our verify below can't just compare values.
				EATEST_VERIFY_F(((eaValue < 0) == (stdValue < 0)) && ((eaValue > 0) == (stdValue > 0)), "Strcmp failure for iteration %d.", i);
			#else
				// To consider: Implement some fallback validation.
			#endif
		}
	}


	// int Strncmp(const char*  pString1, const char*  pString2, size_t n);
	// int Strncmp(const char16_t* pString1, const char16_t* pString2, size_t n);
	// int Strncmp(const char32_t* pString1, const char32_t* pString2, size_t n);
	{
		char buffer1[] = "01234567abc"; 
		char buffer2[] = "01234567bbc"; 
		char buffer3[] = "01234567cbc"; 

		EATEST_VERIFY(Strncmp(buffer1, buffer1, 12) == 0);
		EATEST_VERIFY(Strncmp(buffer2, buffer1, 12) >  0);
		EATEST_VERIFY(Strncmp(buffer3, buffer2, 12) >  0);
		EATEST_VERIFY(Strncmp(buffer2, buffer3, 12) <  0);
		EATEST_VERIFY(Strncmp(buffer1, buffer2, 12) <  0);
	}
	{
		char16_t buffer1[16]; Strlcpy(buffer1, EA_CHAR16("01234567abc"), EAArrayCount(buffer1)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t buffer2[16]; Strlcpy(buffer2, EA_CHAR16("01234567bbc"), EAArrayCount(buffer2)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t buffer3[16]; Strlcpy(buffer3, EA_CHAR16("01234567cbc"), EAArrayCount(buffer3)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
 
		EATEST_VERIFY(Strncmp(buffer1, buffer1, 12) == 0);
		EATEST_VERIFY(Strncmp(buffer2, buffer1, 12) >  0);
		EATEST_VERIFY(Strncmp(buffer3, buffer2, 12) >  0);
		EATEST_VERIFY(Strncmp(buffer2, buffer3, 12) <  0);
		EATEST_VERIFY(Strncmp(buffer1, buffer2, 12) <  0);
	}
	{
		char32_t buffer1[32]; Strlcpy(buffer1, EA_CHAR32("01234567abc"), EAArrayCount(buffer1)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t buffer2[32]; Strlcpy(buffer2, EA_CHAR32("01234567bbc"), EAArrayCount(buffer2)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t buffer3[32]; Strlcpy(buffer3, EA_CHAR32("01234567cbc"), EAArrayCount(buffer3)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
 
		EATEST_VERIFY(Strncmp(buffer1, buffer1, 12) == 0);
		EATEST_VERIFY(Strncmp(buffer2, buffer1, 12) >  0);
		EATEST_VERIFY(Strncmp(buffer3, buffer2, 12) >  0);
		EATEST_VERIFY(Strncmp(buffer2, buffer3, 12) <  0);
		EATEST_VERIFY(Strncmp(buffer1, buffer2, 12) <  0);
	}


	// int Stricmp(const char*  pString1, const char*  pString2);
	// int Stricmp(const char16_t* pString1, const char16_t* pString2);
	// int Stricmp(const char32_t* pString1, const char32_t* pString2);
	{
		char buffer1[] = "01asdf67A";
		char buffer2[] = "01aSDf67b";
		char buffer3[] = "01AsdF67C";

		EATEST_VERIFY(Stricmp(buffer1, buffer1) == 0);
		EATEST_VERIFY(Stricmp(buffer2, buffer1) >  0);
		EATEST_VERIFY(Stricmp(buffer3, buffer2) >  0);
		EATEST_VERIFY(Stricmp(buffer2, buffer3) <  0);
		EATEST_VERIFY(Stricmp(buffer1, buffer2) <  0);
	}
	{
		char16_t buffer1[16]; Strlcpy(buffer1, EA_CHAR16("01asdf67A"), EAArrayCount(buffer1)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t buffer2[16]; Strlcpy(buffer2, EA_CHAR16("01aSDf67b"), EAArrayCount(buffer2)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t buffer3[16]; Strlcpy(buffer3, EA_CHAR16("01AsdF67C"), EAArrayCount(buffer3)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Stricmp(buffer1, buffer1) == 0);
		EATEST_VERIFY(Stricmp(buffer2, buffer1) >  0);
		EATEST_VERIFY(Stricmp(buffer3, buffer2) >  0);
		EATEST_VERIFY(Stricmp(buffer2, buffer3) <  0);
		EATEST_VERIFY(Stricmp(buffer1, buffer2) <  0);
	}
	{
		char32_t buffer1[32]; Strlcpy(buffer1, EA_CHAR32("01asdf67A"), EAArrayCount(buffer1)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t buffer2[32]; Strlcpy(buffer2, EA_CHAR32("01aSDf67b"), EAArrayCount(buffer2)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t buffer3[32]; Strlcpy(buffer3, EA_CHAR32("01AsdF67C"), EAArrayCount(buffer3)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Stricmp(buffer1, buffer1) == 0);
		EATEST_VERIFY(Stricmp(buffer2, buffer1) >  0);
		EATEST_VERIFY(Stricmp(buffer3, buffer2) >  0);
		EATEST_VERIFY(Stricmp(buffer2, buffer3) <  0);
		EATEST_VERIFY(Stricmp(buffer1, buffer2) <  0);
	}


	// int Strnicmp(const char*  pString1, const char*  pString2, size_t n);
	// int Strnicmp(const char16_t* pString1, const char16_t* pString2, size_t n);
	// int Strnicmp(const char32_t* pString1, const char32_t* pString2, size_t n);
	{
		char buffer1[] = "01asdf67AAsWE";
		char buffer2[] = "01aSDf67basWe";
		char buffer3[] = "01AsdF67CaSwe";

		EATEST_VERIFY(Strnicmp(buffer1, buffer1, 13) == 0);
		EATEST_VERIFY(Strnicmp(buffer2, buffer1, 13) >  0);
		EATEST_VERIFY(Strnicmp(buffer3, buffer2, 13) >  0);
		EATEST_VERIFY(Strnicmp(buffer2, buffer3, 13) <  0);
		EATEST_VERIFY(Strnicmp(buffer1, buffer2, 13) <  0);
	}
	{
		char16_t buffer1[24]; Strlcpy(buffer1, EA_CHAR16("01asdf67AAsWE"), EAArrayCount(buffer1)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t buffer2[24]; Strlcpy(buffer2, EA_CHAR16("01aSDf67basWe"), EAArrayCount(buffer2)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t buffer3[24]; Strlcpy(buffer3, EA_CHAR16("01AsdF67CaSwe"), EAArrayCount(buffer3)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.

		EATEST_VERIFY(Strnicmp(buffer1, buffer1, 13) == 0);
		EATEST_VERIFY(Strnicmp(buffer2, buffer1, 13) >  0);
		EATEST_VERIFY(Strnicmp(buffer3, buffer2, 13) >  0);
		EATEST_VERIFY(Strnicmp(buffer2, buffer3, 13) <  0);
		EATEST_VERIFY(Strnicmp(buffer1, buffer2, 13) <  0);
	}
	{
		char32_t buffer1[24]; Strlcpy(buffer1, EA_CHAR32("01asdf67AAsWE"), EAArrayCount(buffer1)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t buffer2[24]; Strlcpy(buffer2, EA_CHAR32("01aSDf67basWe"), EAArrayCount(buffer2)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t buffer3[24]; Strlcpy(buffer3, EA_CHAR32("01AsdF67CaSwe"), EAArrayCount(buffer3)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.

		EATEST_VERIFY(Strnicmp(buffer1, buffer1, 13) == 0);
		EATEST_VERIFY(Strnicmp(buffer2, buffer1, 13) >  0);
		EATEST_VERIFY(Strnicmp(buffer3, buffer2, 13) >  0);
		EATEST_VERIFY(Strnicmp(buffer2, buffer3, 13) <  0);
		EATEST_VERIFY(Strnicmp(buffer1, buffer2, 13) <  0);
	}


	// int StrcmpAlnum(const char*  pString1, const char*  pString2);
	// int StrcmpAlnum(const char16_t* pString1, const char16_t* pString2);
	// No 32 bit version because this function is deprecated.
	{
		EATEST_VERIFY(StrcmpAlnum("",       "")         == 0);
		EATEST_VERIFY(StrcmpAlnum("abc",    "abc")      == 0);
		EATEST_VERIFY(StrcmpAlnum("a",      "b")        < 0);
		EATEST_VERIFY(StrcmpAlnum("abc",    "abcd")     < 0);
		EATEST_VERIFY(StrcmpAlnum("abcd",   "abc")      > 0);
		EATEST_VERIFY(StrcmpAlnum("a",      "")         > 0);
		EATEST_VERIFY(StrcmpAlnum("",       "1")        < 0);       // Verify numerics evaluate highest.
		EATEST_VERIFY(StrcmpAlnum("abc",    "abd")      < 0);
		EATEST_VERIFY(StrcmpAlnum("a",      "1")        < 0);
		EATEST_VERIFY(StrcmpAlnum("%",      "1")        < 0);       // Verify numerics evaluate highest.
		EATEST_VERIFY(StrcmpAlnum("103",    "12")       > 0);       // Verify that numeric compare is occuring.
		EATEST_VERIFY(StrcmpAlnum("abc12a", "abc12b")   < 0);
		EATEST_VERIFY(StrcmpAlnum("abc123", "abc12a")   > 0);
		EATEST_VERIFY(StrcmpAlnum("abc-2",  "abc-1")    > 0);       // Verify that '-' is not treated as a minus sign.
		EATEST_VERIFY(StrcmpAlnum("abc1.1", "abc1.02")  < 0);       // Verify that '.' is not treated as a decimal point.
		EATEST_VERIFY(StrcmpAlnum("44",     "044")      == 0);      // Verify that digit spans are treated as base 10 numbers.
	}
	{
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16(""),       EA_CHAR16(""))         == 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("abc"),    EA_CHAR16("abc"))      == 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("a"),      EA_CHAR16("b"))        < 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("abc"),    EA_CHAR16("abcd"))     < 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("abcd"),   EA_CHAR16("abc"))      > 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("a"),      EA_CHAR16(""))         > 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16(""),       EA_CHAR16("1"))        < 0);       // Verify numerics evaluate highest.
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("abc"),    EA_CHAR16("abd"))      < 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("a"),      EA_CHAR16("1"))        < 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("%"),      EA_CHAR16("1"))        < 0);       // Verify numerics evaluate highest.
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("103"),    EA_CHAR16("12"))       > 0);       // Verify that numeric compare is occuring.
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("abc12a"), EA_CHAR16("abc12b"))   < 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("abc123"), EA_CHAR16("abc12a"))   > 0);
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("abc-2"),  EA_CHAR16("abc-1"))    > 0);       // Verify that '-' is not treated as a minus sign.
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("abc1.1"), EA_CHAR16("abc1.02"))  < 0);       // Verify that '.' is not treated as a decimal point.
		EATEST_VERIFY(StrcmpAlnum(EA_CHAR16("44"),     EA_CHAR16("044"))      == 0);      // Verify that digit spans are treated as base 10 numbers.
	}


	// int StricmpAlnum(const char*  pString1, const char*  pString2);
	// int StricmpAlnum(const char16_t* pString1, const char16_t* pString2);
	// No 32 bit version because this function is deprecated.
	{
		EATEST_VERIFY(StricmpAlnum("",       "")         == 0);
		EATEST_VERIFY(StricmpAlnum("abc",    "abc")      == 0);
		EATEST_VERIFY(StricmpAlnum("a",      "b")        < 0);
		EATEST_VERIFY(StricmpAlnum("abc",    "abcd")     < 0);
		EATEST_VERIFY(StricmpAlnum("abcd",   "abc")      > 0);
		EATEST_VERIFY(StricmpAlnum("a",      "")         > 0);
		EATEST_VERIFY(StricmpAlnum("",       "1")        < 0);       // Verify numerics evaluate highest.
		EATEST_VERIFY(StricmpAlnum("abc",    "abd")      < 0);
		EATEST_VERIFY(StricmpAlnum("a",      "1")        < 0);
		EATEST_VERIFY(StricmpAlnum("%",      "1")        < 0);       // Verify numerics evaluate highest.
		EATEST_VERIFY(StricmpAlnum("103",    "12")       > 0);       // Verify that numeric compare is occuring.
		EATEST_VERIFY(StricmpAlnum("abc12a", "abc12b")   < 0);
		EATEST_VERIFY(StricmpAlnum("abc123", "abc12a")   > 0);
		EATEST_VERIFY(StricmpAlnum("abc-2",  "abc-1")    > 0);       // Verify that '-' is not treated as a minus sign.
		EATEST_VERIFY(StricmpAlnum("abc1.1", "abc1.02")  < 0);       // Verify that '.' is not treated as a decimal point.
		EATEST_VERIFY(StricmpAlnum("44",     "044")      == 0);      // Verify that digit spans are treated as base 10 numbers.
	}
	{
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16(""),       EA_CHAR16(""))         == 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("aBc"),    EA_CHAR16("abc"))      == 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("a"),      EA_CHAR16("b"))        < 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("ABc"),    EA_CHAR16("aBCd"))     < 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("abcd"),   EA_CHAR16("abc"))      > 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("a"),      EA_CHAR16(""))         > 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16(""),       EA_CHAR16("1"))        < 0);       // Verify numerics evaluate highest.
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("abC"),    EA_CHAR16("aBd"))      < 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("a"),      EA_CHAR16("1"))        < 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("%"),      EA_CHAR16("1"))        < 0);       // Verify numerics evaluate highest.
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("103"),    EA_CHAR16("12"))       > 0);       // Verify that numeric compare is occuring.
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("abc12A"), EA_CHAR16("aBC12b"))   < 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("abc123"), EA_CHAR16("abc12a"))   > 0);
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("ABc-2"),  EA_CHAR16("abc-1"))    > 0);       // Verify that '-' is not treated as a minus sign.
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("abC1.1"), EA_CHAR16("abc1.02"))  < 0);       // Verify that '.' is not treated as a decimal point.
		EATEST_VERIFY(StricmpAlnum(EA_CHAR16("44"),     EA_CHAR16("044"))      == 0);      // Verify that digit spans are treated as base 10 numbers.
	}


	// int Strcoll(const char*  pString1, const char*  pString2);
	// int Strcoll(const char16_t* pString1, const char16_t* pString2);
	// int Strcoll(const char32_t* pString1, const char32_t* pString2);
	{
		// To do: Make a real test. For now we merely verify that we can call the function.
		Strcoll("Hello",  "world");
		Strcoll(EA_CHAR16("Hello"), EA_CHAR16("world"));
		Strcoll(EA_CHAR32("Hello"), EA_CHAR32("world"));
	}


	// int Strncoll(const char*  pString1, const char*  pString2, size_t n);
	// int Strncoll(const char16_t* pString1, const char16_t* pString2, size_t n);
	// int Strncoll(const char32_t* pString1, const char32_t* pString2, size_t n);
	{
		// To do: Make a real test. For now we merely verify that we can call the function.
		Strncoll("Hello",  "world", 1);
		Strncoll(EA_CHAR16("Hello"), EA_CHAR16("world"), 1);
		Strncoll(EA_CHAR32("Hello"), EA_CHAR32("world"), 1);
	}


	// int Stricoll(const char*  pString1, const char*  pString2);
	// int Stricoll(const char16_t* pString1, const char16_t* pString2);
	// int Stricoll(const char32_t* pString1, const char32_t* pString2);
	{
		// To do: Make a real test. For now we merely verify that we can call the function.
		Stricoll("Hello",  "world");
		Stricoll(EA_CHAR16("Hello"), EA_CHAR16("world"));
		Stricoll(EA_CHAR32("Hello"), EA_CHAR32("world"));
	}


	// int Strnicoll(const char*  pString1, const char*  pString1, size_t n);
	// int Strnicoll(const char16_t* pString1, const char16_t* pString1, size_t n);
	// int Strnicoll(const char32_t* pString1, const char32_t* pString1, size_t n);
	{
		// To do: Make a real test. For now we merely verify that we can call the function.
		Strnicoll("Hello", "world", 1);
		Strnicoll(EA_CHAR16("Hello"), EA_CHAR16("world"), 1);
		Strnicoll(EA_CHAR32("Hello"), EA_CHAR32("world"), 1);
	}

	return nErrorCount;
}
EA_RESTORE_VC_WARNING()


static int TestEcvt()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// These tests all require a buffer of at least 350 in size to be used

	// char*  EcvtBuf(double dValue, int nDigitCount, int* decimalPos, int* sign, char* buffer);
	// char16_t* EcvtBuf(double dValue, int nDigitCount, int* decimalPos, int* sign, char16_t* buffer);
	// char32_t* EcvtBuf(double dValue, int nDigitCount, int* decimalPos, int* sign, char32_t* buffer);
	{
		char buffer[kEcvtBufMaxSize];
		int decimalPos, sign;

		EcvtBuf(1.0, 10, &decimalPos, &sign, buffer);
		EATEST_VERIFY(buffer[0] == '1');
	}
	{
		char16_t buffer[kEcvtBufMaxSize];
		int decimalPos, sign;

		EcvtBuf(1.0, 10, &decimalPos, &sign, buffer);
		EATEST_VERIFY(buffer[0] == '1');
	}
	{
		char32_t buffer[kEcvtBufMaxSize];
		int decimalPos, sign;

		EcvtBuf(1.0, 10, &decimalPos, &sign, buffer);
		EATEST_VERIFY(buffer[0] == '1');
	}


	// char*  FcvtBuf(double dValue, int nDigitCountAfterDecimal, int* decimalPos, int* sign, char*  buffer);
	// char16_t* FcvtBuf(double dValue, int nDigitCountAfterDecimal, int* decimalPos, int* sign, char16_t* buffer);
	// char32_t* FcvtBuf(double dValue, int nDigitCountAfterDecimal, int* decimalPos, int* sign, char32_t* buffer);
	{
		char buffer[kFcvtBufMaxSize];
		int decimalPos, sign;

		FcvtBuf(1.0, 10, &decimalPos, &sign, buffer);
		EATEST_VERIFY(buffer[0] == '1');
	}
	{
		char16_t buffer[kFcvtBufMaxSize];
		int decimalPos, sign;

		FcvtBuf(1.0, 10, &decimalPos, &sign, buffer);
		EATEST_VERIFY(buffer[0] == '1');
	}
	{
		char32_t buffer[kFcvtBufMaxSize];
		int decimalPos, sign;

		FcvtBuf(1.0, 10, &decimalPos, &sign, buffer);
		EATEST_VERIFY(buffer[0] == '1');
	}

	return nErrorCount;
}


static int TestItoa()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// char*  I32toa(int32_t nValue, char*  pResult, int nBase);
	// char16_t* I32toa(int32_t nValue, char16_t* pResult, int nBase);
	// char32_t* I32toa(int32_t nValue, char32_t* pResult, int nBase);
	{
		char sn8[32];

		EATEST_VERIFY(I32toa(INT32_MIN, sn8, 10) != NULL); // Can't express INT32_MIN as a numeric constant.
		EATEST_VERIFY_F(Strcmp(sn8, "-2147483648") == 0, "I32toa(INT32_MIN, sn8, 10) failed; produced %s instead of -2147483648", sn8);
		EATEST_VERIFY(I32toa(2147483647, sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "2147483647") == 0);
		EATEST_VERIFY(I32toa(-2000000000, sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "-2000000000") == 0);
		EATEST_VERIFY(I32toa(2000000000, sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "2000000000") == 0);
		EATEST_VERIFY(I32toa(0x77359400, sn8, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "77359400") == 0);
		EATEST_VERIFY(I32toa(0xeeee, sn8, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "1110111011101110") == 0);

		// Test random values, in order to exercise our optimized base 10 implementation.
		RandomFast     random;
		LimitStopwatch timer(Stopwatch::kUnitsSeconds, 2, true);

		while(!timer.IsTimeUp())
		{
			int32_t value = (int32_t)random.RandomUint32Uniform();
			I32toa(value, sn8, 10);
			int32_t value2 = AtoI32(sn8);
			EATEST_VERIFY(value == value2);
		}

		// Test speed of optimized base 10 implementation.
		Stopwatch stopwatch(Stopwatch::kUnitsCPUCycles, true);
		for(int i = 0; i < 10000; i++)
			I32toa((int32_t)random.RandomUint32Uniform(), sn8, 10);
		EA::UnitTest::ReportVerbosity(2, "I32toa time: %I64u cycles.\n", stopwatch.GetElapsedTime());
	}
	{
		char16_t sn16[32];

		EATEST_VERIFY(I32toa(INT32_MIN, sn16, 10) != NULL); // Can't express INT32_MIN as a numeric constant.
		EATEST_VERIFY_F(Strcmp(sn16, EA_CHAR16("-2147483648")) == 0, "I32toa(INT32_MIN, sn16, 10) failed; produced %I16s instead of -2147483648", sn16);
		EATEST_VERIFY(I32toa(2147483647, sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("2147483647")) == 0);
		EATEST_VERIFY(I32toa(-2000000000, sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("-2000000000")) == 0);
		EATEST_VERIFY(I32toa(2000000000, sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("2000000000")) == 0);
		EATEST_VERIFY(I32toa(0x77359400, sn16, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("77359400")) == 0);
		EATEST_VERIFY(I32toa(0xeeee, sn16, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("1110111011101110")) == 0);
	}
	{
		char32_t sn32[32];

		EATEST_VERIFY(I32toa(INT32_MIN, sn32, 10) != NULL); // Can't express INT32_MIN as a numeric constant.
		EATEST_VERIFY_F(Strcmp(sn32, EA_CHAR32("-2147483648")) == 0, "I32toa(INT32_MIN, sn32, 10) failed; produced %I16s instead of -2147483648", sn32);
		EATEST_VERIFY(I32toa(2147483647, sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("2147483647")) == 0);
		EATEST_VERIFY(I32toa(-2000000000, sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("-2000000000")) == 0);
		EATEST_VERIFY(I32toa(2000000000, sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("2000000000")) == 0);
		EATEST_VERIFY(I32toa(0x77359400, sn32, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("77359400")) == 0);
		EATEST_VERIFY(I32toa(0xeeee, sn32, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("1110111011101110")) == 0);
	}


	// char*  U32toa(uint32_t nValue, char*  pResult, int nBase);
	// char16_t* U32toa(uint32_t nValue, char16_t* pResult, int nBase);
	// char32_t* U32toa(uint32_t nValue, char32_t* pResult, int nBase);
	{
		char sn8[32];

		EATEST_VERIFY(U32toa(0, sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "0") == 0);
		EATEST_VERIFY(U32toa(4294967295U, sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "4294967295") == 0);
		EATEST_VERIFY(U32toa(UINT32_C(4000000000), sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "4000000000") == 0);
		EATEST_VERIFY(U32toa(2000000000, sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "2000000000") == 0);
		EATEST_VERIFY(U32toa(0x77359400, sn8, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "77359400") == 0);
		EATEST_VERIFY(U32toa(0xeeee, sn8, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "1110111011101110") == 0);

		// Test random values, in order to exercise our optimized base 10 implementation.
		RandomFast     random;
		LimitStopwatch timer(Stopwatch::kUnitsSeconds, 2, true);

		while(!timer.IsTimeUp())
		{
			uint32_t value = random.RandomUint32Uniform();
			U32toa(value, sn8, 10);
			uint32_t value2 = AtoU32(sn8);
			EATEST_VERIFY(value == value2);
		}
	}
	{
		char16_t sn16[32];

		EATEST_VERIFY(U32toa(0, sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("0")) == 0);
		EATEST_VERIFY(U32toa(4294967295U, sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("4294967295")) == 0);
		EATEST_VERIFY(U32toa(UINT32_C(4000000000), sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("4000000000")) == 0);
		EATEST_VERIFY(U32toa(2000000000, sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("2000000000")) == 0);
		EATEST_VERIFY(U32toa(0x77359400, sn16, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("77359400")) == 0);
		EATEST_VERIFY(U32toa(0xeeee, sn16, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("1110111011101110")) == 0);
	}
	{
		char32_t sn32[32];

		EATEST_VERIFY(U32toa(0, sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("0")) == 0);
		EATEST_VERIFY(U32toa(4294967295U, sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("4294967295")) == 0);
		EATEST_VERIFY(U32toa(UINT32_C(4000000000), sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("4000000000")) == 0);
		EATEST_VERIFY(U32toa(2000000000, sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("2000000000")) == 0);
		EATEST_VERIFY(U32toa(0x77359400, sn32, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("77359400")) == 0);
		EATEST_VERIFY(U32toa(0xeeee, sn32, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("1110111011101110")) == 0);
	}


	// char*  I64toa(int64_t nValue, char*  pBuffer, int nBase);
	// char16_t* I64toa(int64_t nValue, char16_t* pBuffer, int nBase);
	// char32_t* I64toa(int64_t nValue, char32_t* pBuffer, int nBase);
	{
		char sn8[128];

		EATEST_VERIFY(I64toa(INT64_MIN, sn8, 10) != NULL); // Can't express INT64_MIN as a numeric constant.
		EATEST_VERIFY(Strcmp(sn8, "-9223372036854775808") == 0);
		EATEST_VERIFY(I64toa(INT64_C(9223372036854775807), sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "9223372036854775807") == 0);
		EATEST_VERIFY(I64toa(INT64_C(-20000000000000), sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "-20000000000000") == 0);
		EATEST_VERIFY(I64toa(INT64_C(20000000000000), sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "20000000000000") == 0);
		EATEST_VERIFY(I64toa(INT64_C(0x7735940012345), sn8, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "7735940012345") == 0);
		EATEST_VERIFY(I64toa(0xeeee, sn8, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "1110111011101110") == 0);

		// Test random values, in order to exercise our optimized base 10 implementation.
		RandomFast     random;
		LimitStopwatch timer(Stopwatch::kUnitsSeconds, 2, true);

		while(!timer.IsTimeUp())
		{
			int64_t value = (int64_t)(((uint64_t)random.RandomUint32Uniform() << 32) | random.RandomUint32Uniform());
			I64toa(value, sn8, 10);
			int64_t value2 = AtoI64(sn8);
			EATEST_VERIFY(value == value2);
		}
	}
	{
		char16_t sn16[128];

		EATEST_VERIFY(I64toa(INT64_MIN, sn16, 10) != NULL); // Can't express INT64_MIN as a numeric constant.
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("-9223372036854775808")) == 0);
		EATEST_VERIFY(I64toa(INT64_C(9223372036854775807), sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("9223372036854775807")) == 0);
		EATEST_VERIFY(I64toa(INT64_C(-20000000000000), sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("-20000000000000")) == 0);
		EATEST_VERIFY(I64toa(INT64_C(20000000000000), sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("20000000000000")) == 0);
		EATEST_VERIFY(I64toa(INT64_C(0x7735940012345), sn16, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("7735940012345")) == 0);
		EATEST_VERIFY(I64toa(0xeeee, sn16, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("1110111011101110")) == 0);
	}
	{
		char32_t sn32[128];

		EATEST_VERIFY(I64toa(INT64_MIN, sn32, 10) != NULL); // Can't express INT64_MIN as a numeric constant.
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("-9223372036854775808")) == 0);
		EATEST_VERIFY(I64toa(INT64_C(9223372036854775807), sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("9223372036854775807")) == 0);
		EATEST_VERIFY(I64toa(INT64_C(-20000000000000), sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("-20000000000000")) == 0);
		EATEST_VERIFY(I64toa(INT64_C(20000000000000), sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("20000000000000")) == 0);
		EATEST_VERIFY(I64toa(INT64_C(0x7735940012345), sn32, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("7735940012345")) == 0);
		EATEST_VERIFY(I64toa(0xeeee, sn32, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("1110111011101110")) == 0);
	}


	// char*  U64toa(uint64_t nValue, char*  pBuffer, int nBase);
	// char16_t* U64toa(uint64_t nValue, char16_t* pBuffer, int nBase);
	// char32_t* U64toa(uint64_t nValue, char32_t* pBuffer, int nBase);
	{
		char sn8[128];

		EATEST_VERIFY(U64toa(0, sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "0") == 0);
		EATEST_VERIFY(U64toa(UINT64_C(18446744073709551615), sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "18446744073709551615") == 0);
		EATEST_VERIFY(U64toa(UINT64_C(40000000000000), sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "40000000000000") == 0);
		EATEST_VERIFY(U64toa(UINT64_C(20000000000000), sn8, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "20000000000000") == 0);
		EATEST_VERIFY(U64toa(UINT64_C(0x7735940012345), sn8, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "7735940012345") == 0);
		EATEST_VERIFY(U64toa(0xeeee, sn8, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn8, "1110111011101110") == 0);

		// Test random values, in order to exercise our optimized base 10 implementation.
		RandomFast     random;
		LimitStopwatch timer(Stopwatch::kUnitsSeconds, 2, true);

		while(!timer.IsTimeUp())
		{
			uint64_t value = (((uint64_t)random.RandomUint32Uniform() << 32) | random.RandomUint32Uniform());
			U64toa(value, sn8, 10);
			uint64_t value2 = AtoU64(sn8);
			EATEST_VERIFY(value == value2);
		}
	}
	{
		char16_t sn16[1216];

		EATEST_VERIFY(U64toa(0, sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("0")) == 0);
		EATEST_VERIFY(U64toa(UINT64_C(18446744073709551615), sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("18446744073709551615")) == 0);
		EATEST_VERIFY(U64toa(UINT64_C(40000000000000), sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("40000000000000")) == 0);
		EATEST_VERIFY(U64toa(UINT64_C(20000000000000), sn16, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("20000000000000")) == 0);
		EATEST_VERIFY(U64toa(UINT64_C(0x7735940012345), sn16, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("7735940012345")) == 0);
		EATEST_VERIFY(U64toa(0xeeee, sn16, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn16, EA_CHAR16("1110111011101110")) == 0);
	}
	{
		char32_t sn32[1232];

		EATEST_VERIFY(U64toa(0, sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("0")) == 0);
		EATEST_VERIFY(U64toa(UINT64_C(18446744073709551615), sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("18446744073709551615")) == 0);
		EATEST_VERIFY(U64toa(UINT64_C(40000000000000), sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("40000000000000")) == 0);
		EATEST_VERIFY(U64toa(UINT64_C(20000000000000), sn32, 10) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("20000000000000")) == 0);
		EATEST_VERIFY(U64toa(UINT64_C(0x7735940012345), sn32, 16) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("7735940012345")) == 0);
		EATEST_VERIFY(U64toa(0xeeee, sn32, 2) != NULL);
		EATEST_VERIFY(Strcmp(sn32, EA_CHAR32("1110111011101110")) == 0);
	}

	return nErrorCount;
}


static int TestStrtod()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// double StrtodEnglish(const char*  pString, char**  ppStringEnd);
	// double StrtodEnglish(const char16_t* pString, char16_t** ppStringEnd);
	// double StrtodEnglish(const char32_t* pString, char32_t** ppStringEnd);
	{
		const char* p;
		char* pEnd = NULL;

		p = "";
		EATEST_VERIFY(0 == StrtodEnglish(p, &pEnd));

		// Need a test here which results in HUGE_VAL
		//p = "";
		//EATEST_VERIFY(0 == StrtodEnglish(p, &pEnd));

		p = "-111.111";
		EATEST_VERIFY(TestString_DoubleEqual(-111.111, StrtodEnglish(p, &pEnd)));

		p = "111e111";
		EATEST_VERIFY(TestString_DoubleEqual(111e111, StrtodEnglish(p, &pEnd)));

		p = "-111e-111";
		EATEST_VERIFY(TestString_DoubleEqual(-111e-111, StrtodEnglish(p, &pEnd)));

		p = "137";
		EATEST_VERIFY(TestString_DoubleEqual(137, StrtodEnglish(p, &pEnd)));

		p = "999999";
		EATEST_VERIFY(TestString_DoubleEqual(999999, StrtodEnglish(p, &pEnd)));

		p = "123.456";
		EATEST_VERIFY(TestString_DoubleEqual(123.456, StrtodEnglish(p, &pEnd)));

		p = "123232111.000123";
		EATEST_VERIFY(TestString_DoubleEqual(123232111.000123, StrtodEnglish(p, &pEnd)));

		p = "-888111.000123";
		EATEST_VERIFY(TestString_DoubleEqual(-888111.000123, StrtodEnglish(p, &pEnd)));

		p = "-123.456";
		EATEST_VERIFY(TestString_DoubleEqual(-123.456, StrtodEnglish(p, &pEnd)));

		p = "+999999";
		EATEST_VERIFY(TestString_DoubleEqual(+999999, StrtodEnglish(p, &pEnd)));

		p = "+999999e2";
		EATEST_VERIFY(TestString_DoubleEqual(+999999e2, StrtodEnglish(p, &pEnd)));

		p = "-234.567E-3";
		EATEST_VERIFY(TestString_DoubleEqual(-234.567E-3, StrtodEnglish(p, &pEnd)));

		p = "321654.987e11";
		EATEST_VERIFY(TestString_DoubleEqual(321654.987e11, StrtodEnglish(p, &pEnd)));

		p = "-321654.987E0";
		EATEST_VERIFY(TestString_DoubleEqual(-321654.987E0, StrtodEnglish(p, &pEnd)));

		p = "+321654.987e-0";
		EATEST_VERIFY(TestString_DoubleEqual(+321654.987e-0, StrtodEnglish(p, &pEnd)));
	}
	{
		const char16_t* p;
		char16_t* pEnd = NULL;

		p = EA_CHAR16("");
		EATEST_VERIFY(0 == StrtodEnglish(p, &pEnd));

		// Need a test here which results in HUGE_VAL
		//p = "";
		//EATEST_VERIFY(0 == StrtodEnglish(p, &pEnd));

		p = EA_CHAR16("-111.111");
		EATEST_VERIFY(TestString_DoubleEqual(-111.111, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("111e111");
		EATEST_VERIFY(TestString_DoubleEqual(111e111, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("-111e-111");
		EATEST_VERIFY(TestString_DoubleEqual(-111e-111, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("137");
		EATEST_VERIFY(TestString_DoubleEqual(137, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("999999");
		EATEST_VERIFY(TestString_DoubleEqual(999999, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("123.456");
		EATEST_VERIFY(TestString_DoubleEqual(123.456, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("123232111.000123");
		EATEST_VERIFY(TestString_DoubleEqual(123232111.000123, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("-888111.000123");
		EATEST_VERIFY(TestString_DoubleEqual(-888111.000123, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("-123.456");
		EATEST_VERIFY(TestString_DoubleEqual(-123.456, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("+999999");
		EATEST_VERIFY(TestString_DoubleEqual(+999999, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("+999999e2");
		EATEST_VERIFY(TestString_DoubleEqual(+999999e2, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("-234.567E-3");
		EATEST_VERIFY(TestString_DoubleEqual(-234.567E-3, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("321654.987e11");
		EATEST_VERIFY(TestString_DoubleEqual(321654.987e11, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("-321654.987E0");
		EATEST_VERIFY(TestString_DoubleEqual(-321654.987E0, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR16("+321654.987e-0");
		EATEST_VERIFY(TestString_DoubleEqual(+321654.987e-0, StrtodEnglish(p, &pEnd)));
	}
	{
		const char32_t* p;
		char32_t* pEnd = NULL;

		p = EA_CHAR32("");
		EATEST_VERIFY(0 == StrtodEnglish(p, &pEnd));

		// Need a test here which results in HUGE_VAL
		//p = "";
		//EATEST_VERIFY(0 == StrtodEnglish(p, &pEnd));

		p = EA_CHAR32("-111.111");
		EATEST_VERIFY(TestString_DoubleEqual(-111.111, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("111e111");
		EATEST_VERIFY(TestString_DoubleEqual(111e111, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("-111e-111");
		EATEST_VERIFY(TestString_DoubleEqual(-111e-111, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("137");
		EATEST_VERIFY(TestString_DoubleEqual(137, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("999999");
		EATEST_VERIFY(TestString_DoubleEqual(999999, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("123.456");
		EATEST_VERIFY(TestString_DoubleEqual(123.456, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("123232111.000123");
		EATEST_VERIFY(TestString_DoubleEqual(123232111.000123, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("-888111.000123");
		EATEST_VERIFY(TestString_DoubleEqual(-888111.000123, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("-123.456");
		EATEST_VERIFY(TestString_DoubleEqual(-123.456, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("+999999");
		EATEST_VERIFY(TestString_DoubleEqual(+999999, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("+999999e2");
		EATEST_VERIFY(TestString_DoubleEqual(+999999e2, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("-234.567E-3");
		EATEST_VERIFY(TestString_DoubleEqual(-234.567E-3, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("323254.987e11");
		EATEST_VERIFY(TestString_DoubleEqual(323254.987e11, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("-323254.987E0");
		EATEST_VERIFY(TestString_DoubleEqual(-323254.987E0, StrtodEnglish(p, &pEnd)));

		p = EA_CHAR32("+323254.987e-0");
		EATEST_VERIFY(TestString_DoubleEqual(+323254.987e-0, StrtodEnglish(p, &pEnd)));
	}


	// double Strtod(const char*  pString, char**  ppStringEnd);
	// double Strtod(const char16_t* pString, char16_t** ppStringEnd);
	// double Strtod(const char32_t* pString, char32_t** ppStringEnd);
	{
		char  sn18[] = "-111.111";
		char* pEnd = NULL;
		EATEST_VERIFY(TestString_DoubleEqual(-111.111, Strtod(sn18, &pEnd)));

		char sn28[] = "111e111";
		EATEST_VERIFY(TestString_DoubleEqual(111e111, Strtod(sn28, &pEnd)));

		char sn38[] = "-111e-111";
		EATEST_VERIFY(TestString_DoubleEqual(-111e-111, Strtod(sn38, &pEnd)));
	}
	{
		char16_t  sn18[16]; Strlcpy(sn18, EA_CHAR16("-111.111"), EAArrayCount(sn18)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t* pEnd = NULL;
		EATEST_VERIFY(TestString_DoubleEqual(-111.111, Strtod(sn18, &pEnd)));

		char16_t sn28[16]; Strlcpy(sn28, EA_CHAR16("111e111"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(TestString_DoubleEqual(111e111, Strtod(sn28, &pEnd)));

		char16_t sn38[16]; Strlcpy(sn38, EA_CHAR16("-111e-111"), EAArrayCount(sn38)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(TestString_DoubleEqual(-111e-111, Strtod(sn38, &pEnd)));
	}
	{
		char32_t  sn18[32]; Strlcpy(sn18, EA_CHAR32("-111.111"), EAArrayCount(sn18)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t* pEnd = NULL;
		EATEST_VERIFY(TestString_DoubleEqual(-111.111, Strtod(sn18, &pEnd)));

		char32_t sn28[32]; Strlcpy(sn28, EA_CHAR32("111e111"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(TestString_DoubleEqual(111e111, Strtod(sn28, &pEnd)));

		char32_t sn38[32]; Strlcpy(sn38, EA_CHAR32("-111e-111"), EAArrayCount(sn38)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(TestString_DoubleEqual(-111e-111, Strtod(sn38, &pEnd)));
	}

	return nErrorCount;
}


static int TestStrtoi()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// int64_t StrtoI64(const char*  pString, char**  ppStringEnd, int nBase);
	// int64_t StrtoI64(const char16_t* pString, char16_t** ppStringEnd, int nBase);
	// int64_t StrtoI64(const char32_t* pString, char32_t** ppStringEnd, int nBase);
	{
		int64_t result;

		char  sn18[] = "-1011101110111011101";
		char* pEnd = NULL;
		EATEST_VERIFY(INT64_C(-1011101110111011101) == StrtoI64(sn18, &pEnd, 10));

		char sn28[] = "7fffabcdffffabcd";
		EATEST_VERIFY(UINT64_C(0x7fffabcdffffabcd) == StrtoI64(sn28, &pEnd, 16));

		char sn38[] = "101110011011101110111001010000111111100001100101";
		EATEST_VERIFY(UINT64_C(0xB9BBB943F865) == StrtoI64(sn38, &pEnd, 2));

		char sn1[] = "1";
		for(int base = 2; base <= 36; base++)
		{
			result = StrtoI64(sn1, &pEnd, base);
			EATEST_VERIFY(result != 0); // Currently just test to make sure it didn't crash.
		}

		{
			// Exercize the ability to read extreme values for various bases.
			char pINT64_MIN_10_[] = "-9223372036854775809"; // One less than valid.
			char pINT64_MIN_10[]  = "-9223372036854775808";
			char pINT64_MAX_10[]  = "+9223372036854775807";
			char pINT64_MAX_10_[] = "+9223372036854775808"; // One more than valid.

			errno = 0;
			result =  StrtoI64(pINT64_MIN_10_, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MIN) && (errno == ERANGE));

			errno = 0;
			result =  StrtoI64(pINT64_MIN_10, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MIN) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_10, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_10_, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MAX) && (errno == ERANGE));


			char pINT64_MIN_16_[] = "-8000000000000001";    // One less than valid.
			char pINT64_MIN_16[]  = "-8000000000000000";
			char pINT64_MAX_16[]  = "+7fffffffffffffff";
			char pINT64_MAX_16_[] = "+8000000000000000";    // One more than valid.

			errno = 0;
			result =  StrtoI64(pINT64_MIN_16_, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MIN) && (errno == ERANGE));

			errno = 0;
			result =  StrtoI64(pINT64_MIN_16, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MIN) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_16, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_16_, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MAX) && (errno == ERANGE));
		}
	}
	{
		int64_t result;

		char16_t  sn18[32]; Strlcpy(sn18, EA_CHAR16("-1011101110111011101"), EAArrayCount(sn18)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t* pEnd = NULL;
		EATEST_VERIFY(INT64_C(-1011101110111011101) == StrtoI64(sn18, &pEnd, 10));

		char16_t sn28[24]; Strlcpy(sn28, EA_CHAR16("7fffabcdffffabcd"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(UINT64_C(0x7fffabcdffffabcd) == StrtoI64(sn28, &pEnd, 16));

		char16_t sn38[64]; Strlcpy(sn38, EA_CHAR16("101110011011101110111001010000111111100001100101"), EAArrayCount(sn38)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(UINT64_C(0xB9BBB943F865) == StrtoI64(sn38, &pEnd, 2));

		char16_t sn1[2] = { '1', 0 };
		for(int base = 2; base <= 36; base++)
		{
			int64_t conversionResult = StrtoI64(sn1, &pEnd, base);
			EATEST_VERIFY(conversionResult != 0); // Currently just test to make sure it didn't crash.
		}

		{
			// Exercize the ability to read extreme values for various bases.
			char16_t pINT64_MIN_10_[24]; Strlcpy(pINT64_MIN_10_, "-9223372036854775809", EAArrayCount(pINT64_MIN_10_)); 
			char16_t pINT64_MIN_10[24];  Strlcpy(pINT64_MIN_10 , "-9223372036854775808", EAArrayCount(pINT64_MIN_10)); 
			char16_t pINT64_MAX_10[24];  Strlcpy(pINT64_MAX_10 , "+9223372036854775807", EAArrayCount(pINT64_MAX_10)); 
			char16_t pINT64_MAX_10_[24]; Strlcpy(pINT64_MAX_10_, "+9223372036854775808", EAArrayCount(pINT64_MIN_10_)); 

			errno = 0;
			result =  StrtoI64(pINT64_MIN_10_, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MIN) && (errno == ERANGE));

			errno = 0;
			result =  StrtoI64(pINT64_MIN_10, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MIN) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_10, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_10_, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MAX) && (errno == ERANGE));


			char16_t pINT64_MIN_16_[24]; Strlcpy(pINT64_MIN_16_, "-8000000000000001", EAArrayCount(pINT64_MIN_16_)); 
			char16_t pINT64_MIN_16[24] ; Strlcpy(pINT64_MIN_16 , "-8000000000000000", EAArrayCount(pINT64_MIN_16)); 
			char16_t pINT64_MAX_16[24] ; Strlcpy(pINT64_MAX_16 , "+7fffffffffffffff", EAArrayCount(pINT64_MAX_16)); 
			char16_t pINT64_MAX_16_[24]; Strlcpy(pINT64_MAX_16_, "+8000000000000000", EAArrayCount(pINT64_MAX_16_)); 

			errno = 0;
			result =  StrtoI64(pINT64_MIN_16_, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MIN) && (errno == ERANGE));

			errno = 0;
			result =  StrtoI64(pINT64_MIN_16, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MIN) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_16, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_16_, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MAX) && (errno == ERANGE));
		}
	}
	{
		int64_t result;
 
		char32_t  sn18[32]; Strlcpy(sn18, EA_CHAR32("-1011101110111011101"), EAArrayCount(sn18)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t* pEnd = NULL;
		EATEST_VERIFY(INT64_C(-1011101110111011101) == StrtoI64(sn18, &pEnd, 10));

		char32_t sn28[24]; Strlcpy(sn28, EA_CHAR32("7fffabcdffffabcd"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(UINT64_C(0x7fffabcdffffabcd) == StrtoI64(sn28, &pEnd, 16));

		char32_t sn38[64]; Strlcpy(sn38, EA_CHAR32("101110011011101110111001010000111111100001100101"), EAArrayCount(sn38)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(UINT64_C(0xB9BBB943F865) == StrtoI64(sn38, &pEnd, 2));

		char32_t sn1[2] = { '1', 0 };
		for(int base = 2; base <= 36; base++)
		{
			int64_t conversionResult = StrtoI64(sn1, &pEnd, base);
			EATEST_VERIFY(conversionResult != 0); // Currently just test to make sure it didn't crash.
		}

		{
			// Exercize the ability to read extreme values for various bases.
			char32_t pINT64_MIN_10_[24]; Strlcpy(pINT64_MIN_10_, "-9223372036854775809", EAArrayCount(pINT64_MIN_10_)); 
			char32_t pINT64_MIN_10[24];  Strlcpy(pINT64_MIN_10 , "-9223372036854775808", EAArrayCount(pINT64_MIN_10)); 
			char32_t pINT64_MAX_10[24];  Strlcpy(pINT64_MAX_10 , "+9223372036854775807", EAArrayCount(pINT64_MAX_10)); 
			char32_t pINT64_MAX_10_[24]; Strlcpy(pINT64_MAX_10_, "+9223372036854775808", EAArrayCount(pINT64_MIN_10_)); 

			errno = 0;
			result =  StrtoI64(pINT64_MIN_10_, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MIN) && (errno == ERANGE));

			errno = 0;
			result =  StrtoI64(pINT64_MIN_10, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MIN) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_10, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_10_, &pEnd, 10);
			EATEST_VERIFY((result == INT64_MAX) && (errno == ERANGE));


			char32_t pINT64_MIN_16_[24]; Strlcpy(pINT64_MIN_16_, "-8000000000000001", EAArrayCount(pINT64_MIN_16_)); 
			char32_t pINT64_MIN_16[24] ; Strlcpy(pINT64_MIN_16 , "-8000000000000000", EAArrayCount(pINT64_MIN_16)); 
			char32_t pINT64_MAX_16[24] ; Strlcpy(pINT64_MAX_16 , "+7fffffffffffffff", EAArrayCount(pINT64_MAX_16)); 
			char32_t pINT64_MAX_16_[24]; Strlcpy(pINT64_MAX_16_, "+8000000000000000", EAArrayCount(pINT64_MAX_16_)); 

			errno = 0;
			result =  StrtoI64(pINT64_MIN_16_, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MIN) && (errno == ERANGE));

			errno = 0;
			result =  StrtoI64(pINT64_MIN_16, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MIN) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_16, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoI64(pINT64_MAX_16_, &pEnd, 16);
			EATEST_VERIFY((result == INT64_MAX) && (errno == ERANGE));
		}
	}


	// uint64_t StrtoU64(const char*  pString, char**  ppStringEnd, int nBase);
	// uint64_t StrtoU64(const char16_t* pString, char16_t** ppStringEnd, int nBase);
	// uint64_t StrtoU64(const char32_t* pString, char32_t** ppStringEnd, int nBase);
	{
		uint64_t result;

		char  sn28[] = "7fffabcdffffabcd";
		char* pEnd = NULL;
		EATEST_VERIFY(UINT64_C(0x7fffabcdffffabcd) == StrtoU64(sn28, &pEnd, 16));

		char sn38[] = "101110011011101110111001010000111111100001100101";
		EATEST_VERIFY(UINT64_C(0xB9BBB943F865) == StrtoU64(sn38, &pEnd, 2));

		char sn48[] = "eb59a646c232da81";
		EATEST_VERIFY(UINT64_C(0xeb59a646c232da81) == StrtoU64(sn48, &pEnd, 16));

		{
			// Exercize the ability to read extreme values for various bases.
		  //char pUINT64_MIN_10_[] =                   "-1";
			char pUINT64_MIN_10[]  =                    "0";
			char pUINT64_MAX_10[]  = "18446744073709551615";
			char pUINT64_MAX_10_[] = "18446744073709551616";

		  // -1 converting to 18446744073709551615 seems to be what conforming Standard C libraries do. So we allow it too.
		  //errno = 0;
		  //result =  StrtoU64(pUINT64_MIN_10_, &pEnd, 10);
		  //EATEST_VERIFY_F((result == 0) && (errno == ERANGE), "StrtoU64: %s -> %I64u errno: %d", pUINT64_MIN_10_, result, (int)errno);

			errno = 0;
			result =  StrtoU64(pUINT64_MIN_10, &pEnd, 10);
			EATEST_VERIFY_F((result == 0) && (errno == 0), "StrtoU64: %s -> %I64u errno: %d", pUINT64_MIN_10, result, (int)errno);

			errno = 0;
			result =  StrtoU64(pUINT64_MAX_10, &pEnd, 10);
			EATEST_VERIFY_F((result == UINT64_MAX) && (errno == 0), "StrtoU64: %s -> %I64u errno: %d", pUINT64_MAX_10, result, (int)errno);

			errno = 0;
			result =  StrtoU64(pUINT64_MAX_10_, &pEnd, 10);
			EATEST_VERIFY_F((result == UINT64_MAX) && (errno == ERANGE), "StrtoU64: %s -> %I64u errno: %d", pUINT64_MAX_10_, result, (int)errno);



		  //char pUINT64_MIN_16_[] =                "-1";
			char pUINT64_MIN_16[]  =                 "0";
			char pUINT64_MAX_16[]  =  "ffffffffffffffff";
			char pUINT64_MAX_16_[] = "10000000000000000";

		  // -1 converting to 18446744073709551615 seems to be what conforming Standard C libraries do. So we allow it too.
		  //errno = 0;
		  //result =  StrtoU64(pUINT64_MIN_16_, &pEnd, 16);
		  //EATEST_VERIFY((result == 0) && (errno == ERANGE));

			errno = 0;
			result =  StrtoU64(pUINT64_MIN_16, &pEnd, 16);
			EATEST_VERIFY((result == 0) && (errno == 0));

			errno = 0;
			result =  StrtoU64(pUINT64_MAX_16, &pEnd, 16);
			EATEST_VERIFY((result == UINT64_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoU64(pUINT64_MAX_16_, &pEnd, 16);
			EATEST_VERIFY((result == UINT64_MAX) && (errno == ERANGE));
		}
	}
	{
		char16_t  sn28[24]; Strlcpy(sn28, EA_CHAR16("7fffabcdffffabcd"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t* pEnd = NULL;
		EATEST_VERIFY(UINT64_C(0x7fffabcdffffabcd) == StrtoU64(sn28, &pEnd, 16));

		char16_t sn38[64]; Strlcpy(sn38, EA_CHAR16("101110011011101110111001010000111111100001100101"), EAArrayCount(sn38)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(UINT64_C(0xB9BBB943F865) == StrtoU64(sn38, &pEnd, 2));

		char16_t sn48[24]; Strlcpy(sn48, EA_CHAR16("eb59a646c232da81"), EAArrayCount(sn48)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(UINT64_C(0xeb59a646c232da81) == StrtoU64(sn48, &pEnd, 16));
	}
	{
		char32_t  sn28[24]; Strlcpy(sn28, EA_CHAR32("7fffabcdffffabcd"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t* pEnd = NULL;
		EATEST_VERIFY(UINT64_C(0x7fffabcdffffabcd) == StrtoU64(sn28, &pEnd, 16));

		char32_t sn38[64]; Strlcpy(sn38, EA_CHAR32("101110011011101110111001010000111111100001100101"), EAArrayCount(sn38)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(UINT64_C(0xB9BBB943F865) == StrtoU64(sn38, &pEnd, 2));

		char32_t sn48[24]; Strlcpy(sn48, EA_CHAR32("eb59a646c232da81"), EAArrayCount(sn48)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(UINT64_C(0xeb59a646c232da81) == StrtoU64(sn48, &pEnd, 16));
	}


	// int32_t StrtoI32(const char*  pString, char**  ppStringEnd, int nBase);
	// int32_t StrtoI32(const char16_t* pString, char16_t** ppStringEnd, int nBase);
	// int32_t StrtoI32(const char32_t* pString, char32_t** ppStringEnd, int nBase);
	{
		int32_t result;

		char  sn18[] = "-1011101110";
		char* pEnd = NULL;
		EATEST_VERIFY(-1011101110 == StrtoI32(sn18, &pEnd, 10));

		char sn28[] = "7fffabcd";
		EATEST_VERIFY(0x7fffabcd == StrtoI32(sn28, &pEnd, 16));

		char sn38[] = "00111001010000111111100001100101";
		EATEST_VERIFY(0x3943F865LL == StrtoI32(sn38, &pEnd, 2));

		char sn48[] = "BEEFEEC0DE"; //number greater than INT32_MAX
		EATEST_VERIFY(INT32_MAX == StrtoI32(sn48, &pEnd, 16));

		char sn58[] = "-BAEBEEC0DE"; //number less than INT32_MIN
		EATEST_VERIFY(INT32_MIN == StrtoI32(sn58, &pEnd, 16));

		{
			// Exercize the ability to read extreme values for various bases.
			char pINT32_MIN_10_[] = "-2147483649";
			char pINT32_MIN_10[]  = "-2147483648";
			char pINT32_MAX_10[]  = "+2147483647";
			char pINT32_MAX_10_[] = "+2147483648";

			errno = 0;
			result =  StrtoI32(pINT32_MIN_10_, &pEnd, 10);
			EATEST_VERIFY((result == INT32_MIN) && (errno == ERANGE));

			errno = 0;
			result =  StrtoI32(pINT32_MIN_10, &pEnd, 10);
			EATEST_VERIFY((result == INT32_MIN) && (errno == 0));

			errno = 0;
			result =  StrtoI32(pINT32_MAX_10, &pEnd, 10);
			EATEST_VERIFY((result == INT32_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoI32(pINT32_MAX_10_, &pEnd, 10);
			EATEST_VERIFY((result == INT32_MAX) && (errno == ERANGE));



			char pINT32_MIN_16_[] = "-80000001";
			char pINT32_MIN_16[]  = "-80000000";
			char pINT32_MAX_16[]  = "+7fffffff";
			char pINT32_MAX_16_[] = "+80000000";

			errno = 0;
			result =  StrtoI32(pINT32_MIN_16_, &pEnd, 16);
			EATEST_VERIFY((result == INT32_MIN) && (errno == ERANGE));

			errno = 0;
			result =  StrtoI32(pINT32_MIN_16, &pEnd, 16);
			EATEST_VERIFY((result == INT32_MIN) && (errno == 0));

			errno = 0;
			result =  StrtoI32(pINT32_MAX_16, &pEnd, 16);
			EATEST_VERIFY((result == INT32_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoI32(pINT32_MAX_16_, &pEnd, 16);
			EATEST_VERIFY((result == INT32_MAX) && (errno == ERANGE));
		}
	}
	{
		char16_t  sn18[16]; Strlcpy(sn18, EA_CHAR16("-1011101110"), EAArrayCount(sn18)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t* pEnd = NULL;
		EATEST_VERIFY(-1011101110 == StrtoI32(sn18, &pEnd, 10));

		char16_t sn28[12]; Strlcpy(sn28, EA_CHAR16("7fffabcd"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(0x7fffabcd == StrtoI32(sn28, &pEnd, 16));

		char16_t sn38[48]; Strlcpy(sn38, EA_CHAR16("00111001010000111111100001100101"), EAArrayCount(sn38)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(0x3943F865LL == StrtoI32(sn38, &pEnd, 2));

		char16_t sn48[16]; Strlcpy(sn48, EA_CHAR16("BEEFEEC0DE"), EAArrayCount(sn48)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(INT32_MAX == StrtoI32(sn48, &pEnd, 16));

		char16_t sn58[16]; Strlcpy(sn58, EA_CHAR16("-BAEBEEC0DE"), EAArrayCount(sn58)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(INT32_MIN == StrtoI32(sn58, &pEnd, 16));
	}
	{
		char32_t  sn18[32]; Strlcpy(sn18, EA_CHAR32("-1011101110"), EAArrayCount(sn18)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t* pEnd = NULL;
		EATEST_VERIFY(-1011101110 == StrtoI32(sn18, &pEnd, 10));

		char32_t sn28[12]; Strlcpy(sn28, EA_CHAR32("7fffabcd"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(0x7fffabcd == StrtoI32(sn28, &pEnd, 16));

		char32_t sn38[48]; Strlcpy(sn38, EA_CHAR32("00111001010000111111100001100101"), EAArrayCount(sn38)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(0x3943F865LL == StrtoI32(sn38, &pEnd, 2));

		char32_t sn48[16]; Strlcpy(sn48, EA_CHAR32("BEEFEEC0DE"), EAArrayCount(sn48)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(INT32_MAX == StrtoI32(sn48, &pEnd, 16));

		char32_t sn58[16]; Strlcpy(sn58, EA_CHAR32("-BAEBEEC0DE"), EAArrayCount(sn58)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(INT32_MIN == StrtoI32(sn58, &pEnd, 16));
	}


	// uint32_t StrtoU32(const char* pString, char**  ppStringEnd, int nBase);
	// uint32_t StrtoU32(const char* pString, char16_t** ppStringEnd, int nBase);
	// uint32_t StrtoU32(const char* pString, char32_t** ppStringEnd, int nBase);
	{
		uint32_t result;

		char  sn28[] = "7fffabcd";
		char* pEnd = NULL;

		EATEST_VERIFY(0x7fffabcd == StrtoU32(sn28, &pEnd, 16));

		char sn38[] = "10111001010000111111100001100101";
		EATEST_VERIFY(0xB943F865LL == StrtoU32(sn38, &pEnd, 2));

		char sn48[] = "BEEFEEC0DE"; //number greater than UINT32_MAX
		EATEST_VERIFY(UINT32_MAX == StrtoU32(sn48, &pEnd, 16));

		{
			// Exercize the ability to read extreme values for various bases.
		  //char pUINT32_MIN_10_[] =         "-1";
			char pUINT32_MIN_10[]  =          "0";
			char pUINT32_MAX_10[]  = "4294967295";
			char pUINT32_MAX_10_[] = "4294967296";

		  // -1 converting to 18446744073709551615 seems to be what conforming Standard C libraries do. So we allow it too.
		  //errno = 0;
		  //result =  StrtoU32(pUINT32_MIN_10_, &pEnd, 10);
		  //EATEST_VERIFY((result == 0) && (errno == ERANGE));

			errno = 0;
			result =  StrtoU32(pUINT32_MIN_10, &pEnd, 10);
			EATEST_VERIFY((result == 0) && (errno == 0));

			errno = 0;
			result =  StrtoU32(pUINT32_MAX_10, &pEnd, 10);
			EATEST_VERIFY((result == UINT32_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoU32(pUINT32_MAX_10_, &pEnd, 10);
			EATEST_VERIFY((result == UINT32_MAX) && (errno == ERANGE));


		  //char pUINT32_MIN_16_[] =        "-1";
			char pUINT32_MIN_16[]  =         "0";
			char pUINT32_MAX_16[]  =  "ffffffff";
			char pUINT32_MAX_16_[] = "100000000";

		  // -1 converting to 18446744073709551615 seems to be what conforming Standard C libraries do. So we allow it too.
		  //errno = 0;
		  //result =  StrtoU32(pUINT32_MIN_16_, &pEnd, 16);
		  //EATEST_VERIFY((result == 0) && (errno == ERANGE));

			errno = 0;
			result =  StrtoU32(pUINT32_MIN_16, &pEnd, 16);
			EATEST_VERIFY((result == 0) && (errno == 0));

			errno = 0;
			result =  StrtoU32(pUINT32_MAX_16, &pEnd, 16);
			EATEST_VERIFY((result == UINT32_MAX) && (errno == 0));

			errno = 0;
			result =  StrtoU32(pUINT32_MAX_16_, &pEnd, 16);
			EATEST_VERIFY((result == UINT32_MAX) && (errno == ERANGE));
		}
	}
	{
		char16_t  sn28[16]; Strlcpy(sn28, EA_CHAR16("7fffabcd"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		char16_t* pEnd = NULL;
		EATEST_VERIFY(0x7fffabcd == StrtoU32(sn28, &pEnd, 16));

		char16_t sn38[48]; Strlcpy(sn38, EA_CHAR16("10111001010000111111100001100101"), EAArrayCount(sn38)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(0xB943F865LL == StrtoU32(sn38, &pEnd, 2));

		char16_t sn48[16]; Strlcpy(sn48, EA_CHAR16("BEEFEEC0DE"), EAArrayCount(sn48)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(UINT32_MAX == StrtoU32(sn48, &pEnd, 16));
	}
	{
		char32_t  sn28[32]; Strlcpy(sn28, EA_CHAR32("7fffabcd"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		char32_t* pEnd = NULL;
		EATEST_VERIFY(0x7fffabcd == StrtoU32(sn28, &pEnd, 16));

		char32_t sn38[48]; Strlcpy(sn38, EA_CHAR32("10111001010000111111100001100101"), EAArrayCount(sn38)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(0xB943F865LL == StrtoU32(sn38, &pEnd, 2));

		char32_t sn48[16]; Strlcpy(sn48, EA_CHAR32("BEEFEEC0DE"), EAArrayCount(sn48)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(UINT32_MAX == StrtoU32(sn48, &pEnd, 16));
	}


	// int32_t AtoI32(const char*  pString);
	// int32_t AtoI32(const char16_t* pString);
	// int32_t AtoI32(const char32_t* pString);
	{
		char sn18[] = "-1011101110";
		EATEST_VERIFY(-1011101110 == AtoI32(sn18));

		char sn28[] = "2147483647";
		EATEST_VERIFY(INT32_MAX == AtoI32(sn28));

		char sn38[] = "-2147483648";
		EATEST_VERIFY(INT32_MIN == AtoI32(sn38));
	}
	{
		char16_t sn18[16]; Strlcpy(sn18, EA_CHAR16("-1011101110"), EAArrayCount(sn18)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(-1011101110 == AtoI32(sn18));

		char16_t sn28[16]; Strlcpy(sn28, EA_CHAR16("2147483647"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(INT32_MAX == AtoI32(sn28));

		char16_t sn38[16]; Strlcpy(sn38, EA_CHAR16("-2147483648"), EAArrayCount(sn38)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(INT32_MIN == AtoI32(sn38));
	}
	{
		char32_t sn18[32]; Strlcpy(sn18, EA_CHAR32("-1011101110"), EAArrayCount(sn18)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(-1011101110 == AtoI32(sn18));

		char32_t sn28[32]; Strlcpy(sn28, EA_CHAR32("2147483647"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(INT32_MAX == AtoI32(sn28));

		char32_t sn38[32]; Strlcpy(sn38, EA_CHAR32("-2147483648"), EAArrayCount(sn38)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(INT32_MIN == AtoI32(sn38));
	}


	// uint32_t AtoU32(const char*  pString);
	// uint32_t AtoU32(const char16_t* pString);
	// uint32_t AtoU32(const char32_t* pString);
	{
		char sn28[] = "4294967295";
		EATEST_VERIFY(UINT32_MAX == AtoU32(sn28));
	}
	{
		char16_t sn28[12]; Strlcpy(sn28, EA_CHAR16("4294967295"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(UINT32_MAX == AtoU32(sn28));
	}
	{
		char32_t sn28[12]; Strlcpy(sn28, EA_CHAR32("4294967295"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(UINT32_MAX == AtoU32(sn28));
	}


	// int64_t AtoI64(const char*  pString);
	// int64_t AtoI64(const char16_t* pString);
	// int64_t AtoI64(const char32_t* pString);
	{
		char sn18[] = "-1011101110111011101";
		EATEST_VERIFY(-1011101110111011101LL == AtoI64(sn18));

		char sn28[] = "9223372036854775807";
		EATEST_VERIFY(INT64_MAX == AtoI64(sn28));

		char sn38[] = "-9223372036854775808";
		EATEST_VERIFY(INT64_MIN == AtoI64(sn38));
	}
	{
		char16_t sn18[24]; Strlcpy(sn18, EA_CHAR16("-1011101110111011101"), EAArrayCount(sn18)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(-1011101110111011101LL == AtoI64(sn18));

		char16_t sn28[24]; Strlcpy(sn28, EA_CHAR16("9223372036854775807"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(INT64_MAX == AtoI64(sn28));

		char16_t sn38[24]; Strlcpy(sn38, EA_CHAR16("-9223372036854775808"), EAArrayCount(sn38)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(INT64_MIN == AtoI64(sn38));
	}
	{
		char32_t sn18[24]; Strlcpy(sn18, EA_CHAR32("-1011101110111011101"), EAArrayCount(sn18)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(-1011101110111011101LL == AtoI64(sn18));

		char32_t sn28[24]; Strlcpy(sn28, EA_CHAR32("9223372036854775807"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(INT64_MAX == AtoI64(sn28));

		char32_t sn38[24]; Strlcpy(sn38, EA_CHAR32("-9223372036854775808"), EAArrayCount(sn38)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(INT64_MIN == AtoI64(sn38));
	}


	// uint64_t AtoU64(const char*  pString);
	// uint64_t AtoU64(const char16_t* pString);
	// uint64_t AtoU64(const char32_t* pString);
	{
		char sn28[] = "18446744073709551615";
		EATEST_VERIFY(UINT64_MAX == AtoU64(sn28));
	}
	{
		char16_t sn28[24]; Strlcpy(sn28, EA_CHAR16("18446744073709551615"), EAArrayCount(sn28)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
		EATEST_VERIFY(UINT64_MAX == AtoU64(sn28));
	}
	{
		char32_t sn28[24]; Strlcpy(sn28, EA_CHAR32("18446744073709553215"), EAArrayCount(sn28)); // Can't do char32_t variable[64] = EA_CHAR32(...) because some compilers don't support 32 bit string literals.
		EATEST_VERIFY(UINT64_MAX == AtoU64(sn28));
	}

	return nErrorCount;
}


static int TestAtof()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// double Atof(const char*  pString);
	// double Atof(const char16_t* pString);
	// double Atof(const char32_t* pString);
	{
		const char* kStrMin = "2.2250738585072014e-307"; // DBL_MIN is usually 2.2250738585072014e-308, but some Standard Libraries have trouble converting 
		const double   kValMin =  2.2250738585072014e-307;  // to and from strings of that value. So we use -307 instead, which is pretty reliably supported.

		const char* kStrMax = "1.7976931348623158e+307"; // DBL_MAX is usually 1.7976931348623158e+308.
		const double   kValMax =  1.7976931348623158e+307;

		{
			double d = Atof(kStrMax);
			EATEST_VERIFY(TestString_DoubleEqual(d, kValMax));

			d = Atof(kStrMin);
			EATEST_VERIFY(TestString_DoubleEqual(d, kValMin));
		}
		{
			char16_t sn18[32]; Strlcpy(sn18, kStrMax, EAArrayCount(sn18)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
			double  d = Atof(sn18);
			EATEST_VERIFY(TestString_DoubleEqual(d, kValMax));

			char16_t sn28[32]; Strlcpy(sn28, kStrMin, EAArrayCount(sn28));
			d = Atof(sn28);
			EATEST_VERIFY(TestString_DoubleEqual(d, kValMin));
		}
		{
			char32_t sn18[32]; Strlcpy(sn18, kStrMax, EAArrayCount(sn18));
			double d = Atof(sn18);
			EATEST_VERIFY(TestString_DoubleEqual(d, kValMax));

			char32_t sn28[32]; Strlcpy(sn28, kStrMin, EAArrayCount(sn28));
			d = Atof(sn28);
			EATEST_VERIFY(TestString_DoubleEqual(d, kValMin));
		}
	}

	// double AtofEnglish(const char*  pString);
	// double AtofEnglish(const char16_t* pString);
	// double AtofEnglish(const char32_t* pString);
	//
	// AtofEnglish - this function fails on boundary values for double_t 
	// as it performs some math inside that drives the values to infinity.
	{
		const char* kStrMin = "2.2250738585071e-307"; // DBL_MIN is usually 2.2250738585072014e-308, but some Standard Libraries have trouble converting 
		const double   kValMin =  2.2250738585071e-307;  // to and from strings of that value. So we use -307 instead, which is pretty reliably supported.

		const char* kStrMax = "1.7976931348622e+307"; // DBL_MAX is usually 1.7976931348623158e+308.
		const double   kValMax =  1.7976931348622e+307;

		{
			EATEST_VERIFY(TestString_DoubleEqual(kValMin, AtofEnglish(kStrMin)));

			EATEST_VERIFY(TestString_DoubleEqual(kValMax, AtofEnglish(kStrMax)));
		}
		{
			char16_t sn18[32]; Strlcpy(sn18, kStrMin, EAArrayCount(sn18)); // Can't do char16_t variable[64] = EA_CHAR16(...) because some compilers don't support 16 bit string literals.
			EATEST_VERIFY(TestString_DoubleEqual(kValMin, AtofEnglish(sn18)));

			char16_t sn28[32]; Strlcpy(sn28, kStrMax, EAArrayCount(sn28));
			EATEST_VERIFY(TestString_DoubleEqual(kValMax, AtofEnglish(sn28)));
		}
		{
			char32_t sn18[32]; Strlcpy(sn18, kStrMin, EAArrayCount(sn18));
			EATEST_VERIFY(TestString_DoubleEqual(kValMin, AtofEnglish(sn18)));

			char32_t sn28[32]; Strlcpy(sn28, kStrMax, EAArrayCount(sn28));
			EATEST_VERIFY(TestString_DoubleEqual(kValMax, AtofEnglish(sn28)));
		}
	}

	return nErrorCount;
}


static int TestFtoa()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// char*  Ftoa(double dValue, char_t* pResult, int nInputLength, int nPrecision, bool bExponentEnabled);
	// char16_t* Ftoa(double dValue, char_t* pResult, int nInputLength, int nPrecision, bool bExponentEnabled);
	// char32_t* Ftoa(double dValue, char_t* pResult, int nInputLength, int nPrecision, bool bExponentEnabled);
	{
		char sn18[128];

		Ftoa(-1.7976931, sn18, 128, 7, false);
		if(Strcmp("-1.7976931", sn18) != 0)
		{
			EA::UnitTest::Report("%s\n", sn18);
			EATEST_VERIFY(Strcmp("-1.7976931", sn18) == 0);
		}

		Ftoa(10.7976931e299, sn18, 128, 5, true);
		if(Strcmp("1.07977e+300", sn18) != 0)
		{
			EA::UnitTest::Report("%s\n", sn18);
			EATEST_VERIFY(Strcmp("1.07977e+300", sn18) == 0);
		}

		Ftoa(-1.7976931, sn18, 128, 4, false);
		if(Strcmp("-1.7977", sn18) != 0)
		{
			EA::UnitTest::Report("%s\n", sn18);
			EATEST_VERIFY(Strcmp("-1.7977", sn18) == 0);
		}

		Ftoa(0.107976931e-299, sn18, 128, 5, true);
		if(Strcmp("1.07977e-300", sn18) != 0)
		{
			EA::UnitTest::Report("%s\n", sn18);
			EATEST_VERIFY(Strcmp("1.07977e-300", sn18) == 0);
		}
	}
	{
		char16_t sn18[128];

		Ftoa(-1.7976931, sn18, 128, 7, false);
		if(Strcmp(EA_CHAR16("-1.7976931"), sn18) != 0)
		{
			EA::UnitTest::Report("%ls\n", sn18);
			EATEST_VERIFY(Strcmp(EA_CHAR16("-1.7976931"), sn18) == 0);
		}

		Ftoa(10.7976931e299, sn18, 128, 5, true);
		if(Strcmp(EA_CHAR16("1.07977e+300"), sn18) != 0)
		{
			EA::UnitTest::Report("%ls\n", sn18);
			EATEST_VERIFY(Strcmp(EA_CHAR16("1.07977e+300"), sn18) == 0);
		}

		Ftoa(-1.7976931, sn18, 128, 4, false);
		if(Strcmp(EA_CHAR16("-1.7977"), sn18) != 0)
		{
			EA::UnitTest::Report("%ls\n", sn18);
			EATEST_VERIFY(Strcmp(EA_CHAR16("-1.7977"), sn18) == 0);
		}

		Ftoa(0.107976931e-299, sn18, 128, 5, true);
		if(Strcmp(EA_CHAR16("1.07977e-300"), sn18) != 0)
		{
			EA::UnitTest::Report("%ls\n", sn18);
			EATEST_VERIFY(Strcmp(EA_CHAR16("1.07977e-300"), sn18) == 0);
		}
	}
	{
		char32_t sn18[128];

		Ftoa(-1.7976931, sn18, 128, 7, false);
		if(Strcmp(EA_CHAR32("-1.7976931"), sn18) != 0)
		{
			EA::UnitTest::Report("%ls\n", sn18);
			EATEST_VERIFY(Strcmp(EA_CHAR32("-1.7976931"), sn18) == 0);
		}

		Ftoa(10.7976931e299, sn18, 128, 5, true);
		if(Strcmp(EA_CHAR32("1.07977e+300"), sn18) != 0)
		{
			EA::UnitTest::Report("%ls\n", sn18);
			EATEST_VERIFY(Strcmp(EA_CHAR32("1.07977e+300"), sn18) == 0);
		}

		Ftoa(-1.7976931, sn18, 128, 4, false);
		if(Strcmp(EA_CHAR32("-1.7977"), sn18) != 0)
		{
			EA::UnitTest::Report("%ls\n", sn18);
			EATEST_VERIFY(Strcmp(EA_CHAR32("-1.7977"), sn18) == 0);
		}

		Ftoa(0.107976931e-299, sn18, 128, 5, true);
		if(Strcmp(EA_CHAR32("1.07977e-300"), sn18) != 0)
		{
			EA::UnitTest::Report("%ls\n", sn18);
			EATEST_VERIFY(Strcmp(EA_CHAR32("1.07977e-300"), sn18) == 0);
		}
	}


	// char*  FtoaEnglish(double dValue, char*  pResult, int nInputLength, int nPrecision, bool bExponentEnabled);
	// char16_t* FtoaEnglish(double dValue, char16_t* pResult, int nInputLength, int nPrecision, bool bExponentEnabled);
	// char32_t* FtoaEnglish(double dValue, char32_t* pResult, int nInputLength, int nPrecision, bool bExponentEnabled);
	{
		char  sn18[128];
		char* pResult;

		pResult = FtoaEnglish(-1.7976931, sn18, 128, 7, false);
		EATEST_VERIFY(pResult && (Strcmp("-1.7976931", sn18) == 0));

		pResult = FtoaEnglish(10.7976931e299, sn18, 128, 5, true);
		EATEST_VERIFY(pResult && (Strcmp("1.07977e+300", sn18) == 0));

		pResult = FtoaEnglish(-1.7976931, sn18, 128, 4, false);
		EATEST_VERIFY(pResult && (Strcmp("-1.7977", sn18) == 0));

		pResult = FtoaEnglish(0.107976931e-299, sn18, 128, 5, true);
		EATEST_VERIFY(pResult && (Strcmp("1.07977e-300", sn18) == 0));

		pResult = FtoaEnglish(10000, sn18, 1, 0, false);
		EATEST_VERIFY(!pResult && (Strcmp("", sn18) == 0));

		pResult = FtoaEnglish(10000, sn18, 10, 0, false);
		EATEST_VERIFY(pResult && (Strcmp("10000", sn18) == 0));

		pResult = FtoaEnglish(10000.003, sn18, 10, 0, false);
		EATEST_VERIFY(pResult && (Strcmp("10000", sn18) == 0));

		pResult = FtoaEnglish(10000.003, sn18, 6, 10, false);
		EATEST_VERIFY(pResult == NULL);

		pResult = FtoaEnglish(10000.12345, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp("10000.123", sn18) == 0));

		pResult = FtoaEnglish(10000.12348, sn18, 20, 8, false);
		EATEST_VERIFY(pResult && (Strcmp("10000.12348", sn18) == 0));

		pResult = FtoaEnglish(0.0, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp("0", sn18) == 0));

		pResult = FtoaEnglish(0.0, sn18, 20, 3, true);
		EATEST_VERIFY(pResult && (Strcmp("0", sn18) == 0));

		pResult = FtoaEnglish(0.0001, sn18, 20, 4, true);
		EATEST_VERIFY(pResult && (Strcmp("0.0001", sn18) == 0));

		pResult = FtoaEnglish(.12345, sn18, 20, 2, false);
		EATEST_VERIFY(pResult && (Strcmp("0.12", sn18) == 0));

		pResult = FtoaEnglish(.012345, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp("0.012", sn18) == 0));

		pResult = FtoaEnglish(.0012345, sn18, 20, 2, false);
		EATEST_VERIFY(pResult && (Strcmp("0", sn18) == 0));

		pResult = FtoaEnglish(.123450000, sn18, 20, 17, false);
		EATEST_VERIFY(pResult && (Strcmp("0.12345", sn18) == 0));
	}
	{
		char16_t  sn18[128];
		char16_t* pResult;

		pResult = FtoaEnglish(-1.7976931, sn18, 128, 7, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("-1.7976931"), sn18) == 0));

		pResult = FtoaEnglish(10.7976931e299, sn18, 128, 5, true);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("1.07977e+300"), sn18) == 0));

		pResult = FtoaEnglish(-1.7976931, sn18, 128, 4, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("-1.7977"), sn18) == 0));

		pResult = FtoaEnglish(0.107976931e-299, sn18, 128, 5, true);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("1.07977e-300"), sn18) == 0));

		pResult = FtoaEnglish(10000, sn18, 1, 0, false);
		EATEST_VERIFY(!pResult && (Strcmp(EA_CHAR16(""), sn18) == 0));

		pResult = FtoaEnglish(10000, sn18, 10, 0, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("10000"), sn18) == 0));

		pResult = FtoaEnglish(10000.003, sn18, 10, 0, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("10000"), sn18) == 0));

		pResult = FtoaEnglish(10000.003, sn18, 6, 10, false);
		EATEST_VERIFY(pResult == NULL);

		pResult = FtoaEnglish(10000.12345, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("10000.123"), sn18) == 0));

		pResult = FtoaEnglish(10000.12348, sn18, 20, 8, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("10000.12348"), sn18) == 0));

		pResult = FtoaEnglish(0.0, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("0"), sn18) == 0));

		pResult = FtoaEnglish(0.0, sn18, 20, 3, true);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("0"), sn18) == 0));

		pResult = FtoaEnglish(0.0001, sn18, 20, 4, true);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("0.0001"), sn18) == 0));

		pResult = FtoaEnglish(.12345, sn18, 20, 2, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("0.12"), sn18) == 0));

		pResult = FtoaEnglish(.012345, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("0.012"), sn18) == 0));

		pResult = FtoaEnglish(.0012345, sn18, 20, 2, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("0"), sn18) == 0));

		pResult = FtoaEnglish(.123450000, sn18, 20, 17, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR16("0.12345"), sn18) == 0));
	}
	{
		char32_t  sn18[128];
		char32_t* pResult;

		pResult = FtoaEnglish(-1.7976931, sn18, 128, 7, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("-1.7976931"), sn18) == 0));

		pResult = FtoaEnglish(10.7976931e299, sn18, 128, 5, true);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("1.07977e+300"), sn18) == 0));

		pResult = FtoaEnglish(-1.7976931, sn18, 128, 4, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("-1.7977"), sn18) == 0));

		pResult = FtoaEnglish(0.107976931e-299, sn18, 128, 5, true);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("1.07977e-300"), sn18) == 0));

		pResult = FtoaEnglish(10000, sn18, 1, 0, false);
		EATEST_VERIFY(!pResult && (Strcmp(EA_CHAR32(""), sn18) == 0));

		pResult = FtoaEnglish(10000, sn18, 10, 0, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("10000"), sn18) == 0));

		pResult = FtoaEnglish(10000.003, sn18, 10, 0, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("10000"), sn18) == 0));

		pResult = FtoaEnglish(10000.003, sn18, 6, 10, false);
		EATEST_VERIFY(pResult == NULL);

		pResult = FtoaEnglish(10000.12345, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("10000.123"), sn18) == 0));

		pResult = FtoaEnglish(10000.12348, sn18, 20, 8, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("10000.12348"), sn18) == 0));

		pResult = FtoaEnglish(0.0, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("0"), sn18) == 0));

		pResult = FtoaEnglish(0.0, sn18, 20, 3, true);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("0"), sn18) == 0));

		pResult = FtoaEnglish(0.0001, sn18, 20, 4, true);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("0.0001"), sn18) == 0));

		pResult = FtoaEnglish(.12345, sn18, 20, 2, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("0.12"), sn18) == 0));

		pResult = FtoaEnglish(.012345, sn18, 20, 3, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("0.012"), sn18) == 0));

		pResult = FtoaEnglish(.0012345, sn18, 20, 2, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("0"), sn18) == 0));

		pResult = FtoaEnglish(.123450000, sn18, 20, 17, false);
		EATEST_VERIFY(pResult && (Strcmp(EA_CHAR32("0.12345"), sn18) == 0));
	}

	return nErrorCount;
}


static int TestReduceFloatString()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// size_t ReduceFloatString(char*  pString, size_t nLength = (size_t)~0);
	// size_t ReduceFloatString(char16_t* pString, size_t nLength = (size_t)~0);
	// size_t ReduceFloatString(char32_t* pString, size_t nLength = (size_t)~0);
	{
		char pBuffer[64];
		size_t  n;

		Strcpy(pBuffer, "2.3400");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("2.34")) && (Strcmp("2.34", pBuffer) == 0));

		Strcpy(pBuffer, "00000");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("0")) && (Strcmp("0", pBuffer) == 0));

		Strcpy(pBuffer, ".0000");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("0")) && (Strcmp("0", pBuffer) == 0));

		Strcpy(pBuffer, "000.0000");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("0")) && (Strcmp("0", pBuffer) == 0));

		Strcpy(pBuffer, "-0.0000");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("-0")) && (Strcmp("-0", pBuffer) == 0));

		Strcpy(pBuffer, "23.430000e10");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("23.43e10")) && (Strcmp("23.43e10", pBuffer) == 0));

		Strcpy(pBuffer, "2.34001");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("2.34001")) && (Strcmp("2.34001", pBuffer) == 0));

		Strcpy(pBuffer, "0.00001");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(".00001")) && (Strcmp(".00001", pBuffer) == 0));

		Strcpy(pBuffer, "-0.00001");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("-0.00001")) && (Strcmp("-0.00001", pBuffer) == 0));

		Strcpy(pBuffer, "23.43e10");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("23.43e10")) && (Strcmp("23.43e10", pBuffer) == 0));

		Strcpy(pBuffer, "15e-0");
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen("15e-0")) && (Strcmp("15e-0", pBuffer) == 0));
	}
	{
		char16_t pBuffer[64];
		size_t   n;

		Strcpy(pBuffer, EA_CHAR16("2.3400"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("2.34"))) && (Strcmp(EA_CHAR16("2.34"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("00000"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("0"))) && (Strcmp(EA_CHAR16("0"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16(".0000"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("0"))) && (Strcmp(EA_CHAR16("0"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("000.0000"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("0"))) && (Strcmp(EA_CHAR16("0"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("-0.0000"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("-0"))) && (Strcmp(EA_CHAR16("-0"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("23.430000e10"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("23.43e10"))) && (Strcmp(EA_CHAR16("23.43e10"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("2.34001"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("2.34001"))) && (Strcmp(EA_CHAR16("2.34001"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("0.00001"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16(".00001"))) && (Strcmp(EA_CHAR16(".00001"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("-0.00001"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("-0.00001"))) && (Strcmp(EA_CHAR16("-0.00001"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("23.43e10"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("23.43e10"))) && (Strcmp(EA_CHAR16("23.43e10"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR16("15e-0"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR16("15e-0"))) && (Strcmp(EA_CHAR16("15e-0"), pBuffer) == 0));
	}
	{
		char32_t pBuffer[64];
		size_t   n;

		Strcpy(pBuffer, EA_CHAR32("2.3400"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("2.34"))) && (Strcmp(EA_CHAR32("2.34"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("00000"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("0"))) && (Strcmp(EA_CHAR32("0"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32(".0000"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("0"))) && (Strcmp(EA_CHAR32("0"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("000.0000"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("0"))) && (Strcmp(EA_CHAR32("0"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("-0.0000"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("-0"))) && (Strcmp(EA_CHAR32("-0"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("23.430000e10"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("23.43e10"))) && (Strcmp(EA_CHAR32("23.43e10"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("2.34001"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("2.34001"))) && (Strcmp(EA_CHAR32("2.34001"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("0.00001"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32(".00001"))) && (Strcmp(EA_CHAR32(".00001"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("-0.00001"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("-0.00001"))) && (Strcmp(EA_CHAR32("-0.00001"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("23.43e10"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("23.43e10"))) && (Strcmp(EA_CHAR32("23.43e10"), pBuffer) == 0));

		Strcpy(pBuffer, EA_CHAR32("15e-0"));
		n = ReduceFloatString(pBuffer);
		EATEST_VERIFY((n == Strlen(EA_CHAR32("15e-0"))) && (Strcmp(EA_CHAR32("15e-0"), pBuffer) == 0));
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestConvertString
//
static int TestConvertString()
{
	int nErrorCount = 0;

	eastl::string8  s8 = EA_CHAR8("hello world");
	eastl::string16 s16;
	eastl::string32 s32;

	// template <typename Dest, typename Source>
	// inline bool Strlcpy(Dest& d, const Source& s);
	{
		// 8 -> 16
		EA::StdC::Strlcpy(s16, s8);
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s8.clear();

		// 16 -> 8
		EA::StdC::Strlcpy(s8, s16);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s16.clear();


		// 8 -> 16
		// Note that using this non-const string causes this specialization to be called instead of the specialization that takes const Source*.
		char pNonConstString8[] = "hello world";
		EA::StdC::Strlcpy(s16, pNonConstString8);
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s8.clear();

		// 16 -> 8
		// Note that using this non-const string causes this specialization to be called instead of the specialization that takes const Source*.
		char16_t pNonConstString16[] = EA_CHAR16("hello world");
		EA::StdC::Strlcpy(s8, pNonConstString16);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s16.clear();
	}
	{
		// 8 -> 32
		EA::StdC::Strlcpy(s32, s8);
		EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
		s8.clear();

		// 32 -> 8
		EA::StdC::Strlcpy(s8, s32);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s32.clear();
	}
	{
		// 8 -> 8
		EA::StdC::Strlcpy<eastl::string8, eastl::string8>(s8, eastl::string8(EA_CHAR8("hello world")));
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s8.clear();

		// 16 -> 16
		EA::StdC::Strlcpy<eastl::string16, eastl::string16>(s16, eastl::string16(EA_CHAR16("hello world")));
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s16.clear();

		// 32 -> 32
		EA::StdC::Strlcpy<eastl::string32, eastl::string32>(s32, eastl::string32(EA_CHAR32("hello world")));
		EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
		s32.clear();
	}


	// template <typename Dest, typename Source>
	// inline bool Strlcpy(Dest& d, const Source* pSource, size_t sourceLength = (size_t)~0);
	{
		// 8 -> 16
		s8 = EA_CHAR8("hello world");
		EA::StdC::Strlcpy(s16, s8.c_str(), s8.length());
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s8.clear();

		// 16 -> 8
		EA::StdC::Strlcpy(s8, s16.c_str(), s16.length());
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s16.clear();
	}
	{
		// 8 -> 32
		EA::StdC::Strlcpy(s32, s8.c_str(), s8.length());
		EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
		s8.clear();

		// 32 -> 8
		EA::StdC::Strlcpy(s8, s32.c_str(), s32.length());
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s32.clear();
	}
	{
		// 8 -> 8
		EA::StdC::Strlcpy(s8, "hello world", 11);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s8.clear();

		// 16 -> 16
		EA::StdC::Strlcpy(s16, EA_CHAR16("hello world"), 11);
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s32.clear();

		// 32 -> 32
		EA::StdC::Strlcpy(s32, EA_CHAR32("hello world"), 11);
		EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
		s32.clear();
	}


	// template <typename Dest, typename Source>
	// inline Dest Strlcpy(const Source& s);
	{
		// 8 -> 16
		s8 = EA_CHAR8("hello world");
		s16 = EA::StdC::Strlcpy<eastl::string16,  eastl::string8>(s8);
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s8.clear();

		// 16 -> 8
		s8 = EA::StdC::Strlcpy<eastl::string8, eastl::string16>(s16);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s16.clear();
	}
	{
		// 8 -> 32
		s32 = EA::StdC::Strlcpy<eastl::string32, eastl::string8>(s8);
		EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
		s8.clear();

		// 32 -> 8
		s8 = EA::StdC::Strlcpy<eastl::string8, eastl::string32>(s32);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s32.clear();
	}
	{
		// 8 -> 8
		s8 = EA::StdC::Strlcpy<eastl::string8, eastl::string8>(eastl::string8(EA_CHAR8("hello world")));
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s8.clear();

		// 16 -> 16
		s16 = EA::StdC::Strlcpy<eastl::string16, eastl::string16>(eastl::string16(EA_CHAR16("hello world")));
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s16.clear();

		// 32 -> 32
		s32 = EA::StdC::Strlcpy<eastl::string32, eastl::string32>(eastl::string32(EA_CHAR32("hello world")));
		EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
		s32.clear();
	}

	// template <typename Dest, typename Source>
	// inline Dest Strlcpy(Dest& d, const Source* pSource, size_t sourceLength = (size_t)~0);
	{
		// 8 -> 16
		s16 = EA::StdC::Strlcpy<eastl::string16, char>("hello world", 11);
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s8.clear();

		// 16 -> 8
		s8 = EA::StdC::Strlcpy<eastl::string8, char16_t>(EA_CHAR16("hello world"), 11);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s16.clear();
	}
	{
		// 8 -> 32
		s32 = EA::StdC::Strlcpy<eastl::string32, char>("hello world", 11);
		EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
		s8.clear();

		// 32 -> 8
		s8 = EA::StdC::Strlcpy<eastl::string8, char32_t>(EA_CHAR32("hello world"), 11);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s32.clear();
	}
	{
		// 8 -> 8
		s8 = EA::StdC::Strlcpy<eastl::string8, char>("hello world", 11);
		EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
		s8.clear();

		// 16 -> 16
		s16 = EA::StdC::Strlcpy<eastl::string16, char16_t>(EA_CHAR16("hello world"), 11);
		EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
		s16.clear();

		// 32 -> 32
		s32 = EA::StdC::Strlcpy<eastl::string32, char32_t>(EA_CHAR32("hello world"), 11);
		EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
		s32.clear();
	}


	// template <typename Dest, typename Source>
	// inline bool Strlcat(Dest& d, const Source& s);
	{
		// 8 -> 16
		s16 = EA_CHAR16("abc ");
		EA::StdC::Strlcat(s16, eastl::string8(EA_CHAR8("hello world")));
		EATEST_VERIFY(s16 == EA_CHAR16("abc hello world"));

		// 16 -> 8
		s8 = EA_CHAR8("abc ");
		EA::StdC::Strlcat(s8, eastl::string16(EA_CHAR16("hello world")));
		EATEST_VERIFY(s8 == EA_CHAR8("abc hello world"));
	}
	{
		// 8 -> 32
		s32 = EA_CHAR32("abc ");
		EA::StdC::Strlcat(s32, eastl::string8(EA_CHAR8("hello world")));
		EATEST_VERIFY(s32 == EA_CHAR32("abc hello world"));

		// 32 -> 8
		s8 = EA_CHAR8("abc ");
		EA::StdC::Strlcat(s8, eastl::string32(EA_CHAR32("hello world")));
		EATEST_VERIFY(s8 == EA_CHAR8("abc hello world"));
	}
	{
		// 8 -> 8
		s8 = EA_CHAR8("abc ");
		EA::StdC::Strlcat(s8, eastl::string8(EA_CHAR8("hello world")));
		EATEST_VERIFY(s8 == EA_CHAR8("abc hello world"));
		s8.clear();

		// 16 -> 16
		s16 = EA_CHAR16("abc ");
		EA::StdC::Strlcat(s16, eastl::string16(EA_CHAR16("hello world")));
		EATEST_VERIFY(s16 == EA_CHAR16("abc hello world"));
		s16.clear();

		// 32 -> 32
		s32 = EA_CHAR32("abc ");
		EA::StdC::Strlcat(s32, eastl::string32(EA_CHAR32("hello world")));
		EATEST_VERIFY(s32 == EA_CHAR32("abc hello world"));
		s32.clear();
	}


	// template <typename Dest, typename Source>
	// inline bool Strlcat(Dest& d, const Source* s, size_t sourceLength = (size_t)~0);
	{
		// 8 -> 16
		s16 = EA_CHAR16("abc ");
		EA::StdC::Strlcat(s16, "hello world", 11);
		EATEST_VERIFY(s16 == EA_CHAR16("abc hello world"));

		// 16 -> 8
		s8 = EA_CHAR8("abc ");
		EA::StdC::Strlcat(s8, EA_CHAR16("hello world"), 11);
		EATEST_VERIFY(s8 == EA_CHAR8("abc hello world"));
	}
	{
		// 8 -> 32
		s32 = EA_CHAR32("abc ");
		EA::StdC::Strlcat(s32, "hello world", 11);
		EATEST_VERIFY(s32 == EA_CHAR32("abc hello world"));

		// 32 -> 8
		s8 = EA_CHAR8("abc ");
		EA::StdC::Strlcat(s8, EA_CHAR32("hello world"), 11);
		EATEST_VERIFY(s8 == EA_CHAR8("abc hello world"));
	}
	{
		// 8 -> 8
		s8 = EA_CHAR8("abc ");
		EA::StdC::Strlcat(s8, "hello world", 11);
		EATEST_VERIFY(s8 == EA_CHAR8("abc hello world"));
		s8.clear();

		// 16 -> 16
		s16 = EA_CHAR16("abc ");
		EA::StdC::Strlcat(s16, EA_CHAR16("hello world"), 11);
		EATEST_VERIFY(s16 == EA_CHAR16("abc hello world"));
		s16.clear();

		// 32 -> 32
		s32 = EA_CHAR32("abc ");
		EA::StdC::Strlcat(s32, EA_CHAR32("hello world"), 11);
		EATEST_VERIFY(s32 == EA_CHAR32("abc hello world"));
		s32.clear();
	}


	{ // ConvertString is a deprecated name for Strlcpy.
		// template <typename Source, typename Dest>
		// inline bool ConvertString(const Source& s, Dest& d);
		{
			// 8 -> 16
			s8 = EA_CHAR8("hello world");
			EA::StdC::ConvertString(s8, s16);
			EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
			s8.clear();

			// 16 -> 8
			EA::StdC::ConvertString(s16, s8);
			EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
			s16.clear();
		}
		{
			// 8 -> 32
			EA::StdC::ConvertString(s8, s32);
			EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
			s8.clear();

			// 32 -> 8
			EA::StdC::ConvertString(s32, s8);
			EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
			s32.clear();
		}


		// template <typename Source, typename Dest>
		// inline Dest ConvertString(const Source& s);
		{
			// 8 -> 16
			s16 = EA::StdC::ConvertString<eastl::string8,  eastl::string16>(s8);
			EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
			s8.clear();

			// 16 -> 8
			s8 = EA::StdC::ConvertString<eastl::string16, eastl::string8>(s16);
			EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
			s16.clear();
		}
		{
			// 8 -> 32
			s32 = EA::StdC::ConvertString<eastl::string8,  eastl::string32>(s8);
			EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
			s8.clear();

			// 32 -> 8
			s8 = EA::StdC::ConvertString<eastl::string32, eastl::string8>(s32);
			EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
			s32.clear();
		}

		{
			// 8 -> 8
			s8 = EA::StdC::ConvertString<eastl::string8, eastl::string8>(eastl::string8(EA_CHAR8("hello world")));
			EATEST_VERIFY(s8 == EA_CHAR8("hello world"));
			s8.clear();

			// 16 -> 16
			s16 = EA::StdC::ConvertString<eastl::string16, eastl::string16>(eastl::string16(EA_CHAR16("hello world")));
			EATEST_VERIFY(s16 == EA_CHAR16("hello world"));
			s16.clear();

			// 32 -> 32
			s32 = EA::StdC::ConvertString<eastl::string32, eastl::string32>(eastl::string32(EA_CHAR32("hello world")));
			EATEST_VERIFY(s32 == EA_CHAR32("hello world"));
			s32.clear();
		}

		{   //Regression for user-reported problem.
			const auto* pPath8 = EA_CHAR8("/abc/def/ghi/jkl");
			eastl::string16 path16 = EA::StdC::ConvertString<eastl::string8, eastl::string16>(pPath8);
			EATEST_VERIFY((path16.length() == EA::StdC::Strlen(pPath8)));
		}
	}

	return nErrorCount;
}

template <class charType>
int TestStrstripString(const charType* input, const charType* expectedOutput)
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	const size_t strLen = EA::StdC::Strlen(input) + 1;
	const size_t allocSizeInBytes = (strLen) * sizeof(charType);

	charType* str = static_cast<charType*>(malloc(allocSizeInBytes));
	Memclear(str, allocSizeInBytes);
	Strncpy(str, input, strLen);
	EATEST_VERIFY(Strcmp(Strstrip(str), expectedOutput) == 0);
	free(str);

	return nErrorCount;
}

static int TestStrstrip()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	nErrorCount += TestStrstripString("", "");
	nErrorCount += TestStrstripString(" ", "");
	nErrorCount += TestStrstripString("  ", "");
	nErrorCount += TestStrstripString("        ", "");
	nErrorCount += TestStrstripString("a", "a");
	nErrorCount += TestStrstripString("a ", "a");
	nErrorCount += TestStrstripString("a      ", "a");
	nErrorCount += TestStrstripString(" a", "a");
	nErrorCount += TestStrstripString("     a", "a");
	nErrorCount += TestStrstripString(" a ", "a");
	nErrorCount += TestStrstripString("  a  ", "a");
	nErrorCount += TestStrstripString("        a ", "a");
	nErrorCount += TestStrstripString(" a      ", "a");
	nErrorCount += TestStrstripString("a b", "a b");
	nErrorCount += TestStrstripString("a    b", "a    b");
	nErrorCount += TestStrstripString(" a b", "a b");
	nErrorCount += TestStrstripString("      a b", "a b");
	nErrorCount += TestStrstripString("      a b ", "a b");
	nErrorCount += TestStrstripString("a b ", "a b");
	nErrorCount += TestStrstripString("a b    ", "a b");
	nErrorCount += TestStrstripString(" a b    ", "a b");
	nErrorCount += TestStrstripString(" a b ", "a b");
	nErrorCount += TestStrstripString(" a     b ", "a     b");
	nErrorCount += TestStrstripString("    a     b   ", "a     b");
	nErrorCount += TestStrstripString("    a     b   ", "a     b");
	nErrorCount += TestStrstripString("ab", "ab");
	nErrorCount += TestStrstripString("abcdefghiklmnop", "abcdefghiklmnop");
	nErrorCount += TestStrstripString("a b c d e f g h i k l m n o p", "a b c d e f g h i k l m n o p");
	nErrorCount += TestStrstripString(" a b c d e f g h i k l m n o p ", "a b c d e f g h i k l m n o p");
	nErrorCount += TestStrstripString("    abcdefg  hiklmnop    ", "abcdefg  hiklmnop");
	nErrorCount += TestStrstripString("    abcdefg  hiklmnop", "abcdefg  hiklmnop");
	nErrorCount += TestStrstripString("abcdefg  hiklmnop    ", "abcdefg  hiklmnop");
	nErrorCount += TestStrstripString("abcdefg  hiklmnop", "abcdefg  hiklmnop");
	nErrorCount += TestStrstripString(" \t \r\n \n \t \r\n \n ", "");
	nErrorCount += TestStrstripString("\t \r\n \na b\t \r\n \n", "a b");
	nErrorCount += TestStrstripString("\t a\r\n \n \t \r\nb \n", "a\r\n \n \t \r\nb");
	nErrorCount += TestStrstripString(" \t \r\n \na \t \r\n \n ", "a");

	nErrorCount += TestStrstripString(EA_CHAR16(""), EA_CHAR16(""));
	nErrorCount += TestStrstripString(EA_CHAR16(" "), EA_CHAR16(""));
	nErrorCount += TestStrstripString(EA_CHAR16("  "), EA_CHAR16(""));
	nErrorCount += TestStrstripString(EA_CHAR16("        "), EA_CHAR16(""));
	nErrorCount += TestStrstripString(EA_CHAR16("a"), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16("a "), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16("a      "), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16(" a"), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16("     a"), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16(" a "), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16("  a  "), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16("        a "), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16(" a      "), EA_CHAR16("a"));
	nErrorCount += TestStrstripString(EA_CHAR16("a b"), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16("a    b"), EA_CHAR16("a    b"));
	nErrorCount += TestStrstripString(EA_CHAR16(" a b"), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16("      a b"), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16("      a b "), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16("a b "), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16("a b    "), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16(" a b    "), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16(" a b "), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16(" a     b "), EA_CHAR16("a     b"));
	nErrorCount += TestStrstripString(EA_CHAR16("    a     b   "), EA_CHAR16("a     b"));
	nErrorCount += TestStrstripString(EA_CHAR16("    a     b   "), EA_CHAR16("a     b"));
	nErrorCount += TestStrstripString(EA_CHAR16("ab"), EA_CHAR16("ab"));
	nErrorCount += TestStrstripString(EA_CHAR16("abcdefghiklmnop"), EA_CHAR16("abcdefghiklmnop"));
	nErrorCount += TestStrstripString(EA_CHAR16("a b c d e f g h i k l m n o p"), EA_CHAR16("a b c d e f g h i k l m n o p"));
	nErrorCount += TestStrstripString(EA_CHAR16(" a b c d e f g h i k l m n o p "), EA_CHAR16("a b c d e f g h i k l m n o p"));
	nErrorCount += TestStrstripString(EA_CHAR16("    abcdefg  hiklmnop    "), EA_CHAR16("abcdefg  hiklmnop"));
	nErrorCount += TestStrstripString(EA_CHAR16("abcdefg  hiklmnop"), EA_CHAR16("abcdefg  hiklmnop"));
	nErrorCount += TestStrstripString(EA_CHAR16(" \t \r\n \n \t \r\n \n "), EA_CHAR16(""));
	nErrorCount += TestStrstripString(EA_CHAR16("\t \r\n \na b\t \r\n \n"), EA_CHAR16("a b"));
	nErrorCount += TestStrstripString(EA_CHAR16("\t a\r\n \n \t \r\nb \n"), EA_CHAR16("a\r\n \n \t \r\nb"));
	nErrorCount += TestStrstripString(EA_CHAR16(" \t \r\n \na \t \r\n \n "), EA_CHAR16("a"));

	nErrorCount += TestStrstripString(EA_CHAR32(""), EA_CHAR32(""));
	nErrorCount += TestStrstripString(EA_CHAR32(" "), EA_CHAR32(""));
	nErrorCount += TestStrstripString(EA_CHAR32("  "), EA_CHAR32(""));
	nErrorCount += TestStrstripString(EA_CHAR32("        "), EA_CHAR32(""));
	nErrorCount += TestStrstripString(EA_CHAR32("a"), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32("a "), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32("a      "), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32(" a"), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32("     a"), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32(" a "), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32("  a  "), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32("        a "), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32(" a      "), EA_CHAR32("a"));
	nErrorCount += TestStrstripString(EA_CHAR32("a b"), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32("a    b"), EA_CHAR32("a    b"));
	nErrorCount += TestStrstripString(EA_CHAR32(" a b"), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32("      a b"), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32("      a b "), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32("a b "), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32("a b    "), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32(" a b    "), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32(" a b "), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32(" a     b "), EA_CHAR32("a     b"));
	nErrorCount += TestStrstripString(EA_CHAR32("    a     b   "), EA_CHAR32("a     b"));
	nErrorCount += TestStrstripString(EA_CHAR32("ab"), EA_CHAR32("ab"));
	nErrorCount += TestStrstripString(EA_CHAR32("abcdefghiklmnop"), EA_CHAR32("abcdefghiklmnop"));
	nErrorCount += TestStrstripString(EA_CHAR32("a b c d e f g h i k l m n o p"), EA_CHAR32("a b c d e f g h i k l m n o p"));
	nErrorCount += TestStrstripString(EA_CHAR32(" a b c d e f g h i k l m n o p "), EA_CHAR32("a b c d e f g h i k l m n o p"));
	nErrorCount += TestStrstripString(EA_CHAR32("    abcdefg  hiklmnop    "), EA_CHAR32("abcdefg  hiklmnop"));
	nErrorCount += TestStrstripString(EA_CHAR32("    abcdefg  hiklmnop"), EA_CHAR32("abcdefg  hiklmnop"));
	nErrorCount += TestStrstripString(EA_CHAR32("abcdefg  hiklmnop    "), EA_CHAR32("abcdefg  hiklmnop"));
	nErrorCount += TestStrstripString(EA_CHAR32("abcdefg  hiklmnop"), EA_CHAR32("abcdefg  hiklmnop"));
	nErrorCount += TestStrstripString(EA_CHAR32(" \t \r\n \n \t \r\n \n "), EA_CHAR32(""));
	nErrorCount += TestStrstripString(EA_CHAR32("\t \r\n \na b\t \r\n \n"), EA_CHAR32("a b"));
	nErrorCount += TestStrstripString(EA_CHAR32("\t a\r\n \n \t \r\nb \n"), EA_CHAR32("a\r\n \n \t \r\nb"));
	nErrorCount += TestStrstripString(EA_CHAR32(" \t \r\n \na \t \r\n \n "), EA_CHAR32("a"));

	return nErrorCount;
}



static int TestStrstart()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// char
	EATEST_VERIFY( Strstart("",       ""));
	EATEST_VERIFY( Strstart("a",      ""));
	EATEST_VERIFY(!Strstart("",       "a"));
	EATEST_VERIFY( Strstart("abcdef", "a"));
	EATEST_VERIFY(!Strstart("abcdef", "A"));
	EATEST_VERIFY( Strstart("abcdef", "abcdef"));
	EATEST_VERIFY(!Strstart("abcdef", "bcdef "));
	EATEST_VERIFY(!Strstart("abcdef", "Abcdef"));

	EATEST_VERIFY( Stristart("",       ""));
	EATEST_VERIFY( Stristart("a",      ""));
	EATEST_VERIFY(!Stristart("",       "a"));
	EATEST_VERIFY( Stristart("abcdef", "a"));
	EATEST_VERIFY( Stristart("abcdef", "A"));
	EATEST_VERIFY( Stristart("abcdef", "abcdef"));
	EATEST_VERIFY(!Stristart("abcdef", "bcdef "));
	EATEST_VERIFY( Stristart("abcdef", "Abcdef"));

	// char16_t
	EATEST_VERIFY( Strstart(EA_CHAR16(""),       EA_CHAR16("")));
	EATEST_VERIFY( Strstart(EA_CHAR16("a"),      EA_CHAR16("")));
	EATEST_VERIFY(!Strstart(EA_CHAR16(""),       EA_CHAR16("a")));
	EATEST_VERIFY( Strstart(EA_CHAR16("abcdef"), EA_CHAR16("a")));
	EATEST_VERIFY(!Strstart(EA_CHAR16("abcdef"), EA_CHAR16("A")));
	EATEST_VERIFY( Strstart(EA_CHAR16("abcdef"), EA_CHAR16("abcdef")));
	EATEST_VERIFY(!Strstart(EA_CHAR16("abcdef"), EA_CHAR16("bcdef ")));
	EATEST_VERIFY(!Strstart(EA_CHAR16("abcdef"), EA_CHAR16("Abcdef")));

	EATEST_VERIFY( Stristart(EA_CHAR16(""),       EA_CHAR16("")));
	EATEST_VERIFY( Stristart(EA_CHAR16("a"),      EA_CHAR16("")));
	EATEST_VERIFY(!Stristart(EA_CHAR16(""),       EA_CHAR16("a")));
	EATEST_VERIFY( Stristart(EA_CHAR16("abcdef"), EA_CHAR16("a")));
	EATEST_VERIFY( Stristart(EA_CHAR16("abcdef"), EA_CHAR16("A")));
	EATEST_VERIFY( Stristart(EA_CHAR16("abcdef"), EA_CHAR16("abcdef")));
	EATEST_VERIFY(!Stristart(EA_CHAR16("abcdef"), EA_CHAR16("bcdef ")));
	EATEST_VERIFY( Stristart(EA_CHAR16("abcdef"), EA_CHAR16("Abcdef")));

	// char32_t
	EATEST_VERIFY( Strstart(EA_CHAR32(""),       EA_CHAR32("")));
	EATEST_VERIFY( Strstart(EA_CHAR32("a"),      EA_CHAR32("")));
	EATEST_VERIFY(!Strstart(EA_CHAR32(""),       EA_CHAR32("a")));
	EATEST_VERIFY( Strstart(EA_CHAR32("abcdef"), EA_CHAR32("a")));
	EATEST_VERIFY(!Strstart(EA_CHAR32("abcdef"), EA_CHAR32("A")));
	EATEST_VERIFY( Strstart(EA_CHAR32("abcdef"), EA_CHAR32("abcdef")));
	EATEST_VERIFY(!Strstart(EA_CHAR32("abcdef"), EA_CHAR32("bcdef ")));
	EATEST_VERIFY(!Strstart(EA_CHAR32("abcdef"), EA_CHAR32("Abcdef")));

	EATEST_VERIFY( Stristart(EA_CHAR32(""),       EA_CHAR32("")));
	EATEST_VERIFY( Stristart(EA_CHAR32("a"),      EA_CHAR32("")));
	EATEST_VERIFY(!Stristart(EA_CHAR32(""),       EA_CHAR32("a")));
	EATEST_VERIFY( Stristart(EA_CHAR32("abcdef"), EA_CHAR32("a")));
	EATEST_VERIFY( Stristart(EA_CHAR32("abcdef"), EA_CHAR32("A")));
	EATEST_VERIFY( Stristart(EA_CHAR32("abcdef"), EA_CHAR32("abcdef")));
	EATEST_VERIFY(!Stristart(EA_CHAR32("abcdef"), EA_CHAR32("bcdef ")));
	EATEST_VERIFY( Stristart(EA_CHAR32("abcdef"), EA_CHAR32("Abcdef")));

	return nErrorCount;
}



static int TestStrend()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// char
	EATEST_VERIFY( Strend("",       ""));
	EATEST_VERIFY( Strend("f",      ""));
	EATEST_VERIFY(!Strend("",       "f"));
	EATEST_VERIFY( Strend("abcdef", "f"));
	EATEST_VERIFY(!Strend("abcdef", "F"));
	EATEST_VERIFY( Strend("abcdef", "abcdef"));
	EATEST_VERIFY(!Strend("abcdef", "abcde "));
	EATEST_VERIFY(!Strend("abcdef", " abcde"));
	EATEST_VERIFY(!Strend("abcdef", "Abcdef"));
	EATEST_VERIFY(!Strend("abcdef", "abcdefghi"));

	EATEST_VERIFY( Striend("",       ""));
	EATEST_VERIFY( Striend("f",      ""));
	EATEST_VERIFY(!Striend("",       "f"));
	EATEST_VERIFY( Striend("abcdef", "f"));
	EATEST_VERIFY( Striend("abcdef", "F"));
	EATEST_VERIFY( Striend("abcdef", "abcdef"));
	EATEST_VERIFY(!Striend("abcdef", "abcde "));
	EATEST_VERIFY(!Striend("abcdef", " abcde"));
	EATEST_VERIFY( Striend("abcdef", "Abcdef"));
	EATEST_VERIFY(!Striend("abcdef", "abcdefghi"));

	// char16_t
	EATEST_VERIFY( Strend(EA_CHAR16(""),       EA_CHAR16("")));
	EATEST_VERIFY( Strend(EA_CHAR16("f"),      EA_CHAR16("")));
	EATEST_VERIFY(!Strend(EA_CHAR16(""),       EA_CHAR16("f")));
	EATEST_VERIFY( Strend(EA_CHAR16("abcdef"), EA_CHAR16("f")));
	EATEST_VERIFY(!Strend(EA_CHAR16("abcdef"), EA_CHAR16("F")));
	EATEST_VERIFY( Strend(EA_CHAR16("abcdef"), EA_CHAR16("abcdef")));
	EATEST_VERIFY(!Strend(EA_CHAR16("abcdef"), EA_CHAR16("abcde ")));
	EATEST_VERIFY(!Strend(EA_CHAR16("abcdef"), EA_CHAR16(" abcde")));
	EATEST_VERIFY(!Strend(EA_CHAR16("abcdef"), EA_CHAR16("Abcdef")));
	EATEST_VERIFY(!Strend(EA_CHAR16("abcdef"), EA_CHAR16("abcdefghi")));

	EATEST_VERIFY( Striend(EA_CHAR16(""),       EA_CHAR16("")));
	EATEST_VERIFY( Striend(EA_CHAR16("f"),      EA_CHAR16("")));
	EATEST_VERIFY(!Striend(EA_CHAR16(""),       EA_CHAR16("f")));
	EATEST_VERIFY( Striend(EA_CHAR16("abcdef"), EA_CHAR16("f")));
	EATEST_VERIFY( Striend(EA_CHAR16("abcdef"), EA_CHAR16("F")));
	EATEST_VERIFY( Striend(EA_CHAR16("abcdef"), EA_CHAR16("abcdef")));
	EATEST_VERIFY(!Striend(EA_CHAR16("abcdef"), EA_CHAR16("abcde ")));
	EATEST_VERIFY(!Striend(EA_CHAR16("abcdef"), EA_CHAR16(" abcde")));
	EATEST_VERIFY( Striend(EA_CHAR16("abcdef"), EA_CHAR16("Abcdef")));
	EATEST_VERIFY(!Striend(EA_CHAR16("abcdef"), EA_CHAR16("abcdefghi")));

	// char32_t
	EATEST_VERIFY( Strend(EA_CHAR32(""),       EA_CHAR32("")));
	EATEST_VERIFY( Strend(EA_CHAR32("f"),      EA_CHAR32("")));
	EATEST_VERIFY(!Strend(EA_CHAR32(""),       EA_CHAR32("f")));
	EATEST_VERIFY( Strend(EA_CHAR32("abcdef"), EA_CHAR32("f")));
	EATEST_VERIFY(!Strend(EA_CHAR32("abcdef"), EA_CHAR32("F")));
	EATEST_VERIFY( Strend(EA_CHAR32("abcdef"), EA_CHAR32("abcdef")));
	EATEST_VERIFY(!Strend(EA_CHAR32("abcdef"), EA_CHAR32("abcde ")));
	EATEST_VERIFY(!Strend(EA_CHAR32("abcdef"), EA_CHAR32(" abcde")));
	EATEST_VERIFY(!Strend(EA_CHAR32("abcdef"), EA_CHAR32("Abcdef")));
	EATEST_VERIFY(!Strend(EA_CHAR32("abcdef"), EA_CHAR32("abcdefghi")));

	EATEST_VERIFY( Striend(EA_CHAR32(""),       EA_CHAR32("")));
	EATEST_VERIFY( Striend(EA_CHAR32("f"),      EA_CHAR32("")));
	EATEST_VERIFY(!Striend(EA_CHAR32(""),       EA_CHAR32("f")));
	EATEST_VERIFY( Striend(EA_CHAR32("abcdef"), EA_CHAR32("f")));
	EATEST_VERIFY( Striend(EA_CHAR32("abcdef"), EA_CHAR32("F")));
	EATEST_VERIFY( Striend(EA_CHAR32("abcdef"), EA_CHAR32("abcdef")));
	EATEST_VERIFY(!Striend(EA_CHAR32("abcdef"), EA_CHAR32("abcde ")));
	EATEST_VERIFY(!Striend(EA_CHAR32("abcdef"), EA_CHAR32(" abcde")));
	EATEST_VERIFY( Striend(EA_CHAR32("abcdef"), EA_CHAR32("Abcdef")));
	EATEST_VERIFY(!Striend(EA_CHAR32("abcdef"), EA_CHAR32("abcdefghi")));

	return nErrorCount;
}



int TestString()
{
	int nErrorCount = 0;

	// Disable assertions, because we will be explicitly testing the failure 
	// modes of some of the functions here. And we don't want the tests to 
	// abort due to assertion failures.
	bool assertionsEnabled = EA::StdC::GetAssertionsEnabled();
	EA::StdC::SetAssertionsEnabled(false);

	nErrorCount += TestStrtoi();
	nErrorCount += TestAtof();
	nErrorCount += TestFtoa();
	nErrorCount += TestReduceFloatString();
	nErrorCount += TestConvertString();
	nErrorCount += TestStringCore();    
	nErrorCount += TestEcvt();
	nErrorCount += TestItoa();
	nErrorCount += TestStrtod();
	nErrorCount += TestStrstrip();
	nErrorCount += TestStrstart();
	nErrorCount += TestStrend();
	
	EA::StdC::SetAssertionsEnabled(assertionsEnabled);

	return nErrorCount;
}


#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


