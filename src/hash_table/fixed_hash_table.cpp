#include <typeinfo>

#include <stdlib.h>
#include <string.h>

#include "meerkat_assert/asserts.h"

#include "hashes/hash_functions.h"

#include "fixed_hash_table.h"

static FixedHashTableEntry* find_parent_node(const FixedHashTable* table,
                                            KEY_TYPE key, uint64_t key_hash);
static void mark_free(FixedHashTableEntry* entries, size_t entry_count);
static int try_grow(FixedHashTable* table);

__always_inline
static size_t round_to_pow2(size_t x)
{
    size_t result = 1;
    while (result < x)
        result <<= 1;
    return result;
}

int fixed_hash_table_ctor(FixedHashTable* table, const size_t bucket_count)
{
    SAFE_BLOCK_START
    {
        ASSERT_TRUE(table != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    size_t capacity = round_to_pow2(2*bucket_count);
    FixedHashTableEntry* buffer = NULL;

    SAFE_BLOCK_START
    {
        ASSERT_SIMPLE(
            buffer = (FixedHashTableEntry*)calloc(capacity, sizeof(*buffer)),
            action_result != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = ENOMEM;
        return -1;
    }
    SAFE_BLOCK_END

    mark_free(buffer + bucket_count, capacity - bucket_count);

    table->buckets = buffer;
    table->bucket_count = bucket_count;
    table->free = buffer + bucket_count;

    table->capacity = capacity;
    table->distinct_count = 0;

    return 0;
}

int fixed_hash_table_dtor(FixedHashTable* table)
{
    SAFE_BLOCK_START
    {
        ASSERT_TRUE(table != NULL);
        ASSERT_TRUE(table->buckets != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    for (size_t i = 0; i < table->bucket_count; ++i)
    {
        FixedHashTableEntry* entry = table->buckets[i].next;
        while (entry)
        {
            KEY_DTOR(entry->key);
            entry = entry->next;
        }
    }
    free(table->buckets);

    memset(table, 0, sizeof(*table));

    return 0;
}

int fixed_hash_table_add_key(FixedHashTable* table, KEY_TYPE key)
{
    SAFE_BLOCK_START
    {
        ASSERT_TRUE(table != NULL);
        ASSERT_TRUE(table->buckets != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    size_t key_hash = HASH_FUNCTION(key) % table->bucket_count;
    FixedHashTableEntry* key_entry = find_parent_node(table, key, key_hash)->next;

    if (key_entry)
        return -1;

    SAFE_BLOCK_START
    {
        ASSERT_ZERO(
                try_grow(table));
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = ENOMEM;
        return -1;
    }
    SAFE_BLOCK_END

    key_entry = table->free;
    table->free = table->free->next;
    
    key_entry->key = KEY_COPY(key);
    key_entry->next = table->buckets[key_hash].next;
    
    table->buckets[key_hash].next = key_entry;
    ++ table->distinct_count;

    return 0;
}

int fixed_hash_table_has_key(const FixedHashTable* table, KEY_TYPE key)
{
    /* If there is no table, it does not contain any keys */
    if (!table || !table->buckets) return 0;

    size_t key_hash = HASH_FUNCTION(key) % table->bucket_count;

    FixedHashTableEntry* key_entry =
        find_parent_node(table, key, key_hash)->next;

    return !!key_entry;
}

static FixedHashTableEntry* find_parent_node(const FixedHashTable* table,
                                            KEY_TYPE key, uint64_t key_hash)
{
    FixedHashTableEntry* lst_entry = &table->buckets[key_hash];
    FixedHashTableEntry* key_entry = lst_entry->next;

    while (key_entry && !KEY_EQUAL(key_entry->key, key))
    {
        lst_entry = key_entry;
        key_entry = lst_entry->next;
    }
    return lst_entry;
}

static void mark_free(FixedHashTableEntry* entries, size_t entry_count)
{
    for (size_t i = 0; i < entry_count; ++i)
    {
        entries[i].next = i + 1 < entry_count
                            ? entries + i + 1
                            : NULL;
    }
}


static int try_grow(FixedHashTable* table)
{
    const size_t cap_growth = 2;
    if (table->free) return 0;

    const intptr_t old_addr = (intptr_t) table->buckets;

    const size_t old_cap = table->capacity;
    const size_t new_cap = old_cap * cap_growth;
    FixedHashTableEntry* data = NULL;

    SAFE_BLOCK_START
    {
        ASSERT_SIMPLE(
                data = (FixedHashTableEntry*)
                        realloc(table->buckets, new_cap*sizeof(*data)),
                action_result != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        return -1;
    }
    SAFE_BLOCK_END

    const intptr_t new_addr = (intptr_t) data;
    const intptr_t addr_offset = new_addr - old_addr;

    /* Update addresses */
    if (addr_offset)
        for (size_t i = 0; i < old_cap; ++i)
            if (data[i].next)
                data[i].next = (FixedHashTableEntry*)
                                    ((intptr_t)data[i].next + addr_offset);

    mark_free(data + old_cap, new_cap - old_cap);

    table->buckets = data;
    table->free = data + old_cap;
    table->capacity = new_cap;

    return 0;
}
