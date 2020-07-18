#include "example1.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(example1);

//sometimes you may want to get at local data in a module.
//for example: If you plan to pass by reference, this could be useful
//however, it should often be avoided


TEST_SETUP(example1)
{
}

TEST_TEAR_DOWN(example1)
{
}

TEST(example1, getGreatest_should_return_the_greatest_value)
{
  //All of these should pass
  TEST_ASSERT_EQUAL(36, getGreatest(17,36));
  TEST_ASSERT_EQUAL(0, getGreatest(0,0));
  TEST_ASSERT_EQUAL(255, getGreatest(17,255));
  TEST_ASSERT_EQUAL(255, getGreatest(0,255));
}

TEST(example1, getGreatest_prepared_to_fail_the_test)
{
  //All of these should pass
  TEST_ASSERT_EQUAL(0, getGreatest(0,0));
  TEST_ASSERT_EQUAL(17, getGreatest(17,255));
  TEST_ASSERT_EQUAL(255, getGreatest(0,255));
}