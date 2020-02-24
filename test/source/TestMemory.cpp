///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EAMemory.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EARandom.h>
#include <EAStdC/EARandomDistribution.h>
#include <EAStdC/EABitTricks.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>
#include <string.h>
#include <EAStdC/EAAlignment.h>
#include <EASTL/vector.h>

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 4996) // Function is deprecated.
	#pragma warning(disable: 6255) // _alloca indicates failure by raising a stack overflow exception. Consider using _malloca instead.
	#pragma warning(disable: 6211) // Leaking memory due to an exception. Consider using a local catch block to clean up memory.
	#pragma warning(disable: 6200) // Index '15' is out of valid index range '0' to '9' for non-stack buffer 'kPredefinedMemSizes'
#endif


// The memory we will use for testing.
static uint8_t* gMem1 = NULL;
static uint8_t* gMem2 = NULL;

// Define expected fill values for gMem1 / gMem2.
const uint8_t kByte1 = 0xaa;
const uint8_t kByte2 = 0xbb;

// For memcpy tests we allocate two large blocks of memory that are 
// of this alignment. 
static const size_t kBaseMemAlignment = 65536;

// For memcpy tests we allocate two large blocks of memory that are 
// of this alignment. We will copy memory around to and from memory 
// segments within this block.
static const size_t kBaseMemSize = 16777216; // 16 MiB

// These are some predefined sizes that we test.
// We also have random size testing.
static const size_t kPredefinedMemSizes[] = 
{
	0,
	1,
	24,
	96,
	200,
	1024,
	4096,       
	65536,      // 64 KiB
	1048576,    // 1 MiB
	8388608     // 8 MiB
};

static void TestEAAllocaHelper()
{
	void* p = EAAlloca(32768);
	EA_ANALYSIS_ASSUME(p != NULL);
	// Try to force a reference to the memory
	memset(p, 0, 1);
}


static int TestEAAlloca()
{
	int nErrorCount = 0;

	{
		void* p = EAAlloca(37); // It's actually possible that this could throw an exception (Microsoft) or a signal (Unix).
		EA_ANALYSIS_ASSUME(p != NULL);
		memset(p, 0, 37);
	}

	{
		// Call a function using alloca repeatedly to ensure the memory is returned.  If memory is not released when
		// returning from TestEAAllocaHelper, then the test will run out of memory (or use huge amounts of virtual memory
		// on PC and likely crash or timeout).
		for (int i=0; i < 1000000; i++)
		{
			TestEAAllocaHelper();
		}
	}

	return nErrorCount;
}


static int TestEAMalloca()
{
	int nErrorCount = 0;

	{
		void* p = EAMalloca(37);
		if(p)
		{
			memset(p, 0, 37);
			EAFreea(p);
		}

		p = EAMalloca(EAMALLOCA_THRESHOLD * 2); // Allocate something that's too large for alloca.
		if(p)
		{
			memset(p, 0, EAMALLOCA_THRESHOLD * 2);
			EAFreea(p);
		}
	}

	return nErrorCount;
}


static int TestMemset()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// uint8_t* Memset8C(void* pDestination, uint8_t c, size_t count);
	{
		EA::StdC::Random r;
		const void* pCheck;

		for(size_t i = 0; i < 16; ++i)
		{
			// Randomly choose a copy size, but make sure the predifined ones are always tested.
			const size_t copyCount = (i < EAArrayCount(kPredefinedMemSizes) && (kPredefinedMemSizes[i] < 4096)) ? kPredefinedMemSizes[i] : r.RandomUint32Uniform(4096);
			const size_t copySize  = copyCount * sizeof(uint8_t);

			for(size_t j = 0; j < 7; ++j)
			{
				const size_t offset1 = r.RandomUint32Uniform((uint32_t)kBaseMemAlignment);
				uint8_t* pMem1 = gMem1 + offset1;

				EA::StdC::Memset8C(pMem1, kByte2, copyCount); // Copy pMem2's kByte2 values over pMem1's kByte1 values.

				// Verify memory prior to pMem1 is unmodified.
				pCheck = Memcheck8(pMem1 - kBaseMemAlignment, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Verify copied memory.
				pCheck = Memcheck8(pMem1, kByte2, copySize);
				EATEST_VERIFY(pCheck == NULL);

				// Verify memory after pMem1+copySize.
				pCheck = Memcheck8(pMem1 + copySize, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Set the memory back to its original value.
				memset(pMem1, kByte1, copySize);
			}
		}
	}


	// uint8_t* Memset8_128C(void* pDestination, uint8_t c, size_t uint8Count);
	{
		for(size_t i = 0; i < 50; i++)
		{
			const size_t copySize  = i * 128;
			const size_t copyCount = copySize * sizeof(uint8_t);

			uint8_t* pMem1 = gMem1;
			const void* pCheck;

			EA::StdC::Memset8_128C(pMem1, kByte2, copyCount);

			// Verify memory prior to pMem1 is unmodified.
			pCheck = Memcheck8(pMem1 - 256, kByte1, 256);
			EATEST_VERIFY(pCheck == NULL);

			// Verify copied memory.
			pCheck = Memcheck8(pMem1, kByte2, copySize);
			EATEST_VERIFY(pCheck == NULL);

			// Verify memory after pMem1+copySize.
			pCheck = Memcheck8(pMem1 + copySize, kByte1, 256);
			EATEST_VERIFY(pCheck == NULL);

			// Set the memory back to its original value.
			memset(pMem1, kByte1, copySize);
		}
	}


	// uint16_t* Memset16(void* pDestination, uint16_t c, size_t count);
	{
		EA::StdC::Random r;
		const void* pCheck;

		const uint16_t kByte2_16 = ((kByte2 << 8) | (kByte2 + 1));

		for(size_t i = 0; i < 16; ++i)
		{
			// Randomly choose a copy count, but make sure the predifined ones are always tested.
			const size_t copyCount = (i < EAArrayCount(kPredefinedMemSizes) && (kPredefinedMemSizes[i] < 2048)) ? kPredefinedMemSizes[i] : r.RandomUint32Uniform(2048);
			const size_t copySize  = copyCount * sizeof(uint16_t);

			for(size_t j = 0; j < 7; ++j)
			{
				const size_t offset1 = r.RandomUint32Uniform((uint32_t)kBaseMemAlignment) / sizeof(uint16_t) * sizeof(uint16_t); // Divide, then multiply.
				uint8_t* pMem1 = gMem1 + offset1;

				EA::StdC::Memset16(pMem1, kByte2_16, copyCount);

				// Verify memory prior to pMem1 is unmodified.
				pCheck = Memcheck8(pMem1 - kBaseMemAlignment, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Verify copied memory.
				pCheck = Memcheck16(pMem1, kByte2_16, copySize);
				EATEST_VERIFY(pCheck == NULL);

				// Verify memory after pMem1+copySize.
				pCheck = Memcheck8(pMem1 + copySize, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Set the memory back to its original value.
				memset(pMem1, kByte1, copySize);
			}
		}
	}


	// uint32_t* Memset32(void* pDestination, uint32_t c, size_t count);
	{
		EA::StdC::Random r;
		const void* pCheck;

		const uint32_t kByte2_32 = ((kByte2 << 8) | (kByte2 + 1));

		for(size_t i = 0; i < 16; ++i)
		{
			// Randomly choose a copy count, but make sure the predifined ones are always tested.
			const size_t copyCount = (i < EAArrayCount(kPredefinedMemSizes) && (kPredefinedMemSizes[i] < 1024)) ? kPredefinedMemSizes[i] : r.RandomUint32Uniform(1024);
			const size_t copySize  = copyCount * sizeof(uint32_t);

			for(size_t j = 0; j < 7; ++j)
			{
				const size_t offset1 = r.RandomUint32Uniform((uint32_t)kBaseMemAlignment) / sizeof(uint32_t) * sizeof(uint32_t); // Divide, then multiply.
				uint8_t* pMem1 = gMem1 + offset1;

				EA::StdC::Memset32(pMem1, kByte2_32, copyCount);

				// Verify memory prior to pMem1 is unmodified.
				pCheck = Memcheck8(pMem1 - kBaseMemAlignment, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Verify copied memory.
				pCheck = Memcheck32(pMem1, kByte2_32, copySize);
				EATEST_VERIFY(pCheck == NULL);

				// Verify memory after pMem1+copySize.
				pCheck = Memcheck8(pMem1 + copySize, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Set the memory back to its original value.
				memset(pMem1, kByte1, copySize);
			}
		}
	}


	// uint64_t* Memset64(void* pDestination, uint64_t c, size_t count);
	{
		EA::StdC::Random r;
		const void* pCheck;

		const uint64_t kByte2_64 = ((kByte2 << 8) | (kByte2 + 1));

		for(size_t i = 0; i < 16; ++i)
		{
			// Randomly choose a copy count, but make sure the predifined ones are always tested.
			const size_t copyCount = (i < EAArrayCount(kPredefinedMemSizes) && (kPredefinedMemSizes[i] < 512)) ? kPredefinedMemSizes[i] : r.RandomUint32Uniform(512);
			const size_t copySize  = copyCount * sizeof(uint64_t);

			for(size_t j = 0; j < 7; ++j)
			{
				const size_t offset1 = r.RandomUint32Uniform((uint64_t)kBaseMemAlignment) / sizeof(uint64_t) * sizeof(uint64_t); // Divide, then multiply.
				uint8_t* pMem1 = gMem1 + offset1;

				EA::StdC::Memset64(pMem1, kByte2_64, copyCount);

				// Verify memory prior to pMem1 is unmodified.
				pCheck = Memcheck8(pMem1 - kBaseMemAlignment, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Verify copied memory.
				pCheck = Memcheck64(pMem1, kByte2_64, copySize);
				EATEST_VERIFY(pCheck == NULL);

				// Verify memory after pMem1+copySize.
				pCheck = Memcheck8(pMem1 + copySize, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Set the memory back to its original value.
				memset(pMem1, kByte1, copySize);
			}
		}
	}



	// void* MemsetPointer(void* pDestination, const void* const pValue, size_t count)
	{
		const size_t kBufferSize = 2000;
		void** const pBuffer = new void*[kBufferSize];

		for(size_t i = 1; i < 2000; i *= 3)
		{
			memset(pBuffer, 0, kBufferSize * sizeof(void*));

			void* p = MemsetPointer(pBuffer, (void*)(uintptr_t)i, i);
			EATEST_VERIFY(p == pBuffer);

			for(size_t k = 0; k < 2000; ++k)
			{
				if(k < i)
					EATEST_VERIFY(pBuffer[k] == (void*)(uintptr_t)i);
				else
					EATEST_VERIFY(pBuffer[k] == (void*)(uintptr_t)0);
			}
		}

		delete[] pBuffer;
	}


	// void* MemsetN(void* pDestination, const void* pSource, size_t sourceBytes, size_t count);
	{
		// To do: We need a more extensive test.

		char        buffer[2000];
		const char* pattern = "012345678";
		size_t         sl = Strlen(pattern);

		EATEST_VERIFY(buffer == MemsetN(buffer, pattern, sl, 2000));
		EATEST_VERIFY(buffer[sl] == '0' && buffer[77*sl+1] == '1');
		EATEST_VERIFY(buffer[sl*33+4] == '4' && buffer[123*sl+8] == '8');
		EATEST_VERIFY(buffer[sl*98+3] == '3' && buffer[181*sl+6] == '6');
	}

	return nErrorCount;
}


static int TestMemfill()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// void Memfill8(void* pDestination, uint8_t c,  size_t byteCount);
	{
		const void* result = nullptr;
		const int ARR_SIZE = 4096;

		uint8_t buf[ARR_SIZE];
		void* pMem = static_cast<void*>(&buf);

		Memfill8(pMem, kByte1, ARR_SIZE);
		result = Memcheck8(pMem, kByte1, ARR_SIZE);
		EATEST_VERIFY(result == NULL);

		Memfill8(pMem, kByte2, ARR_SIZE);
		result = Memcheck8(pMem, kByte2, ARR_SIZE);
		EATEST_VERIFY(result == NULL);
	}

	// void Memfill16(void* pDestination, uint16_t c, size_t byteCount);
	{
		// Test different alignments, sizes 0 to 257, 1023 to 1026
		uint16_t  val16      = 0x1234;
		uint16_t* val16Array = new uint16_t[2048 + 32];    // To consider: Would it make the code better if we created constants for these 
		uint8_t*  buf8Array  = new  uint8_t[4096 + 64];    //              sizes, or would it instead just make this harder to follow?
		uint8_t*  buf8Array2 = new  uint8_t[4096 + 64];
		size_t    j;

		EATEST_VERIFY(val16Array && buf8Array && buf8Array2);

		Memset16(val16Array, val16, 2048 + 32);

		for(int32_t i = 0; i < 4; i++)
		{
			for(j = 0; j <= 257; j++)
			{
				memset(buf8Array, 0, 4096);
				memset(buf8Array2, 0, 4096);
				Memfill16(buf8Array + i, val16, j);

				for(size_t k = 0; k < (j / sizeof(uint16_t) + 1); ++k)
					memcpy(buf8Array2 + i + (k * sizeof(uint16_t)), val16Array, j - (k * sizeof(uint16_t)));

				EATEST_VERIFY(memcmp(buf8Array, buf8Array2, 4096) == 0);
			}

			for(j = 1023; j <= 1026; j++)
			{
				memset(buf8Array, 0, 4096);
				memset(buf8Array2, 0, 4096);
				Memfill16(buf8Array + i, val16, j);

				for(size_t k = 0; k < (j / sizeof(uint16_t) + 1); ++k)
					memcpy(buf8Array2 + i + (k * sizeof(uint16_t)), val16Array, j - (k * sizeof(uint16_t)));

				EATEST_VERIFY(memcmp(buf8Array, buf8Array2, 4096) == 0);
			}
		}

		delete[] val16Array; 
		delete[] buf8Array;
		delete[] buf8Array2;
	}

	// void Memfill24(void* pDestination, uint32_t c, size_t byteCount);
	{
		// To do.
	}

	// void Memfill32(void* pDestination, uint32_t c, size_t byteCount);
	{
		// To do.
	}

	// void Memfill64(void* pDestination, uint64_t c, size_t byteCount);
	{
		// To do.
	}

	// void MemfillSpecific(void* pDestination, const void* pSource, size_t destByteCount, size_t sourceByteCount);
	{
		// To do.
	}

	return nErrorCount;
}


static int TestMemclear()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	// void MemclearC(void* pDestination, size_t n);
	{
		EA::StdC::Random r;
		const void* pCheck;

		for(size_t i = 0; i < 16; ++i)
		{
			// Randomly choose a copy size, but make sure the predefined ones are always tested.
			const size_t copyCount = (i < EAArrayCount(kPredefinedMemSizes) && (kPredefinedMemSizes[i] < 4096)) ? kPredefinedMemSizes[i] : r.RandomUint32Uniform(4096);
			const size_t copySize  = copyCount * sizeof(uint8_t);

			for(size_t j = 0; j < 7; ++j)
			{
				const size_t offset1 = r.RandomUint32Uniform((uint32_t)kBaseMemAlignment);
				uint8_t* pMem1 = gMem1 + offset1;

				EA::StdC::MemclearC(pMem1, copyCount); // Set zero values over pMem1's kByte1 values.

				// Verify memory prior to pMem1 is unmodified.
				pCheck = Memcheck8(pMem1 - kBaseMemAlignment, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Verify copied memory.
				pCheck = Memcheck8(pMem1, 0, copySize);
				EATEST_VERIFY(pCheck == NULL);

				// Verify memory after pMem1+copySize.
				pCheck = Memcheck8(pMem1 + copySize, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Set the memory back to its original value.
				memset(pMem1, kByte1, copySize);
			}
		}
	}

	return nErrorCount;
}


static int TestMemcheck()
{
	using namespace EA::StdC;

	int nErrorCount = 0;
	const void* pCheck;

	// const void* Memcheck8(const void* p, uint8_t c, size_t byteCount);
	{
		const uint8_t bytes[5] = { 0x00, 0x01, 0x01, 0x01, 0x00 };

		pCheck = Memcheck8(bytes + 0, 0x00, 1);
		EATEST_VERIFY(pCheck == NULL);

		pCheck = Memcheck8(bytes + 0, 0x01, 1);
		EATEST_VERIFY(pCheck == bytes);

		pCheck = Memcheck8(bytes + 0, 0x00, 2);
		EATEST_VERIFY(pCheck == bytes + 1);

		pCheck = Memcheck8(bytes + 1, 0x01, 3);
		EATEST_VERIFY(pCheck == NULL);
	}

	// const void* Memcheck16(const void* p, uint16_t c, size_t byteCount);
	{
		union U16 {
			uint16_t c16;
			uint8_t  c8[2];
		};
		const U16 bytes[5] = { { 0x0000 }, { 0x0001 }, { 0x0001 }, { 0x0001 }, { 0x0101 } };

		pCheck = Memcheck16(bytes + 0, 0x0000, 2);
		EATEST_VERIFY(pCheck == NULL);

		pCheck = Memcheck16(bytes + 0, 0x0001, 2);
		#ifdef EA_SYSTEM_BIG_ENDIAN
			EATEST_VERIFY(pCheck == bytes[0].c8 + 1);
		#else
			EATEST_VERIFY(pCheck == bytes[0].c8 + 0);
		#endif

		pCheck = Memcheck16(bytes + 1, 0x0001, 6);
		EATEST_VERIFY(pCheck == NULL);

		pCheck = Memcheck16(bytes[0].c8 + 1, 0x0001, 2);
		#ifdef EA_SYSTEM_BIG_ENDIAN
			EATEST_VERIFY(pCheck == bytes[0].c8 + 1);
		#else
			EATEST_VERIFY(pCheck == NULL);  // Due to byte ordering, little-endian sees this as matching.
		#endif

		pCheck = Memcheck16(bytes[0].c8 + 1, 0x0000, 2);
		#ifdef EA_SYSTEM_BIG_ENDIAN
			EATEST_VERIFY(pCheck == NULL);  // Due to byte ordering, big-endian sees this as matching.
		#else
			EATEST_VERIFY(pCheck == bytes[1].c8 + 0);
		#endif
	}

	// const void* Memcheck32(const void* p, uint32_t c, size_t byteCount);
	{
		union U32 {
			uint32_t c32;
			uint8_t  c8[4];
		};
		const U32 bytes[5] = { { 0x00010203 }, { 0x00010203 }, { 0x00010203 }, { 0x00010203 }, { 0x00010203 } };

		for(int i = 0; i <= 4; ++i)
		{
			pCheck = Memcheck32(bytes[0].c8 + i, 0x00010203, 9);
			EATEST_VERIFY(pCheck == NULL);
		}

		for(int i = 0; i <= 4; ++i)
		{
			pCheck = Memcheck32(bytes[0].c8 + i, 0x01020300, 9);
			EATEST_VERIFY(pCheck != NULL);
		}
	}

	// const void* Memcheck64(const void* p, uint64_t c, size_t byteCount);
	{
		union U64 {
			uint64_t c64;
			uint8_t  c8[8];
		};

		// Some platforms' (e.g. x86) compilers don't align 64 bit values on 64 bit boundaries. So we guarantee it here, as Memcheck64 expects it.
		// Additionally, some of the platforms we test for require 16 bit alignment of types, so we use that instead of 8.
		static EA_ALIGNED(const U64, bytes[5], 16) = { { UINT64_C(0x0001020304050607) }, { UINT64_C(0x0001020304050607) }, { UINT64_C(0x0001020304050607) }, { UINT64_C(0x0001020304050607) }, { UINT64_C(0x0001020304050607) } };

		for(int i = 0; i <= 8; ++i)
		{
			pCheck = Memcheck64(bytes[0].c8 + i, UINT64_C(0x0001020304050607), 18);
			EATEST_VERIFY(pCheck == NULL);
		}

		for(int i = 0; i <= 8; ++i)
		{
			pCheck = Memcheck64(bytes[0].c8 + i, UINT64_C(0x0102030405060700), 18);
			EATEST_VERIFY(pCheck != NULL);
		}
	}

	return nErrorCount;
}


static int TestMemchr()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{   // Memchr8
		const char* const s = "qwertyuiopASDFGHJKL:!@#$%^&*,=/";

		EATEST_VERIFY((char*)Memchr(s, (char)'q', Strlen(s)) - s ==  0);
		EATEST_VERIFY((char*)Memchr(s, (char)'F', Strlen(s)) - s == 13);
		EATEST_VERIFY((char*)Memchr(s, (char)':', Strlen(s)) - s == 19);
		EATEST_VERIFY((char*)Memchr(s, (char)'&', Strlen(s)) - s == 26);
	}

	return nErrorCount;
}


static int TestMemcmp()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{   // Memcmp8
		char buffer1[] = "01234567a";
		char buffer2[] = "01234567b";
		char buffer3[] = "01234567c";

		EATEST_VERIFY(Memcmp(buffer1, buffer1, 9) == 0);
		EATEST_VERIFY(Memcmp(buffer2, buffer1, 9) >  0);
		EATEST_VERIFY(Memcmp(buffer3, buffer2, 9) >  0);
		EATEST_VERIFY(Memcmp(buffer2, buffer3, 9) <  0);
		EATEST_VERIFY(Memcmp(buffer1, buffer2, 9) <  0);
	}

	#if EASTDC_MEMCPY16_ENABLED
		{   // Memcmp16
			char16_t buffer1[] = EA_CHAR16("01234567a");
			char16_t buffer2[] = EA_CHAR16("01234567b");
			char16_t buffer3[] = EA_CHAR16("01234567c");

			EATEST_VERIFY(Memcmp(buffer1, buffer1, 9) == 0);
			EATEST_VERIFY(Memcmp(buffer2, buffer1, 9) >  0);
			EATEST_VERIFY(Memcmp(buffer3, buffer2, 9) >  0);
			EATEST_VERIFY(Memcmp(buffer2, buffer3, 9) <  0);
			EATEST_VERIFY(Memcmp(buffer1, buffer2, 9) <  0);
		}
	#endif

	return nErrorCount;
}


static int TestMemmem()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	const size_t  kSize = 37;
	const char buffer1[kSize] = "abcdefghijklmnopqrstuvwxyz0123456789";

	EATEST_VERIFY(Memmem(buffer1,     0, "",            0) == NULL);            // An empty haystack always results in NULL, regardless of the needle.
	EATEST_VERIFY(Memmem(buffer1, kSize, "",            0) == buffer1);         // Otherwise, an empty needle results in success.
	EATEST_VERIFY(Memmem(buffer1,     0, "_",           1) == NULL);            // 
	EATEST_VERIFY(Memmem("_",         1, buffer1,   kSize) == NULL);            // Search of a needle that is bigger than the haystack. Always failure.
	EATEST_VERIFY(Memmem(buffer1, kSize, "_",           1) == NULL);
	EATEST_VERIFY(Memmem(buffer1, kSize, buffer1,   kSize) == buffer1);
	EATEST_VERIFY(Memmem(buffer1, kSize, "a",           1) == buffer1);
	EATEST_VERIFY(Memmem(buffer1, kSize, "abc",         3) == buffer1);
	EATEST_VERIFY(Memmem(buffer1, kSize, "bcd",         3) == buffer1 + 1);
	EATEST_VERIFY(Memmem(buffer1, kSize, "tuv",         3) == buffer1 + 19);
	EATEST_VERIFY(Memmem(buffer1, kSize, "9",           1) == buffer1 + 35);
	EATEST_VERIFY(Memmem(buffer1, kSize, "789",         3) == buffer1 + 33);
	EATEST_VERIFY(Memmem(buffer1, kSize, "9__",         3) == NULL);
	EATEST_VERIFY(Memmem("\1\0",      2, "\1\0",        2) != NULL);
	EATEST_VERIFY(Memmem("\1\1",      2, "\1\0",        2) == NULL);

	return nErrorCount;
}


static int TestMemcpy()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{   // MemcpyC
		char buffer1[] = "         ";
		char buffer2[] = "01234567b";

		EATEST_VERIFY(buffer1 == MemcpyC(buffer1, buffer2, 9));
		EATEST_VERIFY(Memcmp(buffer2, buffer1, 9) ==  0);
	}

	#if EASTDC_MEMCPY16_ENABLED
		{   // Memcpy16
			char16_t buffer1[] = EA_CHAR16("         ");
			char16_t buffer2[] = EA_CHAR16("01234567b");

			EATEST_VERIFY(buffer1 == (char16_t*)Memcpy((void*)buffer1, (void*)buffer2, 9 * sizeof(char16_t)));
			EATEST_VERIFY(memcmp(buffer2, buffer1, 9 * sizeof(char16_t)) ==  0);
		}
	#endif


	{ // char* MemcpyC(void* pDestination, const void* pSource, size_t nByteCount);

		EA::StdC::Random r;
		const void* pCheck;

		for(size_t i = 0; i < EAArrayCount(kPredefinedMemSizes); ++i)
		{
			const size_t copySize = kPredefinedMemSizes[i];

			for(size_t j = 0; j < 7; ++j)
			{
				const size_t offset1 = r.RandomUint32Uniform((uint32_t)kBaseMemAlignment);
				const size_t offset2 = r.RandomUint32Uniform((uint32_t)kBaseMemAlignment);

				uint8_t* pMem1 = gMem1 + offset1;
				uint8_t* pMem2 = gMem2 + offset2;

				EA::StdC::MemcpyC(pMem1, pMem2, copySize); // Copy pMem2's kByte2 values over pMem1's kByte1 values.

				// Verify memory prior to pMem1 is unmodified.
				pCheck = Memcheck8(pMem1 - kBaseMemAlignment, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Verify copied memory.
				pCheck = Memcheck8(pMem1, kByte2, copySize);
				EATEST_VERIFY(pCheck == NULL);

				// Verify memory after pMem1+copySize.
				pCheck = Memcheck8(pMem1 + copySize, kByte1, kBaseMemAlignment);
				EATEST_VERIFY(pCheck == NULL);

				// Set the memory back to its original value.
				memset(pMem1, kByte1, copySize);
			}
		}
	}


	{   // char* Memcpy128(void* pDestination, const void* pSource, size_t nByteCount);

		EA::StdC::Stopwatch stopwatch1(EA::StdC::Stopwatch::kUnitsCPUCycles);
		EA::StdC::Stopwatch stopwatch2(EA::StdC::Stopwatch::kUnitsCPUCycles);
		EA::StdC::Stopwatch stopwatch3(EA::StdC::Stopwatch::kUnitsCPUCycles);

		for(int t = 0; t < 2; t++)
		{
			stopwatch1.Reset();
			stopwatch2.Reset();
			stopwatch3.Reset();

			for(size_t i = 0; i < 50; i++)
			{
				const size_t copySize = i * 128;
				const void* pCheck;

				uint8_t* pMem1 = gMem1;
				uint8_t* pMem2 = gMem2;

				stopwatch1.Start();
				EA::StdC::Memcpy128(pMem1, pMem2, copySize); // Copy pMem2's kByte2 values over pMem1's kByte1 values.
				stopwatch1.Stop();

				// Verify memory prior to pMem1 is unmodified.
				pCheck = Memcheck8(pMem1 - 256, kByte1, 256);
				EATEST_VERIFY(pCheck == NULL);

				// Verify copied memory.
				pCheck = Memcheck8(pMem1, kByte2, copySize);
				EATEST_VERIFY(pCheck == NULL);

				// Verify memory after pMem1+copySize.
				pCheck = Memcheck8(pMem1 + copySize, kByte1, 256);
				EATEST_VERIFY(pCheck == NULL);

				// Set the memory back to its original value.
				memset(pMem1, kByte1, copySize);

				// Compare to regular memcpy.
				stopwatch2.Start();
				memcpy(pMem1, pMem2, copySize);
				stopwatch2.Stop();
				memset(pMem1, kByte1, copySize);

				// Compare to regular __builtin_memcpy.
				stopwatch3.Start();
				#if defined(__GNUC__)
					__builtin_memcpy(pMem1, pMem2, copySize);
				#else
					memcpy(pMem1, pMem2, copySize);
				#endif
				stopwatch3.Stop();
				memset(pMem1, kByte1, copySize);
			}

			if(t == 1)
				EA::UnitTest::ReportVerbosity(1, "Memcpy128: %I64u cycles; memcpy: %I64u cycles, __builtin_memcpy: %I64u\n", 
												stopwatch1.GetElapsedTime(), stopwatch2.GetElapsedTime(), stopwatch3.GetElapsedTime());
		}
	}

	{   // char* Memcpy128C(void* pDestination, const void* pSource, size_t nByteCount);
		for(size_t i = 0; i < 50; i++)
		{
			const size_t copySize = i * 128;
			const void* pCheck;

			uint8_t* pMem1 = gMem1;
			uint8_t* pMem2 = gMem2;

			EA::StdC::Memcpy128C(pMem1, pMem2, copySize); // Copy pMem2's kByte2 values over pMem1's kByte1 values.

			// Verify memory prior to pMem1 is unmodified.
			pCheck = Memcheck8(pMem1 - 256, kByte1, 256);
			EATEST_VERIFY(pCheck == NULL);

			// Verify copied memory.
			pCheck = Memcheck8(pMem1, kByte2, copySize);
			EATEST_VERIFY(pCheck == NULL);

			// Verify memory after pMem1+copySize.
			pCheck = Memcheck8(pMem1 + copySize, kByte1, 256);
			EATEST_VERIFY(pCheck == NULL);

			// Set the memory back to its original value.
			memset(pMem1, kByte1, copySize);
		}
	}

	return nErrorCount;
}


static int TestMemmove()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{    // Memmove8
		char buffer1[] = "..........."; 
		char buffer2[] = ".......0123"; 
		char buffer3[] = "0123......."; 

		EATEST_VERIFY(buffer1 == MemmoveC(buffer1, buffer2, Strlen(buffer2)));
		EATEST_VERIFY(memcmp(buffer1, buffer2, Strlen(buffer2)) ==  0);
		EATEST_VERIFY(memset(buffer1, (char )0, Strlen(buffer1)) != NULL);

		EATEST_VERIFY(buffer1 == MemmoveC(buffer1, buffer2+7, Strlen(buffer2) - 7));
		EATEST_VERIFY(memcmp(buffer1, buffer2+7, Strlen(buffer2) - 7) ==  0);

		EATEST_VERIFY(buffer2+5 == MemmoveC(buffer2+5, buffer2+7, Strlen(buffer2) - 7));
		EATEST_VERIFY(memcmp(buffer2+5, buffer1, Strlen(buffer2) - 7) ==  0);

		EATEST_VERIFY(buffer1 == MemmoveC(buffer1, buffer3, Strlen(buffer3)));
		EATEST_VERIFY(buffer3+2 == MemmoveC(buffer3+2, buffer3, Strlen(buffer3) - 2));
		EATEST_VERIFY(memcmp(buffer3+2, buffer1, Strlen(buffer3) - 2) ==  0);

		// To do: We need a much better test than this.
	}

	#if EASTDC_MEMCPY16_ENABLED
		{   // Memmove16
			char16_t buffer1[] = EA_CHAR16("..........."); 
			char16_t buffer2[] = EA_CHAR16(".......0123"); 
			char16_t buffer3[] = EA_CHAR16("0123......."); 

			EATEST_VERIFY(buffer1 == (char16_t*)Memmove((void*)buffer1, (void*)buffer2, Strlen(buffer2) * sizeof(char16_t)));
			EATEST_VERIFY(memcmp(buffer1, buffer2, Strlen(buffer2) * sizeof(char16_t)) ==  0);
			EATEST_VERIFY(memset(buffer1, (char16_t )0, Strlen(buffer1) * sizeof(char16_t)) != NULL);

			EATEST_VERIFY(buffer1 == (char16_t*)Memmove((void*)buffer1, (void*)(buffer2+7), (Strlen(buffer2) - 7) * sizeof(char16_t)));
			EATEST_VERIFY(memcmp(buffer1, buffer2+7, (Strlen(buffer2) - 7) * sizeof(char16_t)) ==  0);

			EATEST_VERIFY(buffer2+5 == (char16_t*)Memmove((void*)(buffer2+5), (void*)(buffer2+7), (Strlen(buffer2) - 7) * sizeof(char16_t)));
			EATEST_VERIFY(memcmp(buffer2+5, buffer1, (Strlen(buffer2) - 7) * sizeof(char16_t)) ==  0);

			EATEST_VERIFY(buffer1   == (char16_t*)Memmove((void*)buffer1,     (void*)buffer3, Strlen(buffer3) * sizeof(char16_t)));
			EATEST_VERIFY(buffer3+2 == (char16_t*)Memmove((void*)(buffer3+2), (void*)buffer3, (Strlen(buffer3) - 2) * sizeof(char16_t)));
			EATEST_VERIFY(memcmp(buffer3+2, buffer1, (Strlen(buffer3) - 2) * sizeof(char16_t)) ==  0);
		}
	#endif

	return nErrorCount;
}


static int TestTimingSafe()
{
	using namespace EA::StdC;

	int nErrorCount = 0;

	{
		// bool TimingSafeMemEqual(const void* p1, const void* p2, size_t n);
		// int  TimingSafeMemcmp(const void* p1, const void* p2, size_t n);
		// bool TimingSafeMemIsClear(const void* p, size_t n);

		{   // Basic accuracy tests.
			char buffer1[] = "01234567a";
			char buffer2[] = "01234567b";
			char buffer3[] = "01234567c";
			char buffer4[] = "\0\0\0\0\0\0\0\0\0";

			EATEST_VERIFY(TimingSafeMemcmp(buffer1, buffer1, 0) == Memcmp(buffer1, buffer1, 0));
			EATEST_VERIFY(TimingSafeMemcmp(buffer2, buffer1, 9) == Memcmp(buffer2, buffer1, 9));
			EATEST_VERIFY(TimingSafeMemcmp(buffer3, buffer2, 9) == Memcmp(buffer3, buffer2, 9));
			EATEST_VERIFY(TimingSafeMemcmp(buffer2, buffer3, 9) == Memcmp(buffer2, buffer3, 9));
			EATEST_VERIFY(TimingSafeMemcmp(buffer1, buffer2, 9) == Memcmp(buffer1, buffer2, 9));

			EATEST_VERIFY(TimingSafeMemEqual(buffer1, buffer1, 0) == (Memcmp(buffer1, buffer1, 0) == 0));
			EATEST_VERIFY(TimingSafeMemEqual(buffer1, buffer1, 9) == (Memcmp(buffer1, buffer1, 9) == 0));
			EATEST_VERIFY(TimingSafeMemEqual(buffer2, buffer1, 9) == (Memcmp(buffer2, buffer1, 9) == 0));
			EATEST_VERIFY(TimingSafeMemEqual(buffer3, buffer2, 9) == (Memcmp(buffer3, buffer2, 9) == 0));
			EATEST_VERIFY(TimingSafeMemEqual(buffer2, buffer3, 9) == (Memcmp(buffer2, buffer3, 9) == 0));
			EATEST_VERIFY(TimingSafeMemEqual(buffer1, buffer2, 9) == (Memcmp(buffer1, buffer2, 9) == 0));

			EATEST_VERIFY(TimingSafeMemIsClear(buffer1, 0) == true);
			EATEST_VERIFY(TimingSafeMemIsClear(buffer1, 1) == false);
			EATEST_VERIFY(TimingSafeMemIsClear(buffer1, 9) == false);
			EATEST_VERIFY(TimingSafeMemIsClear(buffer4, 1) == true);
			EATEST_VERIFY(TimingSafeMemIsClear(buffer4, 9) == true);
		}


		{   // Timing tests.
			// It's not easy to fully validate the constant timing of these functions, due to the 
			// tiny cycle count differences potentially involved. However, we can pretty easily
			// test extreme cases and verify that at least the basic logic of the functions are 
			// timing constant and not optimized away by the compiler.

			Stopwatch stopwatch1(Stopwatch::kUnitsCPUCycles, false);
			Stopwatch stopwatch2(Stopwatch::kUnitsCPUCycles, false);
			bool      success = false;

			eastl::vector<uint8_t> vLarge1((eastl_size_t)100000, (uint8_t)0); // Some large sized memory.
			eastl::vector<uint8_t> vLarge2((eastl_size_t)100000, (uint8_t)0);

			// We run this test multiple times because it may fail due to some execution hiccup and we want to give it 
			// another chance. In a sense this is a bad idea because it seems to be going against what the function is 
			// intended to be tested for, but it's impossible to truly know why something didn't execute in constant
			// time without looking at the executed machine code by hand.

			// TimingSafeMemEqual
			for(int i = 0; (i < 3) && !success; i++)
			{
				stopwatch1.Restart();
				bool bResult = TimingSafeMemEqual(vLarge1.data(), vLarge2.data(), vLarge1.size());
				stopwatch1.Stop();
				EATEST_VERIFY(bResult == true);

				vLarge1[0] = 1;     // Change the first and last bytes. With regular memcmp this changed byte would result in memcmp returning very quickly, but we don't want that.
				vLarge1[vLarge1.size()-1] = 1;
				stopwatch2.Restart();
				bResult = TimingSafeMemEqual(vLarge1.data(), vLarge2.data(), vLarge1.size());
				stopwatch2.Stop();
				EATEST_VERIFY(bResult == false);
				success = (((stopwatch1.GetElapsedTimeFloat() - stopwatch2.GetElapsedTimeFloat()) / stopwatch1.GetElapsedTimeFloat()) < 0.25); // We give it a lot of leeway so our unit tests don't frequently fail.
				vLarge1[0] = 0;     // Restore the changed bytes.
				vLarge1[vLarge1.size()-1] = 0;
			}

			EATEST_VERIFY_MSG(success, "TimingSafeMemEqual didn't seem to be able to execute in constant time.");


			// TimingSafeMemcmp
			for(int i = 0; (i < 3) && !success; i++)
			{
				stopwatch1.Restart();
				int iResult = TimingSafeMemcmp(vLarge1.data(), vLarge2.data(), vLarge1.size());
				stopwatch1.Stop();
				EATEST_VERIFY(iResult == 0);

				vLarge1[0] = 1; // Change the first and last bytes. With regular memcmp this changed byte would result in memcmp returning very quickly, but we don't want that.
				vLarge1[vLarge1.size()-1] = 1;
				stopwatch2.Restart();
				iResult = TimingSafeMemcmp(vLarge1.data(), vLarge2.data(), vLarge1.size());
				stopwatch2.Stop();
				EATEST_VERIFY(iResult == 1);
				success = ((fabsf(stopwatch1.GetElapsedTimeFloat() - stopwatch2.GetElapsedTimeFloat()) / stopwatch1.GetElapsedTimeFloat()) < 0.25); // We give it a lot of leeway so our unit tests don't frequently fail.
				vLarge1[0] = 0;
				vLarge1[vLarge1.size()-1] = 0;
			}

			EATEST_VERIFY_MSG(success, "TimingSafeMemcmp didn't seem to be able to execute in constant time.");


			// TimingSafeMemIsClear
			for(int i = 0; (i < 3) && !success; i++)
			{
				stopwatch1.Restart();
				bool bResult = TimingSafeMemIsClear(vLarge1.data(), vLarge1.size());
				stopwatch1.Stop();
				EATEST_VERIFY(bResult == true);

				vLarge1[0] = 1;
				vLarge1[vLarge1.size()-1] = 1;
				stopwatch2.Restart();
				bResult = TimingSafeMemIsClear(vLarge1.data(), vLarge1.size());
				stopwatch2.Stop();
				EATEST_VERIFY(bResult == false);
				success = ((fabsf(stopwatch1.GetElapsedTimeFloat() - stopwatch2.GetElapsedTimeFloat()) / stopwatch1.GetElapsedTimeFloat()) < 0.25); // We give it a lot of leeway so our unit tests don't frequently fail.
				vLarge1[0] = 0;
				vLarge1[vLarge1.size()-1] = 0;
			}

			EATEST_VERIFY_MSG(success, "TimingSafeMemIsClear didn't seem to be able to execute in constant time.");
		}
	}

	return nErrorCount;
}


static void TestMemcpySpeed()
{
	using namespace EA::StdC;

	struct SizeOffset
	{
		size_t mSize;
		size_t mOffset1;
		size_t mOffset2;
	};

	Stopwatch   s(Stopwatch::kUnitsCPUCycles);
	uint32_t    kSeed = 0x12345678;
	Random      r(kSeed);
	size_t      kSizeArraySize = 512;     // We don't want this too large, else we start getting a lot of cache effects.
	SizeOffset* sizeArray = new SizeOffset[kSizeArraySize];

	for(size_t i = 0; i < kSizeArraySize; ++i)
	{
		sizeArray[i].mOffset1 = (size_t)(uint32_t)RandomInt32UniformRange(r, 0, 32);
		sizeArray[i].mOffset2 = (size_t)(uint32_t)RandomInt32UniformRange(r, 0, 32);
	}


	////////////////////////
	// Small copies
	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mSize = (size_t)(uint32_t)RandomInt32UniformRange(r, 0, 256);

	s.Restart();
	for(size_t j = 0; j < 128; ++j) // Do a double loop so that we can get a lot of copies done without sizeArray being so large that it starts having cache effects.
	{
		for(size_t i = 0; i < kSizeArraySize; ++i)
		{
			const SizeOffset& so = sizeArray[i];
			MemcpyC(gMem1 + so.mOffset1, gMem2 + so.mOffset2, so.mSize);
		}
	}
	s.Stop();
	//Printf("%I64u\n", s.GetElapsedTime());


	////////////////////////
	// Medium copies
	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mSize = (size_t)(uint32_t)RandomInt32UniformRange(r, 256, 4096);

	s.Restart();
	for(size_t j = 0; j < 64; ++j) // Do a double loop so that we can get a lot of copies done without sizeArray being so large that it starts having cache effects.
	{
		for(size_t i = 0; i < kSizeArraySize; ++i)
		{
			const SizeOffset& so = sizeArray[i];
			MemcpyC(gMem1 + so.mOffset1, gMem2 + so.mOffset2, so.mSize);
		}
	}
	s.Stop();
	//Printf("%I64u\n", s.GetElapsedTime());


	////////////////////////
	// Large copies
	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mSize = (size_t)(uint32_t)RandomInt32UniformRange(r, 4096, 262144);

	s.Restart();
	for(size_t j = 0; j < 32; ++j) // Do a double loop so that we can get a lot of copies done without sizeArray being so large that it starts having cache effects.
	{
		for(size_t i = 0; i < kSizeArraySize; ++i)
		{
			const SizeOffset& so = sizeArray[i];
			MemcpyC(gMem1 + (so.mOffset1 * 8), gMem2 + (so.mOffset2 * 8), so.mSize); // Test with 8 byte alignments.
		}
	}
	s.Stop();
	//Printf("%I64u\n", s.GetElapsedTime());


	////////////////////////
	// Giant copies
	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mSize = (size_t)(uint32_t)RandomInt32UniformRange(r, 262144, 4194304);

	s.Restart();
	for(size_t j = 0; j < 16; ++j) // Do a double loop so that we can get a lot of copies done without sizeArray being so large that it starts having cache effects.
	{
		for(size_t i = 0; i < kSizeArraySize; ++i)
		{
			const SizeOffset& so = sizeArray[i];
			MemcpyC(gMem1 + (so.mOffset1 * 128), gMem2 + (so.mOffset2 * 128), so.mSize); // Test with 128 byte alignments.
		}
	}
	s.Stop();
	//Printf("%I64u\n", s.GetElapsedTime());

	delete[] sizeArray;
}


static void TestMemmoveSpeed()
{
	using namespace EA::StdC;
	// To do.
}


static void TestMemsetSpeed()
{
	using namespace EA::StdC;
	// To do.
}


static void TestMemclearSpeed()
{
	using namespace EA::StdC;

	struct SizeOffset
	{
		size_t mSize;
		size_t mOffset1;
	};

	Stopwatch   s(Stopwatch::kUnitsCPUCycles);
	uint32_t    kSeed = 0x12345678;
	Random      r(kSeed);
	size_t      kSizeArraySize = 512;     // We don't want this too large, else we start getting a lot of cache effects.
	SizeOffset* sizeArray = new SizeOffset[kSizeArraySize];

	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mOffset1 = (size_t)(uint32_t)RandomInt32UniformRange(r, 0, 32);


	////////////////////////
	// Small clears
	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mSize = (size_t)(uint32_t)RandomInt32UniformRange(r, 0, 256);

	s.Restart();
	for(size_t j = 0; j < 128; ++j) // Do a double loop so that we can get a lot of copies done without sizeArray being so large that it starts having cache effects.
	{
		for(size_t i = 0; i < kSizeArraySize; ++i)
		{
			const SizeOffset& so = sizeArray[i];
			MemclearC(gMem1 + so.mOffset1, so.mSize);
			// memset(gMem1 + so.mOffset1, 0, so.mSize);
		}
	}
	s.Stop();
	//Printf("%I64u\n", s.GetElapsedTime());


	////////////////////////
	// Medium clears
	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mSize = (size_t)(uint32_t)RandomInt32UniformRange(r, 256, 4096);

	s.Restart();
	for(size_t j = 0; j < 64; ++j) // Do a double loop so that we can get a lot of copies done without sizeArray being so large that it starts having cache effects.
	{
		for(size_t i = 0; i < kSizeArraySize; ++i)
		{
			const SizeOffset& so = sizeArray[i];
			MemclearC(gMem1 + so.mOffset1, so.mSize);
			// memset(gMem1 + so.mOffset1, 0, so.mSize);
		}
	}
	s.Stop();
	//Printf("%I64u\n", s.GetElapsedTime());


	////////////////////////
	// Large clears
	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mSize = (size_t)(uint32_t)RandomInt32UniformRange(r, 4096, 262144);

	s.Restart();
	for(size_t j = 0; j < 32; ++j) // Do a double loop so that we can get a lot of copies done without sizeArray being so large that it starts having cache effects.
	{
		for(size_t i = 0; i < kSizeArraySize; ++i)
		{
			const SizeOffset& so = sizeArray[i];
			MemclearC(gMem1 + (so.mOffset1 * 8), so.mSize); // Test with 8 byte alignments.
			// memset(gMem1 + (so.mOffset1 * 8), 0, so.mSize); // Test with 8 byte alignments.
		}
	}
	s.Stop();
	//Printf("%I64u\n", s.GetElapsedTime());


	////////////////////////
	// Giant clears
	for(size_t i = 0; i < kSizeArraySize; ++i)
		sizeArray[i].mSize = (size_t)(uint32_t)RandomInt32UniformRange(r, 262144, 4194304);

	s.Restart();
	for(size_t j = 0; j < 16; ++j) // Do a double loop so that we can get a lot of copies done without sizeArray being so large that it starts having cache effects.
	{
		for(size_t i = 0; i < kSizeArraySize; ++i)
		{
			const SizeOffset& so = sizeArray[i];
			MemclearC(gMem1 + (so.mOffset1 * 128), so.mSize); // Test with 128 byte alignments.
			// memset(gMem1 + (so.mOffset1 * 128), 0, so.mSize); // Test with 128 byte alignments.
		}
	}
	s.Stop();
	//Printf("%I64u\n", s.GetElapsedTime());

	delete[] sizeArray;
}



int TestMemory()
{
	int nErrorCount = 0;

	// Set up large aligned memory blocks for memory tests.
	// kBaseMemSize * 2 because we will set gMem1 to be kBaseMemSize bytes into pMem1Aligned so we can read bytes prior to the tested space.
	const size_t size           = (kBaseMemSize * 2) + kBaseMemAlignment;
	uint8_t*     pMem1Unaligned = new uint8_t[size];
	uint8_t*     pMem2Unaligned = new uint8_t[size];

	if(pMem1Unaligned && pMem2Unaligned)
	{
		memset(pMem1Unaligned, kByte1, size);
		memset(pMem2Unaligned, kByte2, size);

		// Set gMem1/gMem2 to be kBaseMemSize bytes into pMem1Unaligned and be of alignment = kBaseMemAlignment.
		gMem1 = (uint8_t*)(((uintptr_t)pMem1Unaligned + kBaseMemSize + (kBaseMemAlignment - 1)) & ~(kBaseMemAlignment - 1));
		gMem2 = (uint8_t*)(((uintptr_t)pMem2Unaligned + kBaseMemSize + (kBaseMemAlignment - 1)) & ~(kBaseMemAlignment - 1));

		nErrorCount += TestEAAlloca();
		nErrorCount += TestEAMalloca();
		nErrorCount += TestMemset();
		nErrorCount += TestMemfill();
		nErrorCount += TestMemclear();
		nErrorCount += TestMemcheck();
		nErrorCount += TestMemchr();
		nErrorCount += TestMemcmp();
		nErrorCount += TestMemmem();
		nErrorCount += TestMemcpy();
		nErrorCount += TestMemmove();
		nErrorCount += TestTimingSafe();

		TestMemcpySpeed();
		TestMemmoveSpeed();
		TestMemsetSpeed();
		TestMemclearSpeed();

		EA_CACHE_PREFETCH_128(gMem1);
		EA_CACHE_ZERO_128(gMem1);

		delete[] pMem1Unaligned;
		delete[] pMem2Unaligned;
	}

	// template<size_t> StaticMemory
	struct MyClass{ char buffer[37]; };
	EA::StdC::StaticMemory<sizeof(MyClass)> mStaticMemory;
	MyClass* pClass = new(mStaticMemory.Memory()) MyClass;
	memset(pClass->buffer, 0, sizeof(pClass->buffer));
	EATEST_VERIFY(EA::StdC::Memcheck8(pClass->buffer, 0, sizeof(pClass->buffer)) == NULL);

	return nErrorCount;
}

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif






