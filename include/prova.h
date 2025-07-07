#ifndef PROVA_PROVA_H
#define PROVA_PROVA_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PV_FAILURE 0
#define PV_SUCCESS 1

#define PV_CONTINUE_ON_TEST_FAILURE 0
#define PV_TERMINATE_ON_TEST_FAILURE 1
#define PV_DEFAULT_CONFIG PV_CONTINUE_ON_TEST_FAILURE

#ifndef PV_ACTUAL_RESULT_BUFFER_SIZE
	#define PV_ACTUAL_RESULT_BUFFER_SIZE 512
#endif

typedef struct pv_test_result
{
	const char* name;
	const char* description;
	const char* failed_assertion_description;
	const char* expected_condition;
	char* actual_result;
} pv_test_result;

typedef struct pv_test_suite_result
{
	const char* name;
	const char* description;
	pv_test_result* test_results;
	const uint32_t tests_amount;
	const uint32_t tests_ran;
	const uint32_t tests_passed;
	const uint32_t tests_failed;
	const uint32_t pass_rate;
} pv_test_suite_result;

typedef pv_test_result (*pv_test_function)(void);
typedef pv_test_suite_result (*pv_test_suite_function)(void);

#define PV_TEST(test_name) pv_test_result test_name(void)

#define PV_REGISTRY static const char* pv__test_name = __func__; static char* pv__test_description = "" 

#define PV_DESCRIPTION(description) pv__test_description = ", " description

#define PV_TEST_SUITE(suite_name, suite_description, on_test_failure_behavior, ...) \
pv_test_suite_result suite_name(void)\
{\
	pv_test_function test_functions[] = {__VA_ARGS__};\
	const size_t num_tests = sizeof(test_functions)/sizeof(pv_test_function);\
	pv_test_result* test_results = malloc(sizeof(pv_test_result) * (sizeof(test_functions)/sizeof(pv_test_function)));\
	if (test_results == NULL)\
	{\
		printf("(memory allocation failed)\n");\
		exit(-1);\
	}\
	uint32_t ran = 0;\
	uint32_t failed = 0;\
	uint32_t passed = 0;\
	for (int i = 0; i < num_tests; i++)\
	{\
		test_results[i] = (test_functions[i])();\
		ran++;\
		if (test_results[i].actual_result != NULL)\
		{\
			failed++;\
			if (on_test_failure_behavior == PV_TERMINATE_ON_TEST_FAILURE)\
			{\
				goto ret;\
			}\
			continue;\
		}\
		passed++;\
	}\
ret:\
	return (pv_test_suite_result)\
	{\
		.name = #suite_name,\
		.description = suite_description,\
		.test_results = test_results,\
		.tests_amount = num_tests,\
		.tests_ran = ran,\
		.tests_passed = passed,\
		.tests_failed = failed,\
		.pass_rate = (uint32_t)(passed / (double)(num_tests) * 100)\
	};\
}\

#define PV_MAIN(...) \
int main(void)\
{\
	pv_test_suite_function test_suite_functions[] = {__VA_ARGS__};\
	const size_t num_test_suites = sizeof(test_suite_functions)/sizeof(pv_test_suite_function);\
	pv_test_suite_result test_suite_results[sizeof(test_suite_functions)/sizeof(pv_test_suite_function)];\
	int retcode = 0;\
	for (int i = 0; i < num_test_suites; i++)\
	{\
		pv_test_suite_result result = (test_suite_functions[i])();\
		printf("%s\n=======================\n\n", result.description);\
		for (int j = 0; j < result.tests_ran; j++)\
		{\
			const pv_test_result* test = &result.test_results[j];\
			printf("Test %d/%u: %s%s (%s)\n", j+1, result.tests_amount, test->name, test->description, (test->actual_result == NULL) ? "PASSED" : "FAILED");\
			if (test->actual_result != NULL)\
			{\
				printf("\tReason: Expected %s but %s%s\n", test->expected_condition, test->actual_result, test->failed_assertion_description);\
				free(test->actual_result);\
				retcode = 1;\
			}\
			printf("\n");\
		}\
		printf("-----------------------\n\nSummary: %lu tests, %u ran, %u passed, %u failed, %u%% pass rate\n\n=======================\n\n", result.tests_amount, result.tests_ran, result.tests_passed, result.tests_failed, result.pass_rate);\
		free(result.test_results);\
	}\
	return retcode;\
}\

#define PV__FORMAT_SPECIFIER(a) \
_Generic((a),\
	char: "%c",\
	char*: "%s",\
	const char*: "%s",\
	unsigned char: "%hhu",\
	unsigned short: "%hu",\
	unsigned int: "%u",\
	unsigned long: "%lu",\
	unsigned long long: "%llu",\
	short: "%hd",\
	int: "%d",\
	long: "%ld",\
	long long: "%lld",\
	void*: "%p",\
	float: "%f",\
	double: "%f",\
	default: "%s"\
)

#define PV__FORMAT_VALUE(a) \
_Generic((a),\
	char: (a),\
	char*: (a),\
	const char*: (a),\
	unsigned char: (a),\
	unsigned short: (a),\
	unsigned int: (a),\
	unsigned long: (a),\
	unsigned long long: (a),\
	short: (a),\
	int: (a),\
	long: (a),\
	long long: (a),\
	void*: (a),\
	float: (a),\
	double: (a),\
	default: "(unable to represent value)"\
)

#define PV__RETURN_FAILURE_DETAILED(a, b, comparison_operator, detail) \
{\
char* actual_result_buffer = malloc(sizeof(char) * PV_ACTUAL_RESULT_BUFFER_SIZE);\
char actual_result_temp_buffer[PV_ACTUAL_RESULT_BUFFER_SIZE];\
if (actual_result_buffer == NULL)\
{\
	printf("(memory allocation failed)\n");\
	exit(-1);\
}\
snprintf(actual_result_temp_buffer, sizeof(char) * PV_ACTUAL_RESULT_BUFFER_SIZE, #a " = %s and " #b " = %s", PV__FORMAT_SPECIFIER(a), PV__FORMAT_SPECIFIER(b));\
snprintf(actual_result_buffer, sizeof(char) * PV_ACTUAL_RESULT_BUFFER_SIZE, actual_result_temp_buffer, PV__FORMAT_VALUE(a), PV__FORMAT_VALUE(b));\
actual_result_buffer[PV_ACTUAL_RESULT_BUFFER_SIZE - 1] = '\0';\
return (pv_test_result)\
{\
	.name = pv__test_name,\
	.description = pv__test_description,\
	.failed_assertion_description = detail,\
	.expected_condition = #a " " comparison_operator " " #b ,\
	.actual_result = actual_result_buffer\
};\
}

#define PV_RETURN_SUCCESS \
return (pv_test_result)\
{\
	.name = pv__test_name,\
	.description = pv__test_description,\
	.failed_assertion_description = NULL,\
	.expected_condition = NULL,\
	.actual_result = NULL\
};\

#define PV_EXPECT_EQ_DETAILED(a,b,detail) do { if ((a) != (b)) PV__RETURN_FAILURE_DETAILED(a, b, "==", detail) } while (0)
#define PV_EXPECT_GT_DETAILED(a,b,detail) do { if ((a) <= (b)) PV__RETURN_FAILURE_DETAILED(a, b, ">", detail) } while (0)
#define PV_EXPECT_LT_DETAILED(a,b,detail) do { if ((a) >= (b)) PV__RETURN_FAILURE_DETAILED(a, b, "<", detail) } while (0)
#define PV_EXPECT_NEQ_DETAILED(a,b,detail) do { if ((a) == (b)) PV__RETURN_FAILURE_DETAILED(a, b, "!=", detail) } while (0)

#define PV_EXPECT_EQ(a,b) PV_EXPECT_EQ_DETAILED(a, b, "")
#define PV_EXPECT_GT(a,b) PV_EXPECT_GT_DETAILED(a, b, "")
#define PV_EXPECT_LT(a,b) PV_EXPECT_LT_DETAILED(a, b, "")
#define PV_EXPECT_NEQ(a,b) PV_EXPECT_NEQ_DETAILED(a, b, "")

#endif // PROVA_PROVA_H
