#define swap(_a, _b) {__typeof__(_a) _c = _a; _a = _b; _b = _c;}

typedef unsigned long uint;
typedef unsigned char uchar;

typedef enum
{
    false, true
} bool;

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))
#define pow2(n) (uint)(1<<(n))
#define mask(n) (uint)((1<<(n))-1)

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
// Map
// An incrementally resizing hashtable map with open addressing
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    void *key, *value;
} MapBucket;

typedef struct
{
    uint (*hash)(void*); // algorithm used to hash keys
    bool (*comp)(void*, void*); // algorithm used to compare keys
    uint indexA;       // keeps track of how far we are in moving items from tableA
    uint sizeA, sizeB; // number of buckets occupied
    uint log2capA, log2capB; // capacity: actual size of tables as a power of 2
    MapBucket *tableA, *tableB; // "old" table and "new" table
} Map;

uint stringhash(void *stringptr);
uint ptrhash(void *ptr);
bool stringcomp(void *str1, void*str2);
bool ptrcomp(void *ptr1, void *ptr2);

Map *Map_new(uint (*hash)(void*), bool (*comp)(void*,void*));
Map *Map_new_sized(int log2size, uint (*hash)(void*), bool (*comp)(void*,void*));
bool Map_has(Map *map, void *key);
void *Map_get(Map *map, void *key);
void *Map_remove(Map *map, void *key);
void *Map_set(Map *map, void *key, void *value);
void Map_del(Map *map);

////////////////////////////////////////////////////////////////////////////////
// ChainedMap
// An incrementally resizing hashtable map with chaining
////////////////////////////////////////////////////////////////////////////////

// Use this instead of Map when lookup is likely to result in finding no
// such key.

typedef struct chainedMapBucket
{
    void *key, *value;
    struct chainedMapBucket *next;
} ChainedMapBucket;

typedef struct
{
    uint (*hash)(void*); // algorithm used to hash keys
    bool (*comp)(void*, void*); // algorithm used to compare keys
    uint indexA;       // keeps track of how far we are in moving items from tableA
    uint sizeA, sizeB; // number of buckets occupied
    uint log2capA, log2capB; // capacity: actual size of tables as a power of 2
    ChainedMapBucket *tableA, *tableB; // "old" table and "new" table
} ChainedMap;

ChainedMap *ChainedMap_new(uint (*hash)(void*), bool (*comp)(void*,void*));
ChainedMap *ChainedMap_new_sized(int log2size, uint (*hash)(void*), bool (*comp)(void*,void*));
bool ChainedMap_has(ChainedMap *map, void *key);
void *ChainedMap_get(ChainedMap *map, void *key);
void *ChainedMap_remove(ChainedMap *map, void *key);
void *ChainedMap_set(ChainedMap *map, void *key, void *value);
void ChainedMap_del(ChainedMap *map);

////////////////////////////////////////////////////////////////////////////////
// MultiMap
// Like Map, but each key can have 1 or more values (instead of just 1).
// Uses an open addressing scheme with linear probing.
////////////////////////////////////////////////////////////////////////////////

typedef struct multiMapList
{
    void *value;
    struct multiMapList *next;
} MultiMapList;

typedef struct
{
    void *key;
    struct multiMapList *next;
} MultiMapBucket;

typedef struct
{
    uint (*hash)(void*); // algorithm used to hash keys
    bool (*comp)(void*, void*); // algorithm used to compare keys
    uint indexA;       // keeps track of how far we are in moving items from tableA
    uint sizeA, sizeB; // number of buckets occupied
    uint log2capA, log2capB; // capacity: actual size of tables as a power of 2
    MultiMapList *tableA, *tableB; // "old" table and "new" table
} MultiMap;

MultiMap *MultiMap_new(uint (*hash)(void*), bool (*comp)(void*,void*));
MultiMap *MultiMap_new_sized(uint log2size, uint (*hash)(void*), bool (*comp)(void*,void*));
bool MultiMap_has(MultiMap *map, void *key);
MultiMapList *MultiMap_get(MultiMap *map, void *key);
void MultiMap_removeAll(MultiMap *map, void *key);
MultiMapList *MultiMap_remove(MultiMap *map, void *key, void *value);
MultiMapList *MultiMap_add(MultiMap *map, void *key, void *value);
void MultiMap_del(MultiMap *map);

////////////////////////////////////////////////////////////////////////////////
// Set
// A set type implemented by hash table
////////////////////////////////////////////////////////////////////////////////

typedef struct setBucket
{
    void *value;
    struct setBucket *next;
} SetBucket;

typedef struct
{
    uint (*hash)(void*); // algorithm used to hash keys
    bool (*comp)(void*, void*); // algorithm used to compare keys
    uint indexA;       // keeps track of how far we are in moving items from tableA
    uint sizeA, sizeB; // number of buckets occupied
    uint log2capA, log2capB; // capacity: actual size of tables as a power of 2
    SetBucket *tableA, *tableB; // "old" table and "new" table
} Set;

Set *Set_new(uint (*hash)(void*), bool (*comp)(void*,void*));
Set *Set_new_sized(uint log2size, uint (*hash)(void*), bool (*comp)(void*,void*));
bool Set_has(Set *set, void *value);
void *Set_get(Set *set, void *value);
void *Set_remove(Set *set, void *value);
void *Set_add(Set *set, void *value);
void *Set_intersection(Set *set1, Set *set2);
void *Set_union(Set *set1, Set *set2);
void *Set_difference(Set *set1, Set *set2);
void *Set_symdifference(Set *set1, Set *set2);
void Set_del(Set *set);

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
