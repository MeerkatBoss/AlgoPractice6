/**
 * @file fixed_hash_table.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __HASH_TABLE_FIXED_HASH_TABLE_H
#define __HASH_TABLE_FIXED_HASH_TABLE_H

#include <stddef.h>

#ifndef HASH_PRESET
#define HASH_PRESET "presets/hash_int.h"
#endif

#include HASH_PRESET

struct FixedHashTableEntry;

struct FixedHashTableEntry
{
    KEY_TYPE key;

    FixedHashTableEntry* next;
};

struct FixedHashTable
{
    FixedHashTableEntry* buckets;
    size_t bucket_count;

    FixedHashTableEntry* free;

    size_t capacity;
    size_t distinct_count;
};

int fixed_hash_table_ctor      (FixedHashTable* table, size_t bucket_count);

int fixed_hash_table_dtor      (FixedHashTable* table);

int fixed_hash_table_add_key   (FixedHashTable* table, KEY_TYPE key);

int fixed_hash_table_has_key   (const FixedHashTable* table, KEY_TYPE key);

#endif /* fixed_hash_table.h */

