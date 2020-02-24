///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EAProcess.h>
#include <EAStdCTest/EAStdCTest.h>
#include <EATest/EATest.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#if defined(EA_PLATFORM_WINDOWS) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
	template <typename T> // T is one of char, char16_t, char32_t.
	static void TestEnvironmentVar(int& nErrorCount)
	{
		using namespace EA::StdC;

		T      name[20];
		T      valueIn[32];
		T      valueOut[32];
		bool   bResult;
		size_t nRequiredStrlen;
		size_t sizeError = (size_t)-1;

		// Verify that nothing is found:
		Strlcpy(name, "NameNonExisting", EAArrayCount(name));
		valueOut[0] = 0;
		nRequiredStrlen = GetEnvironmentVar(name, valueOut, EAArrayCount(valueOut));
		EATEST_VERIFY((nRequiredStrlen == sizeError) && (valueOut[0] == 0));

		// Verify that something can be set, then found:
		Strlcpy(name,  "NameExisting", EAArrayCount(name));
		Strlcpy(valueIn, "ValueExisting", EAArrayCount(valueIn));
		bResult = SetEnvironmentVar(name, valueIn);
		EATEST_VERIFY(bResult);
		nRequiredStrlen = GetEnvironmentVar(name, valueOut, EAArrayCount(valueOut));
		EATEST_VERIFY((nRequiredStrlen == 13) && (Strcmp(valueIn, valueOut) == 0));

		// Verify that a too-small output capacity is handled properly:
		memset(valueOut, 0xff, sizeof(valueOut));
		nRequiredStrlen = GetEnvironmentVar(name, valueOut, 3);
		EATEST_VERIFY((nRequiredStrlen == 13) && (valueOut[3] == (T)-1));

		// Verify that something can be reset, then found with the new value:
		Strlcpy(valueIn, "ValueExistingNew", EAArrayCount(valueIn));
		bResult = SetEnvironmentVar(name, valueIn);
		EATEST_VERIFY(bResult);
		nRequiredStrlen = GetEnvironmentVar(name, valueOut, EAArrayCount(valueOut));
		EATEST_VERIFY((nRequiredStrlen == 16) && (Strcmp(valueIn, valueOut) == 0));

		// Verify that something can be cleared, then not found:
		bResult = SetEnvironmentVar(name, NULL);
		EATEST_VERIFY(bResult);
		valueOut[0] = 0;
		nRequiredStrlen = GetEnvironmentVar(name, valueOut, EAArrayCount(valueOut));
		EATEST_VERIFY((nRequiredStrlen == sizeError) && (valueOut[0] == 0));

		// Verify that something can once again be set, then found:
		Strlcpy(name, "NameExisting", EAArrayCount(name));
		Strlcpy(valueIn, "ValueExistingNew2", EAArrayCount(valueIn));
		bResult = SetEnvironmentVar(name, valueIn);
		EATEST_VERIFY(bResult);
		nRequiredStrlen = GetEnvironmentVar(name, valueOut, EAArrayCount(valueOut));
		EATEST_VERIFY((nRequiredStrlen == 17) && (Strcmp(valueIn, valueOut) == 0));

		// Verify that GetEnvironmentVar(T) yields the same result as GetEnvironmentVar(char) (T may be char16_t).
		char valueOut8[32];
		T       valueOutT[32];
		nRequiredStrlen = GetEnvironmentVar("NameExisting", valueOut8, EAArrayCount(valueOut8));
		Strlcpy(valueOutT, valueOut8, EAArrayCount(valueOutT)); // Need to convert char to T, as Strcmp only exists for like types.
		EATEST_VERIFY((nRequiredStrlen == 17) && (Strcmp(valueOut, valueOutT) == 0));

		// Verify that GetEnvironmentVar(T) yields the same result as GetEnvironmentVar(char16_t) (T may be char).
		char16_t name16[32];
		char16_t valueOut16[32];
		Strlcpy(name16, name, EAArrayCount(name16));
		nRequiredStrlen = GetEnvironmentVar(name16, valueOut16, EAArrayCount(valueOut16));
		Strlcpy(valueOutT, valueOut16, EAArrayCount(valueOutT)); // Need to convert char16_t to T, as Strcmp only exists for like types.
		EATEST_VERIFY((nRequiredStrlen == 17) && (Strcmp(valueOut, valueOutT) == 0));
	}
#endif



int TestProcess()
{
	int nErrorCount(0);

	{
		// size_t GetCurrentProcessPath(char*  pPath);
		// size_t GetCurrentProcessPath(char16_t* pPath);
		// size_t GetCurrentProcessPath(char32_t* pPath);
		// size_t GetCurrentProcessDirectory(char*  pDirectory);
		// size_t GetCurrentProcessDirectory(char16_t* pDirectory);
		// size_t GetCurrentProcessDirectory(char32_t* pDirectory);
	
		// Currently we have known support for Windows. With other platforms support is inconsistent.
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) || defined(EA_PLATFORM_APPLE) || defined(EA_PLATFORM_LINUX) || (defined(EA_PLATFORM_SONY) && EA_SCEDBG_ENABLED)
			char16_t path16[EA::StdC::kMaxPathLength];
			char  path8[EA::StdC::kMaxPathLength];
			size_t   n;
			size_t   processPathLen;

			n = EA::StdC::GetCurrentProcessPath(path16);
			EATEST_VERIFY(n == EA::StdC::Strlen(path16));
			EATEST_VERIFY(n > 0);

			processPathLen = EA::StdC::GetCurrentProcessPath(path8);
			EATEST_VERIFY(processPathLen == EA::StdC::Strlen(path8));
			EATEST_VERIFY(processPathLen > 0);

			EATEST_VERIFY(EA::StdC::Strlen(path16) == EA::StdC::Strlen(path8));

			#if defined(EA_PLATFORM_APPLE) // Test kPathFlagBundlePath
				if(n && !EA::StdC::Strend(path8, ".app")) // If we have a path to a file within a bundle (e.g. "/Dir/MyApp.app/Contents/MacOS/MyApp")
				{
					n = EA::StdC::GetCurrentProcessPath(path8, EAArrayCount(path8), EA::StdC::kPathFlagBundlePath);
					EATEST_VERIFY(n > 0); // Should be like "/Dir/MyApp" or "/Dir/MyApp.app"
				}
			#endif
			
			n = EA::StdC::GetCurrentProcessDirectory(path16);
			EATEST_VERIFY(n == EA::StdC::Strlen(path16));
			EATEST_VERIFY(n > 0);

			n = EA::StdC::GetCurrentProcessDirectory(path8);
			EATEST_VERIFY(n == EA::StdC::Strlen(path8));
			EATEST_VERIFY(n > 0);

			EATEST_VERIFY(EA::StdC::Strlen(path16) == EA::StdC::Strlen(path8));

			EATEST_VERIFY(processPathLen > n);

			#if defined(EA_PLATFORM_APPLE) // Test kPathFlagBundlePath
				// We could do a similar test for iOS.
				if(n && EA::StdC::Strend(path8, "MacOS/")) // If we have a path to a file within a MacOS bundle (e.g. "/Dir/MyApp.app/Contents/MacOS/")
				{
					n = EA::StdC::GetCurrentProcessDirectory(path8, EAArrayCount(path8), EA::StdC::kPathFlagBundlePath);
					EATEST_VERIFY((n > 0) && !EA::StdC::Strend(path8, "MacOS/")); // Should be like "/Dir/"
				}
			#elif defined(EA_PLATFORM_SONY)
				// Do some additional testing of SetCurrentProcessPath with GetCurrentProcessPath / GetCurrentProcessDirectory on Kettle
				#define KETTLE_PROCESS_FOLDER "/host/folder/subfolder/"
				#define KETTLE_PROCESS_ELF "test.elf"
				EA::StdC::SetCurrentProcessPath(KETTLE_PROCESS_FOLDER KETTLE_PROCESS_ELF);

				n = EA::StdC::GetCurrentProcessPath(path16);
				EATEST_VERIFY(EA::StdC::Strcmp(path16, EA_CHAR16(KETTLE_PROCESS_FOLDER KETTLE_PROCESS_ELF)) == 0);
				EATEST_VERIFY(n == EA::StdC::Strlen(KETTLE_PROCESS_FOLDER KETTLE_PROCESS_ELF));

				n = EA::StdC::GetCurrentProcessPath(path8);
				EATEST_VERIFY(EA::StdC::Strcmp(path8, KETTLE_PROCESS_FOLDER KETTLE_PROCESS_ELF) == 0);
				EATEST_VERIFY(n == EA::StdC::Strlen(KETTLE_PROCESS_FOLDER KETTLE_PROCESS_ELF));

				n = EA::StdC::GetCurrentProcessDirectory(path16);
				EATEST_VERIFY(EA::StdC::Strcmp(path16, EA_CHAR16(KETTLE_PROCESS_FOLDER)) == 0);
				EATEST_VERIFY(n == EA::StdC::Strlen(KETTLE_PROCESS_FOLDER));
				EATEST_VERIFY(n > 0);

				n = EA::StdC::GetCurrentProcessDirectory(path8);
				EATEST_VERIFY(EA::StdC::Strcmp(path8, KETTLE_PROCESS_FOLDER) == 0);
				EATEST_VERIFY(n == EA::StdC::Strlen(KETTLE_PROCESS_FOLDER));
			#endif
			
		#endif
	}

	{
		// size_t GetEnvironmentVar(const char*  pName, char*  pValue, size_t valueCapacity);
		// size_t GetEnvironmentVar(const char16_t* pName, char16_t* pValue, size_t valueCapacity);
		// size_t GetEnvironmentVar(const char32_t* pName, char32_t* pValue, size_t valueCapacity);
		// bool   SetEnvironmentVar(const char*  pName, const char* pValue);
		// bool   SetEnvironmentVar(const char16_t* pName, const char16_t* pValue);
		// bool   SetEnvironmentVar(const char32_t* pName, const char32_t* pValue);

		#if defined(EA_PLATFORM_WINDOWS) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // To do: Enable this for other platforms when possible.
			TestEnvironmentVar<char>(nErrorCount);
			TestEnvironmentVar<char16_t>(nErrorCount);
		  //TestEnvironmentVar<char32_t>(nErrorCount); // Doesn't yet exist.
		#endif
	}

	/*
	{
		// int Spawn(const char16_t* pPath, const char16_t* const* pArgumentArray, bool wait = false);
		// int Spawn(const char*  pPath, const char*  const* pArgumentArray, bool wait = false);

		// int ExecuteShellCommand(const char16_t* pCommand);
		// int ExecuteShellCommand(const char*  pCommand);

		// bool SearchEnvironmentPath(const char16_t* pFileName, char16_t* pPath, const char16_t* pEnvironmentVar = NULL);
		// bool SearchEnvironmentPath(const char*  pFileName, char*  pPath, const char*  pEnvironmentVar = NULL);

		// bool OpenFile(const char16_t* pPath); // e.g. http://www.bozo.com/somefile.html
		// bool OpenFile(const char*  pPath); // e.g. /system/settings/somefile.txt
	}
	*/

	return nErrorCount;
}

