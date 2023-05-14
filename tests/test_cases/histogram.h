/**
 * @file histogram.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-27
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __TESTS_TEST_CASES_HISTOGRAM_H
#define __TESTS_TEST_CASES_HISTOGRAM_H

#include <stddef.h>

#include "meerkat_args/argparser.h"

#include "test_utils/config.h"

int run_test_histogram(int argc, const char* const* argv,
                       const TestConfig* config);

#endif /* histogram.h */
