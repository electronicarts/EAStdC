///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER
	#pragma warning(disable: 4244) // This warning is being generated due to a bug in VC++.
#endif

#include <EABase/eabase.h>
#include <EAStdC/EARandom.h>
#include <EAStdC/EARandomDistribution.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>
#include <EASTL/bitset.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
	#pragma warning(disable: 4275) // non dll-interface class 'stdext::exception' used as base for dll-interface class 'std::bad_cast'
#endif

#ifndef EA_PLATFORM_ANDROID
	#include <algorithm>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if defined(_MSC_VER) && defined(EA_PLATFORM_MICROSOFT)
	#include <crtdbg.h>
#endif
#if defined(EA_PLATFORM_WINDOWS)
	#include <Windows.h>
#endif
#if EASTDC_TIME_H_AVAILABLE
	#include <time.h>
#endif

#ifdef _MSC_VER
	#pragma warning(pop)
	#pragma warning(push)
	#pragma warning(disable: 4365) // 'argument' : conversion from 'int' to 'uint32_t', signed/unsigned mismatch
#endif


using namespace EA::StdC;


// Forward declarations
void rt_init(int binmode);
void rt_add(void* buf, int bufl);
void rt_end(double* r_ent, double* r_chisq, double* r_mean, double* r_montepicalc, double* r_scc);

static int TestDieHard()
{
	int nErrorCount(0);

	// Write out 9MB file for DieHard tests.
	#if defined(EA_PLATFORM_WINDOWS) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
		if(GetAsyncKeyState(VK_SCROLL)) // If the Scroll Lock key is alive.
		{
			// Ideally we would port the DieHard code to here, but it is not well 
			// written for modularity. For the time being, we write out the 9MB
			// data file that DieHard.exe can analyze. As of this writing, DieHard.exe
			// is part of the EAOS UTF Research repository.

			{
				RandomLinearCongruential randomLC;

				FILE* pFile = fopen("RandomLinearCongruentialData.txt", "w");

				if(pFile)
				{
					for(uint32_t i = 0; i < 12000000; i += 4)
					{
						const uint32_t value = randomLC.RandomUint32Uniform();
						fwrite(&value, 1, 4, pFile);
					}

					fclose(pFile);
				}
			}

			{
				RandomMersenneTwister randomMT;

				FILE* pFile = fopen("RandomMersenneTwisterData.txt", "w");

				if(pFile)
				{
					for(uint32_t i = 0; i < 12000000; i += 4)
					{
						const uint32_t value = randomMT.RandomUint32Uniform();
						fwrite(&value, 1, 4, pFile);
					}

					fclose(pFile);
				}
			}
		}
	#endif

	return nErrorCount;
}


namespace 
{
	#if defined(EA_PLATFORM_DESKTOP) && !defined(EA_DEBUG)
		// This exists for the purpose of testing distributions. It implements a seed that is continuously 
		// increasting and thus over the course of 0x100000000 (2^32) calls to RandomUint32Uniform returns a statistically
		// even distribution of bits. Note that truly random data won't behave this way and formal tests for
		// randomness would identify this as being not random. But that's not the purpose of this class; 
		// the purpose is to help test if there are distribution problems in the range and distribution adapters.
		// Note that it's important that you do 0x100000000 calls with this or else the results of it won't 
		// be evenly distributed as designed.
		class FakeIncrementingRandom
		{
		public:
			FakeIncrementingRandom() 
				: mnSeed(0) {}
			
			//FakeIncrementingRandom(const FakeIncrementingRandom& x) 
			//   : mnSeed(x.mnSeed) {}

			//FakeIncrementingRandom& operator=(const FakeIncrementingRandom& x)
			//    { mnSeed = x.mnSeed; return *this; }

			//uint32_t GetSeed() const
			//    { return mnSeed; }

			//void SetSeed(uint32_t nSeed)
			//    { mnSeed = nSeed; }

			//uint32_t operator()(uint32_t nLimit)
			//    { return EA::StdC::RandomLimit(*this, nLimit); }

			uint32_t RandomUint32Uniform()
				{ return mnSeed++; }

		protected:
			uint32_t mnSeed;
		};
	#endif
}


// TestRandom
// Note that thus function itself is not meant as a comprehensive 
// test for randomness. Instead this function does a basic test 
// for randomness and then optionally writes out files to disk
// for analysis by a comprehensive tool like DieHard. 
//
int TestRandom()
{
	int nErrorCount(0);

	{   // Bug report regression.
		// User Fei Jiang reports that RandomLinearCongruential::RandomUnit32Uniform(uint32_t nLimit) returns 
		// different values on PS3 in debug vs. debug-opt builds with SN compiler.

		RandomLinearCongruential rlc(UINT32_C(2474210934));
		uint32_t seed = rlc.GetSeed();
		//EA::UnitTest::Report("seed: %u\n", (unsigned)seed);
		EATEST_VERIFY(seed == UINT32_C(2474210934));

		uint32_t result = rlc.RandomUint32Uniform(57);
		//EA::UnitTest::Report("result: %u\n", (unsigned)result);
		EATEST_VERIFY(result == 23);  // 743483
	}

	// Load priming
	// We call a function from each generator used below to minimize 
	// an loading effects on benchmarking.
	int rTemp = rand();
	EATEST_VERIFY(rTemp >= 0); // "Returns a pseudo-random integral number in the range 0 to RAND_MAX."

	RandomLinearCongruential randomLCPrimer;
	randomLCPrimer.RandomUint32Uniform();

	RandomMersenneTwister randomMTPrimer;
	randomMTPrimer.RandomUint32Uniform();

	TestDieHard();

	//#define SPEED_TESTS_ENABLED
	#ifdef SPEED_TESTS_ENABLED
		// Speed tests.
		// Results on a Pentium 4 PC were:
		//    rand(): 8172 clocks.
		//    RandomLinearCongruential: 4687 clocks.
		//    RandomMersenneTwister: 6157 clocks.
		{
			clock_t timeStart;
			clock_t timeTotal;
			const int kIterationCount(5000000);

			timeStart = clock();
			for(int i(0); i < kIterationCount; i++)
				EA::UnitTest::WriteToEnsureFunctionCalled() = (int)rand();
			timeTotal = clock() - timeStart;
			EA::UnitTest::Report("rand(): %d clocks.\n", (int)timeTotal);

			timeStart = clock();
			for(int i(0); i < kIterationCount; i++)
				EA::UnitTest::WriteToEnsureFunctionCalled() = (int)(rand() % 37997);
			timeTotal = clock() - timeStart;
			EA::UnitTest::Report("rand() w/limit: %d clocks.\n", (int)timeTotal);


			RandomLinearCongruential randomLC;
			timeStart = clock();
			for(int i(0); i < kIterationCount; i++)
				EA::UnitTest::WriteToEnsureFunctionCalled() = (int)randomLC.RandomUint32Uniform();
			timeTotal = clock() - timeStart;
			EA::UnitTest::Report("RandomLinearCongruential: %d clocks.\n", (int)timeTotal);

			timeStart = clock();
			for(int i(0); i < kIterationCount; i++)
				EA::UnitTest::WriteToEnsureFunctionCalled() = (int)randomLC.RandomUint32Uniform(37997);
			timeTotal = clock() - timeStart;
			EA::UnitTest::Report("RandomLinearCongruential w/limit: %d clocks.\n", (int)timeTotal);


			RandomMersenneTwister randomMT;
			timeStart = clock();
			for(int i(0); i < kIterationCount; i++)
				EA::UnitTest::WriteToEnsureFunctionCalled() = (int)randomMT.RandomUint32Uniform();
			timeTotal = clock() - timeStart;
			EA::UnitTest::Report("RandomMersenneTwister: %d clocks.\n", (int)timeTotal);

			timeStart = clock();
			for(int i(0); i < kIterationCount; i++)
				EA::UnitTest::WriteToEnsureFunctionCalled() = (int)randomMT.RandomUint32Uniform(32997);
			timeTotal = clock() - timeStart;
			EA::UnitTest::Report("RandomMersenneTwister w/limit: %d clocks.\n", (int)timeTotal);
		}
	#endif


	// Test output ranges
	{  // RandomLinearCongruential test
		RandomLinearCongruential random;
		int32_t  nRandom;
		double   dRandom;

		for(unsigned i(0); i < 100; i++)
		{
			for(uint32_t j(5); j < (UINT32_MAX / 2); j *= 5)
			{
				uint32_t nU32 = random.RandomUint32Uniform(j);
				EATEST_VERIFY(nU32 < j);

				dRandom = random.RandomDoubleUniform((double)j);
				EATEST_VERIFY(0.0 <= dRandom && dRandom < j);

				dRandom = random.RandomDoubleUniform();
				EATEST_VERIFY(0.0 <= dRandom && dRandom < 1.0);
			}

			nRandom = Random2(random);
			EATEST_VERIFY(nRandom < 2);

			nRandom = Random4(random);
			EATEST_VERIFY(nRandom < 4);

			nRandom = Random8(random);
			EATEST_VERIFY(nRandom < 8);

			nRandom = Random16(random);
			EATEST_VERIFY(nRandom < 16);

			nRandom = Random32(random);
			EATEST_VERIFY(nRandom < 32);

			nRandom = Random64(random);
			EATEST_VERIFY(nRandom < 642);

			nRandom = Random128(random);
			EATEST_VERIFY(nRandom < 128);

			nRandom = Random256(random);
			EATEST_VERIFY(nRandom < 256);


			// RandomPowerOfTwo
			for(uint32_t k(1); k < 31; k++)
			{
				nRandom = RandomPowerOfTwo(random, k);
				EATEST_VERIFY((uint32_t)nRandom < (uint32_t)(2 << k));
			}


			// RandomInt32UniformRange
			for(int32_t nBegin(-10000); nBegin < 10000; nBegin += Random256(random))
			{
				int32_t nEnd    = nBegin + 1 + Random256(random);
				int32_t iRandom = RandomInt32UniformRange(random, nBegin, nEnd);

				EATEST_VERIFY_F((iRandom >= nBegin) && (iRandom < nEnd), "RandomInt32UniformRange failure: iRandom: %I32d, nBegin: %I32d, nEnd: %I32d", iRandom, nBegin, nEnd);
			}


			// RandomDoubleUniformRange
			for(int32_t dBegin(-10000); dBegin < 10000; dBegin += Random256(random))
			{
				int32_t dEnd = dBegin + 1 + Random256(random);
				dRandom = RandomDoubleUniformRange(random, (double)dBegin, (double)dEnd);

				EATEST_VERIFY_F((dRandom >= dBegin) && (dRandom < dEnd), "RandomDoubleUniformRange failure: dRandom: %f, dBegin: %f, dEnd: %f", dRandom, (double)dBegin, (double)dEnd);
			}


			// RandomUint32WeightedChoice
			const uint32_t kLimit = 37;
			float weights[kLimit];
			for(uint32_t q(0); q < kLimit; q++)
				weights[q] = (float)RandomDoubleUniformRange(random, 0.5, 30.0);
			for(uint32_t r(0); r < 1000; r++)
			{
				uint32_t nU32 = RandomUint32WeightedChoice(random, kLimit, weights);

				EATEST_VERIFY_F(nU32 < kLimit, "RandomUint32WeightedChoice failure: nU32: %I32u, kLimit: %I32u", nU32, kLimit);
			}


			// RandomInt32GaussianRange
			for(int r(0); r < 1000; r++)
			{
				const int32_t nBegin  = (int32_t)random.RandomUint32Uniform(1000);
				const int32_t nEnd    = nBegin + (int32_t)random.RandomUint32Uniform(1000) + 1;
				const int32_t iRandom = RandomInt32GaussianRange(random, nBegin, nEnd);

				EATEST_VERIFY((iRandom >= nBegin) && (iRandom < nEnd));
			}


			// RandomFloatGaussianRange
			for(int r(0); r < 1000; r++)
			{
				const float fBegin  = (float)random.RandomDoubleUniform(1000);
				const float fEnd    = fBegin + (float)random.RandomDoubleUniform(1000) + 1.0f;
				const float fRandom = RandomFloatGaussianRange(random, fBegin, fEnd);

				EATEST_VERIFY((fRandom >= fBegin) && (fRandom < fEnd));
			}


			// RandomInt32TriangleRange
			for(int r(0); r < 1000; r++)
			{
				const int32_t nBegin  = (int32_t)random.RandomUint32Uniform(1000);
				const int32_t nEnd    = nBegin + (int32_t)random.RandomUint32Uniform(1000) + 1;
				const int32_t iRandom = RandomInt32TriangleRange(random, nBegin, nEnd);

				EATEST_VERIFY((iRandom >= nBegin) && (iRandom < nEnd));
			}


			// RandomFloatTriangleRange
			for(int r(0); r < 1000; r++)
			{
				const float fBegin  = (float)random.RandomDoubleUniform(1000);
				const float fEnd    = fBegin + (float)random.RandomDoubleUniform(1000) + 1.0f;
				const float fRandom = RandomFloatTriangleRange(random, fBegin, fEnd);

				EATEST_VERIFY((fRandom >= fBegin) && (fRandom < fEnd));
			}


		}
	}


	{ // RandomInt32Poisson
		const float fMean = 5.f;
		const size_t maxK = 30;

		RandomMersenneTwister random;

		for(int i = 0; i < 1000; i++)
		{
			int32_t rn = RandomInt32Poisson(random.RandomDoubleUniform(), fMean);
			EATEST_VERIFY(rn < maxK);
		}
	}


	{  // RandomLinearCongruential test
		RandomMersenneTwister random;
		int32_t nRandom;
		double  dRandom;

		for(unsigned i(0); i < 1000; i++)
		{
			for(uint32_t j(5); j < UINT32_MAX / 2; j *= 5)
			{
				uint32_t nU32 = random.RandomUint32Uniform(j);
				EATEST_VERIFY(nU32 < j);

				dRandom = random.RandomDoubleUniform((double)j);
				EATEST_VERIFY(0.0 <= dRandom && dRandom < j);

				dRandom = random.RandomDoubleUniform();
				EATEST_VERIFY(0.0 <= dRandom && dRandom < 1.0);
			}

			nRandom = Random2(random);
			EATEST_VERIFY(nRandom < 2);

			nRandom = Random4(random);
			EATEST_VERIFY(nRandom < 4);

			nRandom = Random8(random);
			EATEST_VERIFY(nRandom < 8);

			nRandom = Random16(random);
			EATEST_VERIFY(nRandom < 16);

			nRandom = Random32(random);
			EATEST_VERIFY(nRandom < 32);

			nRandom = Random64(random);
			EATEST_VERIFY(nRandom < 64);

			nRandom = Random128(random);
			EATEST_VERIFY(nRandom < 128);

			nRandom = Random256(random);
			EATEST_VERIFY(nRandom < 256);

			for(uint32_t k(1); k < 31; k++)
			{
				nRandom = RandomPowerOfTwo(random, k);
				EATEST_VERIFY((uint32_t)nRandom < (uint32_t)(2 << k));
			}

			for(int nBegin(-10000); nBegin < 10000; nBegin += Random256(random))
			{
				int32_t nEnd    = nBegin + 1 + Random256(random);
				int32_t iRandom = RandomInt32UniformRange(random, nBegin, nEnd);

				EATEST_VERIFY((iRandom >= nBegin) && (iRandom < nEnd));
			}

			for(int dBegin(-10000); dBegin < 10000; dBegin += Random256(random))
			{
				int32_t dEnd = dBegin + 1 + Random256(random);
				dRandom = RandomDoubleUniformRange(random, (double)dBegin, (double)dEnd);

				EATEST_VERIFY((dRandom >= dBegin) && (dRandom < dEnd));
			}

			const unsigned int kLimit = 37;
			float weights[kLimit];
			for(unsigned int q(0); q < kLimit; q++)
				weights[q] = (float)RandomDoubleUniformRange(random, 0.5, 30.0);
			for(unsigned int r(0); r < 100; r++)
			{
				uint32_t nU32 = RandomUint32WeightedChoice(random, kLimit, weights);

				EATEST_VERIFY(nU32 < kLimit);
			}
		}
	}


	//NOTICE:
	//Need Paul to look at this.
	//At times, getting values outside of the assertion range.
	#if !defined(EA_PLATFORM_IPHONE)

		// Do basic randomness testing.
		// Just because a random number generator passes known basic tests
		// doesn't mean it doesn't have a major flaw.

		{  // C runtime rand() test, provided for comparison.
			int nErrorCountCRand(0); //We don't want to report these as part of our test.

			rt_init(false);
			for(int i(0); i < 100000; i++)
			{
				uint8_t nRandom = (uint8_t)(rand() & UINT8_MAX);
				rt_add(&nRandom, sizeof(nRandom));
			}

			// See the rt_end documentation for detailed explanations
			// of what each of these metrics mean.
			double r_ent, r_chisq, r_mean, r_montepicalc, r_scc;
			rt_end(&r_ent, &r_chisq, &r_mean, &r_montepicalc, &r_scc);

			if(r_ent < 7.8)
				nErrorCountCRand++;
			else if(r_chisq < 200)
				nErrorCountCRand++;
			else if(r_mean < 127.2 || r_mean > 127.9)
				nErrorCountCRand++;
			else if(r_montepicalc < 3.11 || r_montepicalc > 3.17)
				nErrorCountCRand++;
			else if(r_scc > 0.01)
				nErrorCountCRand++;
		}


		{  // RandomLinearCongruential test
			RandomLinearCongruential random;

			rt_init(false);
			for(int i(0); i < 100000; i++)
			{
				uint32_t nRandom = random.RandomUint32Uniform();
				rt_add(&nRandom, sizeof(nRandom));
			}

			// See the rt_end documentation for detailed explanations
			// of what each of these metrics mean.
			double r_ent, r_chisq, r_mean, r_montepicalc, r_scc;
			rt_end(&r_ent, &r_chisq, &r_mean, &r_montepicalc, &r_scc);

			EATEST_VERIFY(r_ent >= 7.8);
		  //EATEST_VERIFY(r_chisq >= 200);                      Disabled until we can figure out why it occasionally fails.
		  //EATEST_VERIFY(r_mean >= 127.2 && r_mean < 127.9);   Disabled until we can figure out why it occasionally fails.
			EATEST_VERIFY(r_montepicalc >= 3.11 && r_montepicalc < 3.17);
			EATEST_VERIFY(r_scc <= 0.01);
		}



		{  // RandomMersenneTwister test
			RandomMersenneTwister random;

			rt_init(false);
			for(int i(0); i < 100000; i++)
			{
				uint32_t nRandom = random.RandomUint32Uniform();
				rt_add(&nRandom, sizeof(nRandom));
			}

			// See the rt_end documentation for detailed explanations
			// of what each of these metrics mean.
			double r_ent, r_chisq, r_mean, r_montepicalc, r_scc;
			rt_end(&r_ent, &r_chisq, &r_mean, &r_montepicalc, &r_scc);

			EATEST_VERIFY(r_ent >= 7.8);
		  //EATEST_VERIFY(r_chisq >= 200);                      Disabled until we can figure out why it occasionally fails.
		  //EATEST_VERIFY(r_mean >= 127.2 && r_mean < 127.9);   Disabled until we can figure out why it occasionally fails.
			EATEST_VERIFY(r_montepicalc >= 3.11 && r_montepicalc < 3.17);
			EATEST_VERIFY(r_scc <= 0.01);
		}
	#endif


	{ // RandomMersenneTwister seed serialization test.
		RandomMersenneTwister rmt;
		uint32_t              seedArray[RandomMersenneTwister::kSeedArrayCount * 2];
		uint32_t              rand1, rand2;
		unsigned              size;

		size = rmt.GetSeed(seedArray, RandomMersenneTwister::kSeedArrayCount);
		EATEST_VERIFY(size == RandomMersenneTwister::kSeedArrayCount);
		rand1 = rmt.RandomUint32Uniform();
		rmt.RandomUint32Uniform();
		rmt.SetSeed(seedArray, size);
		rand2 = rmt.RandomUint32Uniform();
		EATEST_VERIFY(rand1 == rand2);

		size = rmt.GetSeed(seedArray, RandomMersenneTwister::kSeedArrayCount * 2);
		EATEST_VERIFY(size == RandomMersenneTwister::kSeedArrayCount);
		rand1 = rmt.RandomUint32Uniform();
		rmt.RandomUint32Uniform();
		rmt.SetSeed(seedArray, size);
		rand2 = rmt.RandomUint32Uniform();
		EATEST_VERIFY(rand1 == rand2);

		size = rmt.GetSeed(seedArray, RandomMersenneTwister::kSeedArrayCount / 2);
		EATEST_VERIFY(size == RandomMersenneTwister::kSeedArrayCount / 2);
		rand1 = rmt.RandomUint32Uniform();
		rmt.RandomUint32Uniform();
		rmt.SetSeed(seedArray, size);
		// We can't test for equality or inequality of rand1 and rand2

		// This is just a pathological test.
		size = rmt.GetSeed(seedArray, 0);
		EATEST_VERIFY(size == 0);
		rand1 = rmt.RandomUint32Uniform();
		rmt.RandomUint32Uniform();
		rmt.SetSeed(seedArray, size);
		rand2 = rmt.RandomUint32Uniform();
		EATEST_VERIFY(rand1 != rand2);      // They should be different (actually one out of 4 billion times they shouldn't be) because we didn't read the entire state, but only half of it.
	}


	{
		#if defined(EA_PLATFORM_DESKTOP) && !defined(EA_DEBUG) // Do this test only on fast machines, as it's compute-intensive.
			// Range tests with FakeIncrementingRandom
			const size_t sizes[] = { 2, 5, 10 };
			eastl::vector<uint32_t> countBuckets(sizes[EAArrayCount(sizes) - 1], 0);

			for(size_t a = 0; a < EAArrayCount(sizes); a++)
			{
				size_t s = sizes[a];

				FakeIncrementingRandom fir;
				eastl::fill(countBuckets.begin(), countBuckets.end(), 0);

				for(uint64_t i = 0, iEnd = UINT64_C(0x100000000) / s * s; i < iEnd; i++)
				{
					if((i % 0x10000000) == 0)
					   EA::UnitTest::Report("."); // Keepalive output.
					uint32_t b = EA::StdC::RandomLimit(fir, static_cast<uint32_t>(s));
					countBuckets[b]++;
				}

				for(eastl_size_t b = 1, c = countBuckets[0]; b < s; b++)
				{
					if(countBuckets[b] != c)
					{
						EATEST_VERIFY(countBuckets[b] == c);
						EA::UnitTest::Report("Random distribution result buckets for limit of %I32u:\n   ", (uint32_t)s);
						for(eastl_size_t bb = 0, bbEnd = s; bb < bbEnd; bb++)
							EA::UnitTest::Report("%I32u%s", (uint32_t)countBuckets[bb], ((bb % 16) == 15) ? "\n" : " ");
						EA::UnitTest::Report("\n");
						break;
					}
				}

				EA::UnitTest::Report(".\n"); // Keep alive output.
			}
		#endif
	}

	// Write out files suitable for the DieHard test suite.
	// The version of DieHard that this author most recently
	// worked with requires 8404992 bytes of data in a file.
	// A copy of DieHard.exe should accompany this test.
	// Currently, you drag a file onto it to get the results
	// of the test. In the future we can implement the entire
	// test within this file. It is about 3500 lines of code
	// and would require some massaging to make it work
	// smoothly with a unit testing system.

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// Ent Chi-Squared functions
//
// Home:
//     http://www.fourmilab.ch/random/
// License:
//     This software is in the public domain. Permission to use, copy, modify, 
//     and distribute this software and its documentation for any purpose and 
//     without fee is hereby granted, without any conditions or restrictions. 
//     This software is provided "as is" without express or implied warranty. 
///////////////////////////////////////////////////////////////////////////////
// 
// Entropy
//      The information density of the contents of the file, expressed as a 
//      number of bits per character. The results above, which resulted from 
//      processing an image file compressed with JPEG, indicate that the 
//      file is extremely dense in information--essentially random. 
//      Hence, compression of the file is unlikely to reduce its size. 
//      By contrast, the C source code of the program has entropy of about 
//      4.9 bits per character, indicating that optimal compression of the 
//      file would reduce its size by 38%. [Hamming, pp. 104-108]
// 
// Chi-square Test
//      The chi-square test is the most commonly used test for the randomness
//      of data, and is extremely sensitive to errors in pseudorandom sequence 
//      generators. The chi-square distribution is calculated for the stream 
//      of bytes in the file and expressed as an absolute number and a 
//      percentage which indicates how frequently a truly random sequence 
//      would exceed the value calculated. We interpret the percentage as the 
//      degree to which the sequence tested is suspected of being non-random. 
//      If the percentage is greater than 99% or less than 1%, the sequence is 
//      almost certainly not random. If the percentage is between 99% and 95% 
//      or between 1% and 5%, the sequence is suspect. Percentages between 90% 
//      and 95% and 5% and 10% indicate the sequence is "almost suspect". 
//      Note that our JPEG file, while very dense in information, is far from 
//      random as revealed by the chi-square test.
// 
//      Applying this test to the output of various pseudorandom sequence 
//      generators is interesting. The low-order 8 bits returned by the 
//      standard Unix rand() function, for example, yields:
//            Chi square distribution for 500000 samples is 0.01, and randomly 
//            would exceed this value 99.99 percent of the times. 
// 
//      While an improved generator [Park & Miller] reports:
//            Chi square distribution for 500000 samples is 212.53, and randomly 
//            would exceed this value 95.00 percent of the times. 
// 
//      Thus, the standard Unix generator (or at least the low-order bytes 
//      it returns) is unacceptably non-random, while the improved generator 
//      is much better but still sufficiently non-random to cause concern for 
//      demanding applications. Contrast both of these software generators 
//      with the chi-square result of a genuine random sequence created by 
//      timing radioactive decay events.
//            Chi square distribution for 32768 samples is 237.05, and randomly 
//            would exceed this value 75.00 percent of the times. 
// 
//      See [Knuth, pp. 35-40] for more information on the chi-square test. 
//      An interactive chi-square calculator is available at this site.
// 
// Arithmetic Mean
//      This is simply the result of summing the all the bytes (bits if the -b 
//      option is specified) in the file and dividing by the file length. 
//      If the data are close to random, this should be about 127.5 (0.5 for -b 
//      option output). If the mean departs from this value, the values are 
//      consistently high or low.
// 
// Monte Carlo Value for Pi
//      Each successive sequence of six bytes is used as 24 bit X and Y 
//      co-ordinates within a square. If the distance of the randomly-generated 
//      point is less than the radius of a circle inscribed within the square, 
//      the six-byte sequence is considered a "hit". The percentage of hits can 
//      be used to calculate the value of Pi. For very large streams 
//      (this approximation converges very slowly), the value will approach the 
//      correct value of Pi if the sequence is close to random. A 32768 byte 
//      file created by radioactive decay yielded:
//            Monte Carlo value for Pi is 3.139648438 (error 0.06 percent). 
// 
// Serial Correlation Coefficient
//      This quantity measures the extent to which each byte in the file 
//      depends upon the previous byte. For random sequences, this value 
//      (which can be positive or negative) will, of course, be close to zero. 
//      A non-random byte stream such as a C program will yield a serial 
//      correlation coefficient on the order of 0.5. Wildly predictable data 
//      such as uncompressed bitmaps will exhibit serial correlation coefficients 
//      approaching 1. See [Knuth, pp. 64-65] for more details.
///////////////////////////////////////////////////////////////////////////////


#define RFALSE       0
#define RTRUE        1
#define BINARY_MODE  RTRUE
#define BYTE_MODE    RFALSE
#define MONTEN       6 /* Bytes used as Monte Carlo co-ordinates. This should be no more bits than the mantissa of your "double" floating point type. */
#define log2of10     3.32192809488736234787

static int binary = RFALSE;    /* Treat input as a bitstream */
static long ccount[256];        /* Bins to count occurrences of values */
static long totalc = 0;         /* Total bytes counted */
static double prob[256];        /* Probabilities per bin for entropy */
static int mp, sccfirst;
static unsigned int monte[MONTEN];
static long inmont, mcount;
static double cexp, incirc, montex, montey, montepi, scc, sccun, sccu0, scclast, scct1, scct2, scct3, ent, chisq, datasum;


/*  LOG2  --  Calculate log to the base 2  */
static double Local_log2(double x)
{
	return log2of10 * log10(x);
}


/*  RT_INIT  --  Initialise random test counters. Call with BINARY_MODE or BYTE_MODE */
void rt_init(int binmode)
{
	int i;

	binary = binmode;            /* Set binary / byte mode */

	/* Initialise for calculations */
	ent = 0.0;                   /* Clear entropy accumulator */
	chisq = 0.0;                 /* Clear Chi-Square */
	datasum = 0.0;               /* Clear sum of bytes for arithmetic mean */

	mp = 0;                      /* Reset Monte Carlo accumulator pointer */
	mcount = 0;                  /* Clear Monte Carlo tries */
	inmont = 0;                  /* Clear Monte Carlo inside count */
	incirc = 65535.0 * 65535.0;  /* In-circle distance for Monte Carlo */

	sccfirst = RTRUE;            /* Mark first time for serial correlation */
	scct1 = scct2 = scct3 = 0.0; /* Clear serial correlation terms */

	incirc = pow(pow(256.0, (double) (MONTEN / 2)) - 1, 2.0);

	for (i = 0; i < 256; i++) {
		ccount[i] = 0;
	}
	totalc = 0;
}



/*  RT_ADD  --    Add one or more bytes to accumulation.    */
void rt_add(void* buf, int bufl)
{
	unsigned char* bp =(unsigned char*)buf;
	int oc, c, bean;

	while (bean = 0, (bufl-- > 0))
	{
		oc = *bp++;

		do {
			if (binary) {
				c = !!(oc & 0x80);
			} 
			else {
				c = oc;
			}
			ccount[c]++;         /* Update counter for this bin */
			totalc++;

			/* Update inside / outside circle counts for Monte Carlo computation of PI */
			if (bean == 0) {
				monte[mp++] = (unsigned int)oc;         /* Save character for Monte Carlo */

				if (mp >= MONTEN) {      /* Calculate every MONTEN character */
					int mj;

					mp = 0;
					mcount++;
					montex = montey = 0;
					for (mj = 0; mj < MONTEN / 2; mj++) {
						montex = (montex * 256.0) + monte[mj];
						montey = (montey * 256.0) + monte[(MONTEN / 2) + mj];
					}
					if ((montex * montex + montey *  montey) <= incirc) {
						inmont++;
					}
				}
			}

			/* Update calculation of serial correlation coefficient */
			sccun = (double)c;

			if (sccfirst) {
				sccfirst = RFALSE;
				scclast = 0;
				sccu0 = sccun;
			} 
			else {
				scct1 = scct1 + scclast * sccun;
			}
			scct2 = scct2 + sccun;
			scct3 = scct3 + (sccun * sccun);
			scclast = sccun;
			oc <<= 1;
		} while (binary && (++bean < 8));
	}
}



/*  RT_END  -- Complete calculation and return results.  */
void rt_end(double* r_ent, double* r_chisq, double* r_mean,
			double* r_montepicalc, double* r_scc)
{
	int i;
	double a;

	/* Complete calculation of serial correlation coefficient */

	scct1 = scct1 + scclast * sccu0;
	scct2 = scct2 * scct2;
	scc = totalc * scct3 - scct2;
	if (scc == 0.0) {
		scc = -100000;
	} 
	else {
		scc = (totalc * scct1 - scct2) / scc;
	}

	/* Scan bins and calculate probability for each bin and Chi-Square distribution */

	cexp = totalc / (binary ? 2.0 : 256.0);  /* Expected count per bin */
	for (i = 0; i < (binary ? 2 : 256); i++) {
		prob[i] = (double) ccount[i] / totalc;
		a = ccount[i] - cexp;
		chisq = chisq + (a * a) / cexp;
		datasum += ((double) i) * ccount[i];
	}

	/* Calculate entropy */
	for (i = 0; i < (binary ? 2 : 256); i++) {
		if (prob[i] > 0.0) {
			ent += prob[i] * Local_log2(1 / prob[i]);
		}
	}

	/* Calculate Monte Carlo value for PI from percentage of hits within the circle */
	montepi = 4.0 * (((double) inmont) / mcount);

	/* Return results through arguments */
	*r_ent = ent;
	*r_chisq = chisq;
	*r_mean = datasum / totalc;
	*r_montepicalc = montepi;
	*r_scc = scc;
}
///////////////////////////////////////////////////////////////////////////////







#if 0


static double get_double()
{
	return 1.0;
}


static double CalculateSqrm(double a, double b)
{
	return ((a - b) * (a - b)) / b;
}


static double CalculatePhi(double x)
{
	static const double v[15] = 
	{ 
		1.2533141373155,    .6556795424187985,  .4213692292880545,
		.3045902987101033,  .2366523829135607,  .1928081047153158,
		.1623776608968675,  .1401041834530502,  .1231319632579329,
		.1097872825783083,  .09902859647173193, .09017567550106468,
		.08276628650136917, .0764757610162485,  .07106958053885211
	};

	// Local variables
	double cphi, a, b, h;
	double z, sum, pwr;
	int    i, j;

	if (fabs(x) > 7.0)
	{
		if (x >= 0.0)
			return 1.0;
		return 0.0;
	}

	if (x>=0.0)
		cphi = 0.0;
	else
		cphi = 1.0;

	j   = (int) (fabs(x) + 0.5);
	j   = std::min<int>(j, 14);
	z   = (double) j;
	h   = fabs(x) - z;
	a   = v[j];
	b   = z * a - 1.0;
	pwr = 1.0;
	sum = a + h * b;

	for (i = 2; i <= (24-j); i += 2)
	{
		a    = (a + z * b) / i;
		b    = (b + z * a) / (i + 1);
		pwr *= h * h;
		sum += pwr * (a + h * b);
	}

	cphi = sum * exp(x * -0.5 * x - 0.918938533204672);

	if (x < 0.0)
		return cphi;

	return 1.0 - cphi;
}


static double CalculateChisq(double x, int n)
{
	// System generated locals
	double ret_val;

	// Local variables
	double  d;
	long    i, l;
	double  s, t;
	double  xmin;

	if (x <= 0.0)
		return 0.0;

	if (n > 20)
	{
		t = (pow( x / n, 0.33333) - 1.0 + 0.22222 / n) / sqrt(0.22222 / n);
		return CalculatePhi(std::min(t, 8.0));
	}

	l = 4 - n % 2;

	d = (double) std::min(1, n / 3);
	ret_val = 0.0;

	for (i = l; i <= n; i += 2)
	{
		d = d * x / (i - 2);
		ret_val += d;
	}

	xmin = std::min(x * 0.5, 50.0);

	if (l == 3)
	{
		s = sqrt( xmin );
		return  CalculatePhi(s/0.7071068) - exp(-xmin) * 0.564189 * ret_val / s;
	}

	return 1.0 - exp(-xmin) * (ret_val + 1.0);
}


///////////////////////////////////////////////////////////////////////////////
// TestCraps
//
// This is the Craps test. It plays 200,000 games of craps, finds
// the number of wins and the number of throws necessary to end
// each game. The number of wins should be (very close to) a
// normal with mean 200000p and variance 200000p(1 - p), with
// p = 244 / 495. Throws necessary to complete the game can vary
// from 1 to infinity, but counts for all > 21 are lumped with 21.
// A chi-square test is made on the #-of-throws cell counts.
// Each 32-bit integer from the test file provides the value for
// the throw of a die, by floating to [0, 1), multiplying by 6
// and taking 1 plus the integer part of the result.
//
static void TestCraps(double& pvalueWins, double& pvalueThrows)
{
	static  long   nt[22];
	static  double e[22];

	double  t;
	double  pwins;
	double  av;            // Expected win count.
	double  sd;
	double  ex;
	double  sum;
	long    ng;
	long    gc;
	long    nwins;        // Actual win count.
	double  pthrows;
	int     nthrows;
	int     point;
	int     i, m, k;

	e[1] = 1.0 / 3.0;
	sum  = e[1];

	for (k = 2; k <= 20; ++k)
	{
		e[k] = ( pow(27.0/36.0, (double) k-2) * 27.0 +
				 pow(26.0/36.0, (double) k-2) * 40.0 +
				 pow(25.0/36.0, (double) k-2) * 55.0 ) / 648.0;

		sum += e[k];
	}

	e[21] = 1.0 - sum;
	ng    = 200000;
	nwins = 0;

	for (i = 1; i <= 21; ++i)
		nt[i] = 0;

	for (gc = 1; gc <= ng; ++gc)
	{
		point = (int)(get_double() * 6.0) + (int)(get_double() * 6.0) + 2;
		nthrows = 1;

		if ((point == 7) || (point == 11))
			++nwins;
		else if ((point != 2) && (point != 3) && (point != 12))
		{
			for(;;)
			{
				++nthrows;

				k = (int)(get_double() * 6.0) + (int)(get_double() * 6.0) + 2;

				if (k == 7)
					break;

				if (k == point)
				{
					++nwins;
					break;
				}
			}
		}

		m = std::min<int>(21, nthrows);
		++nt[m];
	}

	av = ng * 244.0 / 495.0;
	sd = sqrt(av * 251.0 / 495.0);
	t  = (nwins - av) / sd;

	//dprintf("  Results of craps test for %s\n", filename);
	//dprintf("  No. of wins:  Observed Expected\n");
	//dprintf("  %9ld %11.2f\n", nwins, av);

	pwins = CalculatePhi(t);

	//dprintf(" %8ld= No. of wins, z-score=%6.3f pvalue=%7.5f\n", nwins, t, pwins);
	//dprintf("    Analysis of Throws-per-Game:\n");

	sum = 0.0;

	for (i = 1; i <= 21; ++i)
	{
		ex = ng * e[i];
		sum += CalculateSqrm((double)nt[i], ex);
	}

	pthrows = CalculateChisq(sum, 20);

	//dprintf(" Chisq=%7.2f for 20 degrees of freedom, p=%8.5f\n", sum, pthrows);
	//dprintf("                    Throws Observed Expected  Chisq      Sum\n");

	//sum = 0.0;

	//for (i = 1; i <= 21; ++i)
	//{
	//     ex = ng * e[i];
	//     t = sqrm((double)nt[i], ex);
	//     sum += t;
	//
	//     dprintf("%19d %8ld %10.1f %7.3f %8.3f\n", i, nt[i], ex, t, sum);
	//}

	//save_pvalue(pwins);
	//save_pvalue(pthrows);

	//dprintf("                SUMMARY  FOR %s\n", filename);
	//dprintf("                     p-value for no. of wins:%8.6f\n", pwins);
	//dprintf("                     p-value for throws/game:%8.6f\n", pthrows);

	pvalueWins   = pwins;
	pvalueThrows = pthrows;
}

#endif



#ifdef _MSC_VER
	#pragma warning(pop)
#endif













