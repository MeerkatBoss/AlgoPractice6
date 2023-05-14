/**
 * @file hash_functions.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-18
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __HASH_FUNCTIONS_H
#define __HASH_FUNCTIONS_H

#include <stdint.h>

uint64_t hash_int_identity      (int32_t value);
uint64_t hash_int_multiplicative(int32_t value);

uint64_t hash_double_round       (double value);
uint64_t hash_double_reinterpret (double value);

uint64_t hash_str_length    (const char* value);
uint64_t hash_str_sum_char  (const char* value);
uint64_t hash_str_polynome  (const char* value);
uint64_t hash_str_crc64     (const char* value);

#endif /* hash_functions.h */
