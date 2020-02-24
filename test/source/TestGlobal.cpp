///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EAGlobal.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif



struct GlobalTestObject
{
	int sequence;
	static int sSeqCounter;

	GlobalTestObject() 
	  : sequence(++sSeqCounter) {}
};

int GlobalTestObject::sSeqCounter;

// This structure will require 8 byte alignment on most platforms
struct AlignedTestObject
{
	uint64_t data;
};

int TestGlobal()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	const uint32_t id = 0x8e9c5fd7;

	GlobalTestObject::sSeqCounter = 0;

	// create an OS global object and verify that the same one is returned on a second query
	{
		AutoOSGlobalPtr<GlobalTestObject, id> pObj;

		EATEST_VERIFY(pObj->sequence == 1);

		{
			AutoOSGlobalPtr<GlobalTestObject, id> pObj2;

			EATEST_VERIFY(pObj.get() == pObj2.get());
			EATEST_VERIFY(pObj2->sequence == 1);
		}
	}

	// create another OS global object; the last one should have been destroyed so this
	// one should be a different count
	{
		AutoStaticOSGlobalPtr<GlobalTestObject, id> pObj;

		EATEST_VERIFY(pObj->sequence == 2);

		{
			AutoStaticOSGlobalPtr<GlobalTestObject, id> pObj2;

			EATEST_VERIFY(pObj.get() == pObj2.get());
			EATEST_VERIFY(pObj2->sequence == 2);
		}
	}

	{
		AutoStaticOSGlobalPtr<AlignedTestObject, 0x52534562> pObj1;
		AutoStaticOSGlobalPtr<AlignedTestObject, 0x93715355> pObj2;
	
		size_t requiredAlignment = EA_ALIGN_OF(AlignedTestObject);
		// Ensure the two objects are unique.
		EATEST_VERIFY(pObj1.get() != pObj2.get() );
		EATEST_VERIFY( (reinterpret_cast<uintptr_t>(pObj1.get()) % requiredAlignment) == 0 );
		EATEST_VERIFY( (reinterpret_cast<uintptr_t>(pObj2.get()) % requiredAlignment) == 0 );
		pObj1->data = 1;
		pObj2->data = 2;
		// Verify the data is correct in the objects.  This is done to catch errors where the two global objects could potentially overlap.
		EATEST_VERIFY( pObj1->data == 1 );
		EATEST_VERIFY( pObj2->data == 2 );
	}

	// create a global pointer object
	{
		const uint32_t kGUID = 0xef020e29;

		GlobalPtr<int, kGUID> pMemory;

		EATEST_VERIFY(!pMemory);  // Global pointer should be nulled on creation.

		if(pMemory)  // Global pointer should be nulled on creation.
			EATEST_VERIFY(pMemory != NULL);

		int* const p = new int[2];
		pMemory = p;

		EATEST_VERIFY((int*)pMemory == p);

		pMemory[0] = 10;
		pMemory[1] = 20;

		{
			GlobalPtr<int, kGUID> pMemory2A;
			GlobalPtr<int, kGUID> pMemory2B(pMemory2A);

			EATEST_VERIFY(pMemory2A == pMemory);
			EATEST_VERIFY(pMemory2B == pMemory);
			EATEST_VERIFY(*pMemory2A == 10);
			EATEST_VERIFY(pMemory2A[0] == 10);
			EATEST_VERIFY(pMemory2A[1] == 20);
		}

		pMemory = NULL;
		EATEST_VERIFY(!pMemory);
		delete[] p;
	}

	return nErrorCount;
}














