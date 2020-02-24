///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EADateTime.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAMemory.h>
#include <EAStdCTest/EAStdCTest.h>
#include <eathread/eathread.h>
#include <EATest/EATest.h>
#include <string.h>
#include <EAAssert/eaassert.h>


#if defined(EA_PLATFORM_MICROSOFT)
	EA_DISABLE_ALL_VC_WARNINGS()
	#if defined(EA_PLATFORM_XBOXONE) || defined(CS_UNDEFINED_STRING)
		#include <winsock2.h> // for timeval
	#else
		#include <winsock.h> // for timeval
	#endif
	#include <Windows.h>
	EA_RESTORE_ALL_VC_WARNINGS()

	bool GetLocaleInfoHelper(LCTYPE lcType, char* lcData, int cchData)
	{
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			return (GetLocaleInfoA(LOCALE_USER_DEFAULT, lcType, lcData, cchData) != 0);
		#else
			wchar_t* temp = static_cast<wchar_t*>(EAAlloca(cchData * sizeof(wchar_t)));
			const bool res = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, lcType, temp, cchData) != 0;
			EA::StdC::Strlcpy(lcData, temp, static_cast<size_t>(cchData));
			return res;
		#endif
	}
#endif

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 6011) // Dereferencing NULL pointer.
#endif


#if defined(EA_HAVE_localtime_DECL)
	static int VerifyTmMatch(const tm& tmTest, const EA::StdC::DateTime& dtTest)
	{
		using namespace EA::StdC;

		int nErrorCount = 0;

		tm temp;

		// What we do here is the same as DateTimeToTm(), but also serves to test it:
		temp.tm_year = dtTest.GetParameter(kParameterYear) - 1900;      // tm_year: years since 1900
		temp.tm_mon  = dtTest.GetParameter(kParameterMonth) - 1;        // tm_mon:  months since January - [0,11]
		temp.tm_mday = dtTest.GetParameter(kParameterDayOfMonth);       // tm_mday: day of the month - [1,31]
		temp.tm_wday = dtTest.GetParameter(kParameterDayOfWeek) - 1;    // tm_wday: days since Sunday - [0,6]
		temp.tm_yday = dtTest.GetParameter(kParameterDayOfYear) - 1;    // tm_yday: days since January 1 - [0,365]
		temp.tm_hour = dtTest.GetParameter(kParameterHour);             // tm_hour: hours since midnight - [0,23]
		temp.tm_min  = dtTest.GetParameter(kParameterMinute);           // tm_min:  minutes after the hour - [0,59
		temp.tm_sec  = dtTest.GetParameter(kParameterSecond);           // tm_sec:  seconds after the minute - [0,59]

		EATEST_VERIFY(temp.tm_year == tmTest.tm_year);
		EATEST_VERIFY(temp.tm_mon  == tmTest.tm_mon);
		EATEST_VERIFY(temp.tm_mday == tmTest.tm_mday);
		EATEST_VERIFY(temp.tm_wday == tmTest.tm_wday);
		EATEST_VERIFY(temp.tm_yday == tmTest.tm_yday);
		EATEST_VERIFY(temp.tm_hour == tmTest.tm_hour);
		EATEST_VERIFY(temp.tm_min  == tmTest.tm_min);
		EATEST_VERIFY(temp.tm_sec  == tmTest.tm_sec);

		return nErrorCount;
	}

	static int VerifyTmMatch(const tm& tmA, const tm& tmB)
	{
		int nErrorCount = 0;

		EATEST_VERIFY(tmA.tm_year == tmB.tm_year);
		EATEST_VERIFY(tmA.tm_mon  == tmB.tm_mon);
		EATEST_VERIFY(tmA.tm_mday == tmB.tm_mday);
		EATEST_VERIFY(tmA.tm_wday == tmB.tm_wday);
		EATEST_VERIFY(tmA.tm_yday == tmB.tm_yday);
		EATEST_VERIFY(tmA.tm_hour == tmB.tm_hour);
		EATEST_VERIFY(tmA.tm_min  == tmB.tm_min);
		EATEST_VERIFY(tmA.tm_sec  == tmB.tm_sec);

		return nErrorCount;
	}
#endif


#if defined(EA_HAVE_localtime_DECL)
	// TestGMTime
	//
	// Tests an individual univeral time value by comparing it to what the standard C gmtime and 
	// localtime functions say on platforms that support these functions. The EAStdC DateTime class
	// is somewhat like a C++ version of the tm struct, with member functions and finer precision (nanoseconds).
	// The proper calculations for time can be tricky, and we use a conforming Standard C library 
	// implementation as a benchmark for results. 
	// We equate GM time with Universal time.
	//
	static int TestGMTime(time_t timeGM)
	{
		using namespace EA::StdC;

		int nErrorCount = 0;

		tm        tmLocal = *localtime(&timeGM);
		tm        tmGM    = *gmtime(&timeGM);
		tm        tmLocalEA;
		DateTime  dt;

		// void TmToDateTime(const tm& time, DateTime& dateTime);
		TmToDateTime(tmLocal, dt);
		nErrorCount += VerifyTmMatch(tmLocal, dt);

		// void DateTimeToTm(const DateTime& dateTime, tm& time);
		DateTimeToTm(dt, tmLocalEA);
		nErrorCount += VerifyTmMatch(tmLocal, tmLocalEA);

		// Set(uint32_t nYear, uint32_t nMonth, uint32_t nDayOfMonth, uint32_t nHour = 0, uint32_t nMinute = 0, uint32_t nSecond = 0, uint32_t nNanosecond = 0);
		dt.Set(1900 + tmGM.tm_year, tmGM.tm_mon + 1, tmGM.tm_mday, tmGM.tm_hour, tmGM.tm_min, tmGM.tm_sec, 0);
		nErrorCount += VerifyTmMatch(tmGM, dt);

		// DateTime(uint32_t nYear, uint32_t nMonth, uint32_t nDayOfMonth, uint32_t nHour = 0, uint32_t nMinute = 0, uint32_t nSecond = 0, uint32_t nNanosecond = 0);
		dt = DateTime(1900 + tmGM.tm_year, tmGM.tm_mon + 1, tmGM.tm_mday, tmGM.tm_hour, tmGM.tm_min, tmGM.tm_sec, 0);
		nErrorCount += VerifyTmMatch(tmGM, dt);

		// int64_t GetTimeZoneBias();
		// bool    IsDSTDateTime(int64_t dateTimeSeconds);
		// int64_t GetDaylightSavingsBias();
		const int32_t timeZoneBias        = (int32_t)GetTimeZoneBias();
		const bool    isDaylightSavings   = IsDSTDateTime(TimeTSecondsSecondsToDateTime(timeGM));
		const int32_t daylightSavingsBias = (int32_t)GetDaylightSavingsBias();

		dt.Set(1900 + tmGM.tm_year, tmGM.tm_mon + 1, tmGM.tm_mday, tmGM.tm_hour, tmGM.tm_min, tmGM.tm_sec, 0);
		dt.AddTime(kParameterSecond, timeZoneBias); // Offset by the time zone bias.
		EATEST_VERIFY(isDaylightSavings == (tmLocal.tm_isdst > 0));
		if(isDaylightSavings)
			dt.AddTime(kParameterSecond, daylightSavingsBias); // Offset by the daylight savings time bias.

		nErrorCount += VerifyTmMatch(tmLocal, dt);

		return nErrorCount;
	}
#endif


#define LOCAL_MAX(x, y) ((x) > (y) ? (x) : (y))

int TestDateTime()
{
	using namespace EA::StdC;

	int nErrorCount(0);

	DateTime dateTimeTest(1970, 1, 1, 0, 0, 0);
	EATEST_VERIFY(DateTimeSecondsToTimeTSeconds(dateTimeTest.GetSeconds()) == 0);

	// C tm struct conversion and setting local time.
	#if defined(EA_HAVE_localtime_DECL)
		// Regression of end-of-month bug, where it is June 1 in Universal time but May 31 in local time (California, -8 hours from Prime Meridian).
		// nTime = 1338519441: tm_isdst = 1, tm_yday = 151, tm_wday = 4, tm_year = 112, tm_mon = 4, tm_mday = 31, tm_hour = 19, tm_min = 57, tm_sec = 21.
		nErrorCount += TestGMTime(1338519441);

		// Test a big range of dates
		// http://www.convert-unix-time.com/?t=946080000
		time_t timeGMBegin  =    946684800; // UTC: Saturday 1st January 2000 12:00:00 AM
		time_t timeGMEnd    =   1207008000; // UTC: Tuesday 1st April 2008 12:00:00 AM
		time_t timeInterval = time_t(2220.0f / LOCAL_MAX(0.1f, EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeCPU))); // 37 minutes divided by the CPU relative speed (making this test run at acceptable speed on slow platforms)
		int    errorCount   = 0;

		for(time_t t = timeGMBegin; (t < timeGMEnd) && !errorCount; t += timeInterval)
		{
			errorCount = TestGMTime(t);
			nErrorCount += errorCount;
		}
	#endif


	{   // Verify that DateTime matches time()/localtime()
		#if defined(EA_HAVE_localtime_DECL)
			DateTime      dateTime2(EA::StdC::kTimeFrameLocal);
			const  time_t nTime = time(NULL);
			struct tm*    pTime = localtime(&nTime);
			uint32_t      value;
			int           i;

			// We have a small problem: it's possible that the system time turned over to a 
			// new second right between the two time calls below. If that seems to be the case
			// then we do an update of dateTime2 here, which should execute within the same second.
			// Only in a pathological case would it not execute within the same second.
			for(i = 0; (i < 5) && (dateTime2.GetParameter(kParameterSecond) != (uint32_t)pTime->tm_sec); i++)
			{
				dateTime2 = DateTime(EA::StdC::kTimeFrameLocal);
				pTime     = localtime(&nTime);
			}
			EATEST_VERIFY(i < 5); // Sanity check. i should almost always be 0, and rarely be 1. Just about never anything higher.

			value = dateTime2.GetParameter(kParameterYear);
			EATEST_VERIFY_F(value == (uint32_t)(1900 + pTime->tm_year), "TestDateTime DateTime year failure: value: %u, expected: %u. DateTime seconds: %I64u, time_t: %I64d", 
										value, (uint32_t)(1900 + pTime->tm_year), dateTime2.GetSeconds(), (int64_t)nTime);

			value = dateTime2.GetParameter(kParameterMonth);
			EATEST_VERIFY_F(value == (uint32_t)(kMonthJanuary + pTime->tm_mon), "TestDateTime DateTime month failure: value: %u, expected: %u. DateTime seconds: %I64u, time_t: %I64d", 
										value, (uint32_t)(kMonthJanuary + pTime->tm_mon), dateTime2.GetSeconds(), (int64_t)nTime);

			value = dateTime2.GetParameter(kParameterDayOfMonth);
			EATEST_VERIFY_F(value == (uint32_t)pTime->tm_mday, "TestDateTime DateTime day of month failure: value: %u, expected: %u. DateTime seconds: %I64u, time_t: %I64d", 
										value, (uint32_t)pTime->tm_mday, dateTime2.GetSeconds(), (int64_t)nTime);

			value = dateTime2.GetParameter(kParameterHour);
			EATEST_VERIFY_F(value == (uint32_t)pTime->tm_hour, "TestDateTime DateTime hour failure: value: %u, expected: %u. DateTime seconds: %I64u, time_t: %I64d",
										value, (uint32_t)pTime->tm_hour, dateTime2.GetSeconds(), (int64_t)nTime);

			value = dateTime2.GetParameter(kParameterMinute);
			EATEST_VERIFY_F(value == (uint32_t)pTime->tm_min, "TestDateTime DateTime minute failure: value: %u, expected: %u. DateTime seconds: %I64u, time_t: %I64d", 
										value, (uint32_t)pTime->tm_min, dateTime2.GetSeconds(), (int64_t)nTime);

			value = dateTime2.GetParameter(kParameterSecond);
			EATEST_VERIFY_F((value - (uint32_t)pTime->tm_sec < 5), "TestDateTime DateTime second failure: value: %u, expected: %u. DateTime seconds: %I64u, time_t: %I64d", 
										value, (uint32_t)pTime->tm_sec, dateTime2.GetSeconds(), (int64_t)nTime);
		#endif
	}

	{
		// Basic test of setting/getting parameters.
		DateTime dateTime(2004, 11, 9, 0, 0, 0);
		DateTime dateTime2(EA::StdC::kTimeFrameLocal);

		dateTime.SetParameter(kParameterYear, 1888);
		dateTime.SetParameter(kParameterMonth, 5);
		dateTime.SetParameter(kParameterDayOfMonth, 16);
		dateTime.SetParameter(kParameterHour, 16);
		dateTime.SetParameter(kParameterMinute, 44);
		dateTime.SetParameter(kParameterSecond, 36);

		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)1888);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)5);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)16);
		EATEST_VERIFY(dateTime.GetParameter(kParameterHour) == (uint32_t)16);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMinute) == (uint32_t)44);
		EATEST_VERIFY(dateTime.GetParameter(kParameterSecond) == (uint32_t)36);

		dateTime.SetParameter(kParameterDayOfYear, 244);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)244);

		dateTime.SetParameter(kParameterDayOfWeek, kDayOfWeekThursday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekThursday);

		dateTime.SetParameter(kParameterWeekOfYear, 23);
		EATEST_VERIFY(dateTime.GetParameter(kParameterWeekOfYear) == (uint32_t)23);

		dateTime.SetParameter(kParameterWeekOfMonth, 2);
		EATEST_VERIFY(dateTime.GetParameter(kParameterWeekOfMonth) == (uint32_t)2);


		// Basic day of week day determination
		dateTime.Set(2004, 9, 25, 0, 0, 0); // 9/25/2004 - Saturday

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekSaturday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2004);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)9);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)25);

		// Day of week determination on the very beginning of a year
		dateTime.Set(1995, 1, 1, 0, 0, 0); // 1/1/1995 - Sunday

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekSunday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)1995);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)1);

		// Day of week determination on the very beginning of a leap year
		dateTime.Set(1980, 1, 1, 0, 0, 0); // 1/1/1980 - Tuesday

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekTuesday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)1980);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)1);

		// Day of week determination on the very end of a year - note: it should be the 365th day of the year
		dateTime.Set(2007, 12, 31, 0, 0, 0); // 12/31/2007 - Monday

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekMonday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2007);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)12);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)31);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)365);

		// Day of week determination on the very end of the leap year - note: it should be the 366th day of the year
		dateTime.Set(2004,12,31,0,0,0); // 12/31/2004 - Friday

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekFriday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2004);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)12);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)31);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)366);

		// Day of week determination on a leap year - note: this day exists only during leap years
		dateTime.Set(2004, 2, 29, 0, 0, 0); // 2/29/2004 - Sunday

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekSunday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2004);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)2);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)29);

		// Change the year and we no longer have 2/29 - instead we should have 3/1/2005 - Tuesday
		dateTime.Set(2005, 0xffffffff, 0xffffffff, 0, 0, 0);

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekTuesday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2005);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)3);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)1);

		// Go back to 1980 and we should have 2/29 back - Friday
		dateTime.Set(1980, 2, 29, 0, 0, 0);

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekFriday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)1980);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)2);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)29);


		// Wrapping test - handling of values intentionally out of range
		dateTime.Set(2004, 14, 32, 25, 66, 126);

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekSaturday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2005);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)3);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)5);
		EATEST_VERIFY(dateTime.GetParameter(kParameterHour) == (uint32_t)2);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMinute) == (uint32_t)8);
		EATEST_VERIFY(dateTime.GetParameter(kParameterSecond) == (uint32_t)6);


		// Changing the day of the week so that we go back to a different year 
		// 01/02/2008 - Wednesday - go back 2 days to Monday and we should have
		// 12/31/2007 - Monday
		dateTime.Set(2008, 1, 2, 0, 0, 0);

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekWednesday);

		dateTime.SetParameter(kParameterDayOfWeek,kDayOfWeekMonday);

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekMonday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2007);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)12);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)31);


		// Set day of year for a non-leap year
		dateTime.Set(1983, 1, 1, 0, 0, 0);
		dateTime.SetParameter(kParameterDayOfYear, 365);

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekSaturday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)1983);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)12);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)31);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)365);

		dateTime.SetParameter(kParameterDayOfYear, 366); // intentional overflow

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekSunday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)1984);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)1);


		// Set day of year for a leap year
		dateTime.Set(1984, 1, 1, 0, 0, 0);
		dateTime.SetParameter(kParameterDayOfYear, 366);

		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekMonday);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)1984);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)12);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)31);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)366);


		// Comparisons test
		dateTime.Set(1866, 1, 2, 20, 10, 8);
		dateTime2.Set(1866, 1, 2, 20, 10, 6);

		EATEST_VERIFY(dateTime.Compare(dateTime2,true,true) == 1);   // >
		EATEST_VERIFY(dateTime.Compare(dateTime2,true,false) == 0);  // ==
		EATEST_VERIFY(dateTime.Compare(dateTime2,false,true) == 1);  // >

		dateTime.Set(1866, 1, 1, 20, 10, 8);

		EATEST_VERIFY(dateTime.Compare(dateTime2,true,true) == -1);   // <
		EATEST_VERIFY(dateTime.Compare(dateTime2,true,false) == -1);  // <
		EATEST_VERIFY(dateTime.Compare(dateTime2,false,true) == 1);   // >


		// Arithmetic
		dateTime.Set(2004, 12, 30, 0, 0, 0);
		dateTime.AddTime(kParameterYear, -2);
		dateTime.AddTime(kParameterMonth, 1);

		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2003);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)30);

		dateTime.AddTime(kParameterMonth, -11); // note that we don't have 30 days in Feb hence the date should spill into March

		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2002);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)3);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)2);

		dateTime.AddTime(kParameterMonth, 25); // add more than one year

		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2004);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMonth) == (uint32_t)4);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)2);

		dateTime.AddTime(kParameterDayOfMonth, 10);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfMonth) == (uint32_t)12);

		dateTime.SetParameter(kParameterDayOfYear, 366);
		dateTime.AddTime(kParameterDayOfYear, 40);
		EATEST_VERIFY(dateTime.GetParameter(kParameterYear) == (uint32_t)2005);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)40);

		dateTime.SetParameter(kParameterDayOfWeek, kDayOfWeekMonday);
		dateTime.AddTime(kParameterDayOfWeek, 3);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekThursday);

		dateTime.SetParameter(kParameterHour, 10);
		dateTime.AddTime(kParameterHour, 16);
		dateTime.AddTime(kParameterHour, -1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterHour) == (uint32_t)1);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfWeek) == (uint32_t)kDayOfWeekFriday);

		dateTime.AddTime(kParameterMinute, 125);
		EATEST_VERIFY(dateTime.GetParameter(kParameterHour) == (uint32_t)3);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMinute) == (uint32_t)5);

		dateTime.AddTime(kParameterSecond, 65);
		EATEST_VERIFY(dateTime.GetParameter(kParameterMinute) == (uint32_t)6);
		EATEST_VERIFY(dateTime.GetParameter(kParameterSecond) == (uint32_t)5);

		dateTime.SetParameter(kParameterDayOfYear, 100);
		dateTime.AddTime(kParameterWeekOfYear, 2);
		EATEST_VERIFY(dateTime.GetParameter(kParameterDayOfYear) == (uint32_t)114);
	}

	// Nanosecond precision
	{
		// DateTime(int64_t nSeconds, uint32_t nNanosecond = 0);
		DateTime dtNS1(1000, 2000);
		EATEST_VERIFY(dtNS1.GetSeconds()     == 1000);
		EATEST_VERIFY(dtNS1.GetNanoseconds() == (1000 * EA::StdC::int128_t(1000000000)) + 2000);
		dtNS1.SetNanoseconds(3000);
		EATEST_VERIFY(dtNS1.GetSeconds()     == 0);
		EATEST_VERIFY(dtNS1.GetNanoseconds() == 3000);

		// DateTime(const int128_t& nanoseconds);
		// EA::StdC::int128_t GetNanoseconds() const;
		// void SetNanoseconds(const EA::StdC::int128_t& nanoseconds);
		DateTime dtNS2;
		EA::StdC::int128_t nsNow = dtNS2.GetNanoseconds();
		DateTime dtNS3(nsNow);
		EATEST_VERIFY(dtNS2 == dtNS3);

		nsNow -= (INT64_C(1000000000) * 3600); // Subtrace one hour
		dtNS2.SetNanoseconds(nsNow);
		EATEST_VERIFY((dtNS2.GetSeconds() + 3600) == dtNS3.GetSeconds());

		// void Set(uint32_t nYear, uint32_t nMonth,  uint32_t nDay, uint32_t nHour, 
		//          uint32_t nMinute, uint32_t nSecond, uint32_t nNanosecond);
		dtNS3.Set(dtNS2.GetParameter(kParameterYear), dtNS2.GetParameter(kParameterMonth),
				  dtNS2.GetParameter(kParameterDayOfMonth), dtNS2.GetParameter(kParameterHour),
				  dtNS2.GetParameter(kParameterMinute), dtNS2.GetParameter(kParameterSecond),
				  dtNS2.GetParameter(kParameterNanosecond));

		EATEST_VERIFY(dtNS2 == dtNS3);
	}

	// Millisecond precision
	{
		DateTime dtNS1(1);  // 1 second
		EATEST_VERIFY(dtNS1.GetSeconds() == 1);
		EATEST_VERIFY(dtNS1.GetMilliseconds() == 1000);

		dtNS1.SetMilliseconds(8);
		EATEST_VERIFY(dtNS1.GetNanoseconds() == 8000000);
		EATEST_VERIFY(dtNS1.GetMilliseconds() == 8);
		EATEST_VERIFY(dtNS1.GetSeconds() == 0);

		dtNS1.SetNanoseconds(1);
		EATEST_VERIFY(dtNS1.GetSeconds() == 0);
		EATEST_VERIFY(dtNS1.GetMilliseconds() == 0);
		EATEST_VERIFY(dtNS1.GetNanoseconds() == 1);

		dtNS1.SetNanoseconds(8000006);
		EATEST_VERIFY(dtNS1.GetSeconds() == 0);
		EATEST_VERIFY(dtNS1.GetMilliseconds() == 8);
		EATEST_VERIFY(dtNS1.GetNanoseconds() == 8000006);
	}

	// void DateTimeToFileTime(const DateTime& dateTime, _FILETIME& time);
	// void FileTimeToDateTime(const _FILETIME& time, DateTime& dateTime);
	// void DateTimeToSystemTime(const DateTime& dateTime, _SYSTEMTIME& time);
	// void SystemTimeToDateTime(const _SYSTEMTIME& time, DateTime& dateTime);
	#if defined(EA_PLATFORM_MICROSOFT)
	{
		DateTime    dateTime(1234, 5, 6, 7, 8, 9);
		DateTime    dateTime2(6789, 6, 5, 4, 3, 2);
		_FILETIME   fileTime;
		_SYSTEMTIME systemTime;

		// Our current test is feeble: it merely converts to FILETIME and 
		// then back and verifies that the time is the same. 
		dateTime.Set();

		DateTimeToFileTime(dateTime, fileTime);
		FileTimeToDateTime(fileTime, dateTime2);
		EATEST_VERIFY(dateTime == dateTime2);

		DateTimeToSystemTime(dateTime, systemTime);
		SystemTimeToDateTime(systemTime, dateTime2);
		EATEST_VERIFY(dateTime == dateTime2);
	}
	#endif

	{
		// http://pubs.opengroup.org/onlinepubs/007908799/xsh/strftime.html
		// http://msdn.microsoft.com/en-us/library/fe06s4ak%28v=VS.100%29.aspx

		// Posix alternative formats:
		// "Some conversion specifiers can be modified by the E or O modifier characters 
		//  to indicate that an alternative format or specification should be used rather 
		//  than the one normally used by the unmodified conversion specifier. If the 
		//  alternative format or specification does not exist for the current locale, 
		//  (see ERA in the XBD specification, Section 5.3.5) the behaviour will be as if 
		//  the unmodified conversion specification were used."
		//
		// Microsoft alternative formats:
		// Also, Microsoft (alternatively to Posix  E and O) supports using the # char 
		// after % to indicate alternative behaviour as follows:
		//     %#a, %#A, %#b, %#B, %#h, %#p, %#X, %#z, %#Z, %#%             # flag is ignored.
		//     %#c                                                          Long date and time representation, appropriate for current locale. For example: "Tuesday, March 14, 1995, 12:41:29".
		//     %#x                                                          Long date representation, appropriate to current locale. For example: "Tuesday, March 14, 1995".
		//     %#d, %#H, %#I, %#j, %#m, %#M, %#S, %#U, %#w, %#W, %#y, %#Y   Remove leading zeros (if any).


		// %a   Replaced by the locale's abbreviated weekday name. [ tm_wday]
		// %A   Replaced by the locale's full weekday name. [ tm_wday]
		// %b   Replaced by the locale's abbreviated month name. [ tm_mon]
		// %B   Replaced by the locale's full month name. [ tm_mon]
		// %c   Replaced by the locale's appropriate date and time representation. (See the Base Definitions volume of IEEE Std 1003.1-2001, <time.h>.)
		// %C   Replaced by the year divided by 100 and truncated to an integer, as a decimal number [00,99]. [ tm_year]
		// %d   Replaced by the day of the month as a decimal number [01,31]. [ tm_mday]
		// %D   Equivalent to %m / %d / %y. [ tm_mon, tm_mday, tm_year]
		// %e   Replaced by the day of the month as a decimal number [1,31]; a single digit is preceded by a space. [ tm_mday]
		// %F   Equivalent to %Y - %m - %d (the ISO 8601:2000 standard date format). [ tm_year, tm_mon, tm_mday]
		// %g   Replaced by the last 2 digits of the week-based year (see below) as a decimal number [00,99]. [ tm_year, tm_wday, tm_yday]
		// %G   Replaced by the week-based year (see below) as a decimal number (for example, 1977). [ tm_year, tm_wday, tm_yday]
		// %h   Equivalent to %b. [ tm_mon]
		// %H   Replaced by the hour (24-hour clock) as a decimal number [00,23]. [ tm_hour]
		// %I   Replaced by the hour (12-hour clock) as a decimal number [01,12]. [ tm_hour]
		// %j   Replaced by the day of the year as a decimal number [001,366]. [ tm_yday]
		// %m   Replaced by the month as a decimal number [01,12]. [ tm_mon]
		// %M   Replaced by the minute as a decimal number [00,59]. [ tm_min]
		// %n   Replaced by a <newline>.
		// %p   Replaced by the locale's equivalent of either a.m. or p.m. [ tm_hour]
		// %r   Replaced by the time in a.m. and p.m. notation; [CX] [Option Start]  in the POSIX locale this shall be equivalent to %I : %M : %S %p. [Option End] [ tm_hour, tm_min, tm_sec]
		// %R   Replaced by the time in 24-hour notation ( %H : %M ). [ tm_hour, tm_min]
		// %S   Replaced by the second as a decimal number [00,60]. [ tm_sec]
		// %t   Replaced by a <tab>.
		// %T   Replaced by the time ( %H : %M : %S ). [ tm_hour, tm_min, tm_sec]
		// %u   Replaced by the weekday as a decimal number [1,7], with 1 representing Monday. [ tm_wday]
		// %U   Replaced by the week number of the year as a decimal number [00,53]. The first Sunday of January is the first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]
		// %V   Replaced by the week number of the year (Monday as the first day of the week) as a decimal number [01,53]. If the week containing 1 January has four or more days in the new year, then it is considered week 1. Otherwise, it is the last week of the previous year, and the next week is week 1. Both January 4th and the first Thursday of January are always in week 1. [ tm_year, tm_wday, tm_yday]
		// %w   Replaced by the weekday as a decimal number [0,6], with 0 representing Sunday. [ tm_wday]
		// %W   Replaced by the week number of the year as a decimal number [00,53]. The first Monday of January is the first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]
		// %x   Replaced by the locale's appropriate date representation. (See the Base Definitions volume of IEEE Std 1003.1-2001, <time.h>.)
		// %X   Replaced by the locale's appropriate time representation. (See the Base Definitions volume of IEEE Std 1003.1-2001, <time.h>.)
		// %y   Replaced by the last two digits of the year as a decimal number [00,99]. [ tm_year]
		// %Y   Replaced by the year as a decimal number (for example, 1997). [ tm_year]
		// %z   Replaced by the offset from UTC in the ISO 8601:2000 standard format ( +hhmm or -hhmm ), or by no characters if no timezone is determinable. For example, "-0430" means 4 hours 30 minutes behind UTC (west of Greenwich). [CX] [Option Start]  If tm_isdst is zero, the standard time offset is used. If tm_isdst is greater than zero, the daylight savings time offset is used. If tm_isdst is negative, no characters are returned. [Option End] [ tm_isdst]
		// %Z   Replaced by the timezone name or abbreviation, or by no bytes if no timezone information exists. [ tm_isdst]
		// %%   Replaced by %. 

		const size_t kBufferSize = 2048;
		char*        pBuffer = new char[kBufferSize];
		tm           tmValue1, tmValue2;
		size_t       n;
		char*     p;

		{
			DateTime dt(1999, 12, 31, 23, 59, 58);

			memset(&tmValue1, 0, sizeof(tmValue1));
			memset(&tmValue2, 0, sizeof(tmValue2));
			DateTimeToTm(dt, tmValue1);

			// In our simplest test, we simply make a string with all formats and make sure it doesn't crash or hang.
			Strftime(pBuffer, kBufferSize, "%a | %#a | %A | %#A | %b | %#b | %B | %#B | %c | %#c | %C | %d | %#d | %D | %e | %h | %H | %#H | %I | %#I | %j | %#j | %m | %#m | %M | %#M | %n | %p | %#p | %r | %R | %S | %#S | %t | %T | %u | %U | %#U | %V | %w | %#w | %W | %#W | %x | %#x | %X | %#X | %y | %#y | %Y | %#Y | %Z | %#Z | %% | %#%", &tmValue1, NULL);

			// Generate a string buffer and parse it. The format string differs from the above as some specifiers
			// like %u and %W aren't supported by Strptime.
			Strftime(pBuffer, kBufferSize, "%a | %#a | %A | %#A | %b | %#b | %B | %#B | %c | %#c | %C | %d | %#d | %D | %e | %h | %H | %#H | %I | %#I | %j | %#j | %m | %#m | %M | %#M | %n | %p | %#p | %r | %R | %S | %#S | %t | %T | %w | %#w | %x | %#x | %X | %#X | %y | %#y | %Y | %#Y | %% | %#%", &tmValue1, NULL);
			char *ptr = Strptime(pBuffer, "%a | %#a | %A | %#A | %b | %#b | %B | %#B | %c | %#c | %C | %d | %#d | %D | %e | %h | %H | %#H | %I | %#I | %j | %#j | %m | %#m | %M | %#M | %n | %p | %#p | %r | %R | %S | %#S | %t | %T | %w | %#w | %x | %#x | %X | %#X | %y | %#y | %Y | %#Y | %% | %#%", &tmValue2, NULL);

			EATEST_VERIFY(ptr != NULL);
			EATEST_VERIFY(tmValue1.tm_sec   == tmValue2.tm_sec);
			EATEST_VERIFY(tmValue1.tm_min   == tmValue2.tm_min);
			EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
			EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
			EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
			EATEST_VERIFY(tmValue1.tm_wday  == tmValue2.tm_wday);
			EATEST_VERIFY(tmValue1.tm_yday  == tmValue2.tm_yday);
			EATEST_VERIFY(tmValue1.tm_isdst == tmValue2.tm_isdst);
		}

		{
			DateTime dt(2000, 1, 1, 0, 0, 0);

			{
				// %t   Replaced by a <tab>.
				// %%   Replaced by %. 
				// %n   Replaced by a <newline>.

				memcpy(&tmValue2, &tmValue1, sizeof(tmValue2));

				const char* kFormat = "%t %t%%%n%n%%";
				const char* kResult = "\t \t%\n\n%";
				n = Strftime(pBuffer, kBufferSize, kFormat, &tmValue1, NULL);
				EATEST_VERIFY((n == Strlen(kResult)) && (Strcmp(pBuffer, kResult) == 0));
				EATEST_VERIFY(memcmp(&tmValue1, &tmValue2, sizeof(tmValue2)) == 0);   // Verify that tmValue1 was not written to.
				p = Strptime(pBuffer, kFormat, &tmValue2, NULL);
				EATEST_VERIFY(p == (pBuffer + n));
				EATEST_VERIFY(memcmp(&tmValue1, &tmValue2, sizeof(tmValue2)) == 0);   // Verify that tmValue2 was not written to.
			}

			{
				// %a   Replaced by the locale's abbreviated weekday name. [ tm_wday]
				// %A   Replaced by the locale's full weekday name. [ tm_wday]

				const char* kExpectedResults[7] = 
				{
					"Sun | Sun | Sunday | Sunday",
					"Mon | Mon | Monday | Monday",
					"Tue | Tue | Tuesday | Tuesday",
					"Wed | Wed | Wednesday | Wednesday",
					"Thu | Thu | Thursday | Thursday",
					"Fri | Fri | Friday | Friday",
					"Sat | Sat | Saturday | Saturday",
				};

				for(uint32_t d = kDayOfWeekSunday; d <= kDayOfWeekSaturday; d++)
				{
					dt.SetParameter(kParameterDayOfWeek, d);
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%a | %#a | %A | %#A", &tmValue1, NULL);
					EA_COMPILETIME_ASSERT(kDayOfWeekSunday == 1);
					EATEST_VERIFY(n == Strlen(kExpectedResults[d - 1]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[d - 1]) == 0);  // d-1 because kDayOfWeekSunday is 1 and not 0.

					// This isn't the right way to test this; we need to test each format in turn and not all of them at once.
					p = Strptime(pBuffer, "%a | %#a | %A | %#A", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_wday  == tmValue2.tm_wday);
				}
			}

			{
				// %b   Replaced by the locale's abbreviated month name. [ tm_mon]
				// %B   Replaced by the locale's full month name. [ tm_mon]
				// %h   Equivalent to %b. [ tm_mon]
				// %m   Replaced by the month as a decimal number [01,12]. [ tm_mon]

				const char* kExpectedResults[12] = 
				{
					"Jan | Jan | January | January | Jan | Jan | 01 | 1",
					"Feb | Feb | February | February | Feb | Feb | 02 | 2",
					"Mar | Mar | March | March | Mar | Mar | 03 | 3",
					"Apr | Apr | April | April | Apr | Apr | 04 | 4",
					"May | May | May | May | May | May | 05 | 5",
					"Jun | Jun | June | June | Jun | Jun | 06 | 6",
					"Jul | Jul | July | July | Jul | Jul | 07 | 7",
					"Aug | Aug | August | August | Aug | Aug | 08 | 8",
					"Sep | Sep | September | September | Sep | Sep | 09 | 9",
					"Oct | Oct | October | October | Oct | Oct | 10 | 10",
					"Nov | Nov | November | November | Nov | Nov | 11 | 11",
					"Dec | Dec | December | December | Dec | Dec | 12 | 12",
				};

				for(uint32_t m = kMonthJanuary; m <= kMonthDecember; m++)
				{
					dt.SetParameter(kParameterMonth, m);
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%b | %#b | %B | %#B | %h | %#h | %m | %#m", &tmValue1, NULL);
					EA_COMPILETIME_ASSERT(kMonthJanuary == 1);
					EATEST_VERIFY(n == Strlen(kExpectedResults[m - 1]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[m - 1]) == 0); // m-1 because kMonthJanuary is 1 and not 0.

					// This isn't the right way to test this; we need to test each format in turn and not all of them at once.
					p = Strptime(pBuffer, "%b | %#b | %B | %#B | %h | %#h | %m | %#m", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_mon  == tmValue2.tm_mon);
				}
			}

			{
				// %c   Replaced by the locale's appropriate date and time representation. (See the Base Definitions volume of IEEE Std 1003.1-2001, <time.h>.)
				dt.SetParameter(kParameterYear, 2012);
				dt.SetParameter(kParameterMonth, 1);
				dt.SetParameter(kParameterDayOfMonth, 9);
				dt.SetParameter(kParameterHour, 13);
				dt.SetParameter(kParameterMinute, 24);
				dt.SetParameter(kParameterSecond, 5);
				DateTimeToTm(dt, tmValue1);

				const int kResultSize = 512;
				char kExpectedResult[kResultSize] = { '\0' };

				#if defined (EA_PLATFORM_WINDOWS)
					char kLongMonth[32];
					char kLongDayOfWeek[32];
					char kShortMonth[32];
					char kShortDayOfWeek[32];
					char shortDateFormat[80];
					char longDateFormat[80];
					char timeFormat[80];

					char* dateFormat[2] = 
					{
						&shortDateFormat[0],
						&longDateFormat[0],
					};

					char* ptr;
					size_t i = 0;
					kExpectedResult[0] = '\0';

					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SSHORTDATE, shortDateFormat, sizeof(shortDateFormat)/sizeof(shortDateFormat[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SLONGDATE, longDateFormat, sizeof(longDateFormat)/sizeof(longDateFormat[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_STIMEFORMAT, timeFormat, sizeof(timeFormat)/sizeof(timeFormat[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SABBREVDAYNAME1, kShortDayOfWeek, sizeof(kShortDayOfWeek)/sizeof(kShortDayOfWeek[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SABBREVMONTHNAME1, kShortMonth, sizeof(kShortMonth)/sizeof(kShortMonth[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SDAYNAME1, kLongDayOfWeek, sizeof(kLongDayOfWeek)/sizeof(kLongDayOfWeek[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SMONTHNAME1, kLongMonth, sizeof(kLongMonth)/sizeof(kLongMonth[0])) != 0);

					char tmp[kResultSize];
					for(int j = 0; j < 2; j++)
					{
						i = 0;
						pBuffer[0] = '\0';

						strncpy(tmp, dateFormat[j], kResultSize);

						ptr = strtok(tmp, "/, -");
						while(ptr != 0)
						{
							if(strncmp(ptr, "d", strlen(ptr)) == 0)
								strncat(pBuffer, "9", kBufferSize);
							else if(strncmp(ptr, "dd", strlen(ptr)) == 0)
								strncat(pBuffer, "09", kBufferSize);
							else if(strncmp(ptr, "ddd", strlen(ptr)) == 0)
								strncat(pBuffer, kShortDayOfWeek, kBufferSize);
							else if(strncmp(ptr, "dddd", strlen(ptr)) == 0)
								strncat(pBuffer, kLongDayOfWeek, kBufferSize);
							else if(strncmp(ptr, "M", strlen(ptr)) == 0)
								strncat(pBuffer, "1", kBufferSize);
							else if(strncmp(ptr, "MM", strlen(ptr)) == 0)
								strncat(pBuffer, "01", kBufferSize);
							else if(strncmp(ptr, "MMM", strlen(ptr)) == 0)
								strncat(pBuffer, kShortMonth, kBufferSize);
							else if(strncmp(ptr, "MMMM", strlen(ptr)) == 0)
								strncat(pBuffer, kLongMonth, kBufferSize);
							else if(strncmp(ptr, "y", strlen(ptr)) == 0)
								strncat(pBuffer, "2", kBufferSize);
							else if(strncmp(ptr, "yy", strlen(ptr)) == 0)
								strncat(pBuffer, "12", kBufferSize);
							else if(strncmp(ptr, "yyyy", strlen(ptr)) == 0)
								strncat(pBuffer, "2012", kBufferSize);
							else if(strncmp(ptr, "yyyyy", strlen(ptr)) == 0)
								strncat(pBuffer, "2012", kBufferSize);
							else
								EA_FAIL_M("Unsupported date format");

							i += strlen(ptr);
							size_t separators = strcspn(&dateFormat[j][i], "dmyM");
							strncat(pBuffer, &dateFormat[j][i], separators);
							i += separators;

							ptr = strtok(NULL, "/, -");
						}

						strncpy(kExpectedResult, pBuffer, kResultSize);

						pBuffer[0] = '\0';
						strncpy(tmp, timeFormat, kResultSize);
						ptr = strtok(tmp, ": ");
						i = 0;
						while(ptr != 0)
						{
							if(strncmp(ptr, "h", strlen(ptr)) == 0)
								strncat(pBuffer, "1", kBufferSize);
							else if(strncmp(ptr, "hh", strlen(ptr)) == 0)
								strncat(pBuffer, "01", kBufferSize);
							else if(strncmp(ptr, "H", strlen(ptr)) == 0)
								strncat(pBuffer, "13", kBufferSize);
							else if(strncmp(ptr, "HH", strlen(ptr)) == 0)
								strncat(pBuffer, "13", kBufferSize);
							else if(strncmp(ptr, "m", strlen(ptr)) == 0)
								strncat(pBuffer, "24", kBufferSize);
							else if(strncmp(ptr, "mm", strlen(ptr)) == 0)
								strncat(pBuffer, "24", kBufferSize);
							else if(strncmp(ptr, "s", strlen(ptr)) == 0)
								strncat(pBuffer, "5", kBufferSize);
							else if(strncmp(ptr, "ss", strlen(ptr)) == 0)
								strncat(pBuffer, "05", kBufferSize);
							else if(strncmp(ptr, "t", strlen(ptr)) == 0)
								strncat(pBuffer, "P", kBufferSize);
							else if(strncmp(ptr, "tt", strlen(ptr)) == 0)
								strncat(pBuffer, "PM", kBufferSize);
							else
								EA_FAIL_M("Unsupported date format");

							i+=strlen(ptr);
							size_t separators = strcspn(&timeFormat[i], "hHmst");
							strncat(pBuffer, &timeFormat[i], separators);
							i += separators;

							ptr = strtok(NULL, ": ");
						}
						strncat(kExpectedResult, " ", kResultSize);
						strncat(kExpectedResult, pBuffer, kResultSize);

						if(j == 0)
						{
							n = Strftime(pBuffer, kBufferSize, "%#c", &tmValue1, NULL);
							EATEST_VERIFY(n == Strlen(kExpectedResult));
							EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

							ptr = Strptime(pBuffer, "%#c", &tmValue2, NULL);
							EATEST_VERIFY(ptr == (pBuffer + n));
							EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
							EATEST_VERIFY(tmValue1.tm_mon  == tmValue2.tm_mon);
							EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
							EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
							EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);
							EATEST_VERIFY(tmValue1.tm_sec  == tmValue2.tm_sec);
						}
						else
						{
							n = Strftime(pBuffer, kBufferSize, "%c", &tmValue1, NULL);
							EATEST_VERIFY(n == Strlen(kExpectedResult));
							EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

							ptr = Strptime(pBuffer, "%c", &tmValue2, NULL);
							EATEST_VERIFY(ptr == (pBuffer + n));
							EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
							EATEST_VERIFY(tmValue1.tm_mon  == tmValue2.tm_mon);
							EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
							EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
							EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);
							EATEST_VERIFY(tmValue1.tm_sec  == tmValue2.tm_sec);
						}
					}

				#else
					char* ptr;

					//Takes on the form of %a %b %d %H:%M:%S %Y
					strncpy(kExpectedResult, "Mon Jan 09 13:24:05 2012", kResultSize);
					n = Strftime(pBuffer, kBufferSize, "%c", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					ptr = Strptime(pBuffer, "%c", &tmValue2, NULL);
					EATEST_VERIFY(ptr == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
					EATEST_VERIFY(tmValue1.tm_mon  == tmValue2.tm_mon);
					EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
					EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
					EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);
					EATEST_VERIFY(tmValue1.tm_sec  == tmValue2.tm_sec);

					strncpy(kExpectedResult, "Mon Jan 9 13:24:5 2012", kResultSize);
					n = Strftime(pBuffer, kBufferSize, "%#c", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					ptr = Strptime(pBuffer, "%#c", &tmValue2, NULL);
					EATEST_VERIFY(ptr == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
					EATEST_VERIFY(tmValue1.tm_mon  == tmValue2.tm_mon);
					EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
					EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
					EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);
					EATEST_VERIFY(tmValue1.tm_sec  == tmValue2.tm_sec);

				#endif

			}

			{
				//strftime %C   Replaced by the year divided by 100 and truncated to an integer, as a decimal number [00,99]. [ tm_year]
				//strptime %C   The century number [00,99]; leading zeros are permitted but not required.

				const uint32_t years[5] = 
				{
					1999,
					2000,
					999,
					99,
					2100,
				};

				const char* kExpectedResults[5] = 
				{
					"19 | 19",
					"20 | 20",
					"09 | 9",
					"00 | 0",
					"21 | 21",
				};

				for(uint32_t y =0; y < sizeof(years)/sizeof(uint32_t); y++)
				{
					dt.SetParameter(kParameterYear, years[y]);
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%C | %#C", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[y]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[y]) == 0);

					p = Strptime(pBuffer, "%C | %#C", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					//Divide by 100 to find number of centuries past 1900, add 19 for 1900 itself, then multiply by 100 to give the proper year
					EATEST_VERIFY(((tmValue1.tm_year+1900)/100)*100 == tmValue2.tm_year);
				}
			}

			{
				// %d   Replaced by the day of the month as a decimal number [01,31]. [ tm_mday]
				// %e   Replaced by the day of the month as a decimal number [1,31]; a single digit is preceded by a space. [ tm_mday]
				char kExpectedResult[32];

				for(uint32_t m = kDayOfMonthMin; m <= kDayOfMonthMax; m++)
				{
					sprintf(kExpectedResult, "%02u | %u | %2u", m, m, m);
					dt.SetParameter(kParameterDayOfMonth, m);
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%d | %#d | %e", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					p = Strptime(pBuffer, "%d | %#d | %e", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
				}
			}

			{
				// %D   Equivalent to %m/%d/%y. [ tm_mon, tm_mday, tm_year]
				const char* kExpectedResults[12] = 
				{
					"01/01/66",
					"02/02/67",
					"03/03/68",
					"04/04/69",
					"05/05/70",
					"06/06/71",
					"07/07/72",
					"08/08/73",
					"09/09/74",
					"10/10/75",
					"11/11/76",
					"12/12/77",
				};

				const uint32_t yearOffset = 2065;
				for(uint32_t m = kMonthJanuary; m <= kMonthDecember; m++)
				{
					dt.SetParameter(kParameterDayOfMonth, m);
					dt.SetParameter(kParameterMonth, m);

					//strptime %y - The year within century. When a century is not otherwise specified, values in the range [69,99] shall refer to years 1969 to 1999 
					//inclusive, and values in the range [00,68] shall refer to years 2000 to 2068 inclusive; leading zeros shall be permitted but shall not be required.

					dt.SetParameter(kParameterYear, m + yearOffset); // Add offset ( > 2068 - 12) so the tmValue2.tm_year value changes ranges from [2000, 2068] to [1969, 1999] when using strptime
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%D", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[m - 1]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[m - 1]) == 0); // m-1 because kMonthJanuary is 1 and not 0.

					p = Strptime(pBuffer, "%D", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
					EATEST_VERIFY(tmValue1.tm_mon   == tmValue2.tm_mon);

					//Check which range the year will be in
					if(m + yearOffset <= 2068)
					{
						//tm-year range from [2000, 2068], inclusively
						EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
					}
					else
					{
						//tm-year range from [1969, 1999], inclusively
						EATEST_VERIFY((tmValue1.tm_year - 100) == tmValue2.tm_year); // We subtract the offset -> (2000-1900)
					}
				}
			}

			{
				// %F   Equivalent to %Y - %m - %d (the ISO 8601:2000 standard date format). [ tm_year, tm_mon, tm_mday]
				const char* kExpectedResults[12] = 
				{
					"2001-01-01",
					"2002-02-02",
					"2003-03-03",
					"2004-04-04",
					"2005-05-05",
					"2006-06-06",
					"2007-07-07",
					"2008-08-08",
					"2009-09-09",
					"2010-10-10",
					"2011-11-11",
					"2012-12-12",
				};

				const uint32_t yearOffset = 2000;
				for(uint32_t m = kMonthJanuary; m <= kMonthDecember; m++)
				{
					dt.SetParameter(kParameterDayOfMonth, m);
					dt.SetParameter(kParameterMonth, m);
					dt.SetParameter(kParameterYear, m + yearOffset); //Add offset to make the years more legible/realistic
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%F", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[m - 1]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[m - 1]) == 0);
				}
			}

			{
				//Unsupported as of yet - we currently maintain no concept of "week number of year"
				// %g   Replaced by the last 2 digits of the week-based year (see below) as a decimal number [00,99]. [ tm_year, tm_wday, tm_yday]
				// %G   Replaced by the week-based year (see below) as a decimal number (for example, 1977). [ tm_year, tm_wday, tm_yday]
			}

			{
				// %H   Replaced by the hour (24-hour clock) as a decimal number [00,23]. [ tm_hour]
				// %I   Replaced by the hour (12-hour clock) as a decimal number [01,12]. [ tm_hour]
				// %M   Replaced by the minute as a decimal number [00,59]. [ tm_min]
				// %S   Replaced by the second as a decimal number [00,60]. [ tm_sec]

				char kExpectedResult[32];
				const int32_t amToPmDifference = 12;
				for(int32_t t = 1; t < kHoursPerDay; t++)
				{
					sprintf(kExpectedResult, "%02d | %d | %02d | %d | %02d | %d", t, t, t, t, t, t);
					dt.SetParameter(kParameterHour,   (uint32_t)t);
					dt.SetParameter(kParameterMinute, (uint32_t)t);
					dt.SetParameter(kParameterSecond, (uint32_t)t);
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%H | %#H | %M | %#M | %S | %#S", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					p = Strptime(pBuffer, "%H | %#H | %M | %#M | %S | %#S", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
					EATEST_VERIFY(tmValue1.tm_min   == tmValue2.tm_min);
					EATEST_VERIFY(tmValue1.tm_sec   == tmValue2.tm_sec);

					if(t <= amToPmDifference)
					{
						sprintf(kExpectedResult, "%02d | %d | %02d | %d | %02d | %d", t, t, t, t, t, t);
						n = Strftime(pBuffer, kBufferSize, "%I | %#I | %M | %#M | %S | %#S", &tmValue1, NULL);
						EATEST_VERIFY(n == Strlen(kExpectedResult));
						EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

						p = Strptime(pBuffer, "%I | %#I | %M | %#M | %S | %#S", &tmValue2, NULL);
						EATEST_VERIFY(p == (pBuffer + n));
						EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
						EATEST_VERIFY(tmValue1.tm_min   == tmValue2.tm_min);
						EATEST_VERIFY(tmValue1.tm_sec   == tmValue2.tm_sec);
					}
					else
					{
						sprintf(kExpectedResult, "%02d | %d | %02d | %d | %02d | %d", t - amToPmDifference, t - amToPmDifference, t, t, t, t);//Adjust for AM to PM rollover
						n = Strftime(pBuffer, kBufferSize, "%I | %#I | %M | %#M | %S | %#S", &tmValue1, NULL);
						EATEST_VERIFY(n == Strlen(kExpectedResult));
						EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

						p = Strptime(pBuffer, "%I | %#I | %M | %#M | %S | %#S", &tmValue2, NULL);
						EATEST_VERIFY(p == (pBuffer + n));
						EATEST_VERIFY((tmValue1.tm_hour - amToPmDifference)  == tmValue2.tm_hour);
						EATEST_VERIFY(tmValue1.tm_min                        == tmValue2.tm_min);
						EATEST_VERIFY(tmValue1.tm_sec                        == tmValue2.tm_sec);
					}
				}
			}

			{
				// %j   Replaced by the day of the year as a decimal number [001,366]. [ tm_yday]
				const char* kExpectedResults[4] = 
				{
					"366 | 366",
					"365 | 365",
					"001 | 1",
					"099 | 99",
				};

				for(uint32_t d = 0; d < 4; d++)
				{
					switch(d)
					{
					case 0://Leap Year End
						dt.SetParameter(kParameterDayOfMonth, 31);
						dt.SetParameter(kParameterMonth, 12);
						dt.SetParameter(kParameterYear, 2012);
						break;
					case 1://Regular Year End
						dt.SetParameter(kParameterDayOfMonth, 31);
						dt.SetParameter(kParameterMonth, 12);
						dt.SetParameter(kParameterYear, 2011);
						break;
					case 2://Year Beginning
						dt.SetParameter(kParameterDayOfMonth, 1);
						dt.SetParameter(kParameterMonth, 1);
						dt.SetParameter(kParameterYear, 1970);
						break;
					case 3://Mid-Regular-Year, two significant digits
						dt.SetParameter(kParameterDayOfMonth, 9);
						dt.SetParameter(kParameterMonth, 4);
						dt.SetParameter(kParameterYear, 2011);
						break;
					}
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%j | %#j", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[d]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[d]) == 0);

					p = Strptime(pBuffer, "%j | %#j", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_yday  == tmValue2.tm_yday);
				}
			}

			{
				// %p   Replaced by the locale's equivalent of either a.m. or p.m. [ tm_hour]
				// %r   Replaced by the time in a.m. and p.m. notation; [CX] [Option Start]  in the POSIX locale this shall be equivalent to %I : %M : %S %p. [Option End] [ tm_hour, tm_min, tm_sec]
				// %R   Replaced by the time in 24-hour notation ( %H : %M ). [ tm_hour, tm_min]
				// %T   Replaced by the time ( %H : %M : %S ). [ tm_hour, tm_min, tm_sec]
				const char* kExpectedResults[8] = 
				{
					"AM", "10:11:12 AM", "10:11", "10:11:12",
					"PM", "01:02:03 PM", "13:02", "13:02:03",
				};

				for(int32_t d = 0; d < 2; d++)
				{
					switch(d)
					{
					case 0://AM time leading zeroes
						dt.SetParameter(kParameterHour, 10);
						dt.SetParameter(kParameterMinute, 11);
						dt.SetParameter(kParameterSecond, 12);
						break;
					case 1://PM time
						dt.SetParameter(kParameterHour, 13);
						dt.SetParameter(kParameterMinute, 2);
						dt.SetParameter(kParameterSecond, 3);
						break;
					}
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%p", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[4*d]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[4*d]) == 0);

					int32_t oldHour = tmValue2.tm_hour;
					p = Strptime(pBuffer, "%p", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY((oldHour + (12*d)) == tmValue2.tm_hour);

					n = Strftime(pBuffer, kBufferSize, "%r", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[4*d+1]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[4*d+1]) == 0);

					p = Strptime(pBuffer, "%r", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
					EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);
					EATEST_VERIFY(tmValue1.tm_sec  == tmValue2.tm_sec);

					n = Strftime(pBuffer, kBufferSize, "%R", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[4*d+2]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[4*d+2]) == 0);

					p = Strptime(pBuffer, "%R", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
					EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);

					n = Strftime(pBuffer, kBufferSize, "%T", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[4*d+3]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[4*d+3]) == 0);

					p = Strptime(pBuffer, "%T", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
					EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);
					EATEST_VERIFY(tmValue1.tm_sec  == tmValue2.tm_sec);
				}
			}

			{
				// %u   Replaced by the weekday as a decimal number [1,7], with 1 representing Monday. [ tm_wday]
				// %w   Replaced by the weekday as a decimal number [0,6], with 0 representing Sunday. [ tm_wday]

				for(uint32_t d = 1; d <= kDaysPerWeek; d++)
				{
					char kExpectedResult[8];
					Snprintf(kExpectedResult, 8, "%d", d-1);
					dt.SetParameter(kParameterDayOfWeek, d);
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%w", &tmValue1, NULL);
					EATEST_VERIFY(kDayOfWeekSunday == 1);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					p = Strptime(pBuffer, "%w", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_wday  == tmValue2.tm_wday);

					Snprintf(kExpectedResult, 8, "%d", ((d+5)%7) + 1); //((d+5)%7) + 1) lets wrap around in the range [1,7]
					n = Strftime(pBuffer, kBufferSize, "%u", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);
				}
			}

			{
				//Unsupported as of yet - we currently maintain no concept of "week number of year"
				// %U   Replaced by the week number of the year as a decimal number [00,53]. The first Sunday of January is the first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]
				// %V   Replaced by the week number of the year (Monday as the first day of the week) as a decimal number [01,53]. If the week containing 1 January has four or more days in the new year, then it is considered week 1. Otherwise, it is the last week of the previous year, and the next week is week 1. Both January 4th and the first Thursday of January are always in week 1. [ tm_year, tm_wday, tm_yday]
				// %W   Replaced by the week number of the year as a decimal number [00,53]. The first Monday of January is the first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]
			}

			{
				// %x   Replaced by the locale's appropriate date representation. (See the Base Definitions volume of IEEE Std 1003.1-2001, <time.h>.)
				// %X   Replaced by the locale's appropriate time representation. (See the Base Definitions volume of IEEE Std 1003.1-2001, <time.h>.)

				dt.SetParameter(kParameterYear, 2012);
				dt.SetParameter(kParameterMonth, 1);
				dt.SetParameter(kParameterDayOfMonth, 9);
				dt.SetParameter(kParameterHour, 13);
				dt.SetParameter(kParameterMinute, 24);
				dt.SetParameter(kParameterSecond, 5);
				DateTimeToTm(dt, tmValue1);

				const int kResultSize = 512;
				char kExpectedResult[kResultSize] = { '\0' };

				#if defined (EA_PLATFORM_WINDOWS)
					char kLongMonth[32];
					char kLongDayOfWeek[32];
					char kShortMonth[32];
					char kShortDayOfWeek[32];
					char shortDateFormat[80];
					char longDateFormat[80];
					char timeFormat[80];

					char* dateFormat[2] = 
					{
						&shortDateFormat[0],
						&longDateFormat[0],
					};

					char* ptr;
					size_t i = 0;
					kExpectedResult[0] = '\0';

					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SSHORTDATE, shortDateFormat, sizeof(shortDateFormat)/sizeof(shortDateFormat[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SLONGDATE, longDateFormat, sizeof(longDateFormat)/sizeof(longDateFormat[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_STIMEFORMAT, timeFormat, sizeof(timeFormat)/sizeof(timeFormat[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SABBREVDAYNAME1, kShortDayOfWeek, sizeof(kShortDayOfWeek)/sizeof(kShortDayOfWeek[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SABBREVMONTHNAME1, kShortMonth, sizeof(kShortMonth)/sizeof(kShortMonth[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SDAYNAME1, kLongDayOfWeek, sizeof(kLongDayOfWeek)/sizeof(kLongDayOfWeek[0])) != 0);
					EATEST_VERIFY(GetLocaleInfoHelper(LOCALE_SMONTHNAME1, kLongMonth, sizeof(kLongMonth)/sizeof(kLongMonth[0])) != 0);

					char tmp[kResultSize];
					for(int j = 0; j < 2; j++)
					{
						i = 0;
						pBuffer[0] = '\0';

						strncpy(tmp, dateFormat[j], kResultSize);

						ptr = strtok(tmp, "/, -");
						while(ptr != 0)
						{
							if(strncmp(ptr, "d", strlen(ptr)) == 0)
								strncat(pBuffer, "9", kBufferSize);
							else if(strncmp(ptr, "dd", strlen(ptr)) == 0)
								strncat(pBuffer, "09", kBufferSize);
							else if(strncmp(ptr, "ddd", strlen(ptr)) == 0)
								strncat(pBuffer, kShortDayOfWeek, kBufferSize);
							else if(strncmp(ptr, "dddd", strlen(ptr)) == 0)
								strncat(pBuffer, kLongDayOfWeek, kBufferSize);
							else if(strncmp(ptr, "M", strlen(ptr)) == 0)
								strncat(pBuffer, "1", kBufferSize);
							else if(strncmp(ptr, "MM", strlen(ptr)) == 0)
								strncat(pBuffer, "01", kBufferSize);
							else if(strncmp(ptr, "MMM", strlen(ptr)) == 0)
								strncat(pBuffer, kShortMonth, kBufferSize);
							else if(strncmp(ptr, "MMMM", strlen(ptr)) == 0)
								strncat(pBuffer, kLongMonth, kBufferSize);
							else if(strncmp(ptr, "y", strlen(ptr)) == 0)
								strncat(pBuffer, "2", kBufferSize);
							else if(strncmp(ptr, "yy", strlen(ptr)) == 0)
								strncat(pBuffer, "12", kBufferSize);
							else if(strncmp(ptr, "yyyy", strlen(ptr)) == 0)
								strncat(pBuffer, "2012", kBufferSize);
							else if(strncmp(ptr, "yyyyy", strlen(ptr)) == 0)
								strncat(pBuffer, "2012", kBufferSize);
							else
								EA_FAIL_M("Unsupported date format");

							i += strlen(ptr);
							size_t separators = strcspn(&dateFormat[j][i], "dmyM");
							strncat(pBuffer, &dateFormat[j][i], separators);
							i += separators;

							ptr = strtok(NULL, "/, -");
						}
						strncpy(kExpectedResult, pBuffer, kResultSize);

						if(j == 0)
						{
							n = Strftime(pBuffer, kBufferSize, "%#x", &tmValue1, NULL);
							EATEST_VERIFY(n == Strlen(kExpectedResult));
							EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

							ptr = Strptime(pBuffer, "%#x", &tmValue2, NULL);
							EATEST_VERIFY(ptr == (pBuffer + n));
							EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
							EATEST_VERIFY(tmValue1.tm_mon  == tmValue2.tm_mon);
							EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
						}
						else
						{
							n = Strftime(pBuffer, kBufferSize, "%x", &tmValue1, NULL);
							EATEST_VERIFY(n == Strlen(kExpectedResult));
							EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);
						}
					}

					pBuffer[0] = '\0';
					strncpy(tmp, timeFormat, kResultSize);
					ptr = strtok(tmp, ": ");
					i = 0;
					while(ptr != 0)
					{
						if(strncmp(ptr, "h", strlen(ptr)) == 0)
							strncat(pBuffer, "1", kBufferSize);
						else if(strncmp(ptr, "hh", strlen(ptr)) == 0)
							strncat(pBuffer, "01", kBufferSize);
						else if(strncmp(ptr, "H", strlen(ptr)) == 0)
							strncat(pBuffer, "13", kBufferSize);
						else if(strncmp(ptr, "HH", strlen(ptr)) == 0)
							strncat(pBuffer, "13", kBufferSize);
						else if(strncmp(ptr, "m", strlen(ptr)) == 0)
							strncat(pBuffer, "24", kBufferSize);
						else if(strncmp(ptr, "mm", strlen(ptr)) == 0)
							strncat(pBuffer, "24", kBufferSize);
						else if(strncmp(ptr, "s", strlen(ptr)) == 0)
							strncat(pBuffer, "5", kBufferSize);
						else if(strncmp(ptr, "ss", strlen(ptr)) == 0)
							strncat(pBuffer, "05", kBufferSize);
						else if(strncmp(ptr, "t", strlen(ptr)) == 0)
							strncat(pBuffer, "P", kBufferSize);
						else if(strncmp(ptr, "tt", strlen(ptr)) == 0)
							strncat(pBuffer, "PM", kBufferSize);
						else
							EA_FAIL_M("Unsupported date format");

						i+=strlen(ptr);
						size_t separators = strcspn(&timeFormat[i], "hHmst");
						strncat(pBuffer, &timeFormat[i], separators);
						i += separators;

						ptr = strtok(NULL, ": ");
					}

					strncpy(kExpectedResult, pBuffer, kResultSize);
					n = Strftime(pBuffer, kBufferSize, "%X", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					ptr = Strptime(pBuffer, "%X", &tmValue2, NULL);
					EATEST_VERIFY(ptr == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
					EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);
					EATEST_VERIFY(tmValue1.tm_sec  == tmValue2.tm_sec);
				#else
					char* ptr;

					strncpy(kExpectedResult, "01/09/12", kResultSize);
					n = Strftime(pBuffer, kBufferSize, "%x", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					ptr = Strptime(pBuffer, "%x", &tmValue2, NULL);
					EATEST_VERIFY(ptr == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
					EATEST_VERIFY(tmValue1.tm_mon  == tmValue2.tm_mon);
					EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);

					strncpy(kExpectedResult, "1/9/12", kResultSize);
					n = Strftime(pBuffer, kBufferSize, "%#x", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					ptr = Strptime(pBuffer, "%#x", &tmValue2, NULL);
					EATEST_VERIFY(ptr == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
					EATEST_VERIFY(tmValue1.tm_mon  == tmValue2.tm_mon);
					EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);

					strncpy(kExpectedResult, "13:24:05", kResultSize);
					n = Strftime(pBuffer, kBufferSize, "%X", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResult));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResult) == 0);

					ptr = Strptime(pBuffer, "%X", &tmValue2, NULL);
					EATEST_VERIFY(ptr == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
					EATEST_VERIFY(tmValue1.tm_min  == tmValue2.tm_min);
					EATEST_VERIFY(tmValue1.tm_sec  == tmValue2.tm_sec);    

				#endif

			}

			{
				// %y   Replaced by the last two digits of the year as a decimal number [00,99]. [ tm_year]
				// %Y   Replaced by the year as a decimal number (for example, 1997). [ tm_year]
				const uint32_t years[5] = 
				{
					2000,
					2001,
					1999,
					2068,
					1969,
				};

				const char* kExpectedResults[5] = 
				{
					"00 | 0 | 2000",
					"01 | 1 | 2001",
					"99 | 99 | 1999",
					"68 | 68 | 2068",
					"69 | 69 | 1969",
				};

				for(uint32_t y = 0; y < 5; y++)
				{
					dt.SetParameter(kParameterYear, years[y]);
					DateTimeToTm(dt, tmValue1);

					n = Strftime(pBuffer, kBufferSize, "%y | %#y | %Y", &tmValue1, NULL);
					EATEST_VERIFY(n == Strlen(kExpectedResults[y]));
					EATEST_VERIFY(Strcmp(pBuffer, kExpectedResults[y]) == 0);

					p = Strptime(pBuffer, "%y | %#y | %Y", &tmValue2, NULL);
					EATEST_VERIFY(p == (pBuffer + n));
					EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
				}
			}

			{
				// %z   Replaced by the offset from UTC in the ISO 8601:2000 standard format ( +hhmm or -hhmm ), or by no characters if no timezone is determinable. For example, "-0430" means 4 hours 30 minutes behind UTC (west of Greenwich). [CX] [Option Start]  If tm_isdst is zero, the standard time offset is used. If tm_isdst is greater than zero, the daylight savings time offset is used. If tm_isdst is negative, no characters are returned. [Option End] [ tm_isdst]
				// %Z   Replaced by the timezone name or abbreviation, or by no bytes if no timezone information exists. [ tm_isdst]
				int  tzBias = (int)GetTimeZoneBias();  // tzBias will be a negative number in the United States.
				int  hour   = (tzBias / 3600);
				int  min    = (tzBias - (hour * 3600)) / 60;
				char kExpectedResult[32];
				char timeZoneName[EA::StdC::kTimeZoneNameCapacity];

				EA::StdC::GetTimeZoneName(timeZoneName, tmValue1.tm_isdst != 0);
				Snprintf(kExpectedResult, 32, "%+03d%02d | %s", hour, min, timeZoneName);
				DateTimeToTm(dt, tmValue1);

				n = Strftime(pBuffer, kBufferSize, "%z | %Z", &tmValue1, NULL);
				EATEST_VERIFY(n == Strlen(kExpectedResult));
				EATEST_VERIFY_F(Strcmp(pBuffer, kExpectedResult) == 0, "TestDateTime Strftime(%z %Z) failure: expected: %s, actual: %s", kExpectedResult, pBuffer);
			}
		}

		{
			#if defined(EA_PLATFORM_DESKTOP) // Currently tested only on desktop platforms because not all platforms completely or properly support the time and gmtime functions.
				time_t t    = time(NULL);
				tm*    pTM  = gmtime(&t);

				memcpy(&tmValue1, pTM, sizeof(tm));
				memset(&tmValue2, 0, sizeof(tmValue2));

				// As with the usage of Strftime/Strptime in the first test case above, these do not use all format specifiers
				// as not all (ie: %u, %W) are currently supported by Strptime.
				Strftime(pBuffer, kBufferSize, "%a | %#a | %A | %#A | %b | %#b | %B | %#B | %c | %#c | %C | %d | %#d | %D | %e | %h | %H | %#H | %I | %#I | %j | %#j | %m | %#m | %M | %#M | %n | %p | %#p | %r | %R | %S | %#S | %t | %T | %w | %#w | %x | %#x | %X | %#X | %y | %#y | %Y | %#Y | %% | %#%", &tmValue1, NULL);
				char *ptr = Strptime(pBuffer, "%a | %#a | %A | %#A | %b | %#b | %B | %#B | %c | %#c | %C | %d | %#d | %D | %e | %h | %H | %#H | %I | %#I | %j | %#j | %m | %#m | %M | %#M | %n | %p | %#p | %r | %R | %S | %#S | %t | %T | %w | %#w | %x | %#x | %X | %#X | %y | %#y | %Y | %#Y | %% | %#%", &tmValue2, NULL);

				EATEST_VERIFY(ptr != NULL);
				EATEST_VERIFY(tmValue1.tm_sec   == tmValue2.tm_sec);
				EATEST_VERIFY(tmValue1.tm_min   == tmValue2.tm_min);
				EATEST_VERIFY(tmValue1.tm_hour  == tmValue2.tm_hour);
				EATEST_VERIFY(tmValue1.tm_mday  == tmValue2.tm_mday);
				EATEST_VERIFY(tmValue1.tm_year  == tmValue2.tm_year);
				EATEST_VERIFY(tmValue1.tm_wday  == tmValue2.tm_wday);
				EATEST_VERIFY(tmValue1.tm_yday  == tmValue2.tm_yday);
				EATEST_VERIFY(tmValue1.tm_isdst == tmValue2.tm_isdst);
			#endif
		}

		{   // Regression
			// Verify basically that %Y supports 4 digit years.
			p = Strptime("1999", "%Y", &tmValue1, NULL);
			//value in tm_year is stored as years past 1900
			EATEST_VERIFY(p && (p[-1] == '9') && (tmValue1.tm_year == (1999 - 1900)));

			n = Strftime(pBuffer, kBufferSize, "%Y", &tmValue1, NULL);
			int32_t nYear = EA::StdC::StrtoI32(pBuffer, NULL, 10);
			EATEST_VERIFY((n == 4) && (nYear == 1999));
		}

		{   // Regression
			p = Strptime("969-12-30T12:33:45Z", "%Y-%m-%dT%H:%M:%SZ", &tmValue1, NULL);
			//value in tm_year is stored as years past 1900
			EATEST_VERIFY(p && (p[-1] == 'Z') && (tmValue1.tm_year == (969 - 1900)));

			p = Strptime("1969-12-30T12:33:45Z", "%Y-%m-%dT%H:%M:%SZ", &tmValue1, NULL);
			//value in tm_year is stored as years past 1900
			EATEST_VERIFY(p && (p[-1] == 'Z') && (tmValue1.tm_year == (1969 - 1900)));
		}

		delete[] pBuffer;
	}


	// Misc
	EATEST_VERIFY(IsLeapYear(2004) == true);
	EATEST_VERIFY(IsLeapYear(1800) == false);
	EATEST_VERIFY(GetDaysInYear(2004) == (uint32_t)366);
	EATEST_VERIFY(GetDaysInYear(1800) == (uint32_t)365);
	EATEST_VERIFY(GetDaysInMonth(kMonthFebruary,2004) == (uint32_t)29);
	EATEST_VERIFY(GetDaysInMonth(kMonthFebruary,1800) == (uint32_t)28);
	EATEST_VERIFY(GetDayOfYear(kMonthFebruary,29,2004) == (uint32_t)60);
	EATEST_VERIFY(GetDayOfYear(kMonthMarch,1,1800) == (uint32_t)60);


	{ // Regression of user-reported bug
		DateTime date1;        
		uint32_t value;

		date1.Set(1, 1, 1, 0, 0, 0);

		value = date1.GetParameter(kParameterYear);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterMonth);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterDayOfYear);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterWeekOfYear);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterDayOfMonth);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterDayOfWeek);
		EATEST_VERIFY(value == kDayOfWeekMonday);

		value = date1.GetParameter(kParameterHour);
		EATEST_VERIFY(value == 0);

		value = date1.GetParameter(kParameterMinute);
		EATEST_VERIFY(value == 0);

		value = date1.GetParameter(kParameterSecond);
		EATEST_VERIFY(value == 0);



		date1.Set(2009, 1, 1, 0, 0, 0);

		value = date1.GetParameter(kParameterYear);
		EATEST_VERIFY(value == 2009);

		value = date1.GetParameter(kParameterMonth);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterDayOfYear);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterWeekOfYear);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterDayOfMonth);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterDayOfWeek);
		EATEST_VERIFY(value == kDayOfWeekThursday);

		value = date1.GetParameter(kParameterHour);
		EATEST_VERIFY(value == 0);

		value = date1.GetParameter(kParameterMinute);
		EATEST_VERIFY(value == 0);

		value = date1.GetParameter(kParameterSecond);
		EATEST_VERIFY(value == 0);


		date1 = 0;
		date1.SetParameter(kParameterYear, 2009);
		date1.SetParameter(kParameterMonth, 1);
		date1.SetParameter(kParameterDayOfMonth, 1);

		value = date1.GetParameter(kParameterYear);
		EATEST_VERIFY(value == 2009);

		value = date1.GetParameter(kParameterMonth);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterDayOfMonth);
		EATEST_VERIFY(value == 1);


		date1 = 0;
		date1.SetParameter(kParameterDayOfMonth, 1);
		date1.SetParameter(kParameterMonth, 1);
		date1.SetParameter(kParameterYear, 2009);

		value = date1.GetParameter(kParameterYear);
		EATEST_VERIFY(value == 2009);

		value = date1.GetParameter(kParameterMonth);
		EATEST_VERIFY(value == 1);

		value = date1.GetParameter(kParameterDayOfMonth);
		EATEST_VERIFY(value == 1);


		//for(int64_t i = 0, secondsPerYear = (kSecondsPerDay * 365), secondsPerFourYears = secondsPerYear * 4; i < secondsPerFourYears; i++)
		//{
		//    date1 = i;
		//    date1.SetParameter(kParameterYear, 2009);
		//    value = date1.GetParameter(kParameterYear);
		//    EATEST_VERIFY(value == 2009);
		//}
	}

	{ // GetTime
		const int kTestCount = 5;
		const int kFailThreshold = 20;

		// 0.2 seconds and 2.0 seconds. It's this high because many platform
		// implementations of the tv_usec value are grainy and so this is the
		// best we can validate against.
		const uint64_t kMaxErrorNs = (EA::StdC::GetTimePrecision() < UINT64_C(100000000)) ? UINT64_C(200000000) : UINT64_C(2000000000);
		const uint64_t kMaxErrorMs = (EA::StdC::GetTimePrecision() < UINT64_C(100000000)) ? UINT64_C(200) : UINT64_C(2000);

		// Converts GetTimeOfDay output to an uint64_t representation.
		auto GetTimeOfDayAsUInt64 = []
		{
			timeval tv;
			timezone_ tz;
			EA::StdC::GetTimeOfDay(&tv, &tz, true);
			return (uint64_t)((tv.tv_sec * UINT64_C(1000000000)) + (tv.tv_usec * UINT64_C(1000)));
		};

		// We need to capture the initial date which our test started running at
		// so we can try to detect if the system clock has been changed while
		// our tests are running.
		uint64_t testStartTimeNs = GetTimeOfDayAsUInt64();
		uint64_t dateChangedDiffNs = 0;
		uint64_t dateChangedDiffMs = 0;
		int failCount = 0;

		for (int i = 0; i < kTestCount; i++)
		{
			uint64_t t1 = GetTime(); // nanoseconds
			uint64_t t2 = GetTimeOfDayAsUInt64();
			uint64_t t3 = GetTimeMilliseconds(); // milliseconds
			uint64_t t1ms = t1 / 1000000;
			const uint64_t diffNs = (t1 > t2) ? (t1 - t2) : (t2 - t1);
			const uint64_t diffMs = (t1ms > t3) ? (t1ms - t3) : (t3 - t1ms);

			// Adjust the clock if the date has been changed
			t2 += dateChangedDiffNs;
			t3 += dateChangedDiffMs;

			// If a thread context switch happened right between the two calls above, resample t1. 
			// We encounter this problem fairly regularly.
			if (diffNs > kMaxErrorNs && failCount < kFailThreshold) 
			{
				// If the time discrepancy is greater than a minute the system clock has likely changed so we adjust all
				// future runs of this tests based on the time difference between the old data and the new one
				if (diffNs > kSecondsPerMinute)
				{
					dateChangedDiffNs = t2 > testStartTimeNs ? (~diffNs) + 1 : diffNs; // take diff or two's complement of diff
					dateChangedDiffMs = dateChangedDiffNs / 1000000;
				}

				// It's very unlikely we could get another context switch so soon.
				// Keep trying until we hit the failure limit.
				failCount++;
				continue;
			}

			// For platforms where the main process can be swapped out for extended periods of time,
			// we disable the test on the buildfarm (via the manifest.xml)
			#if !defined(EASTDC_SWAPPABLE_PROCESS_PLATFORM)
				EATEST_VERIFY_F(diffNs <= kMaxErrorNs,
								"GetTimeOfDay failure on run %d of %d: diffNs: %I64u ns, kMaxErrorNs: %I64u ns. GetTime: "
								"%I64u ns (%I64u s)\nGetTimeOfDay: %I64u ns (%I64u s)\n",
								i, kTestCount, diffNs, kMaxErrorNs, t1, t1 / 1000000000, t2,
								t2 / 1000000000); // Verify that the difference is within N nanoseconds.

				EATEST_VERIFY_F(diffMs <= kMaxErrorMs,
								"GetTimeOfDay failure on run %d of %d: diffMs: %I64u ns, kMaxErrorMs: %I64u ms. GetTime: "
								"%I64u ms (%I64u s)\nGetTimeOfDay: %I64u ms (%I64u s)\n",
								i, kTestCount, diffMs, kMaxErrorMs, t1ms, t1ms / 1000, t3,
								t3 / 1000); // Verify that the difference is within N nanoseconds.
			#endif

			EA::Thread::ThreadSleep(EA::Thread::ThreadTime(500)); // Sleep for N milliseconds, and test again.
		}
	}


	{ // GetTimeOfDay

		timeval   tv;
		timezone_ tz;

		// Test various combinations of arguments.
		int result = EA::StdC::GetTimeOfDay(&tv, &tz, false);
		EATEST_VERIFY(result == 0);

		result = EA::StdC::GetTimeOfDay(&tv, &tz, true);
		EATEST_VERIFY(result == 0);

		result = EA::StdC::GetTimeOfDay(&tv, NULL, true);
		EATEST_VERIFY(result == 0);

		result = EA::StdC::GetTimeOfDay(&tv, NULL, false);
		EATEST_VERIFY(result == 0);

		result = EA::StdC::GetTimeOfDay(NULL, &tz, true);
		EATEST_VERIFY(result == 0);

		result = EA::StdC::GetTimeOfDay(NULL, &tz, false);
		EATEST_VERIFY(result == 0);

		result = EA::StdC::GetTimeOfDay(NULL, NULL, true);
		EATEST_VERIFY(result == 0);

		result = EA::StdC::GetTimeOfDay(NULL, NULL, false);
		EATEST_VERIFY(result == 0);
	}

	{
		// int64_t ConvertEpochSeconds(Epoch src, int64_t srcSeconds, Epoch dest);

		// Do some conversions from kEpochDateTime to other Epochs.
		DateTime dtEpochJulian((uint32_t)-4712,  1,  1, 12,  0,  0);
		int64_t epochJulian = ConvertEpochSeconds(kEpochDateTime, dtEpochJulian.GetSeconds(), kEpochJulian);
		EATEST_VERIFY(epochJulian == 0);

		DateTime dtEpochModifiedJulian(1858, 11, 17,  0,  0,  0);
		int64_t epochModifiedJulian = ConvertEpochSeconds(kEpochDateTime, dtEpochModifiedJulian.GetSeconds(), kEpochModifiedJulian);
		EATEST_VERIFY(epochModifiedJulian == 0);

		DateTime dtEpochGregorian(1752,  9, 14,  0,  0,  0);
		int64_t epochGregorian = ConvertEpochSeconds(kEpochDateTime, dtEpochGregorian.GetSeconds(), kEpochGregorian);
		EATEST_VERIFY(epochGregorian == 0);

		DateTime dtEpoch1900(1900,  1,  1,  0,  0,  0);
		int64_t epoch1900 = ConvertEpochSeconds(kEpochDateTime, dtEpoch1900.GetSeconds(), kEpoch1900);
		EATEST_VERIFY(epoch1900 == 0);

		DateTime dtEpoch1950(1950,  1,  1,  0,  0,  0);
		int64_t epoch1950 = ConvertEpochSeconds(kEpochDateTime, dtEpoch1950.GetSeconds(), kEpoch1950);
		EATEST_VERIFY(epoch1950 == 0);

		DateTime dtEpoch1970(1970,  1,  1,  0,  0,  0);
		int64_t epoch1970 = ConvertEpochSeconds(kEpochDateTime, dtEpoch1970.GetSeconds(), kEpoch1970);
		EATEST_VERIFY(epoch1970 == 0);

		DateTime dtEpoch2000(2000,  1,  1,  0,  0,  0);
		int64_t epoch2000 = ConvertEpochSeconds(kEpochDateTime, dtEpoch2000.GetSeconds(), kEpoch2000);
		EATEST_VERIFY(epoch2000 == 0);

		DateTime dtEpochJ2000(2000,  1,  1, 11, 58, 55);
		int64_t epochJ2000 = ConvertEpochSeconds(kEpochDateTime, dtEpochJ2000.GetSeconds(), kEpochJ2000);
		EATEST_VERIFY(epochJ2000 == 0);

		DateTime dtEpochDateTime(0); // We want to use a full date here instead, but there's a non-trivial issue with the code and documentation currently that prevents this.
		int64_t epochDateTime = ConvertEpochSeconds(kEpochDateTime, dtEpochDateTime.GetSeconds(), kEpochDateTime);
		EATEST_VERIFY(epochDateTime == 0);

		// Do a conversion between Epochs to make sure the math is right.
		int64_t epoch1970RelativeTo1950         = ConvertEpochSeconds(kEpoch1970, epoch1970, kEpoch1950);
		int64_t epoch1970RelativeTo1950Expected = (dtEpoch1970.GetSeconds() - dtEpoch1950.GetSeconds());
		EATEST_VERIFY(epoch1970RelativeTo1950 == epoch1970RelativeTo1950Expected);
	}

	return nErrorCount;
}

#undef LOCAL_MAX


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

