#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(example1)
{
  RUN_TEST_CASE(example1, getGreatest_should_return_the_greatest_value);
  RUN_TEST_CASE(example1, getGreatest_prepared_to_fail_the_test);
}