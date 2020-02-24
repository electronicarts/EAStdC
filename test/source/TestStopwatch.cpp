///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EADateTime.h>
#include <EATest/EATest.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EAStdC/EAString.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if defined(EA_PLATFORM_WINDOWS)
	#include <Windows.h>
#elif defined(EA_PLATFORM_ANDROID)
	#include <sys/system_properties.h>
#endif

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#ifndef EASTDC_SWAPPABLE_PROCESS_PLATFORM
	#if defined(EA_PLATFORM_ANDROID)
		#define EASTDC_SWAPPABLE_PROCESS_PLATFORM
	#endif
#endif

// Tests whether an emulator is being used (for Android devices only)
#ifdef EA_PLATFORM_ANDROID
	bool IsEmulator()
	{
		char buf[PROP_VALUE_MAX];
		// "goldfish" and "ranchu" are the possible names of the kernel that run on the Android emulator (as of 2018/02/08)
		int propLen = __system_property_get("ro.hardware", buf);
		if(propLen != 0 && 
			(EA::StdC::Strncmp(buf, "goldfish", propLen) == 0 || 
				EA::StdC::Strncmp(buf, "ranchu", propLen) == 0))
			return true;
		return false;
	}
#endif

int TestStopwatch()
{
	using namespace EA::StdC;

	int       nErrorCount(0);
	Stopwatch stopwatch(Stopwatch::kUnitsSeconds);
	Stopwatch stopwatchHelper1(Stopwatch::kUnitsSeconds);
	Stopwatch stopwatchHelper2(Stopwatch::kUnitsSeconds);
	uint64_t  nElapsedTime;
	bool      bResult = true;

	// Call the stopwatch once to make sure the code is loaded, etc.
	stopwatch.Start();
	stopwatch.Stop();

	////////////////////////////////////////////////////////////////////////////
	// Do a simple test of the CPU cycle counting.
	//
	// This test will fail on Android when using an Emulator which is what the Build Farm uses
	#ifdef EA_PLATFORM_ANDROID
		if(!IsEmulator())
		{
	#endif
			bool bDisableTimingTest = false;
			#if !EASTDC_VALGRIND_ENABLED
				bDisableTimingTest = true;
			#endif

			uint64_t elapsedTimeLimit = 1000;

			EA::UnitTest::SetHighThreadPriority();
			int nTookTooLongCount(0);
			for(int i(0); i < 20; i++)
			{
				stopwatch.Reset();
				stopwatch.SetUnits(Stopwatch::kUnitsCycles);
				stopwatch.Start();
				stopwatch.Stop();
				nElapsedTime = stopwatch.GetElapsedTime();

				if( (nElapsedTime > elapsedTimeLimit) && (bDisableTimingTest == false) )
				{
					nTookTooLongCount++;
				}
			}

			// Until we can get a way to prevent thread context switches during our tests, we allow as much as one to occur during our tests.
			EATEST_VERIFY(nTookTooLongCount <= 1);

			EA::UnitTest::SetNormalThreadPriority();

	#ifdef EA_PLATFORM_ANDROID
		}
	#endif
	////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////
	// Do a simple test to verify that Stopwatch::GetStopwatchCycle() always 
	// generates a value greater than before. There is a problem on some 
	// multi-processing platforms whereby the processors can have different
	// cpu tick values and the OS moves the process between the given processors,
	// resulting in one processing giving a different tick count than another.
	bool bMultiCPUImprecisionPresent = false;
	
	{
		uint64_t       previous = Stopwatch::GetStopwatchCycle();
		uint64_t       current  = previous;
		LimitStopwatch limitStopwatch(Stopwatch::kUnitsMilliseconds);
		limitStopwatch.SetTimeLimit(1000); // 1 second

		while(!limitStopwatch.IsTimeUp() && (current >= previous))
		{
			previous = current;
			current  = Stopwatch::GetStopwatchCycle();
		}

		if(current < previous) // If the stopwatch appears to have gone backwards...
		{
			// We -may- have a multi-processing platform with unsynchronized CPUs.
			// Many platforms that use CPU-based cycle counting don't have perfectly synchronized
			// multiple CPUs. Stopwatch doesn't try to account for that, as it's not a problem
			// that can be solved in a simple fast way. If we aren't dealing with a 
			// mulitple CPU thing then we may have a true stopwatch rollover situation.
			// Some platforms have weaker CPU cycle counting systems which roll over
			// quickly. Such an example is the Playbook platform, and there is little we 
			// can do about it in the raw GetStopwatchCycle function. Our Stopwatch class
			// can handle it due to having contextual information.

			// Ideally we would have a good way to tell if this is a multi-CPU thing.
			// Allow only a tiny amount of imprecision between CPUs.
			bMultiCPUImprecisionPresent = (current + (current * 100 / 99)) >= previous; // Note by Paul Pedriana: Isn't the math here wrong and the "* 100" shouldn't be there? And even if it was there we still have the problem of that + operation causing current to roll over UINT64_MAX.
			EATEST_VERIFY(bMultiCPUImprecisionPresent);
		}
	}


	////////////////////////////////////////////////////////////////////////////
	// Do a test that verifies that the stopwatch will not report a negative 
	// time and also that each time GetElapsedTime is called, it will 
	// be >= to the result of the last call to GetElapsedTime.
	uint64_t nCurrentElapsedTime(0), nPreviousElapsedTime(0);
	
	stopwatchHelper1.SetUnits(Stopwatch::kUnitsSeconds);
	stopwatchHelper1.Restart();
	stopwatch.SetUnits(Stopwatch::kUnitsCycles);
	stopwatch.Restart();
	bResult = true;

	while(bResult && (stopwatchHelper1.GetElapsedTime() < 2))
	{
		nCurrentElapsedTime = stopwatch.GetElapsedTime();

		bResult = (((int64_t)nCurrentElapsedTime >= 0) && (nCurrentElapsedTime >= nPreviousElapsedTime));
		EATEST_VERIFY((int64_t)nCurrentElapsedTime >= 0);
		EATEST_VERIFY((nCurrentElapsedTime >= nPreviousElapsedTime) || bMultiCPUImprecisionPresent);

		nPreviousElapsedTime = nCurrentElapsedTime;

		// Just idle away some time.
		char buffer[16];
		for(int i(0), iEnd(rand() % 10000); i < iEnd; i++)
			sprintf(buffer, "%d", i); // Just do a little nothing.
	}
	////////////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////////////
	// Do a test whereby we use two stopwatchs (one being EA::StdC::Stopwatch and the other
	// being a system stopwatch) and see if they yield similar results.
	if(EA::StdC::GetTimePrecision() < 10000000) // If the GetTime function has precision of at least 10 milliseconds (10000000 ns)...
	{
		uint64_t  nStopwatchElapsedTime(0);
		uint64_t  nClockStartTime(0);
		uint64_t  nClockEndTime(0);
		uint64_t  nClockElapsedTime(0);
		bool      bStopwatchComparisonTestSucceeded(true);

		//EA::UnitTest::Report("A %u second time comparison test will now be run.\n"
		//         "Press 'esc' to quit this test early.\n", nSecondsForEachTest);
		//EA::UnitTest::Report("The results are printed as percentages.\n"
		//         "100.0%% is perfect, while 90%% and 110%% are both bad.\n");

		stopwatchHelper1.SetUnits(Stopwatch::kUnitsSeconds);
		stopwatchHelper1.Restart();
		stopwatchHelper2.SetUnits(Stopwatch::kUnitsMilliseconds);
		stopwatch.Reset();
		stopwatch.SetUnits(Stopwatch::kUnitsMilliseconds);

		// Now get the current times. We need to somehow prevent a context switch 
		// here because a context switch bewteen the two stopwatch elapsed time 
		// calculations will throw off the precision of the comparison.
		EA::UnitTest::SetHighThreadPriority();
		stopwatch.Start();
		nClockStartTime = EA::StdC::GetTime() / 1000000; // Convert from ns to ms.
		EA::UnitTest::SetNormalThreadPriority();
		bResult = true;

		while(bResult && (stopwatchHelper1.GetElapsedTime() < 5))
		{
			// Sleep for some small random amount of time.
			EA::UnitTest::ThreadSleepRandom(1000, 2000, false);

			// Now get the current times. We need to somehow prevent a context switch 
			// here because a context switch bewteen the two stopwatch elapsed time 
			// calculations will throw off the precision of the comparison.
			EA::UnitTest::SetHighThreadPriority();
			nStopwatchElapsedTime = stopwatch.GetElapsedTime();
			nClockEndTime = EA::StdC::GetTime() / 1000000; // Convert from ns to ms.
			nClockElapsedTime = nClockEndTime - nClockStartTime;
			EA::UnitTest::SetNormalThreadPriority();

			//Now compare the results.
			#if defined(EA_PLATFORM_DESKTOP)
				const float kAllowedAccuracyMin = 0.96f; // Windows/Unix do multitasking and can delay threads arbitrarily.
				const float kAllowedAccuracyMax = 1.04f;
			#else
				const float kAllowedAccuracyMin = 0.98f;
				const float kAllowedAccuracyMax = 1.02f;
			#endif

			const float fAccuracy = (float)nStopwatchElapsedTime / (float)nClockElapsedTime;
			if((fAccuracy < kAllowedAccuracyMin) || (fAccuracy > kAllowedAccuracyMax))
			{
				if(bStopwatchComparisonTestSucceeded)
				{
					bStopwatchComparisonTestSucceeded = false;
					bResult = false;
					EA::UnitTest::Report("Stopwatch accuracy failure. fAccuracy: %f\n", fAccuracy);
					EATEST_VERIFY(false);
				}
			}
			//EA::UnitTest::Report("    Clock: %6d ms, Stopwatch: %6d ms. Accuracy: %5.2f%%\n", nClockElapsedTime, nStopwatchElapsedTime, fAccuracy*100.F);
		}
	}
	////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////
	// Test SetElapsedTime functionality.
	stopwatch.SetUnits(Stopwatch::kUnitsMilliseconds);
	stopwatch.Restart();
	// <No time passed since the restart>
	stopwatch.SetElapsedTime(30000);      // 30 seconds
	nElapsedTime = stopwatch.GetElapsedTime();

	#if defined(EA_PLATFORM_DESKTOP)
		const uint64_t kAllowedElapsedTimeMin = 29990;
		const uint64_t kAllowedElapsedTimeMax = 31000;
	#else
		const uint64_t kAllowedElapsedTimeMin = 29990;
		const uint64_t kAllowedElapsedTimeMax = 30500;
	#endif
	EATEST_VERIFY((nElapsedTime >= kAllowedElapsedTimeMin) && (nElapsedTime <= kAllowedElapsedTimeMax));

	stopwatch.SetUnits(Stopwatch::kUnitsMilliseconds);
	stopwatch.Restart();
	while(stopwatch.GetElapsedTime() < 3000)
	{
		char buffer[32];
		EA::StdC::Sprintf(buffer, "%I64u", stopwatch.GetElapsedTime()); // Just spin.
	}
	stopwatch.SetElapsedTime(30000);      // 30 seconds
	nElapsedTime = stopwatch.GetElapsedTime();
	EATEST_VERIFY((nElapsedTime >= kAllowedElapsedTimeMin) && (nElapsedTime <= kAllowedElapsedTimeMax));

	stopwatch.SetUnits(Stopwatch::kUnitsMinutes);
	stopwatch.Restart();
	stopwatch.SetElapsedTimeFloat(0.5f); // 30 seconds
	stopwatch.SetUnits(Stopwatch::kUnitsMilliseconds);
	nElapsedTime = stopwatch.GetElapsedTime();
	EATEST_VERIFY((nElapsedTime >= kAllowedElapsedTimeMin) && (nElapsedTime <= kAllowedElapsedTimeMax));
	////////////////////////////////////////////////////////////////////////////


	// This test has been disabled as it's proving to be unreliable due to generating false failures.
	#if 0
		////////////////////////////////////////////////////////////////////////////
		// Test SetTimeLimit functionality.
		stopwatchHelper1.SetUnits(Stopwatch::kUnitsMilliseconds);
		stopwatchHelper1.Start();

		LimitStopwatch limitStopwatch(Stopwatch::kUnitsMilliseconds);
		limitStopwatch.SetTimeLimit(5000); // 5 seconds
		bResult = true;

		const float lowLimit = -0.5f;   // If the platform is very slow or is pre-empting this thread a lot, we may need to make this value lower,
		const float highLimit = 5.0f;   // .. and make this value higher.

		while(bResult && !limitStopwatch.IsTimeUp())
		{
			const float fTimeRemaining(limitStopwatch.GetTimeRemainingFloat());

			// Verify that when the time is up, the time remaining is close to zero.
			if(limitStopwatch.IsTimeUp())
			{
				if(!((fTimeRemaining >= lowLimit) && (fTimeRemaining < highLimit)))
				{
					bResult = false;
					EA::UnitTest::Report("LimitStopwatch failure. Time remaining: %f\n", fTimeRemaining);
					EATEST_VERIFY(((fTimeRemaining >= lowLimit) && (fTimeRemaining < highLimit)));
				}
			}
		}
		nElapsedTime = stopwatchHelper1.GetElapsedTime();
		EATEST_VERIFY((nElapsedTime >= 5000) || (nElapsedTime <= 5500));
		////////////////////////////////////////////////////////////////////////////
	#endif


	////////////////////////////////////////////////////////////////////////////
	// Test GetStopwatchCyclesPerUnit / GetCPUCyclesPerUnit
	//
	Stopwatch      stopwatchTemp(Stopwatch::kUnitsMilliseconds, true);
	#if !defined(EASTDC_SWAPPABLE_PROCESS_PLATFORM)
		const uint64_t nStartStopwatchCycle    = Stopwatch::GetStopwatchCycle();
		const uint64_t nStartCPUCycle          = Stopwatch::GetCPUCycle();
	#endif
	const uint64_t nTestDuration        = 1000; // run test for one second
	const int nMaxDrift                 = 50;	// a small amount of drift is just noise

	// Loop until the test duration has passed, making sure the float time tracks the integer time.
	bool bDriftDetectedPrev = false;
	uint64_t elapsedTime_i = 0;
	uint64_t elapsedTime_f = 0;
	while (elapsedTime_i < nTestDuration && elapsedTime_f < nTestDuration)
	{
		// measure time and detect drift
		elapsedTime_i = stopwatchTemp.GetElapsedTime();
		elapsedTime_f = static_cast<uint64_t>(stopwatchTemp.GetElapsedTimeFloat());
		int delta = static_cast<int>(elapsedTime_f - elapsedTime_i);
		bool bDriftDetected = (delta <= -nMaxDrift || nMaxDrift <= delta);

		// fail only once there is significant drift twice in a row.
		// This avoids false positives due to pre-emption between measurements
		EATEST_VERIFY(bDriftDetected == false || bDriftDetectedPrev == false);
		bDriftDetectedPrev = bDriftDetected;
	}
	stopwatchTemp.Stop();

	// For platforms where the main process can be swapped out for extended periods of time, 
	// we disable the test on the buildfarm (via the manifest.xml)
	#if !defined(EASTDC_SWAPPABLE_PROCESS_PLATFORM)
		// Make sure the manual GetUnitsPerStopwatchCycle and GetStopwatchCycle yield results that are equivalent to using Start/Stop/GetElapsedTime.
		uint64_t       nStopwatchCycles        = Stopwatch::GetStopwatchCycle() - nStartStopwatchCycle;
		const float    fUnitsPerStopwatchCycle = Stopwatch::GetUnitsPerStopwatchCycle(Stopwatch::kUnitsSeconds);
		const float    fStopwatchTime          = nStopwatchCycles * fUnitsPerStopwatchCycle;
		const uint64_t nStopwatchTime          = (uint64_t)(int64_t)(fStopwatchTime + 0.49999f); // Add 0.49999f like GetElapsedTime does for integer rounding

		EATEST_VERIFY_F(nStopwatchTime == 1, "TestStopwatch failure: nStopwatchTime: %I64us, expected: 1s", nStopwatchTime);
	
	// Make sure the manual GetUnitsPerCPUCycle and GetCPUCycle yield results that are equivalent to using Start/Stop/GetElapsedTime.
		uint64_t       nCPUCycles        = Stopwatch::GetCPUCycle() - nStartCPUCycle;
		const float    fUnitsPerCPUCycle = Stopwatch::GetUnitsPerCPUCycle(Stopwatch::kUnitsSeconds);
		const float    fCPUTime          = nCPUCycles * fUnitsPerCPUCycle;
		const uint64_t nCPUTime          = (uint64_t)(int64_t)(fCPUTime + 0.49999f);       // Add 0.49999f like GetElapsedTime does for integer rounding

		EATEST_VERIFY_F(nCPUTime == 1, "TestStopwatch failure: nCPUTime: %I64us, expected: 1s", nCPUTime);
	#endif

	/*{
		// Regression to exercize the report that PS4 rdtsc is very slow. Conclusion: it is in fact not intrinsically slow and executes similar to XBox One rdtsc. However, something about the user's code may have been causing usage of it to act slowly for them.
		uint64_t i = 0;
		uint64_t sum = 0; // We have this only to prevent the GetCPUCycle call from being optimized away.
		EA::StdC::Stopwatch stopwatchTime(EA::StdC::Stopwatch::kUnitsMilliseconds, true);
		uint64_t start = EA::StdC::Stopwatch::GetCPUCycle();

		do
		{
			sum += EA::StdC::Stopwatch::GetCPUCycle();
			sum += EA::StdC::Stopwatch::GetCPUCycle();
			sum += EA::StdC::Stopwatch::GetCPUCycle();
			sum += EA::StdC::Stopwatch::GetCPUCycle();
			sum += EA::StdC::Stopwatch::GetCPUCycle();
			sum += EA::StdC::Stopwatch::GetCPUCycle();
			sum += EA::StdC::Stopwatch::GetCPUCycle();
			i += 8;
		} while(stopwatchTime.GetElapsedTime() < 1000);

		uint64_t stop = EA::StdC::Stopwatch::GetCPUCycle();
		uint64_t cost = ((stop - start) / i);
		EA::UnitTest::Report("Stopwatch::GetCPUCycle cost per call: ~%I64u cycles (overestimated due to other stuff we do here). Sum = %I64u (irrelevant).", cost, sum);
	}*/

	return nErrorCount;
}

















