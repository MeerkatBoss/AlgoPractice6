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

static ClosedAddrHashTableEntry* get_parent_node(ClosedAddrHashTableEntry* head,
                                                 uint32_t key);
static int try_rehash(ClosedAddrHashTable* table);

__always_inline
static size_t fibonacci_hash(uint64_t key, size_t size_exp)
{
    const size_t shift = 64 - size_exp;
    key ^= key >> shift;
    return (fib_constant * key) >> shift;
}

void closed_addr_hash_table_ctor(ClosedAddrHashTable* table)
{
    if (!table) return;

    table->buckets = (ClosedAddrHashTableEntry*)
                        calloc(default_size, sizeof(*table->buckets));
    table->bucket_count = default_size;
    table->size_exp = 10;
    table->distinct_count = 0;
}

void closed_addr_hash_table_dtor(ClosedAddrHashTable* table)
{
    if (!table) return;
    for (size_t i = 0; i < table->bucket_count; ++i)
    {
        ClosedAddrHashTableEntry* cur = table->buckets[i].next;
        while (cur)
        {
            ClosedAddrHashTableEntry* tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free(table->buckets);
    memset(table, 0, sizeof(*table));
}

int  closed_addr_hash_table_insert  (ClosedAddrHashTable* table, uint32_t key)
{
    if (!table || !table->buckets) return -1;
    size_t hash = fibonacci_hash(key, table->size_exp);
    ClosedAddrHashTableEntry* node =
                get_parent_node(table->buckets + hash, key)->next;

    if (node) return -1;

    node = (ClosedAddrHashTableEntry*) calloc(1, sizeof(*node));
    node->key = key;
    node->next = table->buckets[hash].next;
    table->buckets[hash].next = node;

    ++ table->distinct_count;
    return try_rehash(table);
}

int closed_addr_hash_table_erase(ClosedAddrHashTable* table, uint32_t key)
{
    if (!table || !table->buckets) return -1;
    size_t hash = fibonacci_hash(key, table->size_exp);
    ClosedAddrHashTableEntry* parent =
                get_parent_node(table->buckets + hash, key);
    ClosedAddrHashTableEntry* node = parent->next;

    if (!node) return -1;
    
    parent->next = node->next;
    free(node);
    -- table->distinct_count;

    return 0;
}

int closed_addr_hash_table_contains(ClosedAddrHashTable* table, uint32_t key)
{
    if (!table || !table->buckets) return 0;
    size_t hash = fibonacci_hash(key, table->size_exp);
    ClosedAddrHashTableEntry* node =
                get_parent_node(table->buckets + hash, key)->next;
    return !!node;
}

static ClosedAddrHashTableEntry* get_parent_node(ClosedAddrHashTableEntry* head,
                                                 uint32_t key)
{
    if (!head) return NULL;

    ClosedAddrHashTableEntry* parent = head;
    ClosedAddrHashTableEntry* current = parent->next;

    while (current && current->key != key)
    {
        parent = current;
        current = parent->next;
    }

    return parent;
}

static int try_rehash(ClosedAddrHashTable* table)
{
    if (fill_factor*(double)table->bucket_count 
            > (double) table->distinct_count)
        return 0;

    const size_t old_size = table->bucket_count;

    ClosedAddrHashTableEntry* old_entries = table->buckets;

    table->buckets = (ClosedAddrHashTableEntry*)
                        calloc(2*old_size, sizeof(*table->buckets));
    if (!table->buckets)
    {
        table->buckets = old_entries;
        return -1;
    }

    table->bucket_count *= 2;
    ++table->size_exp;
    table->distinct_count = 0;

    for (size_t i = 0; i < old_size; ++i)
    {
        ClosedAddrHashTableEntry* cur = old_entries[i].next;
        while (cur)
        {
            ClosedAddrHashTableEntry* tmp = cur;
            closed_addr_hash_table_insert(table, cur->key);

            cur = cur->next;
            free(tmp);
        }
    }

    free(old_entries);
    return 0;
}

