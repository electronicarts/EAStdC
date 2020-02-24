///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////



#include <EABase/eabase.h>
#include <EAStdC/EAFixedPoint.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>



static bool CompareValues(double a, double b)
{
	return (fabs(a - b) < 0.01);
}

static bool CompareValues(EA::StdC::SFixed16 a, double b)
{
	#define Fixed16ToDouble(a) (((double)a) / 65536.0)

	const double c = Fixed16ToDouble(a.AsFixed());
	return (fabs(c - b) < 0.01);
}


// In a DLL build, VC++ doesn't like it when you provide your own 
// versions of functions. So we just skip testing this, for simplicity.
// If we need this to work, we can just add these functions to EAFixedPoint.cpp.
#ifndef EA_DLL

	namespace EA
	{
		namespace StdC
		{
			static bool CompareValues(SFixed24 a, double b){
				#define Fixed24ToDouble(a) (((double)a) / 256.0)
				const double c = Fixed24ToDouble(a.AsFixed());
				return (fabs(c - b) < 0.01);
			}

			template<>
			int32_t SFixed24::FixedMul(const int32_t a, const int32_t b){
				 const int64_t c = (int64_t)a * b;
				 return (int32_t)(c >> 8);
			}

			template<>
			int32_t SFixed24::FixedDiv(const int32_t a, const int32_t b){
				 const int64_t c = ((uint64_t)a) << 8;
				 return (int32_t)(c / b);
			}

			template<>
			int32_t SFixed24::FixedMod(const int32_t a, const int32_t b){
				 const volatile uint64_t c = ((uint64_t)a) << 8;
				 return (int32_t)(uint32_t)(c % b);
			}
		}
	}
#endif



int TestFixedPoint()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	// Test SFixed16
	{
		SFixed16  a(1), b(2), c(3.f), d(1.0);
		double    e = 3.2;
		float     f = 4.5;
		int       g =   6;

		if(a.AsInt() != 1)
			nErrorCount++;
		if(c.AsUnsignedInt() != 3)
			nErrorCount++;
		if(a.AsLong() != 1)
			nErrorCount++;
		if(c.AsUnsignedLong() != 3)
			nErrorCount++;
		if(!CompareValues((double)a.AsFloat(), 1.0))
			nErrorCount++;
		if(!CompareValues(c.AsDouble(), 3.0))
			nErrorCount++;

		a = b * f;
		if(!CompareValues(a, 9.0))
			nErrorCount++;

		a = b / d;
		if(!CompareValues(a, 2.0))
			nErrorCount++;

		a = b + d;
		if(!CompareValues(a, 3.0))
			nErrorCount++;

		a = (c / e) + b + f;
		if(!CompareValues(a, 7.4375))
			nErrorCount++;

		a = c / e * (b % g) + f / c;
		if(!CompareValues(a, 3.375))
			nErrorCount++;

		a = g * -c / (b++);
		if(!CompareValues(a, -9.0))
			nErrorCount++;
		if(!CompareValues(b, 3.0))
			nErrorCount++;
		--b; //Restore it to its original value.
		if(!CompareValues(b, 2.0))
			nErrorCount++;

		a = sin(d) + pow(b, e) * sqrt(d);
		if(!CompareValues(a, 10.031))
			nErrorCount++;

		a = log(e) / log(f);
		if(!CompareValues(a, 0.77333))
			nErrorCount++;
	}

	{
		// FPTemplate operator<<(int numBits) const 
		{
			SFixed16 a(16);

			auto expected = a.value << 1;

			a = (a << 1);

			if(!CompareValues(a.value, expected))
				nErrorCount++;
		}

		// FPTemplate operator>>(int numBits) const 
		{
			SFixed16 a(16);

			auto expected = a.value >> 1;

			a = (a >> 1);

			if(!CompareValues(a.value, expected))
				nErrorCount++;
		}
	}

	// Reported regression - ensure operator<< and operator>> are implemented correctly.
	{
		SFixed16 a(16);

		auto expected = a.value;

		a = (a << 1);
		a = (a >> 1);

		if(!CompareValues(a.value, expected))
			nErrorCount++;
	}

	#ifndef EA_DLL
		// Test SFixed24
		{
			SFixed24  a(1), b(2), c(3.f), d(1.0);
			double    e = 3.2;
			float     f = 4.5;
			int       g =   6;

			if(a.AsInt() != 1)
				nErrorCount++;
			if(c.AsUnsignedInt() != 3)
				nErrorCount++;
			if(a.AsLong() != 1)
				nErrorCount++;
			if(c.AsUnsignedLong() != 3)
				nErrorCount++;
			if(!CompareValues((double)a.AsFloat(), 1.0))
				nErrorCount++;
			if(!CompareValues(c.AsDouble(), 3.0))
				nErrorCount++;

			a = b * f;
			if(!CompareValues(a, 9.0))
				nErrorCount++;

			a = b / d;
			if(!CompareValues(a, 2.0))
				nErrorCount++;

			a = b + d;
			if(!CompareValues(a, 3.0))
				nErrorCount++;

			a = (c / e) + b + f;
			if(!CompareValues(a, 7.4375))
				nErrorCount++;

			a = c / e * (b % g) + f / c;
			if(!CompareValues(a, 3.375))
				nErrorCount++;

			a = g * -c / (b++);
			if(!CompareValues(a, -9.0))
				nErrorCount++;
			if(!CompareValues(b, 3.0))
				nErrorCount++;
			--b; //Restore it to its original value.
			if(!CompareValues(b, 2.0))
				nErrorCount++;

			a = sin(d) + pow(b, e) * sqrt(d);
			if(!CompareValues(a, 10.031))
				nErrorCount++;

			a = log(e) / log(f);
			if(!CompareValues(a, 0.77333))
				nErrorCount++;
		}
	#endif

	// Test core multiplication/division functions
	{
		//SFixed16 SFixed16::FixedMul       (const T t1, const T t2);
		//SFixed16 SFixed16::FixedDiv       (const T t1, const T t2);
		//SFixed16 SFixed16::FixedDivSafe   (const T t1, const T t2);
		//SFixed16 SFixed16::FixedMulDiv    (const T t1, const T t2, const T t3);
		//SFixed16 SFixed16::FixedMulDivSafe(const T t1, const T t2, const T t3); 
		//SFixed16 SFixed16::FixedMod       (const T t1, const T t2);
		//SFixed16 SFixed16::FixedModSafe   (const T t1, const T t2);
	}

	return nErrorCount;
}















