///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EAAlignment.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>

// Conditionally include SSE headers which some of the tests conditionally rely upon.
#if EA_SSE
	#if EA_SSE >= 1
		#include <xmmintrin.h>
	#endif
	#if EA_SSE >= 2
		#include <emmintrin.h>
	#endif

	#if EA_SSE >= 1
		EASTDC_DECLARE_TRIVIAL_DESTRUCTOR(__m128)
		EASTDC_DECLARE_TRIVIAL_DESTRUCTOR(__m128i)
	#endif
#endif


using namespace EA::StdC;
using namespace EA::UnitTest;



///////////////////////////////////////////////////////////////////////////////
// TestStruct
//
// Helper class for our unit test.
//
struct TestStruct
{
	uint32_t a;
	uint64_t b;
	char     c;
	void*    d;

	TestStruct() : a(0), b(0), c(0), d(0) {}
};


///////////////////////////////////////////////////////////////////////////////
// TestTemplate
//
// Helper class for our unit test.
//
template <typename T>
struct TestTemplate
{
	uint32_t a;
	uint64_t b;
	char     c;
	void*    d;

	TestTemplate() : a(0), b(0), c(0), d(0) {}
};




///////////////////////////////////////////////////////////////////////////////
// TestAlignment
//
int TestAlignment()
{
	int nErrorCount(0);

	///////////////////////////////////////////////////////////////////////////
	// EAAlignOf
	///////////////////////////////////////////////////////////////////////////
	{
		size_t a;

		// Type-based AlignOf
		a = EAAlignOf(uint32_t);
		EATEST_VERIFY(a == sizeof(uint32_t));

		#if (EA_PLATFORM_WORD_SIZE >= 8)
			a = EAAlignOf(uint64_t);
			EATEST_VERIFY(a == sizeof(uint64_t));
		#endif

		a = EAAlignOf(TestStruct);
		EATEST_VERIFY(((a * 2) / 2) == a); // This should always be true.

		a = EAAlignOf(TestTemplate<int>);
		EATEST_VERIFY(((a * 2) / 2) ==  a); // This should always be true.


		// Instance-based AlignOf
		uint32_t x32 = 0;
		a = AlignOf(x32);
		EATEST_VERIFY(a == sizeof(uint32_t));

		#if (EA_PLATFORM_WORD_SIZE >= 8)
			uint64_t x64 = 0;
			a = AlignOf(x64);
			EATEST_VERIFY(a == sizeof(uint64_t));
		#endif

		TestStruct testStruct;
		a = AlignOf(testStruct);
		EATEST_VERIFY(((a * 2) / 2) == a); // This should always be true.

		TestTemplate<int> testTemplate;
		a = AlignOf(testTemplate);
		EATEST_VERIFY(((a * 2) / 2) == a); // This should always be true.
	}


	///////////////////////////////////////////////////////////////////////////
	// AlignAddressUp, AlignObjectUp, AlignAddressDown, AlignObjectDown, GetAlignment
	///////////////////////////////////////////////////////////////////////////
	{
		void*  pResult;

		for(char* p = 0; p < (char*)100000; p++)
		{
			for(size_t i = 0; i < 18; i++)
			{
				uintptr_t a = ((size_t)1 << i);

				pResult = AlignAddressUp(p, a);
				EATEST_VERIFY(((uintptr_t)pResult % a) == (uintptr_t)0);

				pResult = AlignObjectUp(p, a);
				EATEST_VERIFY(((uintptr_t)pResult % a) == (uintptr_t)0);

				pResult = AlignAddressDown(p, ((size_t)1 << i));
				EATEST_VERIFY(((uintptr_t)pResult % a) == (uintptr_t)0);

				pResult = AlignObjectDown(p, ((size_t)1 << i));
				EATEST_VERIFY(((uintptr_t)pResult % a) == (uintptr_t)0);

				size_t alignment = GetAlignment(pResult);
				EATEST_VERIFY(alignment >= a); // We test >= because sometimes values are coincidentally aligned to a higher value than intended. Which is OK.

				bool bResult = IsAligned(pResult, a);
				EATEST_VERIFY(bResult);
			}
		}

		for(TestStruct* p = 0, *pTestStruct; p < (TestStruct*)100000; p++)
		{
			for(size_t i = 0; i < 18; i++)
			{
				uintptr_t a = ((size_t)1 << i);

				pTestStruct = AlignObjectUp(p, a);
				EATEST_VERIFY(((uintptr_t)pTestStruct % a) == (uintptr_t)0);

				pTestStruct = AlignObjectDown(p, ((size_t)1 << i));
				EATEST_VERIFY(((uintptr_t)pTestStruct % a) == (uintptr_t)0);

				size_t alignment = GetAlignment(pTestStruct);
				EATEST_VERIFY(alignment >= a); // We test >= because sometimes values are coincidentally aligned to a higher value than intended. Which is OK.

				bool bResult = IsAligned(pTestStruct, a);
				EATEST_VERIFY(bResult);
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////
	// AlignAddressUp
	///////////////////////////////////////////////////////////////////////////
	{
		for(uintptr_t address = 0; address < 1024; ++address)
		{
			for(size_t a = 1; a < 1024*1024; a <<= 1)
			{
				const void* up   = AlignAddressUp  ((void*)address, a);
				const void* down = AlignAddressDown((void*)address, a);

				EATEST_VERIFY(((uintptr_t)up   >= address) && IsAligned(up,   a));
				EATEST_VERIFY(((uintptr_t)down <= address) && IsAligned(down, a));
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////
	// AlignUp, AlignDown
	///////////////////////////////////////////////////////////////////////////
	{
		for(uint8_t u8 = 0; u8 < 150; ++u8)
		{
			for(size_t a = 1; a < 64; a <<= 1)
			{
				uint8_t n = AlignUp(u8, a);
				EATEST_VERIFY(IsAligned(n, a) && (n >= u8));

				n = AlignDown(u8, a);
				EATEST_VERIFY(IsAligned(n, a) && (n <= u8));
			}
		}

		for(int16_t i16 = 0; i16 < 10000; ++i16)
		{
			for(size_t a = 1; a < 1024; a <<= 1)
			{
				int16_t n = AlignUp(i16, a);
				EATEST_VERIFY(IsAligned(n, a) && (n >= i16));

				n = AlignDown(i16, a);
				EATEST_VERIFY(IsAligned(n, a) && (n <= i16));
			}
		}

		for(uint32_t u32 = 0; u32 < 10000; ++u32)
		{
			for(size_t a = 1; a < 1024; a <<= 1)
			{
				uint32_t n = AlignUp(u32, a);
				EATEST_VERIFY(IsAligned(n, a) && (n >= u32));

				n = AlignDown(u32, a);
				EATEST_VERIFY(IsAligned(n, a) && (n <= u32));
			}
		}

		for(int64_t i64 = 0; i64 < 10000; ++i64)
		{
			for(size_t a = 1; a < 1024; a <<= 1)
			{
				int64_t n = AlignUp(i64, a);
				EATEST_VERIFY(IsAligned(n, a) && (n >= i64));

				n = AlignDown(i64, a);
				EATEST_VERIFY(IsAligned(n, a) && (n <= i64));
			}
		}

		for(char* p8 = 0; p8 < (char*)(uintptr_t)1000; ++p8)
		{
			for(size_t a = 1; a < 64; a <<= 1)
			{
				TestStruct* pTestStruct = (TestStruct*)(uintptr_t)p8;
				pTestStruct = AlignUp(pTestStruct, a);
				EATEST_VERIFY(IsAligned(pTestStruct, a) && ((uintptr_t)pTestStruct >= (uintptr_t)p8));

				pTestStruct = (TestStruct*)(uintptr_t)p8;
				pTestStruct = AlignDown(pTestStruct, a);
				EATEST_VERIFY(IsAligned(pTestStruct, a) && ((uintptr_t)pTestStruct <= (uintptr_t)p8));
			}
		}

		for(uint64_t x = 0; x < 1024; ++x)
		{
			for(size_t a = 1; a < 1024*1024; a <<= 1)
			{
				const uint64_t up   = AlignUp(x, a);
				const uint64_t down = AlignDown(x, a);

				EATEST_VERIFY((up   >= x) && IsAligned(up, a));
				EATEST_VERIFY((down <= x) && IsAligned(up, a));

				const uint64_t up1   = AlignUp  <uint64_t, 1>(x);
				const uint64_t down1 = AlignDown<uint64_t, 1>(x);

				EATEST_VERIFY((up1   >= x) && IsAligned(up1, 1));
				EATEST_VERIFY((down1 <= x) && IsAligned(up1, 1));

				const uint64_t up2   = AlignUp  <uint64_t, 2>(x);
				const uint64_t down2 = AlignDown<uint64_t, 2>(x);

				EATEST_VERIFY((up2   >= x) && IsAligned(up2, 2));
				EATEST_VERIFY((down2 <= x) && IsAligned(up2, 2));

				const uint64_t up4   = AlignUp  <uint64_t, 4>(x);
				const uint64_t down4 = AlignDown<uint64_t, 4>(x);

				EATEST_VERIFY((up4   >= x) && IsAligned(up4, 4));
				EATEST_VERIFY((down4 <= x) && IsAligned(up4, 4));

				const uint64_t up8   = AlignUp  <uint64_t, 8>(x);
				const uint64_t down8 = AlignDown<uint64_t, 8>(x);

				EATEST_VERIFY((up8   >= x) && IsAligned(up8, 8));
				EATEST_VERIFY((down8 <= x) && IsAligned(up8, 8));

				const uint64_t up16   = AlignUp  <uint64_t, 16>(x);
				const uint64_t down16 = AlignDown<uint64_t, 16>(x);

				EATEST_VERIFY((up16   >= x) && IsAligned(up16, 16));
				EATEST_VERIFY((down16 <= x) && IsAligned(up16, 16));
			}
		}

		// Try again with address values around INT32_MAX
		for(uint64_t x = 0x7ffffff0; x < 0x800000ff; ++x)
		{
			for(size_t a = 1; a < 1024*1024; a <<= 1)
			{
				const uint64_t up   = AlignUp(x, a);
				const uint64_t down = AlignDown(x, a);

				EATEST_VERIFY((up   >= x) && IsAligned(up, a));
				EATEST_VERIFY((down <= x) && IsAligned(up, a));

				const uint64_t up1   = AlignUp  <uint64_t, 1>(x);
				const uint64_t down1 = AlignDown<uint64_t, 1>(x);

				EATEST_VERIFY((up1   >= x) && IsAligned(up1, 1));
				EATEST_VERIFY((down1 <= x) && IsAligned(up1, 1));

				const uint64_t up2   = AlignUp  <uint64_t, 2>(x);
				const uint64_t down2 = AlignDown<uint64_t, 2>(x);

				EATEST_VERIFY((up2   >= x) && IsAligned(up2, 2));
				EATEST_VERIFY((down2 <= x) && IsAligned(up2, 2));

				const uint64_t up4   = AlignUp  <uint64_t, 4>(x);
				const uint64_t down4 = AlignDown<uint64_t, 4>(x);

				EATEST_VERIFY((up4   >= x) && IsAligned(up4, 4));
				EATEST_VERIFY((down4 <= x) && IsAligned(up4, 4));

				const uint64_t up8   = AlignUp  <uint64_t, 8>(x);
				const uint64_t down8 = AlignDown<uint64_t, 8>(x);

				EATEST_VERIFY((up8   >= x) && IsAligned(up8, 8));
				EATEST_VERIFY((down8 <= x) && IsAligned(up8, 8));

				const uint64_t up16   = AlignUp  <uint64_t, 16>(x);
				const uint64_t down16 = AlignDown<uint64_t, 16>(x);

				EATEST_VERIFY((up16   >= x) && IsAligned(up16, 16));
				EATEST_VERIFY((down16 <= x) && IsAligned(up16, 16));
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////
	// IsAligned
	///////////////////////////////////////////////////////////////////////////
	{
		// template <typename T, int a>
		// inline bool IsAligned(T x)
		bool b = IsAligned<int, 8>(64);
		EATEST_VERIFY(b);
		b = IsAligned<int, 8>(67);
		EATEST_VERIFY(!b);

		// template <typename T, int a>
		// inline bool IsAligned(T* p)
		b = IsAligned<int, 64>((int*)NULL + 0);
		EATEST_VERIFY(b);
		b = IsAligned<int, 64>((int*)NULL + 1);
		EATEST_VERIFY(!b);

		// template <typename T>
		// inline bool IsAligned(T x, size_t a)
		EATEST_VERIFY( IsAligned(64, 8));
		EATEST_VERIFY(!IsAligned(67, 8));


		// template <typename T>
		// inline bool IsAligned(T* p, size_t a)
		EATEST_VERIFY( IsAligned((int*)NULL + 0, 64));
		EATEST_VERIFY(!IsAligned((int*)NULL + 1, 64));
	}


	///////////////////////////////////////////////////////////////////////////
	// AlignedType
	///////////////////////////////////////////////////////////////////////////
	{

		// Disabled until we can find a way that works for all compilers.
		#if 0 // !defined(__GNUC__) || (__GNUC__ < 4) // GCC4 lost the ability to do this
			const size_t kAlignment = 128;

			AlignedType<int, kAlignment>::Type intAlignedAt128;

			EATEST_VERIFY(((uintptr_t)&intAlignedAt128) % kAlignment == (uintptr_t)0);

			intAlignedAt128 = 12345;

			EATEST_VERIFY(intAlignedAt128 == 12345);
		#endif
	}


	///////////////////////////////////////////////////////////////////////////
	// AlignedArray
	///////////////////////////////////////////////////////////////////////////
	{
		const size_t kSize = 10;
		const size_t kAlignment = 64;

		{
			AlignedArray<TestStruct, kSize, kAlignment> mTestStructArray;
			
			// Note that as of this writing the convention is that only the first element 
			// of the array should be aligned. Other elements should follow the first as
			// they naturally would otherwise.
			EATEST_VERIFY(((uintptr_t)&mTestStructArray[0]) % kAlignment == (uintptr_t)0);

			mTestStructArray[0] = mTestStructArray[1];

			AlignedArray<TestStruct, kSize, kAlignment> mTestStructArray2(mTestStructArray);

			mTestStructArray = mTestStructArray2;
		}

		// These tests instantiate SSE types to ensure they are supported by the AlignedArray.  Previously,
		// these tests didn't work on Clang.
		#if defined EA_SSE && (EA_SSE >= 1)
			{
				AlignedArray<__m128, kSize, kAlignment> mTestVectorArray;
				
				// Note that as of this writing the convention is that only the first element 
				// of the array should be aligned. Other elements should follow the first as
				// they naturally would otherwise.
				EATEST_VERIFY(((uintptr_t)&mTestVectorArray[0]) % kAlignment == (uintptr_t)0);

				mTestVectorArray[0] = mTestVectorArray[1];
			}
		#endif

		#if defined EA_SSE && (EA_SSE >= 2)
			{
				AlignedArray<__m128i, kSize, kAlignment> mTestVectorArray;
				
				// Note that as of this writing the convention is that only the first element 
				// of the array should be aligned. Other elements should follow the first as
				// they naturally would otherwise.
				EATEST_VERIFY(((uintptr_t)&mTestVectorArray[0]) % kAlignment == (uintptr_t)0);

				mTestVectorArray[0] = mTestVectorArray[1];
			}
		#endif
	}


	///////////////////////////////////////////////////////////////////////////
	// AlignedObject
	///////////////////////////////////////////////////////////////////////////
	{
		const size_t kAlignment = 64;
		AlignedObject<TestStruct, kAlignment> mTestStruct;
		
		EATEST_VERIFY((uintptr_t)&mTestStruct % kAlignment == (uintptr_t)0);

		mTestStruct->a = 1;
		EATEST_VERIFY(mTestStruct->a == (unsigned)1);

		// (*(&mTestStruct))->b = 2; This is not possible; we need to use '.'
		(*(&mTestStruct)).b = 2;
		EATEST_VERIFY(mTestStruct->b == (unsigned)2);

		AlignedObject<TestStruct, kAlignment> mTestStruct2(mTestStruct);

		mTestStruct = mTestStruct2;

		// Todo: Test the following
		// T* operator &()
		// T* Get()
		// const T* Get() const
		// operator T&()
		// operator const T&() const
		// T* operator->()
		// const T* operator->() const
		// operator T*()
		// operator const T*() const
	}

	return nErrorCount;
}











