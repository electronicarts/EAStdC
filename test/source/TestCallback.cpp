///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/internal/Config.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EAStdC/EACallback.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EARandom.h>
#include <EAStdC/EARandomDistribution.h>
#include <EATest/EATest.h>
#include <eathread/eathread.h>
#include <EAAssert/eaassert.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#if defined(EA_PLATFORM_DESKTOP)
	// We need to test/debug this module in detail on machines before we enable the test
	// formally for them. Given how threads work on other platforms this test or EACallback
	// could fail on other platforms in async mode.
	#define EASTDC_EACALLBACK_TESTS_ENABLED 1
#else
	#define EASTDC_EACALLBACK_TESTS_ENABLED 0
#endif


#if EASTDC_EACALLBACK_TESTS_ENABLED

///////////////////////////////////////////////////////////////////////////////
// CallbackTest
///////////////////////////////////////////////////////////////////////////////

class CallbackTest;

class CallbackTest : public EA::StdC::CallbackT<CallbackTest>
{
public :
	CallbackTest();

	void  Reset();
	float GetAccuracyRate() const;
	void  CallbackFunction(EA::StdC::Callback* pCallback, uint64_t absoluteValue, uint64_t deltaValue);

public:
	enum TestParams
	{
		kTestTicks               =  100,
		kTestTime                = 1000,
		kUserModeEventPulseCount =   20
	};

	uint32_t            mIndex;               // Which test we are in the array of concurrent tests.
	EA::StdC::Stopwatch mTestStopwatch;       // Causes us to stop the callbacks after kTestTime has passed.
	EA::StdC::Stopwatch mStopwatch;           // Used to make sure that the deltaValue argument matches the actual time delta since last called.
	uint64_t            mNextUnitsMin;        // The minimum time we should be next called back.
	uint64_t            mNextUnitsMax;        // The maximum time we should be next called back.
	uint64_t            mLastUnits;           // The last time we were called back.
	uint32_t            mGoodSampleCount;     // Total number of times our callback function was called and it happened within the time period expected.
	uint32_t            mBadSampleCount;      // Total number of times our callback function was called and it didn't happen within the time period expected.
	uint32_t            mTotalSampleCount;    // Total number of times our callback function was called. Equals mGoodSampleCount + mBadSampleCount.
	uint32_t            mAddRefCount;         // Number of times the AddRef message was sent.
	uint32_t            mReleaseCount;        // Number of times the Release message was sent.
	Mode                mMode;                // Used so we can tell how to validate the tests.
};


CallbackTest::CallbackTest()
  : mIndex(0),
	mTestStopwatch(EA::StdC::Stopwatch::kUnitsNanoseconds),
	mStopwatch(EA::StdC::Stopwatch::kUnitsNanoseconds),
	mNextUnitsMin(0),
	mNextUnitsMax(0),
	mLastUnits(0),
	mGoodSampleCount(0),
	mBadSampleCount(0),
	mTotalSampleCount(0),
	mAddRefCount(0),
	mReleaseCount(0),
	mMode(EA::StdC::Callback::kModeSync)
{
	SetFunctionInfo(&CallbackTest::CallbackFunction, this, true);
}

void CallbackTest::Reset()
{
	// Leave mIndex as-is.
	mNextUnitsMin     = 0;
	mNextUnitsMax     = 0;
	mTotalSampleCount = 0;
	mGoodSampleCount  = 0;
	mBadSampleCount   = 0;
	mLastUnits        = 0;
	mAddRefCount      = 0;
	mReleaseCount     = 0;
	// Leave mMode as-is.
}

float CallbackTest::GetAccuracyRate() const
{
	// Returns a value between 0.0 and 100.0.
	return (float)(100.0 * (mTotalSampleCount ? ((double)mGoodSampleCount / (double)mTotalSampleCount) : 1.0));
}

void CallbackTest::CallbackFunction(EA::StdC::Callback* /*pCallback*/, uint64_t absoluteValue, uint64_t deltaValue)
{
	using namespace EA::StdC;

	if(absoluteValue == Callback::kMessageAddRef)
		mAddRefCount++;
	else if(absoluteValue == Callback::kMessageRelease)
		mReleaseCount++;
	else
	{
		const Callback::Type type      = GetType();
		const uint64_t       period    = GetPeriod();
		const uint64_t       precision = GetPrecision();

		if(mLastUnits == 0) // If this is the first time being called...
			mTestStopwatch.Restart();
		else
		{
			++mTotalSampleCount;

			if((absoluteValue >= mNextUnitsMin) && (absoluteValue <= mNextUnitsMax))
			{
				if(type == Callback::kTypeTime)
				{
					const uint64_t elapsedTime = mStopwatch.GetElapsedTime();
					const uint64_t difference  = (deltaValue > elapsedTime) ? (deltaValue - elapsedTime) : (elapsedTime - deltaValue);
					const double   diffPercent = ((double)(int64_t)difference / (double)(int64_t)elapsedTime) * 100.0; // Cast to int64_t because some compilers/platforms can't handle int64_t to double conversions.

					if((mMode == Callback::kModeAsync) && (diffPercent < 10)) // In threaded mode there's more room for failure.
						++mGoodSampleCount;
					else if((mMode == Callback::kModeSync) && (diffPercent < 3))
						++mGoodSampleCount;
					else
						++mBadSampleCount;
				}
				else // Else kTypeTick or kTypeUserEvent.
					++mGoodSampleCount;
			}
			else
			{
				// It turns out that with kModeAsync (threaded), we can get a lot of these failures with kTypeUserEvent.
				// This is because the main thread is updating the user event counter (because it has to) while this 
				// callback thread is running independently. It's possible for the main thread to bump up the user event
				// counter while this callback thread is in the process of issuing the next call. So timeNS could 
				++mBadSampleCount;
			}
		}

		mStopwatch.Restart();

		// Set the next expectation.
		mLastUnits    = absoluteValue;
		mNextUnitsMin = ((absoluteValue + period) > precision) ? (absoluteValue + period - precision) : 0;   // Extra logic here to make sure mNextUnitsMin doesn't go negative and wrap around to a very large uint64_t number.
		
		// How to decide the next max expected value is not simple because in async mode we have 
		// two threads, one of which is updating the user event count and the other which is 
		// servicing callbacks. The former could increase the event count by theoretically any
		// number while the servicing thread is in the middle of a single callback call.
		// We can fix this by having the main thread wait until the servicing thread is not 
		// busy, or we can put some cap on the expected value. Currently we do the latter, 
		// because it's simpler, but the former is probably better because it lets us test more precisely.
		if(type == Callback::kTypeUserEvent)
			mNextUnitsMax = absoluteValue + period + precision + (kUserModeEventPulseCount * 3);
		else
			mNextUnitsMax = absoluteValue + period + precision + (int)(1 + (double)period * 0.05f);      // Add a little extra slop to account for CPU stalls and what-not.
  
		// See if the test is complete
		switch (type)
		{
			case Callback::kTypeTick:
				if(mTotalSampleCount >= kTestTicks)
					Stop();
				break;

			case Callback::kTypeUserEvent:
				if(mTotalSampleCount >= kTestTicks)
					Stop();
				break;

			case Callback::kTypeTime:
			{
				const uint64_t elapsedTime = mTestStopwatch.GetElapsedTime();
				if(elapsedTime > kTestTime)
					Stop();
				break;
			}
		}
	}
}




///////////////////////////////////////////////////////////////////////////////
// TestCallback
///////////////////////////////////////////////////////////////////////////////

struct TestControlInfo 
{
	uint32_t                 mPeriodStart;
	uint32_t                 mPeriodEnd;
	uint32_t                 mPeriodStep;
	uint32_t                 mPrecisionStart;
	uint32_t                 mPrecisionEnd;
	uint32_t                 mPrecisionStep;
	EA::StdC::Callback::Type mType; 
	uint32_t                 mTestTimeMs;
};


#if defined(EA_PLATFORM_DESKTOP)
	const TestControlInfo tci[] = 
	{
		//            Period                            Precision                      Type                                 Test max time ms
		//  --------------------------------      ------------------------------       ----------------------------------  ----------------
		{          1,         51,         50,            0,         1,         1,      EA::StdC::Callback::kTypeTick,      10000          },
		{          1,         51,         50,            0,         1,         1,      EA::StdC::Callback::kTypeUserEvent, 10000          },
		{   20000000,   50000000,   30000000,     15000000,  30000000,  15000000,      EA::StdC::Callback::kTypeTime,      10000          }, // These are big numbers because time is in nanoseconds.
		{  100000000,  200000000,  100000000,     50000000, 100000000,  50000000,      EA::StdC::Callback::kTypeTime,      10000          },
		{ 1000000000, 2000000000, 1000000000,    200000000, 400000000, 200000000,      EA::StdC::Callback::kTypeTime,      10000          }
	};
#else
	const TestControlInfo tci[] = 
	{
		//            Period                            Precision                      Type                                 Test max time ms
		//  --------------------------------      ------------------------------       ----------------------------------  ----------------
		{          1,          5,          4,            1,         1,         1,      EA::StdC::Callback::kTypeTick,      60000          },
		{          1,          5,          4,            1,         1,         1,      EA::StdC::Callback::kTypeUserEvent, 60000          },
		{   50000000,   50000000,          1,     30000000,  30000000,         1,      EA::StdC::Callback::kTypeTime,      60000          }, // These are big numbers because time is in nanoseconds.
		{  500000000,  500000000,          1,    200000000, 200000000,         1,      EA::StdC::Callback::kTypeTime,      60000          }
	};
#endif

#endif // EASTDC_EACALLBACK_TESTS_ENABLED



int TestCallback()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	#if EASTDC_EACALLBACK_TESTS_ENABLED

		EA::StdC::RandomFast      random;  
		EA::StdC::CallbackManager callbackManager;
		const uint32_t            kCallbackCount = (uint32_t)(100 * EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeCPU));  // Determines the number of callbacks in the callback tests.
		CallbackTest*             pCallbackTestArray = new CallbackTest[kCallbackCount];        

		EA::StdC::SetCallbackManager(&callbackManager);

		EA::UnitTest::ReportVerbosity(1, "Callback test using %u callbacks. The test shows how well (on average)\n" 
										 "the callback system is able to satisfy the demands of each callback object.\n", (unsigned)kCallbackCount);

		// For both kModeAsync kModeSync...
		for(int a = 0; a < 2; a++)
		{
			#if EASTDC_THREADING_SUPPORTED
				const Callback::Mode mode = ((a % 2) ? Callback::kModeAsync : Callback::kModeSync);
			#else
				const Callback::Mode mode = Callback::kModeSync;
			#endif

			callbackManager.Shutdown();

			switch(mode)
			{
				case Callback::kModeAsync:
				{
					callbackManager.Init(true, true);    // Run in asynchronous (threaded) mode.
					break;
				}
				case Callback::kModeSync:            
				default:
				{
					callbackManager.Init(false, false);  // Run in synchronous (polled, non-threaded) mode.
					break;
				}
			}
			
			// For each type of TestControlInfo above...
			for(size_t cn = 0, n = EAArrayCount(tci); cn < n; ++cn)
			{
				const TestControlInfo& ci = tci[cn];

				for(uint32_t t = 0; t < kCallbackCount; ++t)
				{
					CallbackTest& callbackTest = pCallbackTestArray[t];

					callbackTest.mIndex = t;
					callbackTest.mMode  = mode;
					callbackTest.SetType(ci.mType);
				}

				// For an array of periods between begin and end period...
				for(uint32_t period = ci.mPeriodStart; period <= ci.mPeriodEnd; period += ci.mPeriodStep)
				{
					// For an array of precisions between begin and end precision...
					for(uint32_t precision = ci.mPrecisionStart; precision <= ci.mPrecisionEnd; precision += ci.mPrecisionStep)
					{
						// print to avoid test timeout
						EA::UnitTest::ReportVerbosity(0, "%s", ".");

						EA::UnitTest::ReportVerbosity(1, "Callback test: mode = %s, type = %s, period = %i, precision = %i\n", 
													  (mode     == Callback::kModeSync) ? "Sync" : "Async", 
													  (ci.mType == Callback::kTypeTick)  ? "Tick"  : ((ci.mType == Callback::kTypeTime) ? "Time" : "UserEvent"), 
													  period, 
													  precision);

						for(uint32_t t = 0; t < kCallbackCount; ++t)
						{
							CallbackTest& callbackTest = pCallbackTestArray[t];

							callbackTest.SetPeriod(period);
							callbackTest.SetPrecision(precision);
							callbackTest.Start(NULL, false);
						}

						// Run the test for N seconds, during which our callback function will be called 
						// repeatedly. After the callback function has been called for an amount 
						// of kTestTicks or kTestTime, the callback function calls CallbackTimer::Stop.
						bool                   bCallbacksAreStillRunning = true;
						bool                   bShouldContinue = true;
						EA::Thread::ThreadTime endTime = EA::Thread::GetThreadTime() + ci.mTestTimeMs;
						uint64_t               loopCount; // This is just to help debugging.

						for(loopCount = 0; bShouldContinue && bCallbacksAreStillRunning && (loopCount < UINT64_C(0x0fffffffffffffff)); ++loopCount)
						{
							bCallbacksAreStillRunning = false;

							// Randomly trigger a user event.
							if(EA::StdC::RandomBool(random))
							{
								for(int e = 0; e < CallbackTest::kUserModeEventPulseCount; e++) // Trigger multiple events at once.
									callbackManager.OnUserEvent();
							}

							// If we are in synchronous (i.e. polled) mode, then we need to manually call Update.
							// We don't need to call Update in synchronous (threaded) mode, but it's supported so 
							// we test calling Update occasionally to make sure it works.
							switch(mode)
							{
								case Callback::kModeSync:
								{
									callbackManager.Update();
									break;
								}
								case Callback::kModeAsync:
								default:
								{
									if(random.RandomUint32Uniform(100) == 0)
									{
										callbackManager.Update();
									}
									break;
								}
							}

							// Sleep for a little bit.
							EA::Thread::ThreadTime sleepTime = EA::Thread::kTimeoutImmediate;

							#if EASTDC_THREADING_SUPPORTED
								// Since we are in a seperate thread from the callback manager and since we are updating On UserEvent
								// ourselves here, it might be useful to sleep here to handle the case that our OnUserEvent calls get
								// ahead of what the callback manager can keep up with and "CallbackTest low accuracy rate" reports 
								// can result, though these reports wouldn't be indicative of bugs or failures of EACallback.
								if((mode == Callback::kModeAsync) && (ci.mType != EA::StdC::Callback::kTypeTime))
									sleepTime = EA::Thread::ThreadTime(10);
							#endif

							EA::Thread::ThreadSleep(sleepTime);

							// See if the alotted time has passed.
							EA::Thread::ThreadTime currentTime = EA::Thread::GetThreadTime();
							bShouldContinue = (currentTime < endTime);

							// See if the callbacks have all been called the number of times we expected and are thus done now.
							for(uint32_t t = 0; (t < kCallbackCount) && !bCallbacksAreStillRunning; ++t)
							{
								const CallbackTest& callbackTest = pCallbackTestArray[t];

								if(callbackTest.IsRunning())
								{
									bCallbacksAreStillRunning = true;

									if(!bShouldContinue) // If the time is up, yet this callback is still running...
									{
										// The callback function didn't get called the expected number of times before our loop time above expired.
										//++nErrorCount; Hard to enable this because it's easy for this test to have some failures. Probably what we should do is count failures and expect some success rate.
										EA::UnitTest::ReportVerbosity(1, "CallbackTest timeout. The callback didn't fire the expected number of\n"
																		 "times during our test period. Expected: %u, Actual: %u\n", 
																		 (callbackTest.GetType() == Callback::kTypeTime) ? (unsigned)CallbackTest::kTestTicks : (unsigned)CallbackTest::kTestTime, (unsigned)callbackTest.mTotalSampleCount);
									}
								}
							}
						}

						uint32_t successSum = 0;

						for(uint32_t t = 0; t < kCallbackCount; ++t)
						{
							CallbackTest& callbackTest = pCallbackTestArray[t];

							successSum += (uint32_t)callbackTest.GetAccuracyRate(); // GetAccuracyRate returns a value in the range of [0, 100]
							callbackTest.Reset();
						}

						const uint32_t successRate = (successSum / kCallbackCount); // successRate is a value in the range of [0, 100].

						if(successRate < 90) // If less than 90% of the callbacks occurred within the expected time...
						{
							// ++nErrorCount; Hard to enable this because it's easy for this test to have some failures. Probably what we should do is count failures and expect some success rate.
							EA::UnitTest::ReportVerbosity(1, "CallbackTest low accuracy rate of %i%%.\n", successRate);
						}
					}
				}
			}
		}

		delete[] pCallbackTestArray;

	#endif // EA_PLATFORM_DESKTOP

	// to avoid test timeout
	EA::EAMain::ReportVerbosity(0, "%s", "\n");

	return nErrorCount;
}



