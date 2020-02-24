///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EABitTricks.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>
#include <EAAssert/eaassert.h>


struct OffsetofTest
{
	uint32_t x0;
	uint32_t x1;
};

struct OffsetofTest1
{
	uint32_t x[2];
};


struct A{ int a; };
struct B{ int b; };
struct C : public A, public B{ int c; };


int TestBitTricks()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	int       n1 = 0, n2 = 0;
	bool       b = false;
	int        i = 0;
	unsigned   u = 0;
	int8_t    i8 = 0;
	uint8_t   u8 = 0;
	int16_t  i16 = 0;
	uint16_t u16 = 0;
	int32_t  i32 = 0;
	uint32_t u32 = 0;
	int64_t  i64 = 0;
	uint64_t u64 = 0;

	// TurnOffLowestBit
	i16 = TurnOffLowestBit((int16_t)0x0005);    // 000000101
	EATEST_VERIFY(i16 == 0x0004);               // 000000100

	u16 = TurnOffLowestBit((uint16_t)0x0005);
	EATEST_VERIFY(u16 == 0x0004);

	i32 = TurnOffLowestBit((int32_t)0x00000058);    // 01011000
	EATEST_VERIFY(i32 == 0x00000050);               // 01010000

	u32 = TurnOffLowestBit((uint32_t)0x00000058);
	EATEST_VERIFY(u32 == 0x00000050);

	u32 = TurnOffLowestBit((uint32_t)0x00000000);
	EATEST_VERIFY(u32 == 0x00000000);


	// IsolateLowestBit
	i16 = IsolateLowestBit((int16_t)0x000C);    // 1100
	EATEST_VERIFY(i16 == 0x0004);               // 0100

	u16 = IsolateLowestBit((uint16_t)0x000C);
	EATEST_VERIFY(u16 == 0x0004);

	i32 = IsolateLowestBit((int32_t)0x00000058);    // 01011000
	EATEST_VERIFY(i32 == 0x00000008);               // 00001000

	u32 = IsolateLowestBit((uint32_t)0x00000058);
	EATEST_VERIFY(u32 == 0x00000008);

	u32 = IsolateLowestBit((uint32_t)0x00000000);
	EATEST_VERIFY(u32 == 0x00000000);


	// IsolateLowest0Bit
	i16 = IsolateLowest0Bit((int16_t)0x0003);   // 0011
	EATEST_VERIFY(i16 == 0x0004);               // 0100

	u16 = IsolateLowest0Bit((uint16_t)0x0003);
	EATEST_VERIFY(u16 == 0x0004);

	i32 = IsolateLowest0Bit((int32_t)0x000000a7);   // 10100111
	EATEST_VERIFY(i32 == 0x00000008);               // 00001000

	u32 = IsolateLowest0Bit((uint32_t)0x000000a7);
	EATEST_VERIFY(u32 == 0x00000008);

	u32 = IsolateLowest0Bit((uint32_t)0xffffffff);
	EATEST_VERIFY(u32 == 0x00000000);


	// GetTrailing0Bits
	i16 = GetTrailing0Bits((int16_t)0x000C);    // 1100
	EATEST_VERIFY(i16 == 0x0003);               // 0011

	u16 = GetTrailing0Bits((uint16_t)0x000C);
	EATEST_VERIFY(u16 == 0x0003);

	i32 = GetTrailing0Bits((int32_t)0x00000058);    // 01011000
	EATEST_VERIFY(i32 == 0x00000007);               // 00000111

	u32 = GetTrailing0Bits((uint32_t)0x00000058);
	EATEST_VERIFY(u32 == 0x00000007);

	u32 = GetTrailing0Bits((uint32_t)0xffffffff);
	EATEST_VERIFY(u32 == 0x00000000);


	// GetTrailing1And0Bits
	i16 = GetTrailing1And0Bits((int16_t)0x000C);    // 1100
	EATEST_VERIFY(i16 ==0x0007);                    // 0111

	u16 = GetTrailing1And0Bits((uint16_t)0x000C);
	EATEST_VERIFY(u16 == 0x0007);

	i32 = GetTrailing1And0Bits((int32_t)0x00000058);    // 01011000
	EATEST_VERIFY(i32 == 0x0000000f);                   // 00001111

	u32 = GetTrailing1And0Bits((uint32_t)0x00000058);
	EATEST_VERIFY(u32 == 0x0000000f);

	u32 = GetTrailing1And0Bits((uint32_t)0xffffffff);
	EATEST_VERIFY(u32 == 0x00000001);


	// PropogateLowestBitDownward
	i16 = PropogateLowestBitDownward((int16_t)0x0004);  // 0100
	EATEST_VERIFY(i16 == 0x007);                               // 0111

	u16 = PropogateLowestBitDownward((uint16_t)0x0004);
	EATEST_VERIFY(u16 == 0x007);

	i32 = PropogateLowestBitDownward((int32_t)0x00000058);  // 01011000
	EATEST_VERIFY(i32 == 0x0000005f);                              // 01011111

	u32 = PropogateLowestBitDownward((uint32_t)0x00000058);
	EATEST_VERIFY(u32 == 0x0000005f);

	u32 = PropogateLowestBitDownward((uint32_t)0x00000000);
	EATEST_VERIFY(u32 == 0xffffffff);


	// TurnOffLowestContiguousBits
	i16 = TurnOffLowestContiguousBits((int16_t)0x000b); // 1011
	EATEST_VERIFY(i16 == 0x0008);                              // 1000

	u16 = TurnOffLowestContiguousBits((uint16_t)0x000b);
	EATEST_VERIFY(u16 == 0x0008);

	i32 = TurnOffLowestContiguousBits((int32_t)0x00000058); // 01011000
	EATEST_VERIFY(i32 == 0x00000040);                              // 01000000

	u32 = TurnOffLowestContiguousBits((uint32_t)0x00000058);
	EATEST_VERIFY(u32 == 0x00000040);

	u32 = TurnOffLowestContiguousBits((uint32_t)0xffffffff);
	EATEST_VERIFY(u32 == 0x00000000);

	u32 = TurnOffLowestContiguousBits((uint32_t)0x00000000);
	EATEST_VERIFY(u32 == 0x00000000);


	/// TurnOnLowest0Bit
	i16 = TurnOnLowest0Bit((int16_t)0x0003);    // 0011
	EATEST_VERIFY(i16 == 0x0007);               // 0111

	u16 = TurnOnLowest0Bit((uint16_t)0x0003);
	EATEST_VERIFY(u16 == 0x0007);

	i32 = TurnOnLowest0Bit((int32_t)0x000000a7);    // 10100111
	EATEST_VERIFY(i32 == 0x000000af);               // 10101111

	u32 = TurnOnLowest0Bit((uint32_t)0x000000a7);
	EATEST_VERIFY(u32 == 0x000000af);

	u32 = TurnOnLowest0Bit((uint32_t)0xffffffff);
	EATEST_VERIFY(u32 == 0xffffffff);


	// GetNextWithEqualBitCount
	u = 45;
	for(u64 = 0; u64 < UINT32_MAX; u64++)
	{
		unsigned u2;
		n1 = CountBits(u);
		u2 = GetNextWithEqualBitCount(u);
		if(u2 < u)
			break; // This function can only go up to max int.
		n2 = CountBits(u2);
		EATEST_VERIFY(n1 == n2);
		u = u2;
	}


	// IsolateSingleBits
	u = IsolateSingleBits((unsigned)0x2ba9aba3);          // 00101011101010011010101110100011
	EATEST_VERIFY(u == 0x28282820);             		  // 00101000001010000010100000100000



	// IsolateSingle0Bits
	u = IsolateSingle0Bits((unsigned)0x2ba9aba3);         // 00101011101010011010101110100011
	EATEST_VERIFY(u == 0x14505440);                       // 00010100010100000101010001000000



	// IsolateSingle0And1Bits
	u = IsolateSingle0And1Bits(0xabababab);     // 10101011101010111010101110101011
	EATEST_VERIFY(u == 0xfc7c7c7c);             // 11111100011111000111110001111100



	// ShiftRightSigned
	i32 = ShiftRightSigned((int32_t)0xc0000000, 2);      // 11000000 00000000 00000000 00000000
	EATEST_VERIFY(i32 == (int32_t)0xf0000000);           // 11110000 00000000 00000000 00000000

	i32 = ShiftRightSigned((int32_t)0x00000000, 2);      // 00000000 00000000 00000000 00000000
	EATEST_VERIFY(i32 == 0x00000000);                    // 00000000 00000000 00000000 00000000



	// CountTrailing0Bits
	i = CountTrailing0Bits((uint32_t)0x00000000); // 00000000 00000000 00000000 00000000
	EATEST_VERIFY(i == 32);

	i = CountTrailing0Bits((uint32_t)0x000000a8); // 00000000 00000000 00000000 10101000
	EATEST_VERIFY(i == 3);

	i = CountTrailing0Bits((uint32_t)0xffffffff); // 11111111 11111111 11111111 11111111
	EATEST_VERIFY(i == 0);


	// CountLeading0Bits
	i = CountLeading0Bits((uint32_t)0x00000000);  // 00000000 00000000 00000000 00000000
	EATEST_VERIFY(i == 32);

	i = CountLeading0Bits((uint32_t)0x37000000);  // 00110111 00000000 00000000 00000000
	EATEST_VERIFY(i == 2);

	i = CountLeading0Bits((uint32_t)0xffffffff);  // 11111111 11111111 11111111 11111111
	EATEST_VERIFY(i == 0);



	// CountTrailing0Bits
	i = CountTrailing0Bits(UINT64_C(0x0000000000000000)); // 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	EATEST_VERIFY(i == 64);

	i = CountTrailing0Bits(UINT64_C(0x00000000000000a8)); // 00000000 00000000 00000000 00000000 00000000 00000000 00000000 10101000
	EATEST_VERIFY(i == 3);

	i = CountTrailing0Bits(UINT64_C(0x0f00000000000000)); // 00001111 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	EATEST_VERIFY(i == 56);

	i = CountTrailing0Bits(UINT64_C(0xffffffffffffffff)); // 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
	EATEST_VERIFY(i == 0);


	// CountLeading0Bits
	i = CountLeading0Bits(UINT64_C(0x0000000000000000));  // 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	EATEST_VERIFY(i == 64);

	i = CountLeading0Bits(UINT64_C(0x0000000037000000));  // 00000000 00000000 00000000 00000000 00110111 00000000 00000000 00000000
	EATEST_VERIFY(i == 34);

	i = CountLeading0Bits(UINT64_C(0x0f00000000000000));  // 00000000 11111111 00000000 00000000 00000000 00000000 00000000 00000000
	EATEST_VERIFY(i == 4);

	i = CountLeading0Bits(UINT64_C(0xffffffffffffffff));  // 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
	EATEST_VERIFY(i == 0);


	// CountBits
	i = CountBits(0x11001010);
	EATEST_VERIFY(i == 4);

	i = CountBits64(UINT64_C(0x1100101011001010));
	EATEST_VERIFY(i == 8);

	i = CountBits64(UINT64_C(0xEBADD76DEBADD76D));
	EATEST_VERIFY(i == 44);


	// RotateLeft
	u32 = RotateLeft(0xf0000000, 2);        // 11110000 00000000 00000000 00000000
	EATEST_VERIFY(u32 == 0xc0000003);       // 11000000 00000000 00000000 00000011


	// RotateRight
	u32 = RotateRight(0xf0000000, 2);       // 11110000 00000000 00000000 00000000
	EATEST_VERIFY(u32 == 0x3c000000);       // 00111100 00000000 00000000 00000000


	// ReverseBits
	u32 = ReverseBits(0xe0000001);          // 11100000 00000000 00000000 00000001
	EATEST_VERIFY(u32 == 0x80000007);       // 10000000 00000000 00000000 00000111

	u64 = ReverseBits(UINT64_C(0xE00CE00400000001));    // 11100000 00001100 11100000 00000100 00000000 00000000 00000000 00000001
	EATEST_VERIFY(u64 == UINT64_C(0x8000000020073007)); // 10000000 00000000 00000000 00000000 00100000 00000111 00110000 00000111


	// IsolateHighestBit
	u32 = IsolateHighestBit(UINT32_C(0x00000044));    // 01000100
	EATEST_VERIFY(u32 == 0x00000040);                 // 01000000

	u32 = IsolateHighestBit(UINT32_C(0x00000000));
	EATEST_VERIFY(u32 == 0x00000000);


	// IsolateHighest0Bit
	u32 = IsolateHighest0Bit(UINT32_C(0x44000000)); // 01000100 00000000 00000000 00000000
	EATEST_VERIFY(u32 == 0x80000000);               // 10000000 00000000 00000000 00000000

	u32 = IsolateHighest0Bit(UINT32_C(0xffffffff)); // 11111111 11111111 11111111 11111111
	EATEST_VERIFY(u32 == 0x00000000);               // 00000000 00000000 00000000 00000000


	// PropogateHighestBitDownward
	u32 = PropogateHighestBitDownward(UINT32_C(0x48000000));  // 01001000 00000000 00000000 00000000
	EATEST_VERIFY(u32 == 0x7FFFFFFF);                         // 01111111 11111111 11111111 11111111

	u32 = PropogateHighestBitDownward(UINT32_C(0x00000000));  // 00000000 00000000 00000000 00000000
	EATEST_VERIFY(u32 == 0x00000000);                         // 00000000 00000000 00000000 00000000


	// GetHighestContiguous0Bits
	u32 = GetHighestContiguous0Bits(UINT32_C(0x19000000));  // 00011001 00000000 00000000 00000000
	EATEST_VERIFY(u32 == 0xe0000000);                       // 11100000 00000000 00000000 00000000

	u32 = GetHighestContiguous0Bits(UINT32_C(0xff000000));  // 11111111 00000000 00000000 00000000
	EATEST_VERIFY(u32 == 0x00000000);                       // 00000000 00000000 00000000 00000000


	// GetBitwiseEquivalence
	u32 = GetBitwiseEquivalence((uint32_t)0x000000cc, (uint32_t)0x000000f0);    // 11001100, 11110000
	EATEST_VERIFY(u32 == 0xffffffc3);                                                  // 11000011


	// AreLessThan2BitsSet
	b = AreLessThan2BitsSet((uint32_t)0x00001000);
	EATEST_VERIFY(b);

	b = AreLessThan2BitsSet((uint32_t)0x01001110);
	EATEST_VERIFY(!b);

	b = AreLessThan2BitsSet((uint64_t)0x00001000);
	EATEST_VERIFY(b);

	b = AreLessThan2BitsSet((uint64_t)0x01001110);
	EATEST_VERIFY(!b);

	b = AreLessThan2BitsSet((uint32_t)0);
	EATEST_VERIFY(b);

	b = AreLessThan2BitsSet((uint64_t)0);
	EATEST_VERIFY(b);


	// GetHighestBit
	u8 = GetHighestBit((uint8_t)0);
	EATEST_VERIFY(u8 == 0x80);

	u16 = GetHighestBit((uint16_t)0);
	EATEST_VERIFY(u16 == 0x8000);

	u32 = GetHighestBit((uint32_t)0);
	EATEST_VERIFY(u32 == 0x80000000);

	u64 = GetHighestBit((uint64_t)0);
	EATEST_VERIFY(u64 == UINT64_C(0x8000000000000000));


	// IsPowerOf2
	u8 = 0; // This version returns true for zero.
	b = IsPowerOf2(u8);
	EATEST_VERIFY(b);

	u8 = 37;
	b = IsPowerOf2(u8);
	EATEST_VERIFY(!b);

	u8 = 128;
	b = IsPowerOf2(u8);
	EATEST_VERIFY(b);

	u32 = 65537;
	b = IsPowerOf2(u32);
	EATEST_VERIFY(!b);

	u32 = 65536;
	b = IsPowerOf2(u32);
	EATEST_VERIFY(b);

	u32 = 0;
	b = IsPowerOf2(u32);
	EATEST_VERIFY(b);

	u64 = 0;
	b = IsPowerOf2(u64);
	EATEST_VERIFY(b);

	u64 = 65537;
	b = IsPowerOf2(u64);
	EATEST_VERIFY(!b);

	u64 = 65536;
	b = IsPowerOf2(u64);
	EATEST_VERIFY(b);


	// RoundUpToPowerOf2
	u32 = RoundUpToPowerOf2(UINT32_C(66));
	EATEST_VERIFY(u32 == 128);

	u32 = RoundUpToPowerOf2(UINT32_C(12));
	EATEST_VERIFY(u32 == 16);

	u32 = RoundUpToPowerOf2(UINT32_C(4));
	EATEST_VERIFY(u32 == 4);

	u32 = RoundUpToPowerOf2(UINT32_C(0));
	EATEST_VERIFY(u32 == 0);


	// IsMultipleOf
	b = IsMultipleOf<int, 4>(3);
	EATEST_VERIFY(!b);

	b = IsMultipleOf<int, 16>(32);
	EATEST_VERIFY(b);


	// IsPowerOf2Minus1
	b = IsPowerOf2Minus1((uint32_t)0x0000000f);
	EATEST_VERIFY(b);

	b = IsPowerOf2Minus1((uint32_t)0x0000000b);
	EATEST_VERIFY(!b);

	b = IsPowerOf2Minus1((uint32_t)0x00000000);
	EATEST_VERIFY(b);


	// CrossesPowerOf2
	b = CrossesPowerOf2((int)4, (int)5, (int)8);
	EATEST_VERIFY(!b);

	b = CrossesPowerOf2((int)5, (int)9, (int)8);
	EATEST_VERIFY(b);


	// CrossesPowerOf2
	b = CrossesPowerOf2<int, 8>(4, 5);
	EATEST_VERIFY(!b);

	b = CrossesPowerOf2<int, 8>(5, 9);
	EATEST_VERIFY(b);


	// GetHighestBitPowerOf2
	u32 = GetHighestBitPowerOf2((uint32_t)0);
	EATEST_VERIFY(u32 == 0);

	u32 = GetHighestBitPowerOf2((uint32_t)1);
	EATEST_VERIFY(u32 == 0);

	u32 = GetHighestBitPowerOf2((uint32_t)2);
	EATEST_VERIFY(u32 == 1);

	u32 = GetHighestBitPowerOf2((uint32_t)3);
	EATEST_VERIFY(u32 == 1);

	u32 = GetHighestBitPowerOf2((uint32_t)4);
	EATEST_VERIFY(u32 == 2);

	u32 = GetHighestBitPowerOf2((uint32_t)7);
	EATEST_VERIFY(u32 == 2);

	u32 = GetHighestBitPowerOf2((uint32_t)8);
	EATEST_VERIFY(u32 == 3);

	u32 = GetHighestBitPowerOf2((uint32_t)0xffffffff);
	EATEST_VERIFY(u32 == 31);


	u32 = GetHighestBitPowerOf2(UINT64_C(0x0000000000000000));
	EATEST_VERIFY(u32 == 0);

	u32 = GetHighestBitPowerOf2(UINT64_C(0x0000000000000001));
	EATEST_VERIFY(u32 == 0);

	u32 = GetHighestBitPowerOf2(UINT64_C(0x0000000000001010));
	EATEST_VERIFY(u32 == 12);

	u32 = GetHighestBitPowerOf2(UINT64_C(0x0000000001101010));
	EATEST_VERIFY(u32 == 24);

	u32 = GetHighestBitPowerOf2(UINT64_C(0x0000000010110111));
	EATEST_VERIFY(u32 == 28);

	u32 = GetHighestBitPowerOf2(UINT64_C(0x0000001010110111));
	EATEST_VERIFY(u32 == 36);

	u32 = GetHighestBitPowerOf2(UINT64_C(0x0001000010110111));
	EATEST_VERIFY(u32 == 48);

	u32 = GetHighestBitPowerOf2(UINT64_C(0x1100000010110111));
	EATEST_VERIFY(u32 == 60);


	// GetNextGreaterEven
	i = GetNextGreaterEven((int)-3);  
	EATEST_VERIFY(i == -2);

	i = GetNextGreaterEven((int)-2);  
	EATEST_VERIFY(i == 0);

	i = GetNextGreaterEven((int)-1);  
	EATEST_VERIFY(i == 0);

	i = GetNextGreaterEven((int)0);  
	EATEST_VERIFY(i == 2);

	i = GetNextGreaterEven((int)1);  
	EATEST_VERIFY(i == 2);

	i = GetNextGreaterEven((int)2);  
	EATEST_VERIFY(i == 4);

	i = GetNextGreaterEven((int)3);  
	EATEST_VERIFY(i == 4);


	// GetNextGreaterOdd
	i = GetNextGreaterOdd((int)-3);  
	EATEST_VERIFY(i == -1);

	i = GetNextGreaterOdd((int)-2);  
	EATEST_VERIFY(i == -1);

	i = GetNextGreaterOdd((int)-1);  
	EATEST_VERIFY(i == 1);

	i = GetNextGreaterOdd((int)0);  
	EATEST_VERIFY(i == 1);

	i = GetNextGreaterOdd((int)1);  
	EATEST_VERIFY(i == 3);

	i = GetNextGreaterOdd((int)2);  
	EATEST_VERIFY(i == 3);

	i = GetNextGreaterOdd((int)3);  
	EATEST_VERIFY(i == 5);


	// RoundUpTo
	i32 = RoundUpTo<int32_t, 4>(3);
	EATEST_VERIFY(i32 == 4);
	u32 = RoundUpTo<uint32_t, 4>(3);
	EATEST_VERIFY(u32 == 4);
	i8 = RoundUpTo<int8_t, 4>(3);
	EATEST_VERIFY(i8 == 4);
	u8 = RoundUpTo<uint8_t, 4>(3);
	EATEST_VERIFY(u8 == 4);
	i64 = RoundUpTo<int64_t, 4>(3);
	EATEST_VERIFY(i64 == 4);
	u64 = RoundUpTo<uint64_t, 4>(3);
	EATEST_VERIFY(u64 == 4);

	i32 = RoundUpTo<int32_t, 4>(8);
	EATEST_VERIFY(i32 == 8);
	u32 = RoundUpTo<uint32_t, 4>(8);
	EATEST_VERIFY(u32 == 8);
	i8 = RoundUpTo<int8_t, 4>(8);
	EATEST_VERIFY(i8 == 8);
	u8 = RoundUpTo<uint8_t, 4>(8);
	EATEST_VERIFY(u8 == 8);
	i64 = RoundUpTo<int64_t, 4>(8);
	EATEST_VERIFY(i64 == 8);
	u64 = RoundUpTo<uint64_t, 4>(8);
	EATEST_VERIFY(u64 == 8);

	i32 = RoundUpTo<int32_t, 4>(0);
	EATEST_VERIFY(i32 == 0);
	u32 = RoundUpTo<uint32_t, 4>(0);
	EATEST_VERIFY(u32 == 0);
	i8 = RoundUpTo<int8_t, 4>(0);
	EATEST_VERIFY(i8 == 0);
	u8 = RoundUpTo<uint8_t, 4>(0);
	EATEST_VERIFY(u8 == 0);
	i64 = RoundUpTo<int64_t, 4>(0);
	EATEST_VERIFY(i64 == 0);
	u64 = RoundUpTo<uint64_t, 4>(0);
	EATEST_VERIFY(u64 == 0);

	i32 = RoundUpTo<int32_t, 4>(-7);
	EATEST_VERIFY(i32 == -4);
	i8 = RoundUpTo<int8_t, 4>(-7);
	EATEST_VERIFY(i8 == -4);
	i64 = RoundUpTo<int64_t, 4>(-7);
	EATEST_VERIFY(i64 == -4);


	// RoundUpToEx
	i32 = RoundUpToEx<int32_t, 4>(3);
	EATEST_VERIFY(i32 == 4);
	u32 = RoundUpToEx<uint32_t, 4>(3);
	EATEST_VERIFY(u32 == 4);
	i8 = RoundUpToEx<int8_t, 4>(3);
	EATEST_VERIFY(i8 == 4);
	u8 = RoundUpToEx<uint8_t, 4>(3);
	EATEST_VERIFY(u8 == 4);
	i64 = RoundUpToEx<int64_t, 4>(3);
	EATEST_VERIFY(i64 == 4);
	u64 = RoundUpToEx<uint64_t, 4>(3);
	EATEST_VERIFY(u64 == 4);

	i32 = RoundUpToEx<int32_t, 4>(8);
	EATEST_VERIFY(i32 == 8);
	u32 = RoundUpToEx<uint32_t, 4>(8);
	EATEST_VERIFY(u32 == 8);
	i8 = RoundUpToEx<int8_t, 4>(8);
	EATEST_VERIFY(i8 == 8);
	u8 = RoundUpToEx<uint8_t, 4>(8);
	EATEST_VERIFY(u8 == 8);
	i64 = RoundUpToEx<int64_t, 4>(8);
	EATEST_VERIFY(i64 == 8);
	u64 = RoundUpToEx<uint64_t, 4>(8);
	EATEST_VERIFY(u64 == 8);

	i32 = RoundUpToEx<int32_t, 4>(0);
	EATEST_VERIFY(i32 == 0);
	u32 = RoundUpToEx<uint32_t, 4>(0);
	EATEST_VERIFY(u32 == 0);
	i8 = RoundUpToEx<int8_t, 4>(0);
	EATEST_VERIFY(i8 == 0);
	u8 = RoundUpToEx<uint8_t, 4>(0);
	EATEST_VERIFY(u8 == 0);
	i64 = RoundUpToEx<int64_t, 4>(0);
	EATEST_VERIFY(i64 == 0);
	u64 = RoundUpToEx<uint64_t, 4>(0);
	EATEST_VERIFY(u64 == 0);

	i32 = RoundUpToEx<int32_t, 4>(-7);
	EATEST_VERIFY(i32 == -8);
	i8 = RoundUpToEx<int8_t, 4>(-7);
	EATEST_VERIFY(i8 == -8);
	i64 = RoundUpToEx<int64_t, 4>(-7);
	EATEST_VERIFY(i64 == -8);


	// RoundDownTo
	i32 = RoundDownTo<int32_t, 4>(5);
	EATEST_VERIFY(i32 == 4);
	u32 = RoundDownTo<uint32_t, 4>(5);
	EATEST_VERIFY(u32 == 4);
	i8 = RoundDownTo<int8_t, 4>(5);
	EATEST_VERIFY(i8 == 4);
	u8 = RoundDownTo<uint8_t, 4>(5);
	EATEST_VERIFY(u8 == 4);
	i64 = RoundDownTo<int64_t, 4>(5);
	EATEST_VERIFY(i64 == 4);
	u64 = RoundDownTo<uint64_t, 4>(5);
	EATEST_VERIFY(u64 == 4);

	i32 = RoundDownTo<int32_t, 4>(4);
	EATEST_VERIFY(i32 == 4);
	u32 = RoundDownTo<uint32_t, 4>(4);
	EATEST_VERIFY(u32 == 4);
	i8 = RoundDownTo<int8_t, 4>(4);
	EATEST_VERIFY(i8 == 4);
	u8 = RoundDownTo<uint8_t, 4>(4);
	EATEST_VERIFY(u8 == 4);
	i64 = RoundDownTo<int64_t, 4>(4);
	EATEST_VERIFY(i64 == 4);
	u64 = RoundDownTo<uint64_t, 4>(4);
	EATEST_VERIFY(u64 == 4);

	i32 = RoundDownTo<int32_t, 4>(0);
	EATEST_VERIFY(i32 == 0);
	u32 = RoundDownTo<uint32_t, 4>(0);
	EATEST_VERIFY(u32 == 0);
	i8 = RoundDownTo<int8_t, 4>(0);
	EATEST_VERIFY(i8 == 0);
	u8 = RoundDownTo<uint8_t, 4>(0);
	EATEST_VERIFY(u8 == 0);
	i64 = RoundDownTo<int64_t, 4>(0);
	EATEST_VERIFY(i64 == 0);
	u64 = RoundDownTo<uint64_t, 4>(0);
	EATEST_VERIFY(u64 == 0);

	i32 = RoundDownTo<int32_t, 4>(-7);
	EATEST_VERIFY(i32 == -8);
	i8 = RoundDownTo<int8_t, 4>(-7);
	EATEST_VERIFY(i8 == -8);
	i64 = RoundDownTo<int64_t, 4>(-7);
	EATEST_VERIFY(i64 == -8);


	// RoundDownToEx
	i32 = RoundDownToEx<int32_t, 4>(5);
	EATEST_VERIFY(i32 == 4);
	u32 = RoundDownToEx<uint32_t, 4>(5);
	EATEST_VERIFY(u32 == 4);
	i8 = RoundDownToEx<int8_t, 4>(5);
	EATEST_VERIFY(i8 == 4);
	u8 = RoundDownToEx<uint8_t, 4>(5);
	EATEST_VERIFY(u8 == 4);
	i64 = RoundDownToEx<int64_t, 4>(5);
	EATEST_VERIFY(i64 == 4);
	u64 = RoundDownToEx<uint64_t, 4>(5);
	EATEST_VERIFY(u64 == 4);

	i32 = RoundDownToEx<int32_t, 4>(4);
	EATEST_VERIFY(i32 == 4);
	u32 = RoundDownToEx<uint32_t, 4>(4);
	EATEST_VERIFY(u32 == 4);
	i8 = RoundDownToEx<int8_t, 4>(4);
	EATEST_VERIFY(i8 == 4);
	u8 = RoundDownToEx<uint8_t, 4>(4);
	EATEST_VERIFY(u8 == 4);
	i64 = RoundDownToEx<int64_t, 4>(4);
	EATEST_VERIFY(i64 == 4);
	u64 = RoundDownToEx<uint64_t, 4>(4);
	EATEST_VERIFY(u64 == 4);

	i32 = RoundDownToEx<int32_t, 4>(0);
	EATEST_VERIFY(i32 == 0);
	u32 = RoundDownToEx<uint32_t, 4>(0);
	EATEST_VERIFY(u32 == 0);
	i8 = RoundDownToEx<int8_t, 4>(0);
	EATEST_VERIFY(i8 == 0);
	u8 = RoundDownToEx<uint8_t, 4>(0);
	EATEST_VERIFY(u8 == 0);
	i64 = RoundDownToEx<int64_t, 4>(0);
	EATEST_VERIFY(i64 == 0);
	u64 = RoundDownToEx<uint64_t, 4>(0);
	EATEST_VERIFY(u64 == 0);

	i32 = RoundDownToEx<int32_t, 4>(-7);
	EATEST_VERIFY(i32 == -4);
	i8 = RoundDownToEx<int8_t, 4>(-7);
	EATEST_VERIFY(i8 == -4);
	i64 = RoundDownToEx<int64_t, 4>(-7);
	EATEST_VERIFY(i64 == -4);


	// RoundUpToMultiple
	i32 = RoundUpToMultiple<int32_t, 6>(37);
	EATEST_VERIFY(i32 == 42);
	u32 = RoundUpToMultiple<uint32_t, 6>(37);
	EATEST_VERIFY(u32 == 42);
	i8 = RoundUpToMultiple<int8_t, 6>(37);
	EATEST_VERIFY(i8 == 42);
	u8 = RoundUpToMultiple<uint8_t, 6>(37);
	EATEST_VERIFY(u8 == 42);
	i64 = RoundUpToMultiple<int64_t, 6>(37);
	EATEST_VERIFY(i64 == 42);
	u64 = RoundUpToMultiple<uint64_t, 6>(37);
	EATEST_VERIFY(u64 == 42);

	i32 = RoundUpToMultiple<int32_t, 6>(41);
	EATEST_VERIFY(i32 == 42);
	u32 = RoundUpToMultiple<uint32_t, 6>(41);
	EATEST_VERIFY(u32 == 42);
	i8 = RoundUpToMultiple<int8_t, 6>(41);
	EATEST_VERIFY(i8 == 42);
	u8 = RoundUpToMultiple<uint8_t, 6>(41);
	EATEST_VERIFY(u8 == 42);
	i64 = RoundUpToMultiple<int64_t, 6>(41);
	EATEST_VERIFY(i64 == 42);
	u64 = RoundUpToMultiple<uint64_t, 6>(41);
	EATEST_VERIFY(u64 == 42);

	i32 = RoundUpToMultiple<int32_t, 6>(42);
	EATEST_VERIFY(i32 == 42);
	u32 = RoundUpToMultiple<uint32_t, 6>(42);
	EATEST_VERIFY(u32 == 42);
	i8 = RoundUpToMultiple<int8_t, 6>(42);
	EATEST_VERIFY(i8 == 42);
	u8 = RoundUpToMultiple<uint8_t, 6>(42);
	EATEST_VERIFY(u8 == 42);
	i64 = RoundUpToMultiple<int64_t, 6>(42);
	EATEST_VERIFY(i64 == 42);
	u64 = RoundUpToMultiple<uint64_t, 6>(42);
	EATEST_VERIFY(u64 == 42);


	// RoundDownToMultiple
	i32 = RoundDownToMultiple<int32_t, 6>(37);
	EATEST_VERIFY(i32 == 36);
	u32 = RoundDownToMultiple<uint32_t, 6>(37);
	EATEST_VERIFY(u32 == 36);
	i8 = RoundDownToMultiple<int8_t, 6>(37);
	EATEST_VERIFY(i8 == 36);
	u8 = RoundDownToMultiple<uint8_t, 6>(37);
	EATEST_VERIFY(u8 == 36);
	i64 = RoundDownToMultiple<int64_t, 6>(37);
	EATEST_VERIFY(i64 == 36);
	u64 = RoundDownToMultiple<uint64_t, 6>(37);
	EATEST_VERIFY(u64 == 36);

	i32 = RoundDownToMultiple<int32_t, 6>(41);
	EATEST_VERIFY(i32 == 36);
	u32 = RoundDownToMultiple<uint32_t, 6>(41);
	EATEST_VERIFY(u32 == 36);
	i8 = RoundDownToMultiple<int8_t, 6>(41);
	EATEST_VERIFY(i8 == 36);
	u8 = RoundDownToMultiple<uint8_t, 6>(41);
	EATEST_VERIFY(u8 == 36);
	i64 = RoundDownToMultiple<int64_t, 6>(41);
	EATEST_VERIFY(i64 == 36);
	u64 = RoundDownToMultiple<uint64_t, 6>(41);
	EATEST_VERIFY(u64 == 36);

	i32 = RoundDownToMultiple<int32_t, 6>(36);
	EATEST_VERIFY(i32 == 36);
	u32 = RoundDownToMultiple<uint32_t, 6>(36);
	EATEST_VERIFY(u32 == 36);
	i8 = RoundDownToMultiple<int8_t, 6>(36);
	EATEST_VERIFY(i8 == 36);
	i64 = RoundDownToMultiple<int64_t, 6>(36);
	EATEST_VERIFY(i64 == 36);

	// ZeroPresent8
	b = ZeroPresent8(UINT32_C(0xffffffff));
	EATEST_VERIFY(!b);
	b = ZeroPresent8(UINT32_C(0x01010101));
	EATEST_VERIFY(!b);
	b = ZeroPresent8(UINT32_C(0x00ffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT32_C(0xff00ffff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT32_C(0xffff00ff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT32_C(0xffffff00));
	EATEST_VERIFY(b);

	b = ZeroPresent8(UINT64_C(0xffffffffffffffff));
	EATEST_VERIFY(!b);
	b = ZeroPresent8(UINT64_C(0x0101010101010101));
	EATEST_VERIFY(!b);
	b = ZeroPresent8(UINT64_C(0x00ffffffffffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT64_C(0xff00ffffffffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT64_C(0xffff00ffffffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT64_C(0xffffff00ffffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT64_C(0xffffffff00ffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT64_C(0xffffffffff00ffff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT64_C(0xffffffffffff00ff));
	EATEST_VERIFY(b);
	b = ZeroPresent8(UINT64_C(0xffffffffffffff00));
	EATEST_VERIFY(b);

	/// ZeroPresent16
	b = ZeroPresent16(UINT32_C(0xffffffff));
	EATEST_VERIFY(!b);
	b = ZeroPresent16(UINT32_C(0xff0000ff));
	EATEST_VERIFY(!b);
	b = ZeroPresent16(UINT32_C(0x0000ffff));
	EATEST_VERIFY(b);
	b = ZeroPresent16(UINT32_C(0xffff0000));
	EATEST_VERIFY(b);

	b = ZeroPresent16(UINT64_C(0xffffffffffffffff));
	EATEST_VERIFY(!b);
	b = ZeroPresent16(UINT64_C(0xff0000ffffffffff));
	EATEST_VERIFY(!b);
	b = ZeroPresent16(UINT64_C(0xffffff0000ffffff));
	EATEST_VERIFY(!b);
	b = ZeroPresent16(UINT64_C(0xffffffffff0000ff));
	EATEST_VERIFY(!b);
	b = ZeroPresent16(UINT64_C(0x0000ffffffffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent16(UINT64_C(0xffff0000ffffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent16(UINT64_C(0xffffffff0000ffff));
	EATEST_VERIFY(b);
	b = ZeroPresent16(UINT64_C(0xffffffffffff0000));
	EATEST_VERIFY(b);

	/// ZeroPresent32
	b = ZeroPresent32(UINT64_C(0xffffffffffffffff));
	EATEST_VERIFY(!b);
	b = ZeroPresent32(UINT64_C(0xff00000000fffff));
	EATEST_VERIFY(!b);
	b = ZeroPresent32(UINT64_C(0xffffff00000000ff));
	EATEST_VERIFY(!b);
	b = ZeroPresent32(UINT64_C(0x00000000ffffffff));
	EATEST_VERIFY(b);
	b = ZeroPresent32(UINT64_C(0xffffffff00000000));
	EATEST_VERIFY(b);


	// Log2
	u32 = Log2(UINT32_C(4));
	EATEST_VERIFY(u32 == 2);

	u32 = Log2(UINT32_C(8));
	EATEST_VERIFY(u32 == 3);

	u32 = Log2(UINT32_C(11));
	EATEST_VERIFY(u32 == 3);

	u32 = Log2(UINT32_C(16));
	EATEST_VERIFY(u32 == 4);


	u64 = Log2(UINT64_C(4));
	EATEST_VERIFY(u64 == 2);

	u64 = Log2(UINT64_C(8));
	EATEST_VERIFY(u64 == 3);

	u64 = Log2(UINT64_C(11));
	EATEST_VERIFY(u64 == 3);

	u64 = Log2(UINT64_C(16));
	EATEST_VERIFY(u64 == 4);


	// Log2Uint32 / Log2Int32 / Log2Uint64 / Log2Int64
	u32 = Log2Uint32<1>::value;
	EATEST_VERIFY(u32 == 0);

	u32 = Log2Uint32<8>::value;
	EATEST_VERIFY(u32 == 3);

	u32 = Log2Uint32<0xffffffff>::value;
	EATEST_VERIFY(u32 == 31);

	i32 = Log2Int32<1>::value;
	EATEST_VERIFY(i32 == 0);

	i32 = Log2Int32<8>::value;
	EATEST_VERIFY(i32 == 3);

	i32 = Log2Int32<0x7fffffff>::value;
	EATEST_VERIFY(i32 == 30);

	u64 = Log2Uint64<UINT64_C(0x0000000000000001)>::value;
	EATEST_VERIFY(u64 == 0);

	u64 = Log2Uint64<UINT64_C(0x1000000000000000)>::value;
	EATEST_VERIFY(u64 == 60);

	u64 = Log2Uint64<UINT64_C(0xffffffffffffffff)>::value;
	EATEST_VERIFY(u64 == 63);

	i64 = Log2Int64<INT64_C(0x0000000000000001)>::value;
	EATEST_VERIFY(i64 == 0);

	i64 = Log2Int64<INT64_C(0x1000000000000000)>::value;
	EATEST_VERIFY(i64 == 60);

	i64 = Log2Int64<INT64_C(0x7fffffffffffffff)>::value;
	EATEST_VERIFY(i64 == 62);


	// SignedAdditionWouldOverflow
	b = SignedAdditionWouldOverflow<int8_t>(2, 3);
	EATEST_VERIFY(!b);

	b = SignedAdditionWouldOverflow<int8_t>(INT8_MAX - 4, INT8_MAX - 10);
	EATEST_VERIFY(b);

	b = SignedAdditionWouldOverflow<int8_t>(INT8_MIN + 4, INT8_MIN + 10);
	EATEST_VERIFY(b);

	b = SignedAdditionWouldOverflow<int>(2, 3);
	EATEST_VERIFY(!b);

	b = SignedAdditionWouldOverflow<int>(INT32_MAX - 4, INT32_MAX - 10);
	EATEST_VERIFY(b);

	b = SignedAdditionWouldOverflow<int>(INT32_MIN + 4, INT32_MIN + 10);
	EATEST_VERIFY(b);

	b = SignedAdditionWouldOverflow<int64_t>(2, 3);
	EATEST_VERIFY(!b);

	b = SignedAdditionWouldOverflow<int64_t>(INT64_MAX - 4, INT64_MAX - 10);
	EATEST_VERIFY(b);

	b = SignedAdditionWouldOverflow<int64_t>(INT64_MIN + 4, INT64_MIN + 10);
	EATEST_VERIFY(b);


	// UnsignedAdditionWouldOverflow
	b = UnsignedAdditionWouldOverflow<uint16_t>(2, 3);
	EATEST_VERIFY(!b);

	b = UnsignedAdditionWouldOverflow<uint16_t>(UINT16_MAX - 4, UINT16_MAX - 10);
	EATEST_VERIFY(b);

	b = UnsignedAdditionWouldOverflow<uint64_t>(1000, 1);
	EATEST_VERIFY(!b);

	b = UnsignedAdditionWouldOverflow<uint64_t>(UINT64_MAX - 4, 10);
	EATEST_VERIFY(b);


	// SignedSubtractionWouldOverflow
	b = SignedSubtractionWouldOverflow<int8_t>(100, 1);
	EATEST_VERIFY(!b);
	
	b = SignedSubtractionWouldOverflow<int8_t>(INT8_MAX - 4, INT8_MIN + 10);
	EATEST_VERIFY(b);
	
	b = SignedSubtractionWouldOverflow<int8_t>(INT8_MIN + 10, INT8_MAX - 4);
	EATEST_VERIFY(b);
	
	b = SignedSubtractionWouldOverflow<int>(1000, 1);
	EATEST_VERIFY(!b);
	
	b = SignedSubtractionWouldOverflow<int>(INT32_MAX - 4, INT32_MIN + 10);
	EATEST_VERIFY(b);
	
	b = SignedSubtractionWouldOverflow<int>(INT32_MIN + 10, INT32_MAX - 4);
	EATEST_VERIFY(b);
	
	b = SignedSubtractionWouldOverflow<int64_t>(1000, 1);
	EATEST_VERIFY(!b);
	
	b = SignedSubtractionWouldOverflow<int64_t>(INT64_MAX - 4, INT64_MIN + 10);
	EATEST_VERIFY(b);
	
	b = SignedSubtractionWouldOverflow<int64_t>(INT64_MIN + 10, INT64_MAX - 4);
	EATEST_VERIFY(b);
	

	// UnsignedSubtractionWouldOverflow
	b = UnsignedSubtractionWouldOverflow<uint16_t>(1000, 1);
	EATEST_VERIFY(!b);

	b = UnsignedSubtractionWouldOverflow<uint16_t>(UINT16_MAX - 10, UINT16_MAX - 4);
	EATEST_VERIFY(b);

	b = UnsignedSubtractionWouldOverflow<uint64_t>(1000, 1);
	EATEST_VERIFY(!b);

	b = UnsignedSubtractionWouldOverflow<uint64_t>(UINT64_MAX - 10, UINT64_MAX - 4);
	EATEST_VERIFY(b);


	// UnsignedMultiplyWouldOverflow
	// Disabled because some compilers mistakenly decide that the code will be dividing by zero.
	//b = UnsignedMultiplyWouldOverflow((uint32_t)0, (uint32_t)0);
	//EATEST_VERIFY(!b);

	b = UnsignedMultiplyWouldOverflow((uint32_t)4, (uint32_t)5);
	EATEST_VERIFY(!b);

	b = UnsignedMultiplyWouldOverflow((uint32_t)0xffffffff, (uint32_t)0xffffffff);
	EATEST_VERIFY(b);

	// Disabled because some compilers mistakenly decide that the code will be dividing by zero.
	//b = UnsignedMultiplyWouldOverflow(UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000));
	//EATEST_VERIFY(!b);

	b = UnsignedMultiplyWouldOverflow(UINT64_C(0x000000000ffffff4), UINT64_C(0x000000000ffffff5));
	EATEST_VERIFY(!b);

	b = UnsignedMultiplyWouldOverflow(UINT64_C(0xffffffffffffffff), UINT64_C(0xffffffffffffffff));
	EATEST_VERIFY(b);


	// UnsignedDivisionWouldOverflow
	b = UnsignedDivisionWouldOverflow(5, 4);
	EATEST_VERIFY(!b);

	b = UnsignedDivisionWouldOverflow(3, 0);
	EATEST_VERIFY(b);


	// SignedDivisionWouldOverflow
	b = SignedDivisionWouldOverflow(5, 4);
	EATEST_VERIFY(!b);

	b = SignedDivisionWouldOverflow(3, 0);
	EATEST_VERIFY(b);

	b = SignedDivisionWouldOverflow(INT32_MIN, -1);
	EATEST_VERIFY(b);


	// GetAverage
	i = GetAverage(3, 4);
	EATEST_VERIFY(i == 3);

	i = GetAverage(3, 3);
	EATEST_VERIFY(i == 3);

	i = GetAverage(-3, -4);
	EATEST_VERIFY(i == -4);

	i = GetAverage(-2, 1);
	EATEST_VERIFY(i == -1);


	// GetAverage_Ceiling
	i = GetAverage_Ceiling(3, 4);
	EATEST_VERIFY(i == 4);

	i = GetAverage_Ceiling(3, 3);
	EATEST_VERIFY(i == 3);

	i = GetAverage_Ceiling(-3, -4);
	EATEST_VERIFY(i == -3);

	i = GetAverage_Ceiling(-2, 1);
	EATEST_VERIFY(i == 0);


	// GetParity
	i = GetParity(UINT32_C(0x01100011));
	EATEST_VERIFY(i == 0);

	i = GetParity(UINT32_C(0x00101010));
	EATEST_VERIFY(i == 1);


	// GetIsBigEndian
	b = GetIsBigEndian();
	#ifdef EA_SYSTEM_BIG_ENDIAN
		EATEST_VERIFY(b);
	#else
		EATEST_VERIFY(!b);
	#endif


	// ToggleBetween0And1
	i = ToggleBetween0And1(0);
	EATEST_VERIFY(i == 1);

	i = ToggleBetween0And1(i);
	EATEST_VERIFY(i == 0);

	i = ToggleBetween0And1(i);
	EATEST_VERIFY(i == 1);


	// ToggleBetweenIntegers
	i32 = 37;
	int32_t i32a = 37, i32b = -38;
	i32 = ToggleBetweenIntegers(i32, i32a, i32b);
	EATEST_VERIFY(i32 == -38);

	i32 = ToggleBetweenIntegers(i32, i32a, i32b);
	EATEST_VERIFY(i32 == 37);


	// IsBetween0AndValue
	b = IsBetween0AndValue(3, 20);
	EATEST_VERIFY(b);

	b = IsBetween0AndValue(-37, 20);
	EATEST_VERIFY(!b);

	b = IsBetween0AndValue(37, 20);
	EATEST_VERIFY(!b);

	b = IsBetween0AndValue(370, 20);
	EATEST_VERIFY(!b);


	// ExchangeValues
	n1 = 1; n2 = 2;
	ExchangeValues(n1, n2);
	EATEST_VERIFY((n1 == 2) && (n2 == 1));

	u64 = 64; uint64_t u64_ = 65;
	ExchangeValues(u64, u64_);
	EATEST_VERIFY((u64 == 65) && (u64_ == 64));

	// FloorMod
	n1 = FloorMod( 10, 3 );
	EATEST_VERIFY((n1 == 1));
	n1 = FloorMod( -10, 3 );
	EATEST_VERIFY((n1 == 2));

	// GetSign
	i = GetSign(INT32_MIN);
	EATEST_VERIFY(i == -1);

	i = GetSign(-1000);
	EATEST_VERIFY(i == -1);

	i = GetSign(0);
	EATEST_VERIFY(i == 0);

	i = GetSign(5);
	EATEST_VERIFY(i == 1);

	i = GetSign(INT32_MAX);
	EATEST_VERIFY(i == 1);


	// GetSignEx
	i = GetSign(-1000);
	EATEST_VERIFY(i == -1);

	i = GetSign(0);
	EATEST_VERIFY(i == 0);

	i = GetSign(5);
	EATEST_VERIFY(i == 1);

	i = GetSign(INT32_MAX);
	EATEST_VERIFY(i == 1);


	// SignExtend12
	i32 = SignExtend12(0x00000fff);
	EATEST_VERIFY(i32 == (int32_t)0xffffffff);

	i32 = SignExtend12(0x000007ff);
	EATEST_VERIFY(i32 == 0x000007ff);

	i32 = SignExtend24(0x00ffffff);
	EATEST_VERIFY(i32 == (int32_t)0xffffffff);

	i32 = SignExtend24(0x007fffff);
	EATEST_VERIFY(i32 == 0x007fffff);


	// IsUnsigned
	EATEST_VERIFY( IsUnsigned( u8));
	EATEST_VERIFY(!IsUnsigned( i8));
	EATEST_VERIFY( IsUnsigned(u16));
	EATEST_VERIFY(!IsUnsigned(i16));
	EATEST_VERIFY( IsUnsigned(u32));
	EATEST_VERIFY(!IsUnsigned(i32));
	EATEST_VERIFY( IsUnsigned(u64));
	EATEST_VERIFY(!IsUnsigned(i64));


	// EAIsUnsigned
	#ifdef _MSC_VER
		#pragma warning(push, 0)
		#pragma warning(disable: 4296) // '>=' : expression is always true
		#pragma warning(disable: 4365) // '=' : conversion from 'int' to 'uint8_t', signed/unsigned mismatch
		#pragma warning(disable: 4706) // assignment within conditional expression
	#endif
	#if !defined(__GNUC__) // GCC generates warnings which you can't disable from code. So just skip the test, as we need to be able to compile this in a "warnings as errors" environment.
		EATEST_VERIFY( EAIsUnsigned( u8));
		EATEST_VERIFY( EAIsUnsigned(u16));
		EATEST_VERIFY( EAIsUnsigned(u32));
		EATEST_VERIFY( EAIsUnsigned(u64));
	#endif
	EATEST_VERIFY(!EAIsUnsigned( i8));
	EATEST_VERIFY(!EAIsUnsigned(i16));
	EATEST_VERIFY(!EAIsUnsigned(i32));
	EATEST_VERIFY(!EAIsUnsigned(i64));
	#ifdef _MSC_VER
		#pragma warning(pop)
	#endif


	// IsTwosComplement
	// All current platforms are twos-complement
	EATEST_VERIFY( IsTwosComplement());
	EATEST_VERIFY(!IsOnesComplement());
	EATEST_VERIFY(!IsSignMagnitude());
	EATEST_VERIFY(!IsOffsetBinary());


	// EAArrayCount
	int testArray[37]; (void) testArray;
	u = EAArrayCount(testArray);
	EATEST_VERIFY(u == 37);


	// EAOffsetOf
	size_t o = EAOffsetOf(OffsetofTest, x1);
	EATEST_VERIFY(o == 4);
	#ifndef EA_COMPILER_GNUC
		// GCC correctly throws an error that the EAOffsetOf implementation used reinterpret_cast which isn't constexpr.
		EA_COMPILETIME_ASSERT(EAOffsetOf(OffsetofTest, x1) == 4); // Verify that this works at compile-time.
	#endif

	int ind = 1;
	o = EAOffsetOf(OffsetofTest1, x[ind]);
	EATEST_VERIFY(o == 4);


	// EAOffsetOfBase   
	o = EAOffsetOfBase<C, B>();
	EATEST_VERIFY(o == EAOffsetOf(C, b));
	//static_assert((EAOffsetOfBase<C, B>() == sizeof(int)), "EAOffsetOfDerived failure"); Not possible unless and until we can make EAOffsetOfBase a constexpr.

	return nErrorCount;
}




