///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#if defined(_MSC_VER)
	#pragma warning (disable: 4290) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif


#include <EAStdC/internal/Config.h>
#include <EAStdC/EAStdC.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/version.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>
#include <EATest/EAMissingImpl.inl>
#include <EAMain/EAEntryPointMain.inl>
#include <new>

#if defined(EASTDCTEST_DEFINES_CHAR16)
	char16_t* char16_convert(const wchar_t* pText)
	{
		typedef eastl::map<const wchar_t*, ea_char16_str> WChar16Map; // Map of wchar_t* to char16_t* 

		static WChar16Map sTable; // Not thread safe.
		ea_char16_str s = ea_char16_str(pText);
		eastl::pair<WChar16Map::iterator, bool> result = sTable.insert(eastl::make_pair(pText, s));
		return result.first->second.mpData;
	}

	char16_t char16_convert(wchar_t c)
	{
		return (char16_t)c;
	}
#endif

#if defined(EASTDCTEST_DEFINES_CHAR32)
	char32_t* char32_convert(const wchar_t* pText)
	{
		typedef eastl::map<const wchar_t*, ea_char32_str> WChar32Map; // Map of wchar_t* to char32_t* 

		static WChar32Map sTable; // Not thread safe.
		ea_char32_str s = ea_char32_str(pText);
		eastl::pair<WChar32Map::iterator, bool> result = sTable.insert(eastl::make_pair(pText, s));
		return result.first->second.mpData;
	}
	char32_t char32_convert(wchar_t character)
	{
		return (char32_t)character;
	}
#endif


// EASTDC_STOPWATCH_DISABLE_CPU_CALIBRATION test
//
// It might be useful to test this macro under some circumstances, but doing 
// so (intentionally) causes the CPU cycle-based timing to not work. So we'd 
// have to disable those tests if we want to test this macro.
//
//#if defined(_MSC_VER)
//    #pragma warning(disable: 4074)
//    #pragma init_seg(compiler)
//#endif
//
//EASTDC_STOPWATCH_DISABLE_CPU_CALIBRATION()



///////////////////////////////////////////////////////////////////////////////
// operator new
//
// EASTL requires some of the following new operators to be defined, while 
// various platform SDKs require others.
///////////////////////////////////////////////////////////////////////////////

void* operator new(size_t size, const char*, int, unsigned, const char*, int)
{
	return ::operator new(size);
}

void* operator new[](size_t size, const char*, int, unsigned, const char*, int)
{
	return ::operator new[](size);
}

void* operator new[](size_t size, size_t, size_t, const char*, int, unsigned, const char*, int)
{
	return ::operator new[](size);
}

void* operator new(size_t size, size_t /*alignment*/, size_t /*alignmentOffset*/, const char* /*name*/, 
					int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
	// We will have a problem if alignment is non-default.
	return ::operator new(size);
}


void operator delete(void* p, size_t, size_t, const char*, int, unsigned, const char*, int)
{
	if (p)
		::operator delete(p);
}

void operator delete(void* p, char const*, int, unsigned, char const*, int)
{
	if (p)
		::operator delete(p);
}


#if !defined(EA_STDCTEST_ENABLE_NEW_REPLACEMENT)
	#define EA_STDCTEST_ENABLE_NEW_REPLACEMENT 0
#endif

#if EA_STDCTEST_ENABLE_NEW_REPLACEMENT

	void* operator new (size_t size, const ::std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		return malloc(size);
	}

	void* operator new[] (size_t size, const ::std::nothrow_t&) EA_THROW_SPEC_NEW_NONE()
	{
		return malloc(size);
	}

	void* operator new[] (size_t size) EA_THROW_SPEC_NEW(std::bad_alloc)
	{
		return malloc(size);
	}

	void* operator new (size_t size) EA_THROW_SPEC_NEW(std::bad_alloc)
	{
		return malloc(size);
	}

	void operator delete (void* p) EA_THROW_SPEC_DELETE_NONE()
	{
		if (p)
			free(p);
	}

	void operator delete[] (void* p) EA_THROW_SPEC_DELETE_NONE()
	{
		if (p)
			free(p);
	}

#endif


// On some Sony platforms, you must explicitly set the LibC heap size. Because the EAStdC 
// tests use direct calls to the LibC memory functions, we cannot get around this. 
// This sets the LibC heap size to 128MB.
#if defined(EA_PLATFORM_SONY) 
	size_t sceLibcHeapSize = 128*1024*1024;
#endif

///////////////////////////////////////////////////////////////////////////////
// EAMain
//

int EAMain(int argc, char** argv)
{
	using namespace EA::UnitTest;

	EA::EAMain::PlatformStartup();
	EA::StdC::Init();

	// Add the tests
	TestApplication testSuite("EAStdC Unit Tests", argc, argv);

	testSuite.AddTest("Callback",     TestCallback);
	testSuite.AddTest("String",       TestString);
	testSuite.AddTest("Sprintf",      TestSprintf);
	testSuite.AddTest("Random",       TestRandom);
	testSuite.AddTest("TextUtil",     TestTextUtil);
	testSuite.AddTest("Scanf",        TestScanf);
	testSuite.AddTest("Memory",       TestMemory);
	testSuite.AddTest("Endian",       TestEndian);
	testSuite.AddTest("DateTime",     TestDateTime);
	testSuite.AddTest("Singleton",    TestSingleton);
	testSuite.AddTest("Process",      TestProcess);
	testSuite.AddTest("Stopwatch",    TestStopwatch);
	testSuite.AddTest("CType",        TestCType);
	testSuite.AddTest("BitTricks",    TestBitTricks);
	testSuite.AddTest("Alignment",    TestAlignment);
	testSuite.AddTest("ByteCrackers", TestByteCrackers);
	testSuite.AddTest("FixedPoint",   TestFixedPoint);
	testSuite.AddTest("Hash",         TestHash);
	testSuite.AddTest("Global",       TestGlobal);
	testSuite.AddTest("MathHelp",     TestMathHelp);
	testSuite.AddTest("Int128",       TestInt128);

	// Parse command line arguments
	const int testResult = testSuite.Run();

	EA::StdC::Shutdown();
	EA::EAMain::PlatformShutdown(testResult);
 
	return testResult;
}

