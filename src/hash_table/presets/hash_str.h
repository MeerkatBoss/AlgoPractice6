#define HASH_TABLE_KEY_STR
#define KEY_TYPE char*
#define KEY_COPY(key) strdup(key)
#define KEY_DTOR(key) free(key)
#define KEY_EQUAL(a, b) ( strcmp((a), (b)) == 0 )

#ifndef HASH_FUNCTION
#define HASH_FUNCTION(key) hash_str_polynome(key)
#endif
