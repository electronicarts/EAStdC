///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAStdC/EAHashString.h>
#include <EAStdC/EAHashCRC.h>
#include <EAStdC/EABitTricks.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>


#if defined(_MSC_VER)
	#pragma warning(disable: 6211) // Leaking memory due to an exception. Consider using a local catch block to clean up memory.
#endif


static int TestHashString()
{
	using namespace EA::StdC;

	int nErrorCount(0);
 
	// GCC 2.x fails due to compiler bugs.
	#if (!defined(__GNUC__) || (__GNUC__ >= 3)) 

	{ // Test CTStringHash

		char     stringArray[] = "01234567890123456789012345678901234567890";
		uint32_t nCTHashArray[33];

		nCTHashArray[0]  = CTStringHash<0>::value;
		nCTHashArray[1]  = CTStringHash<'0'>::value;
		nCTHashArray[2]  = CTStringHash<'0', '1'>::value;
		nCTHashArray[3]  = CTStringHash<'0', '1', '2'>::value;
		nCTHashArray[4]  = CTStringHash<'0', '1', '2', '3'>::value;
		nCTHashArray[5]  = CTStringHash<'0', '1', '2', '3', '4'>::value;
		nCTHashArray[6]  = CTStringHash<'0', '1', '2', '3', '4', '5'>::value;
		nCTHashArray[7]  = CTStringHash<'0', '1', '2', '3', '4', '5', '6'>::value;
		nCTHashArray[8]  = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7'>::value;
		nCTHashArray[9]  = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8'>::value;
		nCTHashArray[10] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'>::value;
		nCTHashArray[11] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'>::value;
		nCTHashArray[12] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1'>::value;
		nCTHashArray[13] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2'>::value;
		nCTHashArray[14] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3'>::value;
		nCTHashArray[15] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4'>::value;
		nCTHashArray[16] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5'>::value;
		nCTHashArray[17] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6'>::value;
		nCTHashArray[18] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7'>::value;
		nCTHashArray[19] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8'>::value;
		nCTHashArray[20] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'>::value;
		nCTHashArray[21] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'>::value;
		nCTHashArray[22] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1'>::value;
		nCTHashArray[23] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2'>::value;
		nCTHashArray[24] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3'>::value;
		nCTHashArray[25] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4'>::value;
		nCTHashArray[26] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5'>::value;
		nCTHashArray[27] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6'>::value;
		nCTHashArray[28] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7'>::value;
		nCTHashArray[29] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8'>::value;
		nCTHashArray[30] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'>::value;
		nCTHashArray[31] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'>::value;
		nCTHashArray[32] = CTStringHash<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1'>::value;

		for(int i = 31; i >= 0; i--)
		{
			stringArray[i] = 0;
			const uint32_t nHashFNV1 = FNV1_String8(stringArray);
			EATEST_VERIFY(nHashFNV1 == nCTHashArray[i]);
		}
	}

	#endif

	return nErrorCount;
}



int TestHash()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	const int kDataLength(16384); // We intentionally choose a power of 2.
	EATEST_VERIFY((kDataLength % 8) == 0); // Code below depends on this.

	uint8_t*  pDataA   = new uint8_t[kDataLength];
	uint8_t*  pDataB   = new uint8_t[kDataLength];
	char*  pData8A  = new char[kDataLength];
	char*  pData8B  = new char[kDataLength];
	char16_t* pData16A = new char16_t[kDataLength];
	char16_t* pData16B = new char16_t[kDataLength];
	char32_t* pData32A = new char32_t[kDataLength];
	char32_t* pData32B = new char32_t[kDataLength];

	// Initialize data
	for(int i(0); i < kDataLength; i++)
	{
		const int c = ((i == 0) ? 1 : i);

		pDataA[i]    = (uint8_t) c;
		pDataB[i]    = (uint8_t) c;
		pData8A[i]   = (char) c;
		pData8B[i]   = (char) c;
		pData16A[i]  = (char16_t)c;
		pData16B[i]  = (char16_t)c;
		pData32A[i]  = (char32_t)c;
		pData32B[i]  = (char32_t)c;
	}

	pDataA[kDataLength - 1]   = 0;
	pDataB[kDataLength - 1]   = 0;
	pData8A[kDataLength - 1]  = 0;
	pData8B[kDataLength - 1]  = 0;
	pData16A[kDataLength - 1] = 0;
	pData16B[kDataLength - 1] = 0;
	pData32A[kDataLength - 1] = 0;
	pData32B[kDataLength - 1] = 0;


	{ // Test DJB2 string hash
		uint32_t nInitialValue(0x12345678);
		uint32_t nHashValue(0);

		nHashValue = DJB2(pDataA, kDataLength, nInitialValue);
		EATEST_VERIFY(nHashValue != 0);
		nHashValue = DJB2_String8(pData8A, nInitialValue);
		EATEST_VERIFY(nHashValue != 0);
		nHashValue = DJB2_String16(pData16A, nInitialValue);
		EATEST_VERIFY(nHashValue != 0);
	}


	{ // Test FNV1 string hash
		uint32_t nInitialValue(0x12345678);
		uint32_t nHashValue(0);

		// To do: Come up with better test validation.
		nHashValue = FNV1(pDataA, kDataLength, nInitialValue);
		EATEST_VERIFY(nHashValue == 0x67f6dbec);
		nHashValue = FNV1_String8(pData8A, nInitialValue);
		EATEST_VERIFY(nHashValue == 0x70533413);
		nHashValue = FNV1_String16(pData16A, nInitialValue);
		EATEST_VERIFY(nHashValue == 0xa1014ae4);
		nHashValue = FNV1_String32(pData32A, nInitialValue);
		EATEST_VERIFY(nHashValue == 0xa1014ae4);
	}


	{ // Test FNV64 string hash
		uint64_t nInitialValue(0x12345678);
		uint64_t nHashValue(0);

		// To do: Come up with better test validation.
		nHashValue = FNV64(pDataA, kDataLength, nInitialValue);
		EATEST_VERIFY(nHashValue == UINT64_C(0xbe387e6512cbab0c));
		nHashValue = FNV64_String8(pData8A, nInitialValue);
		EATEST_VERIFY(nHashValue == UINT64_C(0x78b14197ac736ef3));
		nHashValue = FNV64_String16(pData16A, nInitialValue);
		EATEST_VERIFY(nHashValue == UINT64_C(0xf07159d175cf1dc4));
		nHashValue = FNV64_String32(pData32A, nInitialValue);
		EATEST_VERIFY(nHashValue == UINT64_C(0xf07159d175cf1dc4));
	}

	nErrorCount += TestHashString();


	{ // Test CRC16 binary hash
		uint16_t nHashValue1;
		uint16_t nHashValue2(kCRC16InitialValue);

		// Test one-shot CRC.
		nHashValue1 = CRC16(pDataA, kDataLength);

		// Test iterative CRC.
		for(int i = 0; i < 8; i++)
			nHashValue2 = CRC16(pDataA + (i * (kDataLength / 8)), kDataLength / 8, nHashValue2, i == 7);

		EATEST_VERIFY(nHashValue1 == nHashValue2);
	}


	{ // Test CRC24 binary hash
		uint32_t nHashValue1;
		uint32_t nHashValue2(kCRC24InitialValue);

		// Test one-shot CRC.
		nHashValue1 = CRC24(pDataA, kDataLength);

		// Test iterative CRC.
		for(int i = 0; i < 8; i++)
			nHashValue2 = CRC24(pDataA + (i * (kDataLength / 8)), kDataLength / 8, nHashValue2, i == 7);

		EATEST_VERIFY(EA::StdC::GetHighestBitPowerOf2(nHashValue1) <= 24);
		EATEST_VERIFY(EA::StdC::GetHighestBitPowerOf2(nHashValue2) <= 24);
		EATEST_VERIFY(nHashValue1 == nHashValue2);
	}


	{ // Test CRC32 binary hash
		uint32_t nHashValue1;
		uint32_t nHashValue2(kCRC32InitialValue);

		// Test one-shot CRC.
		nHashValue1 = CRC32(pDataA, kDataLength);

		// Test iterative CRC.
		for(int i = 0; i < 8; i++)
			nHashValue2 = CRC32(pDataA + (i * (kDataLength / 8)), kDataLength / 8, nHashValue2, i == 7);

		EATEST_VERIFY(nHashValue1 == nHashValue2);
	}


	{ // Test CRC32 big-endian binary hash
		uint32_t nHashValue1;
		uint32_t nHashValue2(kCRC32InitialValue);

		// Test one-shot CRC.
		nHashValue1 = CRC32Reverse(pDataA, kDataLength);

		// Test iterative CRC.
		for(int i = 0; i < 8; i++)
			nHashValue2 = CRC32Reverse(pDataA + (i * (kDataLength / 8)), kDataLength / 8, nHashValue2, i == 7);

		EATEST_VERIFY(nHashValue1 == nHashValue2);
	}


	{ // Test CRC64 binary hash
		uint64_t nHashValue1;
		uint64_t nHashValue2(kCRC64InitialValue);

		// Test one-shot CRC.
		nHashValue1 = CRC64(pDataA, kDataLength);

		// Test iterative CRC.
		for(int i = 0; i < 8; i++)
			nHashValue2 = CRC64(pDataA + (i * (kDataLength / 8)), kDataLength / 8, nHashValue2, i == 7);

		EATEST_VERIFY(nHashValue1 == nHashValue2);
	}


	{
		const char a[ 1] = { 'a' };
		const char b[ 2] = { 'b', 'c' };
		const char c[ 3] = { 'c', 'd', 'e' };
		const char d[ 4] = { 'd', 'e', 'f', 'g' };
		const char e[ 5] = { 'd', 'e', 'f', 'g', 'h' };
		const char h[ 8] = { 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k' };
		const char j[10] = { 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm' };

		// To do: Establish correct test result values for these.

		EATEST_VERIFY(CRC16(a, sizeof(a), 0xfbea, false) == 0x67bb);
		EATEST_VERIFY(CRC16(b, sizeof(b), 0xfbea, false) == 0xaa67);
		EATEST_VERIFY(CRC16(c, sizeof(c), 0xfbea, false) == 0x3178);
		EATEST_VERIFY(CRC16(d, sizeof(d), 0xfbea, false) == 0x8c20);

		EATEST_VERIFY(CRC24(a, sizeof(a)) != 0x00000000);
		EATEST_VERIFY(CRC24(b, sizeof(b)) != 0x00000000);
		EATEST_VERIFY(CRC24(c, sizeof(c)) != 0x00000000);
		EATEST_VERIFY(CRC24(d, sizeof(d)) != 0x00000000);

		EATEST_VERIFY(CRC32(a, sizeof(a)) != 0x00000000);
		EATEST_VERIFY(CRC32(b, sizeof(b)) != 0x00000000);
		EATEST_VERIFY(CRC32(c, sizeof(c)) != 0x00000000);
		EATEST_VERIFY(CRC32(d, sizeof(d)) != 0x00000000);

		EATEST_VERIFY(CRC32Reverse(a, sizeof(a)) != 0x00000000);
		EATEST_VERIFY(CRC32Reverse(b, sizeof(b)) != 0x00000000);
		EATEST_VERIFY(CRC32Reverse(c, sizeof(c)) != 0x00000000);
		EATEST_VERIFY(CRC32Reverse(d, sizeof(d)) != 0x00000000);
		EATEST_VERIFY(CRC32Reverse(e, sizeof(e)) != 0x00000000);
		EATEST_VERIFY(CRC32Reverse(h, sizeof(h)) != 0x00000000);
		EATEST_VERIFY(CRC32Reverse(j, sizeof(j)) != 0x00000000);

		EATEST_VERIFY(CRC64(a, sizeof(a)) != 0x00000000);
		EATEST_VERIFY(CRC64(b, sizeof(b)) != 0x00000000);
		EATEST_VERIFY(CRC64(c, sizeof(c)) != 0x00000000);
		EATEST_VERIFY(CRC64(d, sizeof(d)) != 0x00000000);
	}


	delete[] pDataA;
	delete[] pDataB;
	delete[] pData8A;
	delete[] pData8B;
	delete[] pData16A;
	delete[] pData16B;
	delete[] pData32A;
	delete[] pData32B; 

	return nErrorCount;
}












