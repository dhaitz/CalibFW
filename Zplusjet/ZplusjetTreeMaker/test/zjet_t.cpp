#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>



#include <Zplusjet/ZplusjetTreeMaker/interface/ZplusjetTreeMaker.h>

// defins a main which will run all tests
#include <Utilities/Testing/interface/CppUnit_testdriver.icpp>

class ComplexNumberTest : public CppUnit::TestFixture  {
public:
  CPPUNIT_TEST_SUITE( ComplexNumberTest );
  CPPUNIT_TEST( testEquality ); 
  CPPUNIT_TEST( testInstantiate );
  CPPUNIT_TEST_SUITE_END(); 
  
  void testEquality()
  {
    CPPUNIT_ASSERT( 1 == 1 );
  }
  
  void testInstantiate()
  {
	edm::ParameterSet pset;
    ZplusjetTreeMaker  zp( pset );
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( ComplexNumberTest );


