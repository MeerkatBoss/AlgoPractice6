#define HASH_TABLE_KEY_INT
#define KEY_TYPE int32_t
#define KEY_COPY(key) key
#define KEY_DTOR(key)
#define KEY_EQUAL(a, b) ( (a) == (b) )

#ifndef HASH_FUNCTION
#define HASH_FUNCTION(key) hash_int_multiplicative(key)
#endif
