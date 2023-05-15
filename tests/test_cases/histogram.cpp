#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "meerkat_assert/asserts.h"

#include "hash_table/fixed_hash_table.h"

#include "test_utils/display.h"

#include "histogram.h"


void fill_table(FixedHashTable* table, size_t data_size);

static void dump_contents(FILE* output, const FixedHashTable* table);
static size_t get_bucket_size(const FixedHashTableEntry* head);

#define STR(x) __BASIC_STR(x)
#define __BASIC_STR(x) #x

int run_test_histogram([[maybe_unused]] int argc,
                       [[maybe_unused]] const char* const* argv,
                       const TestConfig* config)
{
    FILE *output = NULL;

    SAFE_BLOCK_START
    {
        if (config->filename)
        {
            ASSERT_MESSAGE(
                output = fopen(config->filename,
                                config->append_to_file ? "a" : "w"),
                action_result != NULL,
                "Failed to open output file");
        }
        else output = stdout;

    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fprintf(stderr, "Error: %s\n", assertion_info.message);
        return 1;
    }
    SAFE_BLOCK_END
    
    srand(0);

    FixedHashTable table = {};

    fixed_hash_table_ctor(&table, 1000);
    fill_table(&table, 1'000'000);
    dump_contents(output, &table);
    fixed_hash_table_dtor(&table);
    fclose(output);
    
    return 0;
}

static void dump_contents(FILE* output, const FixedHashTable* table)
{
    fputs(STR(HASH_FUNCTION), output);

    for (size_t i = 0; i < table->bucket_count; ++i)
    {
        const FixedHashTableEntry* entry = &table->buckets[i];
        fprintf(output, ",%zu", get_bucket_size(entry));
    }
    fputc('\n', output);
}

static size_t get_bucket_size(const FixedHashTableEntry* head)
{
    size_t size = 0;
    while ((head = head->next))
        ++ size;

    return size;
}

#if defined HASH_TABLE_KEY_INT

void fill_table(FixedHashTable* table, size_t data_size)
{
    for (size_t i = 0; i < data_size; ++i)
    {
        fixed_hash_table_add_key(table, rand());
        progress_bar(i, data_size, NAN);
    }
}

#elif defined HASH_TABLE_KEY_DOUBLE

void fill_table(FixedHashTable* table, size_t data_size)
{
    for (size_t i = 0; i < data_size; ++i)
    {
        fixed_hash_table_add_key(table,
                (double) rand() * ((double)rand() / (double)rand()));
        progress_bar(i, data_size, NAN);
    }
}

#elif defined HASH_TABLE_KEY_STR

void fill_table(FixedHashTable* table, size_t data_size)
{
    const size_t max_len = 512;
    char buffer[max_len] = "";

    for (size_t i = 0; i < data_size; ++i)
    {
        size_t length = (size_t) rand() % max_len;
        buffer[length] = '\0';
        for (size_t j = 0; j < length; ++j)
            buffer[j] = (char) ('a' + rand()%26);
        fixed_hash_table_add_key(table, buffer);
        progress_bar(i, data_size, NAN);
    }
}

#else
#error Unknown KEY_TYPE
#endif

