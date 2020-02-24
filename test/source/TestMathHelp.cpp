///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAStdC/internal/Config.h>
#include <EAStdC/EAMathHelp.h>
#include <EAStdC/EARandom.h>
#include <EAStdC/EABitTricks.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>
#include <EASTL/string.h>
#include <EAAssert/eaassert.h>
#include <EASTL/type_traits.h>

EA_DISABLE_ALL_VC_WARNINGS()
#include <float.h>
#include <stdio.h>
#include <math.h>
EA_RESTORE_ALL_VC_WARNINGS()


using namespace EA::StdC;



template<class T, class U, size_t count>
bool TestArray(const char *testname, T (&values)[count], U (*testFunc)(T), U (*referenceFunc)(T), bool round_adjust = false, bool unit_only = false)
{
	typedef U tResult;

	const bool isUnsigned = eastl::is_unsigned_v<U>;

	for(size_t i = 0; i < count; ++i)
	{
		const T& v = values[i];

		if (isUnsigned && (v < 0)) // prevent UB from converting floats outside the valid range of [-1, FLOAT_MAX) to uin32_t.
			continue;

		if (unit_only && (v < 0 || v > 1))
			continue;

		const tResult testResult = testFunc(v);
		const tResult refResult  = referenceFunc(v);

		if (testResult != refResult)
		{
			// If we are testing a rounding function, account for differences between
			// always-up and round-to-nearest-even
			if (round_adjust)
			{
				if (v == ((int32_t)v + ((v < 0) ? -0.5f : +0.5f)))
				{
					if (refResult == testFunc(v - 0.01f) || 
						refResult == testFunc(v + 0.01f))
					{
						continue;
					}
				}
			}

			EA::UnitTest::Report("    Function \"%s\" FAILED at index %d\n", testname, (int)i);
			EA::UnitTest::Report("        input[i]:       %s\n", eastl::to_string(v).c_str());
			EA::UnitTest::Report("        test function:  %s\n", eastl::to_string(testResult).c_str()); 
			EA::UnitTest::Report("        reference:      %s\n", eastl::to_string(refResult).c_str()); 

			return false;
		}
	}

	// EA::UnitTest::Report("    Function \"%s\" passed.\n", testname);
	return true;
}



///////////////////////////////////////////////////////////////////
// reference implementations
//
EA_NO_UBSAN static uint32_t ref_RoundToUint32(float32_t v)
{
	return static_cast<uint32_t>(floorf(v + 0.5f));
}


static int32_t ref_FloorToInt32(float32_t v)
{
	return (int32_t)floorf(v);
}


static int32_t ref_CeilToInt32(float32_t v)
{
	return (int32_t)ceilf(v);
}


static int32_t ref_RoundToInt32(float32_t v)
{
	return (int32_t)floorf(v + 0.5f);
}


static int32_t ref_TruncateToInt32(float32_t v)
{
	return (int32_t)v;
}


EA_NO_UBSAN static uint8_t ref_UnitFloatToUint8(float fValue)
{
	return (uint8_t)floorf((fValue * 255.0f) + 0.5f);
}

static uint8_t ref_ClampUnitFloatToUint8(float fValue)
{
	if (fValue < 0.0f)
		fValue = 0.0f;
	if (fValue > 1.0f)
		fValue = 1.0f;

	return (uint8_t)floorf((fValue * 255.0f) + 0.5f);
}


///////////////////////////////////////////////////////////////////
// tests
//
#if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_WINDOWS)
	static bool IsFPUModePP()
	{
		return (_controlfp(0, 0) & _MCW_PC) == _PC_24;
	}
#else
	static bool IsFPUModePP()
	{
		return false;
	}
#endif

static int32_t BitReprOfFloat(float f)
{
	union { float f; int32_t i; } typepun;
	typepun.f = f;
	return typepun.i;
}

static int TestMathHelpConversions(const char* /*testmode*/)
{
	int nErrorCount(0);


	/////////////////////////////////////////////////////////////////////
	// Zero tests
	//
	// Zero must convert to zero, exactly.
	//

	static float zerosource[2] = { 0, -sqrtf(0) };

	// EA::UnitTest::Report("\nZero tests (%s):\n", testmode);

	nErrorCount += !TestArray("RoundToUint32",          zerosource, RoundToUint32,          ref_RoundToUint32);
	nErrorCount += !TestArray("RoundToInt32",           zerosource, RoundToInt32,           ref_RoundToInt32);
	nErrorCount += !TestArray("FloorToInt32",           zerosource, FloorToInt32,           ref_FloorToInt32);
	nErrorCount += !TestArray("CeilToInt32",            zerosource, CeilToInt32,            ref_CeilToInt32);
	nErrorCount += !TestArray("TruncateToInt32",        zerosource, TruncateToInt32,        ref_TruncateToInt32);
	nErrorCount += !TestArray("FastRoundToInt23",       zerosource, FastRoundToInt23,       ref_RoundToInt32);
	nErrorCount += !TestArray("UnitFloatToUint8",       zerosource, UnitFloatToUint8,       ref_UnitFloatToUint8);
	nErrorCount += !TestArray("ClampUnitFloatToUint8",  zerosource, ClampUnitFloatToUint8,  ref_ClampUnitFloatToUint8);


	/////////////////////////////////////////////////////////////////////
	// Square root ramp tests
	//
	// These are designed to catch basic errors. One nasty test case is
	// negative zero -- this is known to trip EAMath's IntRound().
	//

	static float source[3072];

	for(int i = 0; i < 256; ++i)
	{ 
		source[i*12+ 8] = source[i*12+4] = source[i*12+0] =  sqrtf((float)i);
		source[i*12+ 9] = source[i*12+5] = source[i*12+1] =  sqrtf((float)i + 0.5f);
		source[i*12+10] = source[i*12+6] = source[i*12+2] = -sqrtf((float)i);
		source[i*12+11] = source[i*12+7] = source[i*12+3] = -sqrtf((float)i + 0.5f);

		for(int j=0; j<4; ++j)
		{
			int32_t v1 = BitReprOfFloat(source[(i * 12) + 4 + j]);
			int32_t v2 = BitReprOfFloat(source[(i * 12) + 8 + j]);

			if (v1 & 0x7fffffff)
				--v1;
			if (v2 & 0x7fffffff)
				++v2;
		}
	}

	// EA::UnitTest::Report("\nSquare root tests (%s):\n", testmode);

	nErrorCount += !TestArray("RoundToUint32",          source, RoundToUint32,          ref_RoundToUint32, true);
	nErrorCount += !TestArray("RoundToInt32",           source, RoundToInt32,           ref_RoundToInt32, true);
	nErrorCount += !TestArray("FloorToInt32",           source, FloorToInt32,           ref_FloorToInt32);
	nErrorCount += !TestArray("CeilToInt32",            source, CeilToInt32,            ref_CeilToInt32);
	nErrorCount += !TestArray("TruncateToInt32",        source, TruncateToInt32,        ref_TruncateToInt32);
	nErrorCount += !TestArray("FastRoundToInt23",       source, FastRoundToInt23,       ref_RoundToInt32, true);
	nErrorCount += !TestArray("UnitFloatToUint8",       source, UnitFloatToUint8,       ref_UnitFloatToUint8, true, true);
	nErrorCount += !TestArray("ClampUnitFloatToUint8",  source, ClampUnitFloatToUint8,  ref_ClampUnitFloatToUint8, true);



	/////////////////////////////////////////////////////////////////////
	// Epsilon tests
	//
	// These are designed to catch errors caused by accidentally rounding
	// off significant bits through adjustment arithmetic. RZMathHelp's
	// FastRoundToSint32() fails this test.
	//

	static float epsource[261*6];

	// EA::UnitTest::Report("\nEpsilon tests (%s):\n", testmode);

	for(int i=0; i< 261; ++i)
	{
		int j = i - 256;
		epsource[i*6+0] =  ldexpf(1.0f, j);
		epsource[i*6+1] = -ldexpf(1.0f, j);
		epsource[i*6+2] =  ldexpf(1.0f, j) + 1.0f;
		epsource[i*6+3] = -ldexpf(1.0f, j) + 1.0f;
		epsource[i*6+4] =  ldexpf(1.0f, j) - 1.0f;
		epsource[i*6+5] = -ldexpf(1.0f, j) - 1.0f;
	}

	nErrorCount += !TestArray("RoundToUint32",          epsource, RoundToUint32,            ref_RoundToUint32, true);
	nErrorCount += !TestArray("RoundToInt32",           epsource, RoundToInt32,             ref_RoundToInt32, true);
	nErrorCount += !TestArray("FloorToInt32",           epsource, FloorToInt32,             ref_FloorToInt32);
	nErrorCount += !TestArray("CeilToInt32",            epsource, CeilToInt32,              ref_CeilToInt32);
	nErrorCount += !TestArray("TruncateToInt32",        epsource, TruncateToInt32,          ref_TruncateToInt32);
	nErrorCount += !TestArray("FastRoundToInt23",       epsource, FastRoundToInt23,         ref_RoundToInt32, true);
	nErrorCount += !TestArray("UnitFloatToUint8",       epsource, UnitFloatToUint8,         ref_UnitFloatToUint8, true, true);
	nErrorCount += !TestArray("ClampUnitFloatToUint8",  epsource, ClampUnitFloatToUint8,    ref_ClampUnitFloatToUint8, true);

	static float epsource2[261*4];

	// EA::UnitTest::Report("\nUnsigned epsilon tests (%s):\n", testmode);

	for(int i=0; i<261; ++i)
	{
		int j = i-256;
		epsource[i*4+0] =  ldexpf(1.0f, j) + 2147483647.0f;
		epsource[i*4+1] = -ldexpf(1.0f, j) + 2147483647.0f;
		epsource[i*4+2] =  ldexpf(1.0f, j) + 2147483648.0f;
		epsource[i*4+3] = -ldexpf(1.0f, j) + 2147483648.0f;
	}

	nErrorCount += !TestArray("RoundToUint32", epsource2, RoundToUint32, ref_RoundToUint32, true);



	/////////////////////////////////////////////////////////////////////
	// Range tests
	//
	// These check for internal overflows.
	//

	static float uint32rangesource[2] = { 0x80000000, 0xfffffe00 };     // We cannot use the correct bounds because single precision (24-bit) significands will round them out of range.

	// EA::UnitTest::Report("\nUint32 range tests (%s):\n", testmode);
	nErrorCount += !TestArray("RoundToUint32", uint32rangesource, RoundToUint32, ref_RoundToUint32, true);

	static float int32rangesource[2] = { -0x7fffff00, 0x7fffff00 };     // We cannot use the correct bounds because single precision (24-bit) significands will round them out of range.

	// EA::UnitTest::Report("\nInt32 range tests (%s):\n", testmode);
	nErrorCount += !TestArray("RoundToInt32",       int32rangesource, RoundToInt32,     ref_RoundToInt32, true);
	nErrorCount += !TestArray("FloorToInt32",       int32rangesource, FloorToInt32,     ref_FloorToInt32);
	nErrorCount += !TestArray("CeilToInt32",        int32rangesource, CeilToInt32,      ref_CeilToInt32);
	nErrorCount += !TestArray("TruncateToInt32",    int32rangesource, TruncateToInt32,  ref_TruncateToInt32);

	static float Int23rangesource[2] = { -0x003fffff, 0x003fffff };

	// EA::UnitTest::Report("\nInt23 range tests (%s):\n", testmode);
	nErrorCount += !TestArray("FastRoundToInt23", Int23rangesource, FastRoundToInt23, ref_RoundToInt32, true);

	static float Uint8rangesource[3] = { 0, 128, 255 };

	nErrorCount += !TestArray("UnitFloatToUint8",       Uint8rangesource, UnitFloatToUint8,         ref_UnitFloatToUint8, true);
	nErrorCount += !TestArray("ClampUnitFloatToUint8",  Uint8rangesource, ClampUnitFloatToUint8,    ref_ClampUnitFloatToUint8, true);

	return nErrorCount;
}


static int TestMathHelpDiagnosisFunctions(const char* /*testmode*/)
{
	static const union
	{
		uint32_t   i;
		float32_t f;
	} kFloat32Tests[]={
		{ 0x00000000 } ,         // zero
		{ 0x80000000 } ,         // negative zero
		{ 0x3F800000 } ,         // +1
		{ 0xBF800000 } ,         // -1
		{ 0x00000001 } ,         // denormal
		{ 0x80000001 } ,         // negative denormal
		{ 0x7F800000 } ,         // +Inf
		{ 0xFF800000 } ,         // -Inf
		{ 0x7F800001 } ,         // -SNaN
		{ 0xFF800001 } ,         // +SNaN
		{ 0x7FFFFFFF } ,         // -QNaN
		{ 0xFFFFFFFF } ,         // +QNaN
		{ 0xFFC00000 }           // QNaN indefinite
	};

	static const union 
	{
		uint64_t   i;
		float64_t f;
	} kFloat64Tests[]={
		{ UINT64_C(0x0000000000000000) } ,         // zero
		{ UINT64_C(0x8000000000000000) } ,         // negative zero
		{ UINT64_C(0x3FF0000000000000) } ,         // +1
		{ UINT64_C(0xBFF0000000000000) } ,         // -1
		{ UINT64_C(0x0000000000000001) } ,         // denormal
		{ UINT64_C(0x8000000000000001) } ,         // negative denormal
		{ UINT64_C(0x7FF0000000000000) } ,         // +Inf
		{ UINT64_C(0xFFF0000000000000) } ,         // -Inf
		{ UINT64_C(0x7FF0000000000001) } ,         // -SNaN
		{ UINT64_C(0xFFF0000000000001) } ,         // +SNaN
		{ UINT64_C(0x7FFFFFFFFFFFFFFF) } ,         // -QNaN
		{ UINT64_C(0xFFFFFFFFFFFFFFFF) } ,         // +QNaN
		{ UINT64_C(0xFFF8000000000000) }           // QNaN indefinite
	};

	static const struct {
		bool mDenormal:1;
		bool mInfinite:1;
		bool mNAN:1;
		bool mIndefinite:1;
	} kTestReference[]={
		{ false, false, false, false },    // zero
		{ false, false, false, false },    // negative zero
		{ false, false, false, false },    // +1
		{ false, false, false, false },    // -1
		{ true,  false, false, false },    // denormal
		{ true,  false, false, false },    // negative denormal
		{ false, true,  false, false },    // +Inf
		{ false, true,  false, false },    // -Inf
		{ false, false, true,  false },    // -QNaN
		{ false, false, true,  false },    // +QNaN
		{ false, false, true,  false },    // -SNaN
		{ false, false, true,  false },    // +SNaN
		{ false, false, true,  true  },    // -QNaN (indefinite)
	};

	int nTestsFailed = 0;

	// EA::UnitTest::Report("\nClassification tests:\n");
	for(size_t i=0; i<sizeof kTestReference / sizeof kTestReference[0]; ++i)
	{
		const float32_t f32 = kFloat32Tests[i].f;
		const float64_t f64 = kFloat64Tests[i].f;
		const uint32_t i32 = kFloat32Tests[i].i;
		const uint64_t i64 = kFloat64Tests[i].i;
		const bool isDenormal = kTestReference[i].mDenormal;
		const bool isInfinite = kTestReference[i].mInfinite;
		const bool isNAN = kTestReference[i].mNAN;
		const bool isIndefinite = kTestReference[i].mIndefinite;
		const bool isNormal = !isDenormal && !isInfinite && !isNAN;

		if (IsNormal(f32) != isNormal) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsNormal(float32_t) FAIL: %g (%" PRIx32 ")\n", f32, i32);
		}
		if (IsNormal(f64) != isNormal) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsNormal(float64_t) FAIL: %g (%" PRIx64 ")\n", f64, i64);
		}

		if (IsDenormalized(f32) != isDenormal) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsDenormalized(float32_t) FAIL: %g (%" PRIx32 ")\n", f32, i32);
		}
		if (IsDenormalized(f64) != isDenormal) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsDenormalized(float64_t) FAIL: %g (%" PRIx64 ")\n", f64, i64);
		}

		if (IsIndefinite(f32) != isIndefinite) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsIndefinite(float32_t) FAIL: %g (%" PRIx32 ")\n", f32, i32);
		}
		if (IsIndefinite(f64) != isIndefinite) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsIndefinite(float64_t) FAIL: %g (%" PRIx64")\n", f64, i64);
		}

		if (IsInfinite(f32) != isInfinite) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsInfinite(float32_t) FAIL: %g (%" PRIx32 ")\n", f32, i32);
		}
		if (IsInfinite(f64) != isInfinite) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsInfinite(float64_t) FAIL: %g (%" PRIx64 ")\n", f64, i64);
		}

		if (IsNAN(f32) != isNAN) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsNAN(float32_t) FAIL: %g (%" PRIx32 ")\n", f32, i32);
		}
		if (IsNAN(f64) != isNAN) {
			++nTestsFailed;
			EA::UnitTest::Report("    IsNAN(float64_t) FAIL: %g (%" PRIx64 ")\n", f64, i64);
		}
	}

	return nTestsFailed;
}


static int TestMath()
{
	const char* testmode;

	#if defined(EAMATHHELP_MODE_SSE) && EAMATHHELP_MODE_SSE
		testmode = "SSE";
		IsFPUModePP(); // Call this only to prevent compiler warnings related to non-use.
	#elif defined(EAMATHHELP_MODE_X86ASM) && EAMATHHELP_MODE_X86ASM
		testmode = IsFPUModePP() ? "X86ASM-24" : "X86ASM-53";
	#else
		testmode = IsFPUModePP() ? "scalar-24" : "scalar-53";
	#endif

	return    TestMathHelpConversions(testmode)
			+ TestMathHelpDiagnosisFunctions(testmode);
}


int TestMathHelp()
{
	int nErrorCount = 0;

	nErrorCount += TestMath();

	#if defined(EA_PLATFORM_WINDOWS) // To consider: Enable this for other platforms.
		// Verify that (HUGE_VAL == EA::StdC::kFloat64Infinity). We do this not because
		// it's required by the C standard, but because we have code in EAStdC that 
		// assumes they are equal under VC++. This assumption, if broken, isn't a very 
		// big deal and no user would likely notice it, but it's a fine detail.
		double hugeVal = HUGE_VAL;
		double infVal  = EA::StdC::kFloat64Infinity;

		EATEST_VERIFY(memcmp(&hugeVal, &infVal, sizeof(double)) == 0);
	#endif

	// To do: Make this work under x64 / Win64. 
	#if defined(EA_COMPILER_MSVC) && defined(EA_PLATFORM_WIN32) && !EAMATHHELP_MODE_SSE
		_controlfp(_PC_24, _MCW_PC);
		nErrorCount += TestMath();
		_controlfp(_PC_53, _MCW_PC);
	#endif

	return nErrorCount;
}














