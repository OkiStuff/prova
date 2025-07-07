# Prova
Prova (Italian for test) is a simple C11 unit testing framework. Prova only includes 4 types of assertions, `PV_EXPECT_EQ`, `PV_EXPECT_GT`, `PV_EXPECT_LT`, and `PV_EXPECT_NEQ`

| Assertion Type  | Fail Behavior   |
| --------------- | --------------- |
| `PV_EXPECT_EQ`  | Fails if A != B |
| `PV_EXPECT_GT`  | Fails if A <= B |
| `PV_EXPECT_LT`  | Fails if A >= B |
| `PV_EXPECT_NEQ` | Fails if A == B |

Every assertion type has a `DETAILED` version where a description of the assertion can be displayed should it fail: `PV_EXPECT_EQ_DETAILED`, `PV_EXPECT_GT_DETAILED`, `PV_EXPECT_LT_DETAILED`, `PV_EXPECT_NEQ_DETAILED`

## Structure
The structure of a Prova test file is the following:
 - `#include <prova.h>`
 - `PV_TEST`s
 - `PV_TEST_SUITE`s
 - `PV_MAIN`

## Example

```c
#include <prova.h>

PV_TEST(add_test)
{
	PV_REGISTRY;
	PV_DESCRIPTION("Test 1 + 2");

	int a = 1;
	int b = 2;
	int sum = 1 + 2;

	PV_EXPECT_EQ(sum, 3);
	PV_RETURN_SUCCESS;
}

PV_TEST(fail_test)
{
	PV_REGISTRY;
	PV_DESCRIPTION("Test fail");

	PV_EXPECT_EQ(0, 1);
	PV_RETURN_SUCCESS;
}

PV_TEST_SUITE(example_tests, "Example Tests", PV_DEFAULT_CONFIG, add_test, fail_test)

PV_MAIN(example_tests)
```

```bash
$ gcc -o example_tests example_tests.c
$ ./example_tests
Example Tests
=======================

Test 1/2: add_test, Test 1 + 2 (PASSED)

Test 2/2: fail_test, Test fail (FAILED)
	Reason: Expected 0 == 1 but 0 == 0, assertion description: Test if 0 == 1

-----------------------

Summary: 2 tests, 2 ran, 1 passed, 1 failed, 50% pass rate

=======================

```
