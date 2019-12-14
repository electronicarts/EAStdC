///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EASTDC_EASTDCTEST_H
#define EASTDC_EASTDCTEST_H


EA_DISABLE_ALL_VC_WARNINGS()
EA_DISABLE_VC_WARNING(4530) // C++ exception handler used, but unwind semantics are not enabled.
#include <stddef.h>
EA_RESTORE_VC_WARNING()
EA_RESTORE_ALL_VC_WARNINGS()


int TestCallback();
int TestString();
int TestCType();
int TestBitTricks();
int TestAlignment();
int TestByteCrackers();
int TestDateTime();
int TestEndian();
int TestFixedPoint();
int TestRandom();
int TestHash();
int TestGlobal();
int TestMathHelp();
int TestMemory();
int TestProcess();
int TestScanf();
int TestSingleton();
int TestSprintf();
int TestStopwatch();
int TestTextUtil();
int TestInt128();

extern volatile int gWriteToEnsureFunctionCalled;


///////////////////////////////////////////////////////////////////////////////
// EA_WCHAR / EA_CHAR16 / EA_CHAR32
//
// We define these in the case that EABase doesn't define them so that our 
// tests can exercize them. For example, on Windows EABase doesn't define 
// EA_CHAR32, as it's impossible to do so in an efficient way and would be
// bad for user code. But for our unit tests we don't care about this and 
// it's useful to have it.
///////////////////////////////////////////////////////////////////////////////

// Always defind as L""
// The purpose of this in our unit tests is to make it explicit in 
// the test code that we really mean L""/L'' and not EA_CHAR16 or EA_CHAR32.
#if !defined(EA_WCHAR)
	#define EA_WCHAR(s) L ## s
#endif


#if !defined(EA_CHAR16)

	#include <stdlib.h>
	#include <string.h>
	#include <EASTL/map.h>
	#include <EAStdC/EAString.h>

	// This class has no purpose other than to convert 32 bit wchar_t strings to 16 bit char16_t strings.
	// The code below probably wouldn't compile or make sense unless wchar_t was 32 bits or more.
	class ea_char16_str
	{
	public:
		ea_char16_str() : mpData(NULL), mLength(0) {}
		
		ea_char16_str(const wchar_t* pText) 
		{
			mLength = EA::StdC::Strlen(pText);
			mpData  = (char16_t*)malloc((mLength + 1) * sizeof(char16_t));
			for(size_t i = 0; i < (mLength + 1); ++i)
				mpData[i] = (char16_t)pText[i];
		}
		
		ea_char16_str(const ea_char16_str& str)
		{
			CopyFrom(str);
		}

		ea_char16_str& operator=(const ea_char16_str& str)
		{
			if(&str != this)
			{
				if(mpData)
					free(mpData); 
				CopyFrom(str);
			}
			return *this;
		}

		void CopyFrom(const ea_char16_str& str)
		{
			mLength = str.mLength;
			mpData  = (char16_t*)malloc((mLength + 1) * sizeof(char16_t));
			for(size_t i = 0; i < (mLength + 1); ++i)
				mpData[i] = str.mpData[i];
		}

		~ea_char16_str()
		{
			if(mpData)
				free(mpData); 
		}

		char16_t* mpData;
		size_t    mLength;
	};

	char16_t* char16_convert(const wchar_t* pText);

	char16_t char16_convert(wchar_t c);

	#define EA_CHAR16(s) char16_convert(L ## s)
	#define EASTDCTEST_DEFINES_CHAR16
#endif


#if !defined(EA_CHAR32)

	#include <stdlib.h>
	#include <string.h>
	#include <EASTL/map.h>
	#include <EAStdC/EAString.h>

	// This class has no purpose other than to convert 16 bit wchar_t strings to 32 bit char32_t strings.
	// The code below probably wouldn't compile or make sense unless wchar_t was 16 bits or less.
	class ea_char32_str
	{
	public:
		ea_char32_str() : mpData(NULL), mLength(0) {}
		
		ea_char32_str(const wchar_t* pText) 
		{
			mLength = EA::StdC::Strlen(pText);
			mpData  = (char32_t*)malloc((mLength + 1) * sizeof(char32_t));
			if(mpData)
			{
				for(size_t i = 0; i < (mLength + 1); ++i)
					mpData[i] = (char32_t)pText[i];
			}
		}
		
		ea_char32_str(const ea_char32_str& str)
		{
			CopyFrom(str);
		}

		ea_char32_str& operator=(const ea_char32_str& str)
		{
			if(&str != this)
			{
				if(mpData)
					free(mpData); 
				CopyFrom(str);
			}
			return *this;
		}

		void CopyFrom(const ea_char32_str& str)
		{
			mLength = str.mLength;
			mpData  = (char32_t*)malloc((mLength + 1) * sizeof(char32_t));
			if(mpData)
			{
				for(size_t i = 0; i < (mLength + 1); ++i)
					mpData[i] = str.mpData[i];
			}
		}

		~ea_char32_str()
		{
			if(mpData)
				free(mpData); 
		}

		char32_t* mpData;
		size_t    mLength;
	};

	char32_t* char32_convert(const wchar_t* pText);

	char32_t char32_convert(wchar_t character);

	#define EA_CHAR32(s) char32_convert(L ## s)
	#define EASTDCTEST_DEFINES_CHAR32

#endif

inline double DoubleAbsoluteDifference(double x1, double x2)
{
	return (x1 < x2) ? (x2 - x1) : (x1 - x2);
}

inline bool DoubleEqual(double x1, double x2)
{
	if(x1 < 1e-15)
		return (x2 < 1e-15);
	else if(x2 < 1e-15)
		return (x1 < 1e-15);
	else
		return DoubleAbsoluteDifference((x1 - x2) / x1, 1e-15) < 1e-13;
}

inline double FloatAbsoluteDifference(float x1, float x2)
{
	return (x1 < x2) ? (x2 - x1) : (x1 - x2);
}

inline bool FloatEqual(float x1, float x2)
{
	if(x1 < 1e-7f)
		return (x2 < 1e-7f);
	else if(x2 < 1e-7f)
		return (x1 < 1e-7f);
	else
		return FloatAbsoluteDifference((x1 - x2) / x1, 1e-7f) < 1e-5f;
}

#endif // Header include guard



