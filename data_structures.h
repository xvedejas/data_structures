#define swap(_a, _b) {__typeof__(_a) _c = _a; _a = _b; _b = _c;}

typedef unsigned long uint;
typedef unsigned char uchar;

typedef enum
{
    false, true
} bool;

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))

////////////////////////////////////////////////////////////////////////////////
// LinkedList
// A circular doubly-linked list
////////////////////////////////////////////////////////////////////////////////

typedef struct llistElement
{
    struct llistElement *prev, *next;
    void *value;
} LinkedListElement;

typedef struct
{
    LinkedListElement *first;
    uint size; // number of elements
} LinkedList;

LinkedList *LinkedList_new();
void LinkedList_reverse(LinkedList *list);
void *LinkedList_index(LinkedList *list, int index);
bool LinkedList_insert(LinkedList *list, int index, void *value);
void *LinkedList_remove(LinkedList *list, int index);
bool LinkedList_add(LinkedList *list, void *value); // like "push"
bool LinkedList_enqueue(LinkedList *list, void *value);
void *LinkedList_pop(LinkedList *list); // like "dequeue"
int LinkedList_find(LinkedList *list, void *value);
void LinkedList_del(LinkedList *list);

////////////////////////////////////////////////////////////////////////////////
// ArrayList
// A dynamically growing/shrinking array list
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    void **array;
    uint size;     // number of elements contained
    uint cap; // size of the allocated array
} ArrayList;

ArrayList *ArrayList_new();
void *ArrayList_index(ArrayList *list, int index);
// Use growby if you expect to need the additional capacity. Not necessary to
// use since the list will resize itself as needed.
void ArrayList_growby(ArrayList *list, int additional_capacity);
void ArrayList_add(ArrayList *list, void *item);
void *ArrayList_pop(ArrayList *list);
void *ArrayList_remove(ArrayList *list, int index);
bool ArrayList_insert(ArrayList *list, int index, void *value);
int ArrayList_find(ArrayList *list, void *value);
bool ArrayList_has(ArrayList *list, void *value);
ArrayList *ArrayList_join(ArrayList *listA, ArrayList *listB);
void ArrayList_reverse(ArrayList *list);
void ArrayList_del(ArrayList *list);

////////////////////////////////////////////////////////////////////////////////
// HashTable
// An incrementally resizing hashtable map with open addressing
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    void *key, *value;
} HashTableBucket;

typedef struct
{
    uint (*hash)(void*); // algorithm used to hash keys
    bool (*comp)(void*, void*); // algorithm used to compare keys
    uint indexA;       // keeps track of how far we are in moving items from tableA
    uint sizeA, sizeB; // number of buckets occupied
    uint capA, capB;   // capacity: actual size of tables
    HashTableBucket *tableA, *tableB; // "old" table and "new" table
} HashTable;

uint stringhash(void *stringptr);
uint ptrhash(void *ptr);
bool stringcomp(void *str1, void*str2);
bool ptrcomp(void *ptr1, void *ptr2);

HashTable *HashTable_new(uint (*hash)(void*), bool (*comp)(void*,void*));
bool HashTable_has(HashTable *hashtable, void *key);
void *HashTable_get(HashTable *hashtable, void *key);
void *HashTable_remove(HashTable *hashtable, void *key);
void *HashTable_set(HashTable *hashtable, void *key, void *value);
void HashTable_del(HashTable *hashtable);

////////////////////////////////////////////////////////////////////////////////
// Map
// An incrementally resizing hashtable map with chaining
////////////////////////////////////////////////////////////////////////////////

// Use this instead of HashTable when lookup is likely to result in finding no
// such key.

typedef struct mapBucket
{
    void *key, *value;
    struct mapBucket *next;
} MapBucket;

typedef struct
{
    uint (*hash)(void*); // algorithm used to hash keys
    bool (*comp)(void*, void*); // algorithm used to compare keys
    uint indexA;       // keeps track of how far we are in moving items from tableA
    uint sizeA, sizeB; // number of buckets occupied
    uint capA, capB;   // capacity: actual size of tables
    MapBucket *tableA, *tableB; // "old" table and "new" table
} Map;

Map *Map_new(uint (*hash)(void*), bool (*comp)(void*,void*));
bool Map_has(Map *map, void *key);
void *Map_get(Map *map, void *key);
void *Map_remove(Map *map, void *key);
void *Map_set(Map *map, void *key, void *value);
void Map_del(Map *map);

////////////////////////////////////////////////////////////////////////////////
// StrBuilder
// Useful for building lengths of string
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint size, cap;
    char *s;
} StrBuilder;

StrBuilder *StrBuilder_new(char *initial);
void StrBuilder_del(StrBuilder *sb);
void StrBuilder_appendN(StrBuilder *sb, char *s, uint len);
void StrBuilder_append(StrBuilder *sb, char *s);
void StrBuilder_appendC(StrBuilder *sb, char c);
// join() will change the first strbuilder and not affect the other
void StrBuilder_join(StrBuilder *sb1, StrBuilder *sb2);
// tostring() frees the StrBuilder and returns a string
char *StrBuilder_tostring(StrBuilder *sb);

////////////////////////////////////////////////////////////////////////////////
// BitArray2D
// A static 2D array of bits with an additional position parameter for use in
// pixel-perfect collision detection
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint x, y;
    uint width, height;
    uchar *bits;
} BitArray;

BitArray *BitArray_newempty(uint x, uint y, uint width, uint height);
BitArray *BitArray_newsquare(uint x, uint y, uint width, uint height);
bool BitArray_getbit(BitArray *array, uint x, uint y);
void BitArray_setbit(BitArray *array, uint x, uint y, bool on);
void BitArray_setcolumn(BitArray *array, uint x, bool on);
void BitArray_setrow(BitArray *array, uint y, bool on);
// In the -y direction, add n rows of zeros
void BitArray_growup(BitArray *array, uint n);
// In the +y direction, add n rows of zeros
void BitArray_growdown(BitArray *array, uint n);
// In the -x direction, add n columns of zeros
void BitArray_growleft(BitArray *array, uint n);
// In the +x direction, add n columns of zeros
void BitArray_growright(BitArray *array, uint n);
// Shift in the -y direction without wrapping
void BitArray_shiftup(BitArray *array, uint n);
// Shift in the +y direction without wrapping
void BitArray_shiftdown(BitArray *array, uint n);
// Shift in the -x direction without wrapping
void BitArray_shiftleft(BitArray *array, uint n);
// Shift in the +x direction without wrapping
void BitArray_shiftright(BitArray *array, uint n);
// Remove first n rows
void BitArray_shrinkup(BitArray *array, uint n);
// Remove last n rows
void BitArray_shrinkdown(BitArray *array, uint n);
// Remove first n columns
void BitArray_shrinkleft(BitArray *array, uint n);
// Remove last n columns
void BitArray_shrinkright(BitArray *array, uint n);
void BitArray_print();
