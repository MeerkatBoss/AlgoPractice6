#include "open_addr_hash_table.h"

#include <stdlib.h>
#include <string.h>

#include "hashes/hash_functions.h"
#include "closed_addr_hash_table.h"

static const size_t default_size = 1024;
static const double fill_factor = 0.75;

// As per Donald E. Knuth "The Art of Computer Programming" vol 3 ed. 2
// section 6.4
// ~= 2^64 / phi
static const uint64_t fib_constant = 11400714819323198485llu;

static OpenAddrHashTableEntry* find_node(OpenAddrHashTable* table,
                                         uint32_t key);
static int try_rehash(OpenAddrHashTable* table);

__always_inline
static size_t fibonacci_hash(uint64_t key, size_t size_exp)
{
    const size_t shift = 64 - size_exp;
    key ^= key >> shift;
    return (fib_constant * key) >> shift;
}

void open_addr_hash_table_ctor(OpenAddrHashTable* table)
{
    if (!table) return;

    table->data = (OpenAddrHashTableEntry*)
                    calloc(default_size, sizeof(*table->data));
    table->size = default_size;
    table->size_exp = 10;
    table->distinct_count = 0;
}

void open_addr_hash_table_dtor(OpenAddrHashTable* table)
{
    if (!table) return;
    free(table->data);
    memset(table, 0, sizeof(*table));
}

int open_addr_hash_table_insert(OpenAddrHashTable* table, uint32_t key)
{
    if (!table || !table->data) return -1;
    
    OpenAddrHashTableEntry* node = find_node(table, key);
    if (node->status == NODE_OCCUPIED)
        return -1;

    if (node->status == NODE_FREE)
        ++ table->distinct_count;

    node->key = key;
    node->status = NODE_OCCUPIED;

    return try_rehash(table);
}

int open_addr_hash_table_erase(OpenAddrHashTable* table, uint32_t key)
{
    if (!table || !table->data) return -1;
    
    OpenAddrHashTableEntry* node = find_node(table, key);
    if (node->status != NODE_OCCUPIED)
        return -1;

    node->key = 0;
    node->status = NODE_DELETED;

    return 0;
}

int open_addr_hash_table_contains(OpenAddrHashTable* table, uint32_t key)
{
    if (!table || !table->data) return 0;
    
    OpenAddrHashTableEntry* node = find_node(table, key);

    return node->status == NODE_OCCUPIED;
}

static OpenAddrHashTableEntry* find_node(OpenAddrHashTable* table,
                                         uint32_t key)
{
    size_t hash = fibonacci_hash(key, table->size_exp);

    size_t index = hash;
    while (table->data[index].status != NODE_FREE
            && table->data[index].key != key)
        index = (index + 1) & ((1lu << table->size_exp) - 1u);

    if (table->data[index].status == NODE_OCCUPIED)
        return table->data + index;

    index = hash;
    while (table->data[index].status == NODE_OCCUPIED)
        index = (index + 1) & ((1lu << table->size_exp) - 1u);

    return table->data + index;
}

static int try_rehash(OpenAddrHashTable* table)
{
    if (fill_factor*(double)table->size > (double) table->distinct_count)
        return 0;

    OpenAddrHashTable new_table = {};
    new_table.data = (OpenAddrHashTableEntry*)
                    calloc(table->size * 2, sizeof(*new_table.data));
    if (!new_table.data)
        return -1;
    new_table.size = table->size * 2;
    new_table.size_exp = table->size_exp + 1;
    new_table.distinct_count = 0;

    for (size_t i = 0; i < table->size; ++i)
        if (table->data[i].status == NODE_OCCUPIED)
            open_addr_hash_table_insert(&new_table, table->data[i].key);

    open_addr_hash_table_dtor(table);
    table->data = new_table.data;
    table->size_exp = new_table.size_exp;
    table->size = new_table.size;
    table->distinct_count = new_table.distinct_count;

    return 0;
}

