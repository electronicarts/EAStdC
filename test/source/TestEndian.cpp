///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EAEndian.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif



int TestEndian()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	/////////////////////////////////////////////////////////////////////
	// Test Swizzle
	/////////////////////////////////////////////////////////////////////
	{
		uint16_t n16 = 0x0011;
		n16 = Swizzle(n16);

		EATEST_VERIFY(n16 == 0x1100);
	}

	{
		uint32_t n32 = 0x00112233;
		n32 = Swizzle(n32);

		EATEST_VERIFY(n32 == 0x33221100);
	}

	{
		uint64_t n64 = UINT64_C(0x0011223344556677);
		n64 = Swizzle(n64);

		EATEST_VERIFY(n64 == UINT64_C(0x7766554433221100));
	}

	{
		EA::StdC::uint128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		EA::StdC::uint128_t n128Swizzled("0xffeeddccbbaa99887766554433221100", 16);
		EA::StdC::uint128_t n128 = Swizzle(n128Local);

		EATEST_VERIFY(n128 == n128Swizzled);
	}

	{
		int16_t n16 = 0x0011;
		n16 = Swizzle(n16);

		EATEST_VERIFY(n16 == 0x1100);
	}

	{
		uint32_t n32 = 0x00112233;
		n32 = Swizzle(n32);

		EATEST_VERIFY(n32 == 0x33221100);
	}

	{
		uint64_t n64 = UINT64_C(0x0011223344556677);
		n64 = Swizzle(n64);

		EATEST_VERIFY(n64 == UINT64_C(0x7766554433221100));
	}

	{
		EA::StdC::int128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		EA::StdC::int128_t n128Swizzled("0xffeeddccbbaa99887766554433221100", 16);
		EA::StdC::int128_t n128 = Swizzle(n128Local);

		EATEST_VERIFY(n128 == n128Swizzled);
	}

	{
		// void Swizzle(float* pFloat)
		// void Swizzle(double* pDouble)

		float f = 1234.5678f;
		Swizzle(&f);
		// The representation of f at this point is not necessarily a valid floating point representation. That may present a problem
		// for this unit test if the compiler loads the f into a floating point register and then moves it back to memory for the rest
		// of this test, as that could result in the bits no longer being what they were after the above swizzle.
		Swizzle(&f);
		EATEST_VERIFY(f == 1234.5678f);

		double d = 1234.5678;
		Swizzle(&d);
		// The representation of f at this point is not necessarily a valid floating point representation. That may present a problem
		// for this unit test if the compiler loads the f into a floating point register and then moves it back to memory for the rest
		// of this test, as that could result in the bits no longer being what they were after the above swizzle.
		Swizzle(&d);
		EATEST_VERIFY(d == 1234.5678);
	}


	/////////////////////////////////////////////////////////////////////
	// Test ToBigEndian
	/////////////////////////////////////////////////////////////////////
	{
		uint16_t n16Local = 0x0011;
		uint16_t n16 = ToBigEndian(n16Local);
		uint8_t* p16 = (uint8_t*)&n16;

		EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11));
	}

	{
		uint32_t n32Local = 0x00112233;
		uint32_t n32 = ToBigEndian(n32Local);
		uint8_t* p32 = (uint8_t*)&n32;

		EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33));
	}

	{
		uint64_t n64Local = UINT64_C(0x0011223344556677);
		uint64_t n64 = ToBigEndian(n64Local);
		uint8_t* p64 = (uint8_t*)&n64;

		EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) &&
					  (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77));
	}

	{
		EA::StdC::uint128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		EA::StdC::uint128_t n128 = ToBigEndian(n128Local);
		uint8_t*  p128 = (uint8_t*)&n128;

		EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
					  (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
					  (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
					  (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff));
	}  

	{
		int16_t n16Local = 0x0011;
		int16_t n16 = ToBigEndian(n16Local);
		int8_t* p16 = (int8_t*)&n16;

		EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11));
	}

	{
		int32_t n32Local = 0x00112233;
		int32_t n32 = ToBigEndian(n32Local);
		int8_t* p32 = (int8_t*)&n32;

		EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33));
	}

	{
		int64_t n64Local = UINT64_C(0x0011223344556677);
		int64_t n64 = ToBigEndian(n64Local);
		int8_t* p64 = (int8_t*)&n64;

		EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) &&
					  (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77));
	}

	{
		EA::StdC::int128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		EA::StdC::int128_t n128 = ToBigEndian(n128Local);
		uint8_t*  p128 = (uint8_t*)&n128;

		EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
					  (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
					  (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
					  (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff));
	}  


	/////////////////////////////////////////////////////////////////////
	// Test ToLittleEndian
	/////////////////////////////////////////////////////////////////////
	{
		uint16_t n16Local = 0x0011;
		uint16_t n16 = ToLittleEndian(n16Local);
		uint8_t* p16 = (uint8_t*)&n16;

		EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00));
	}

	{
		uint32_t n32Local = 0x00112233;
		uint32_t n32 = ToLittleEndian(n32Local);
		uint8_t* p32 = (uint8_t*)&n32;

		EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00));
	}

	{
		uint64_t n64Local = UINT64_C(0x0011223344556677);
		uint64_t n64 = ToLittleEndian(n64Local);
		uint8_t* p64 = (uint8_t*)&n64;

		EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
					  (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00));
	}

	{
		EA::StdC::uint128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		EA::StdC::uint128_t n128 = ToLittleEndian(n128Local);
		uint8_t*  p128 = (uint8_t*)&n128;

		EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
					  (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
					  (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
					  (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00));
	}

	{
		int16_t n16Local = 0x0011;
		int16_t n16 = ToLittleEndian(n16Local);
		int8_t* p16 = (int8_t*)&n16;

		EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00));
	}

	{
		int32_t n32Local = 0x00112233;
		int32_t n32 = ToLittleEndian(n32Local);
		int8_t* p32 = (int8_t*)&n32;

		EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00));
	}

	{
		int64_t n64Local = UINT64_C(0x0011223344556677);
		int64_t n64 = ToLittleEndian(n64Local);
		int8_t* p64 = (int8_t*)&n64;

		EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
					  (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00));
	}

	{
		EA::StdC::int128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		EA::StdC::int128_t n128 = ToLittleEndian(n128Local);
		uint8_t*  p128 = (uint8_t*)&n128;

		EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
					  (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
					  (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
					  (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00));
	}


	/////////////////////////////////////////////////////////////////////
	// Test ToBigEndianConst
	/////////////////////////////////////////////////////////////////////
	{
		uint16_t n16 = ToBigEndianConst((uint16_t)(0x0011));
		uint8_t* p16 = (uint8_t*)&n16;

		EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11));
	}

	{
		uint32_t n32 = ToBigEndianConst((uint32_t)(0x00112233));
		uint8_t* p32 = (uint8_t*)&n32;

		EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33));
	}

	{
		uint64_t n64 = ToBigEndianConst(UINT64_C(0x0011223344556677));
		uint8_t* p64 = (uint8_t*)&n64;

		EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) &&
					  (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77));
	}

	{
		EA::StdC::uint128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		EA::StdC::uint128_t n128 = ToBigEndianConst(n128Local);
		uint8_t*  p128 = (uint8_t*)&n128;

		EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
					  (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
					  (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
					  (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff));
	}  


	/////////////////////////////////////////////////////////////////////
	// Test ToLittleEndianConst
	/////////////////////////////////////////////////////////////////////
	{
		uint16_t n16 = ToLittleEndian((uint16_t)(0x0011));
		uint8_t* p16 = (uint8_t*)&n16;

		EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00));
	}

	{
		uint32_t n32 = ToLittleEndianConst((uint32_t)(0x00112233));
		uint8_t  p32[4];
		memcpy(p32, &n32, sizeof(n32));

		EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00));
	}

	{
		uint64_t n64 = ToLittleEndianConst(UINT64_C(0x0011223344556677));
		uint8_t  p64[8];
		memcpy(p64, &n64, sizeof(n64));

		EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
					  (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00));  
	}

	{
		EA::StdC::uint128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		EA::StdC::uint128_t n128 = ToLittleEndianConst(n128Local);
		uint8_t*  p128 = (uint8_t*)&n128;

		EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
					  (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
					  (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
					  (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00));
	}  


	/////////////////////////////////////////////////////////////////////
	// Test ReadFromBigEndian
	/////////////////////////////////////////////////////////////////////
	{
		uint8_t  pBuffer[sizeof(uint16_t)] = { 0x00, 0x11 };
		uint16_t n16 = ReadFromBigEndianUint16(pBuffer);
		uint8_t* p16 = (uint8_t*)&n16;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00));
		#else
			EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11));
		#endif
	}

	{
		uint8_t  pBuffer[sizeof(uint32_t)] = { 0x00, 0x11, 0x22, 0x33 };
		uint32_t n32 = ReadFromBigEndianUint32(pBuffer);
		uint8_t* p32 = (uint8_t*)&n32;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00));
		#else
			EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33));
		#endif
	}

	{
		uint8_t  pBuffer[sizeof(uint64_t)] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
		uint64_t n64 = ReadFromBigEndianUint64(pBuffer);
		uint8_t* p64 = (uint8_t*)&n64;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
						  (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00));
		#else
			EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) &&
						  (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77));
		#endif
	}

	{
		int8_t  pBuffer[sizeof(int16_t)] = { 0x00, 0x11 };
		int16_t n16 = ReadFromBigEndianInt16(pBuffer);
		int8_t* p16 = (int8_t*)&n16;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00));
		#else
			EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11));
		#endif
	}

	{
		int8_t  pBuffer[sizeof(int32_t)] = { 0x00, 0x11, 0x22, 0x33 };
		int32_t n32 = ReadFromBigEndianInt32(pBuffer);
		int8_t* p32 = (int8_t*)&n32;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00));
		#else
			EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33));
		#endif
	}

	{
		int8_t  pBuffer[sizeof(int64_t)] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
		int64_t n64 = ReadFromBigEndianInt64(pBuffer);
		int8_t* p64 = (int8_t*)&n64;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
						  (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00));
		#else
			EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) &&
						  (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77));
		#endif
	}

	{
		// This is a bit hard to test fully, as we need to hit a lot of floating point values 
		// to make sure no invalid floating point values are put into FPU registers. We can't 
		// just test random 32 bit memory patterns. With respect to NANs, a swizzled NAN is 
		// converted by the FPU to some other NAN and so we can't do a bit comparison.

		size_t       i;
		char         buffer[32];
		const float  fTestValues[] = { FLT_MIN, FLT_MAX, 0.f, -0.f, 1.f, -1.f };
		const double dTestValues[] = { DBL_MIN, DBL_MAX, 0.f, -0.f, 1.f, -1.f };

		for(i = 0; i < (sizeof(fTestValues) / sizeof(fTestValues[0])); i++)
		{
			WriteToBigEndian(buffer, fTestValues[i]);
			float f = ReadFromBigEndianFloat(buffer);

			EATEST_VERIFY(f == fTestValues[i]);
		}

		for(i = 0; i < (sizeof(dTestValues) / sizeof(dTestValues[0])); i++)
		{
			WriteToBigEndian(buffer, dTestValues[i]);
			double d = ReadFromBigEndianDouble(buffer);

			EATEST_VERIFY(d == dTestValues[i]);
		}
	}

	{
		uint8_t   pBuffer[sizeof(EA::StdC::uint128_t)] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
		EA::StdC::uint128_t n128 = ReadFromBigEndianUint128(pBuffer);
		uint8_t*  p128 = (uint8_t*)&n128;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
						  (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
						  (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
						  (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00));
		#else
			EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
						  (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
						  (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
						  (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff));
		#endif
	}

	{
		uint8_t   pBuffer[sizeof(EA::StdC::int128_t)] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
		EA::StdC::int128_t  n128 = ReadFromBigEndianInt128(pBuffer);
		uint8_t*  p128 = (uint8_t*)&n128;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
						  (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
						  (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
						  (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00));
		#else
			EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
						  (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
						  (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
						  (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff));
		#endif
	}

	{
		// uint32_t ReadFromBigEndian(const void* pSource, int32_t nSourceBytes)

		// To do: Improve this test. Currently it's hardly more than a compile test.
		uint32_t n32, nSource = 0x00112233;
		n32 = ReadFromBigEndian(&nSource, 4);
		EATEST_VERIFY(n32 != 0);
	}


	/////////////////////////////////////////////////////////////////////
	// Test ReadFromLittleEndian
	/////////////////////////////////////////////////////////////////////
	{
		uint8_t  pBuffer[sizeof(uint16_t)] = { 0x00, 0x11 };
		uint16_t n16 = ReadFromLittleEndianUint16(pBuffer);
		uint8_t* p16 = (uint8_t*)&n16;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11));
		#else
			EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00));
		#endif
	}

	{
		uint8_t  pBuffer[sizeof(uint32_t)] = { 0x00, 0x11, 0x22, 0x33 };
		uint32_t n32 = ReadFromLittleEndianUint32(pBuffer);
		uint8_t* p32 = (uint8_t*)&n32;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33));
		#else
			EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00));
		#endif
	}

	{
		uint8_t  pBuffer[sizeof(uint64_t)] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
		uint64_t n64 = ReadFromLittleEndianUint64(pBuffer);
		uint8_t* p64 = (uint8_t*)&n64;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) &&
				   (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77));
		#else
			EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
				   (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00));
		#endif
	}

	{
		int8_t  pBuffer[sizeof(int16_t)] = { 0x00, 0x11 };
		int16_t n16 = ReadFromLittleEndianInt16(pBuffer);
		int8_t* p16 = (int8_t*)&n16;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11));
		#else
			EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00));
		#endif
	}

	{
		int8_t  pBuffer[sizeof(int32_t)] = { 0x00, 0x11, 0x22, 0x33 };
		int32_t n32 = ReadFromLittleEndianInt32(pBuffer);
		int8_t* p32 = (int8_t*)&n32;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33));
		#else
			EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00));
		#endif
	}

	{
		int8_t  pBuffer[sizeof(int64_t)] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
		int64_t n64 = ReadFromLittleEndianInt64(pBuffer);
		int8_t* p64 = (int8_t*)&n64;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) &&
				   (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77));
		#else
			EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
				   (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00));
		#endif
	}

	{
		// This is a bit hard to test fully, as we need to hit a lot of floating point values 
		// to make sure no invalid floating point values are put into FPU registers. We can't 
		// just test random 32 bit memory patterns. With respect to NANs, a swizzled NAN is 
		// converted by the FPU to some other NAN and so we can't do a bit comparison.

		size_t       i;
		char         buffer[32];
		const float  fTestValues[] = { FLT_MIN, FLT_MAX, 0.f, -0.f, 1.f, -1.f };
		const double dTestValues[] = { DBL_MIN, DBL_MAX, 0.f, -0.f, 1.f, -1.f };

		for(i = 0; i < (sizeof(fTestValues) / sizeof(fTestValues[0])); i++)
		{
			WriteToLittleEndian(buffer, fTestValues[i]);
			float f = ReadFromLittleEndianFloat(buffer);

			EATEST_VERIFY(f == fTestValues[i]);
		}

		for(i = 0; i < (sizeof(dTestValues) / sizeof(dTestValues[0])); i++)
		{
			WriteToLittleEndian(buffer, dTestValues[i]);
			double d = ReadFromLittleEndianDouble(buffer);

			EATEST_VERIFY(d == dTestValues[i]);
		}
	}

	{
		uint8_t   pBuffer[sizeof(EA::StdC::uint128_t)] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
		EA::StdC::uint128_t n128 = ReadFromLittleEndianUint128(pBuffer);
		uint8_t*  p128 = (uint8_t*)&n128;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
				   (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
				   (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
				   (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff));
		#else
			EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
				   (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
				   (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
				   (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00));
		#endif
	}

	{
		uint8_t   pBuffer[sizeof(EA::StdC::int128_t)] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
		EA::StdC::int128_t n128 = ReadFromLittleEndianInt128(pBuffer);
		uint8_t*  p128 = (uint8_t*)&n128;

		#ifdef EA_SYSTEM_LITTLE_ENDIAN
			EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
				   (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
				   (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
				   (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff));
		#else
			EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
				   (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
				   (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
				   (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00));
		#endif
	}


	/////////////////////////////////////////////////////////////////////
	// Test WriteToBigEndian
	/////////////////////////////////////////////////////////////////////
	{
		uint8_t p16[sizeof(uint16_t) + 1];
		p16[sizeof(uint16_t)] = 0xfe;
		WriteToBigEndian(p16, (uint16_t)0x0011);

		EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11) &&  p16[sizeof(uint16_t)] == 0xfe);
	}

	{
		uint8_t p32[sizeof(uint32_t) + 1];
		p32[sizeof(uint32_t)] = 0xfe;
		WriteToBigEndian(p32, (uint32_t)0x00112233);

		EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33) && p32[sizeof(uint32_t)] == 0xfe);
	}

	{
		uint8_t p64[sizeof(uint64_t) + 1];
		p64[sizeof(uint64_t)] = 0xfe;
		WriteToBigEndian(p64, UINT64_C(0x0011223344556677));

		EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) && 
					  (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77) && 
					   p64[sizeof(uint64_t)] == 0xfe);
	}

	{
		uint8_t p16[sizeof(int16_t) + 1];
		p16[sizeof(int16_t)] = 0xfe;
		WriteToBigEndian(p16, (int16_t)0x0011);

		EATEST_VERIFY((p16[0] == 0x00) && (p16[1] == 0x11) &&  p16[sizeof(int16_t)] == 0xfe);
	}

	{
		uint8_t p32[sizeof(int32_t) + 1];
		p32[sizeof(int32_t)] = 0xfe;
		WriteToBigEndian(p32, (int32_t)0x00112233);

		EATEST_VERIFY((p32[0] == 0x00) && (p32[1] == 0x11) && (p32[2] == 0x22) && (p32[3] == 0x33) && p32[sizeof(int32_t)] == 0xfe);
	}

	{
		uint8_t p64[sizeof(int64_t) + 1];
		p64[sizeof(int64_t)] = 0xfe;
		WriteToBigEndian(p64, INT64_C(0x0011223344556677));

		EATEST_VERIFY((p64[0] == 0x00) && (p64[1] == 0x11) && (p64[2] == 0x22) && (p64[3] == 0x33) && 
					  (p64[4] == 0x44) && (p64[5] == 0x55) && (p64[6] == 0x66) && (p64[7] == 0x77) && 
					   p64[sizeof(int64_t)] == 0xfe);
	}

	{
		// WriteToBigEndian Float
		// WriteToBigEndian Double
		// These are covered in the ReadFromBigEndian tests.
	}

	{
		uint8_t p128[sizeof(EA::StdC::uint128_t) + 1];
		p128[sizeof(EA::StdC::uint128_t)] = 0xfe;
		EA::StdC::uint128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		WriteToBigEndian(p128, n128Local);

		EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
					  (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
					  (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
					  (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff) &&
					   p128[sizeof(EA::StdC::uint128_t)] == 0xfe);
	}

	{
		uint8_t p128[sizeof(EA::StdC::int128_t) + 1];
		p128[sizeof(EA::StdC::int128_t)] = 0xfe;
		EA::StdC::int128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		WriteToBigEndian(p128, n128Local);

		EATEST_VERIFY((p128[0]  == 0x00) && (p128[1]  == 0x11) && (p128[2]   == 0x22) && (p128[3]  == 0x33) &&
					  (p128[4]  == 0x44) && (p128[5]  == 0x55) && (p128[6]   == 0x66) && (p128[7]  == 0x77) &&
					  (p128[8]  == 0x88) && (p128[9]  == 0x99) && (p128[10]  == 0xaa) && (p128[11] == 0xbb) &&
					  (p128[12] == 0xcc) && (p128[13] == 0xdd) && (p128[14]  == 0xee) && (p128[15] == 0xff) &&
					   p128[sizeof(EA::StdC::int128_t)] == 0xfe);
	}

	{
		// void WriteToBigEndian(const void* pDest, uint32_t data, int32_t nSourceBytes)

		// To do: Improve this test. Currently it's hardly more than a compile test.
		uint32_t n32 = 0, nSource = 0x00112233;
		WriteToBigEndian(&n32, nSource, 4);
		EATEST_VERIFY(n32 != 0);
	}


	/////////////////////////////////////////////////////////////////////
	// Test WriteToLittleEndian
	/////////////////////////////////////////////////////////////////////
	{
		uint8_t p16[sizeof(uint16_t) + 1];
		p16[sizeof(uint16_t)] = 0xfe;
		WriteToLittleEndian(p16, (uint16_t)0x0011);

		EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00) &&  p16[sizeof(uint16_t)] == 0xfe);
	}

	{
		uint8_t p32[sizeof(uint32_t) + 1];
		p32[sizeof(uint32_t)] = 0xfe;
		WriteToLittleEndian(p32, (uint32_t)0x00112233);

		EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00) && p32[sizeof(uint32_t)] == 0xfe);
	}

	{
		uint8_t p64[sizeof(uint64_t) + 1];
		p64[sizeof(uint64_t)] = 0xfe;
		WriteToLittleEndian(p64, UINT64_C(0x0011223344556677));

		EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
					  (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00) &&
					   p64[sizeof(uint64_t)] == 0xfe);
	}

	{
		uint8_t p16[sizeof(int16_t) + 1];
		p16[sizeof(int16_t)] = 0xfe;
		WriteToLittleEndian(p16, (int16_t)0x0011);

		EATEST_VERIFY((p16[0] == 0x11) && (p16[1] == 0x00) &&  p16[sizeof(int16_t)] == 0xfe);
	}

	{
		uint8_t p32[sizeof(int32_t) + 1];
		p32[sizeof(int32_t)] = 0xfe;
		WriteToLittleEndian(p32, (int32_t)0x00112233);

		EATEST_VERIFY((p32[0] == 0x33) && (p32[1] == 0x22) && (p32[2] == 0x11) && (p32[3] == 0x00) && p32[sizeof(int32_t)] == 0xfe);
	}

	{
		uint8_t p64[sizeof(int64_t) + 1];
		p64[sizeof(int64_t)] = 0xfe;
		WriteToLittleEndian(p64, UINT64_C(0x0011223344556677));

		EATEST_VERIFY((p64[0] == 0x77) && (p64[1] == 0x66) && (p64[2] == 0x55) && (p64[3] == 0x44) &&
					  (p64[4] == 0x33) && (p64[5] == 0x22) && (p64[6] == 0x11) && (p64[7] == 0x00) &&
					   p64[sizeof(int64_t)] == 0xfe);
	}

	{
		// To do
		// WriteToLittleEndian Float
		// WriteToLittleEndian Double
		// These are covered in the ReadFromLittleEndian tests.
	}

	{
		uint8_t p128[sizeof(EA::StdC::uint128_t) + 1];
		p128[sizeof(EA::StdC::uint128_t)] = 0xfe;
		EA::StdC::uint128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		WriteToLittleEndian(p128, n128Local);

		EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
					  (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
					  (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
					  (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00) &&
					   p128[sizeof(EA::StdC::uint128_t)] == 0xfe);
	}

	{
		uint8_t p128[sizeof(EA::StdC::int128_t) + 1];
		p128[sizeof(EA::StdC::int128_t)] = 0xfe;
		EA::StdC::int128_t n128Local("0x00112233445566778899aabbccddeeff", 16);
		WriteToLittleEndian(p128, n128Local);

		EATEST_VERIFY((p128[0]  == 0xff) && (p128[1]  == 0xee) && (p128[2]   == 0xdd) && (p128[3]  == 0xcc) &&
					  (p128[4]  == 0xbb) && (p128[5]  == 0xaa) && (p128[6]   == 0x99) && (p128[7]  == 0x88) &&
					  (p128[8]  == 0x77) && (p128[9]  == 0x66) && (p128[10]  == 0x55) && (p128[11] == 0x44) &&
					  (p128[12] == 0x33) && (p128[13] == 0x22) && (p128[14]  == 0x11) && (p128[15] == 0x00) &&
					   p128[sizeof(EA::StdC::int128_t)] == 0xfe);
	}

	return nErrorCount;
}









