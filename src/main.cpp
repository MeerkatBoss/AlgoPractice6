#include <stdio.h>
#include <stdlib.h>

#include "hash_table/closed_addr_hash_table.h"
#include "hash_table/open_addr_hash_table.h"

#if defined OPEN_ADDR

typedef OpenAddrHashTable hash_table_t;

#define hash_table_ctor(...)     open_addr_hash_table_ctor(__VA_ARGS__)
#define hash_table_dtor(...)     open_addr_hash_table_dtor(__VA_ARGS__)
#define hash_table_insert(...)   open_addr_hash_table_insert(__VA_ARGS__)
#define hash_table_erase(...)    open_addr_hash_table_erase(__VA_ARGS__)
#define hash_table_contains(...) open_addr_hash_table_contains(__VA_ARGS__)

#else /* CLOSED_ADDR */

typedef ClosedAddrHashTable hash_table_t;

#define hash_table_ctor(...)     closed_addr_hash_table_ctor(__VA_ARGS__)
#define hash_table_dtor(...)     closed_addr_hash_table_dtor(__VA_ARGS__)
#define hash_table_insert(...)   closed_addr_hash_table_insert(__VA_ARGS__)
#define hash_table_erase(...)    closed_addr_hash_table_erase(__VA_ARGS__)
#define hash_table_contains(...) closed_addr_hash_table_contains(__VA_ARGS__)

#endif

enum command
{
    CMD_INSERT = 0,
    CMD_ERASE = 1,
    CMD_CONTAINS = 2
};

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fputs("Iterations not specified\n", stderr);
        return 1;
    }
    char* end = NULL;
    size_t repeat = strtoul(argv[1], &end, 10);
    if (!end || *end != '\0')
    {
        fputs("Invalid number of iterations\n", stderr);
        return 1;
    }

    srand(0);
    hash_table_t table = {};
    hash_table_ctor(&table);

    for (size_t i = 0; i < repeat; ++i)
    {
        command cmd = (command) (rand() % 3);

        switch (cmd)
        {
        case CMD_INSERT:   hash_table_insert  (&table, (uint32_t)rand()); break;
        case CMD_ERASE:    hash_table_erase   (&table, (uint32_t)rand()); break;
        case CMD_CONTAINS: hash_table_contains(&table, (uint32_t)rand()); break;
        default:
            break;
        }
    }

    hash_table_dtor(&table);
}
