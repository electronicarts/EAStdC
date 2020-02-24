///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAStdC/EASingleton.h>
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


class Widget
{
public:
	Widget(){}
	Widget(const Widget&){}
   ~Widget(){}
	Widget& operator=(const Widget&) { return *this; }
};


class WidgetS : public EA::StdC::Singleton<WidgetS> // Note that this class inherits from a template of itself.
{
	WidgetS(const WidgetS&);            // Used to placate VC++ compiler warning C4625
	WidgetS& operator=(const WidgetS&); // Used to placate VC++ compiler warning C4626
};


class WidgetSA : public EA::StdC::SingletonAdapter<Widget, true>
{
	WidgetSA(const WidgetSA&);            // Used to placate VC++ compiler warning C4625
	WidgetSA& operator=(const WidgetSA&); // Used to placate VC++ compiler warning C4626
};



int TestSingleton()
{
	int nErrorCount(0);

	// Singleton
	WidgetS* pWidgetS = WidgetS::GetInstance();
	EATEST_VERIFY(pWidgetS == NULL);

	// SingletonImplicit
	Widget* pWidget = WidgetSA::GetInstance();
	EATEST_VERIFY(pWidget != NULL);

	WidgetSA::DestroyInstance();

	// SingletonExplicit
	pWidget = WidgetSA::GetInstance();
	EATEST_VERIFY(pWidget != NULL);

	pWidget = WidgetSA::CreateInstance("SomeName");
	EATEST_VERIFY(pWidget != NULL);

	pWidget = WidgetSA::GetInstance();
	EATEST_VERIFY(pWidget != NULL);

	WidgetSA::DestroyInstance();

	return nErrorCount;
}

