// Thread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ThreadHotel.h"

#include <vld.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class Worker : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( Worker );
	CPPUNIT_TEST( TestThreadCreateAndDeleteAndCount );
	CPPUNIT_TEST_SUITE_END();
public:
	Thread::CThreadHotel tpool;
	VOID TestThreadCreateAndDeleteAndCount()
	{
		int count;
		Thread::ThreadID hThread[99];
		hThread[0] = tpool.CreateThread(_T("1"));
		count = tpool.GetThreadCount();
		CPPUNIT_ASSERT(count == 1);
		hThread[1] = tpool.CreateThread(_T("1"));
		count = tpool.GetThreadCount();
		CPPUNIT_ASSERT(count == 1);
		hThread[2] = tpool.CreateThread(_T("2"));
		count = tpool.GetThreadCount();
		CPPUNIT_ASSERT(count == 2);
		tpool.KillThread(hThread[1]);
		count = tpool.GetThreadCount();
		CPPUNIT_ASSERT(count == 1);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION ( Worker );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Worker, "ThreadTest");

int _tmain(int argc, _TCHAR* argv[])
{
	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest( registry.makeTest() );
	bool wasSuccessful = runner.run( "", false );
	return wasSuccessful;
}

