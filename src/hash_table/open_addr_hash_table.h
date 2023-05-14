/**
 * @file open_addr_hash_table.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-05-14
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __HASH_TABLE_OPEN_ADDR_HASH_TABLE_H
#define __HASH_TABLE_OPEN_ADDR_HASH_TABLE_H

#include <stdint.h>
#include <stddef.h>

enum node_status
{
    NODE_FREE = 0,
    NODE_DELETED = 1,
    NODE_OCCUPIED = 2
};

struct OpenAddrHashTableEntry
{
    uint32_t key;
    node_status status;
};

struct OpenAddrHashTable
{
    OpenAddrHashTableEntry* data;

    size_t size_exp;
    size_t size;
    size_t distinct_count;
};

void open_addr_hash_table_ctor    (OpenAddrHashTable* table);
void open_addr_hash_table_dtor    (OpenAddrHashTable* table);
int  open_addr_hash_table_insert  (OpenAddrHashTable* table, uint32_t key);
int  open_addr_hash_table_erase   (OpenAddrHashTable* table, uint32_t key);
int  open_addr_hash_table_contains(OpenAddrHashTable* table, uint32_t key);

#endif /* open_addr_hash_table.h */
