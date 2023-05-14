/**
 * @file closed_addr_hash_table.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __HASH_TABLE_CLOSED_ADDR_HASH_TABLE_H
#define __HASH_TABLE_CLOSED_ADDR_HASH_TABLE_H

#include <stdint.h>
#include <stddef.h>

struct ClosedAddrHashTableEntry
{
    uint32_t key;
    ClosedAddrHashTableEntry* next;
};

struct ClosedAddrHashTable
{
    ClosedAddrHashTableEntry* buckets;

    size_t size_exp;
    size_t bucket_count;
    size_t distinct_count;
};

void closed_addr_hash_table_ctor    (ClosedAddrHashTable* table);
void closed_addr_hash_table_dtor    (ClosedAddrHashTable* table);
int  closed_addr_hash_table_insert  (ClosedAddrHashTable* table, uint32_t key);
int  closed_addr_hash_table_erase   (ClosedAddrHashTable* table, uint32_t key);
int  closed_addr_hash_table_contains(ClosedAddrHashTable* table, uint32_t key);

#endif /* closed_addr_hash_table.h */
