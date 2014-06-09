#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "CUnit/Basic.h"
#include "data_structures.h"

#define swap(_a, _b) {__typeof__(_a) _c = _a; _a = _b; _b = _c;}
#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))
#define pow2(n) (uint)(1<<(n))
#define mask(n) (uint)((1<<(n))-1)

////////////////////////////////////////////////////////////////////////////////
// LinkedList
// A circular doubly-linked list
////////////////////////////////////////////////////////////////////////////////

LinkedList *LinkedList_new()
{
    LinkedList *list = malloc(sizeof(LinkedList));
    list->first = NULL;
    list->size = 0;
    return list;
}

LinkedList *LinkedList_fromarray(void **array, uint size)
{
    assert(array != NULL);
    LinkedList *list = LinkedList_new();
    uint i;
    for (i = 0; i < size; i++)
        LinkedList_add(list, array[i]);
    return list;
}

void LinkedList_reverse(LinkedList *list)
{
    assert(list != NULL);
    assert(list->first != NULL);
    LinkedListElement *first = list->first;
    LinkedListElement *elem = first;
    do
    {
        swap(elem->next, elem->prev);
        elem = elem->prev;
    } while (elem != first);
}

LinkedListElement *_LinkedList_index(LinkedList *list, int index)
{
    assert(list != NULL);
    assert(list->first != NULL);
    int size = (int)list->size;
    if (index < 0) // handle negative indices
    {
        index += size;
    }
    if (index < 0 || index >= size) // handle out of bounds
    {
        return NULL;
    }
    int middle = size >> 1;
    LinkedListElement *elem = list->first;
    int i;
    if (index < middle) // count up
    {
        for (i = 0; i < index; i++)
            elem = elem->next;
    }
    else // count down
    {
        for (i = size; i > index; i--)
            elem = elem->prev;
    }
    return elem;
}

void *LinkedList_index(LinkedList *list, int index)
{
    assert(list != NULL);
    assert(list->first != NULL);
    LinkedListElement *elem = _LinkedList_index(list, index);
    if (elem == NULL)
        return NULL;
    return elem->value;
}

bool LinkedList_insert(LinkedList *list, int index, void *value)
{
    assert(list != NULL);
    if (list->size == 0 && (index == 0 || index == -1))
    {
        LinkedListElement *first = malloc(sizeof(LinkedListElement));
        first->prev = first;
        first->next = first;
        list->first = first;
        first->value = value;
        list->size++;
        return true;
    }
    assert(list->first != NULL);
    LinkedListElement *elem;
    if (index == (int)list->size)
        elem = _LinkedList_index(list, 0);
    else
        elem = _LinkedList_index(list, index);
    if (elem == NULL)
        return false;
    // We insert before the current element
    LinkedListElement *prev = elem->prev;
    LinkedListElement *new = malloc(sizeof(LinkedListElement));
    prev->next = new;
    elem->prev = new;
    new->prev = prev;
    new->next = elem;
    new->value = value;
    list->size++;
    // inserting at beginning is special, since it requires changing the first
    if (index == 0)
        list->first = list->first->prev;
    return true;
}

bool LinkedList_add(LinkedList *list, void *value)
{
    return LinkedList_insert(list, list->size, value);
}

bool LinkedList_enqueue(LinkedList *list, void *value)
{
    return LinkedList_insert(list, 0, value);
}

LinkedListElement *_LinkedList_remove(LinkedList *list, int index)
{
    assert(list != NULL);
    assert(list->first != NULL);
    LinkedListElement *elem = _LinkedList_index(list, index);
    if (elem == NULL)
        return NULL;
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    list->size--;
    return elem;
}

void *LinkedList_remove(LinkedList *list, int index)
{
    LinkedListElement *elem = _LinkedList_remove(list, index);
    if (elem == NULL)
        return false;
    void *value = elem->value;
    free(elem);
    return value;
}

bool LinkedList_remove_first(LinkedList *list, void *value)
{
    LinkedListElement *first = list->first;
    LinkedListElement *elem = first;
    LinkedListElement *next = elem->next;
    do
    {
        if (elem->value == value)
        {
            elem->prev->next = elem->next;
            elem->next->prev = elem->prev;
            free(elem);
            return true;
        }
        elem = next;
        next = elem->next;
    } while (elem != first);
    return false;
}

bool LinkedList_remove_all(LinkedList *list, void *value)
{
    LinkedListElement *first = list->first;
    LinkedListElement *elem = first;
    LinkedListElement *next = elem->next;
    bool found = false;
    do
    {
        if (elem->value == value)
        {
            found = true;
            elem->prev->next = elem->next;
            elem->next->prev = elem->prev;
            free(elem);
        }
        elem = next;
        next = next->next;
    } while (elem != first);
    return found;
}

void *LinkedList_pop(LinkedList *list)
{
    return LinkedList_remove(list, -1);
}

int LinkedList_find(LinkedList *list, void *value)
{
    assert(list != NULL);
    assert(list->first != NULL);
    int i;
    int size = (int)list->size;
    LinkedListElement *elem = list->first;
    for (i = 0; i < size; i++)
    {
        if (elem->value == value)
            return i;
        elem = elem->next;
    }
    return -1;
}

void LinkedList_del(LinkedList *list)
{
    assert(list != NULL);
    assert(list->first != NULL);
    LinkedListElement *first = list->first;
    LinkedListElement *elem = first;
    do
    {
        free(elem);
        elem = elem->next;
    } while (elem != first);
    free(list);
}

void LinkedList_test()
{
    LinkedList *list = LinkedList_new();
    // Test insert()
    LinkedList_insert(list, 0, (void*)1);
    LinkedList_insert(list, 0, (void*)2);
    LinkedList_insert(list, 1, (void*)3);
    LinkedList_insert(list, 1, (void*)4);
    LinkedList_insert(list, 1, (void*)5);
    LinkedList_insert(list, 1, (void*)6);
    // Test index()
    CU_ASSERT(LinkedList_index(list, 0) == (void*)2);
    CU_ASSERT(LinkedList_index(list, -1) == (void*)1);
    CU_ASSERT(LinkedList_index(list, 5) == (void*)1);
    // Test remove()
    LinkedList_remove(list, 3);
    CU_ASSERT(LinkedList_index(list, 0) == (void*)2);
    CU_ASSERT(LinkedList_index(list, -1) == (void*)1);
    CU_ASSERT(LinkedList_index(list, 5) == NULL);
    LinkedList_del(list);
}

////////////////////////////////////////////////////////////////////////////////
// ArrayList
// A dynamically growing/shrinking array list
////////////////////////////////////////////////////////////////////////////////

static ArrayList *_ArrayList_new(int cap)
{
    ArrayList *list = malloc(sizeof(ArrayList));
    list->size = 0;
    list->cap = cap;
    list->array = (void**)malloc(list->cap * sizeof(void*));
    return list;
}

ArrayList *ArrayList_new()
{
    return _ArrayList_new(8);
}

void *ArrayList_index(ArrayList *list, int index)
{
    int size = (int)list->size;
    if (index < 0)
        index += size;
    if (index < 0 || index >= size) // check bounds
        return NULL;
    return list->array[index];
}

void ArrayList_growby(ArrayList *list, int additional_cap)
{
    list->cap += additional_cap;
    list->array = (void**)realloc(list->array, list->cap * sizeof(void*));
}

void ArrayList_grow(ArrayList *list)
{
    ArrayList_growby(list, list->cap >> 1);
}

void ArrayList_add(ArrayList *list, void *item)
{
    if (list->size >= list->cap)
        ArrayList_grow(list);
    list->array[list->size++] = item;
}

void *ArrayList_pop(ArrayList *list)
{
    if (list->size == 0)
        return NULL;
    return list->array[--list->size];
}

void *ArrayList_remove(ArrayList *list, int index)
{
    int size = (int)list->size;
    if (index < 0)
        index += size;
    if (index < 0 || index >= size) // check bounds
        return NULL;
    void **dest = list->array + index;
    void *value = *dest;
    memmove(dest, dest + 1, (list->size - index) * sizeof(void*));
    list->size--;
    return value;
}

bool ArrayList_insert(ArrayList *list, int index, void *value)
{
    if ((list->size == 0 && index == 0) || index == -1)
    {
        ArrayList_add(list, value);
        return true;
    }
    int size = (int)list->size;
    if (index < 0)
        index += size;
    if (index < 0 || index >= size) // check bounds
        return false;
    void **dest = list->array + index;
    list->size++;
    if (list->size >= list->cap)
        ArrayList_grow(list);
    memmove(dest + 1, dest, (list->size - index) * sizeof(void*));
    *dest = value;
    return true;
}

int ArrayList_find(ArrayList *list, void *value)
{
    uint i;
    for (i = 0; i < list->size; i++)
        if (list->array[i] == value)
            return i;
    return -1;
}

bool ArrayList_has(ArrayList *list, void *value)
{
    return (ArrayList_find(list, value) == -1)?false:true;
}

ArrayList *ArrayList_join(ArrayList *listA, ArrayList *listB)
{
    ArrayList *newlist = _ArrayList_new(listA->cap + listB->cap);
    memcpy(newlist->array, listA->array, listA->size * sizeof(void*));
    memcpy(newlist->array + listA->size, listB->array, listB->size * sizeof(void*));
    newlist->size = listA->size + listB->size;
    return newlist;
}

void ArrayList_reverse(ArrayList *list)
{
    uint size = list->size;
    uint i;
    for (i = 0; i < (size >> 1); i++)
        swap(list[i], list[size - 1 - i]);
}

void ArrayList_del(ArrayList *list)
{
    free(list->array);
    free(list);
}

void ArrayList_test()
{
    ArrayList *list = ArrayList_new();
    // Test insert()
    ArrayList_insert(list, 0, (void*)1);
    ArrayList_insert(list, 0, (void*)2);
    ArrayList_insert(list, 1, (void*)3);
    ArrayList_insert(list, 1, (void*)4);
    ArrayList_insert(list, 1, (void*)5);
    ArrayList_insert(list, 1, (void*)6);
    // Test index()
    CU_ASSERT(ArrayList_index(list, 0) == (void*)2);
    CU_ASSERT(ArrayList_index(list, -1) == (void*)1);
    CU_ASSERT(ArrayList_index(list, 5) == (void*)1);
    // Test remove()
    ArrayList_remove(list, 3);
    CU_ASSERT(ArrayList_index(list, 0) == (void*)2);
    CU_ASSERT(ArrayList_index(list, -1) == (void*)1);
    CU_ASSERT(ArrayList_index(list, 5) == NULL);
    ArrayList_del(list);
}

////////////////////////////////////////////////////////////////////////////////
// Map
// An incrementally resizing hashtable map with open addressing
////////////////////////////////////////////////////////////////////////////////

uint stringhash(void *stringptr)
{
    char *string = (char*)stringptr;
    /* "Jenkin's Hash" */
    uint hash;
    uint i;
    uint len = strlen(string);
    for (hash = i = 0; i < len; i++)
    {
        hash += string[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

uint ptrhash(void *ptr)
{
    return (uint)ptr >> 2;
}

bool stringcomp(void *str1, void *str2)
{
    if (str1 == NULL)
        return str2 == NULL;
    return strcmp(str1, str2) == 0;
}

bool ptrcomp(void *ptr1, void *ptr2)
{
    return ptr1 == ptr2;
}

static void _Map_add(Map *map, void *key, void *value, bool recurrant);
static void _Map_transfer(Map *map);
static Map *_Map_new(int log2tablesize, uint (*hash)(void*), bool (*comp)(void*,void*));
static MapBucket *_Map_get(Map *map, void *key);

// add an item to tableB only
static void _Map_add(Map *map, void *key, void *value, bool recurrant)
{
    assert(map != NULL);
    assert(key != NULL);
    uint hash = map->hash(key);
    uint size_mod_B = mask(map->log2capB);
    uint index = hash; // modulus
    MapBucket *bucket;
    do
    {
        index &= size_mod_B;
        bucket = &map->tableB[index];
        index++;
    } while (bucket->key != NULL);
    bucket->key = key;
    bucket->value = value;
    map->sizeB++;
    // also move an item from tableA to tableB
    if (!recurrant)
        _Map_transfer(map);
}

// transfer any item from tableA to tableB
static void _Map_transfer(Map *map)
{
    uint cap = pow2(map->log2capA);
    if (map->tableA != NULL)
    {
        while (map->indexA < cap)
        {
            MapBucket *bucket = &map->tableA[map->indexA];
            if (bucket->key != NULL)
            {
                _Map_add(map, bucket->key, bucket->value, true);
                bucket->key = NULL;
                map->sizeA--;
                return;
            }
            map->indexA++;
        }
        // We have emptied tableA
        assert(map->sizeA == 0);
        free(map->tableA);
        map->tableA = NULL;
    }
    
    // check to see if table B has reached 75% cap.
    uint high_load = pow2(map->log2capB - 2) + pow2(map->log2capB - 1);
    if (map->sizeB < high_load)
        return;
    
    uint log2newtablesize = map->log2capB + 1; // grow by 2x
    if (map->tableA != NULL)
        free(map->tableA);
    map->tableA = map->tableB;
    map->log2capA = map->log2capB;
    map->sizeA = map->sizeB;
    map->indexA = 0;
    map->tableB = calloc(pow2(log2newtablesize), sizeof(MapBucket));
    map->log2capB = log2newtablesize;
    map->sizeB = 0;
}

Map *Map_new_sized(int log2tablesize,
                   uint (*hash)(void*), bool (*comp)(void*,void*))
{
    Map *map = malloc(sizeof(Map));
    map->hash = hash;
    map->comp = comp;
    map->indexA = 0;
    map->sizeA = 0;
    map->sizeB = 0;
    map->log2capA = 0;
    map->log2capB = log2tablesize;
    map->tableA = NULL;
    map->tableB = calloc(pow2(log2tablesize), sizeof(MapBucket));
    return map;
}

Map *Map_new(uint (*hash)(void*), bool (*comp)(void*,void*))
{
    return Map_new_sized(4, hash, comp);
}

MapIterator Map_iter(Map *map)
{
    MapIterator iter;
    iter.map = map;
    iter.tableA = false;
    iter.index = -1;
    return iter;
}

static MapBucket *_Map_iter_next(MapIterator *iter)
{
    Map *map = iter->map;
    MapBucket *table = (iter->tableA)? map->tableA : map->tableB;
    if (iter.index < 0)
    {
        
    }
    MapBucket *bucket = &table[iter.index];
    /// how do we ensure that after the first call, the iterator is still
    /// pointing at the first element (which may be at index 0)? Maybe use -1
    /// for the "uninitialized" step? Maybe rethink how iterators should work?
    
}

static MapBucket *_Map_get(Map *map, void *key)
{
    assert(key != NULL);
    uint hash = map->hash(key);
    uint size_mod_B = mask(map->log2capB);
    uint indexB = hash & size_mod_B; // modulus
    uint i = indexB;
    MapBucket *bucket;
    do
    {
        bucket = &map->tableB[i];
        if (bucket->key != NULL && map->comp(bucket->key, key))
            return bucket;
        i++;
        i &= size_mod_B;
    } while (i != indexB);
    
    // The key wasn't found in table B, now look in table A
    
    if (map->tableA == NULL)
        return NULL;
        
    uint size_mod_A = mask(map->log2capA);
    uint indexA = hash & size_mod_A; // modulus
    i = indexA;
    do
    {
        bucket = &map->tableA[i];
        if (bucket->key != NULL && map->comp(bucket->key, key))
            return bucket;
        i++;
        i &= size_mod_A;
    } while (i != indexA);
    return NULL;
}

bool Map_has(Map *map, void *key)
{
    MapBucket *bucket = _Map_get(map, key);
    if (bucket == NULL)
        return false;
    return true;
}

void *Map_get(Map *map, void *key)
{
    MapBucket *bucket = _Map_get(map, key);
    if (bucket == NULL)
        return NULL;
    return bucket->value;
}

void *Map_remove(Map *map, void *key)
{
    MapBucket *bucket = _Map_get(map, key);
    if (bucket == NULL)
        return NULL;
    void *value = bucket->value;
    bucket->key = NULL;
    // check which table the bucket was in:
    if (bucket >= map->tableA && bucket < map->tableA + pow2(map->log2capA))
        map->sizeA--;
    else
        map->sizeB--;
    return value;
}

void *Map_set(Map *map, void *key, void *value)
{
    MapBucket *bucket = _Map_get(map, key);
    void *oldvalue = NULL;
    
    if (bucket == NULL)
    {
        _Map_add(map, key, value, false);
    }
    else
    {
        oldvalue = bucket->value;
        bucket->value = value;
    }
    return oldvalue;
}

void Map_del(Map *map)
{
    if (map->tableA != NULL)
        free(map->tableA);
    free(map->tableB);
    free(map);
}

void Map_test()
{
    Map *map = Map_new(stringhash, stringcomp);
    Map_set(map, "test0", "a");
    Map_set(map, "test1", "b");
    Map_set(map, "test2", "c");
    Map_set(map, "test3", "d");
    Map_set(map, "test0", "e");
    CU_ASSERT(strcmp(Map_get(map, "test0"), "e") == 0);
    CU_ASSERT(strcmp(Map_get(map, "test1"), "b") == 0);
    CU_ASSERT(strcmp(Map_get(map, "test2"), "c") == 0);
    CU_ASSERT(strcmp(Map_get(map, "test3"), "d") == 0);
    Map_remove(map, "test2");
    CU_ASSERT(strcmp(Map_get(map, "test0"), "e") == 0);
    CU_ASSERT(strcmp(Map_get(map, "test1"), "b") == 0);
    CU_ASSERT(Map_get(map, "test2") == NULL);
    CU_ASSERT(strcmp(Map_get(map, "test3"), "d") == 0);
    Map_del(map);
}

void Map_profile()
{
    /// todo
}

////////////////////////////////////////////////////////////////////////////////
// ChainedMap
// An incrementally resizing hashtable map with chaining
////////////////////////////////////////////////////////////////////////////////

static void _ChainedMap_add(ChainedMap *map, void *key, void *value, bool recurrant);

ChainedMap *ChainedMap_new_sized(int log2tablesize,
                                 uint (*hash)(void*), bool (*comp)(void*,void*))
{
    ChainedMap *map = malloc(sizeof(ChainedMap));
    map->hash = hash;
    map->comp = comp;
    map->indexA = 0;
    map->sizeA = 0;
    map->sizeB = 0;
    map->log2capA = 0;
    map->log2capB = log2tablesize;
    map->tableA = NULL;
    map->tableB = calloc(pow2(log2tablesize), sizeof(ChainedMapBucket));
    return map;
}

ChainedMap *ChainedMap_new(uint (*hash)(void*), bool (*comp)(void*,void*))
{
    return ChainedMap_new_sized(4, hash, comp);
}

static ChainedMapBucket *_ChainedMap_get(ChainedMap *map, ChainedMapBucket *maptable,
                                  uint index, void *key)
{
    ChainedMapBucket *bucket = &maptable[index];
    if (bucket->key != NULL)
    {
        do
        {
            if (map->comp(bucket->key, key))
                return bucket;
            bucket = bucket->next;
        } while (bucket != NULL);
    }
    return NULL;
}

// Move one entry from tableA to tableB
static void _ChainedMap_transfer(ChainedMap *map)
{
    assert(map != NULL);
    if (map->tableA != NULL)
    {
        uint i = map->indexA;
        uint cap = pow2(map->log2capA);
        ChainedMapBucket *bucket, *next;
        for (i = 0; i < cap; i++)
        {
            bucket = &map->tableA[i];
            if (bucket->key != NULL)
            {
                next = bucket->next->next;
                void *key = bucket->key;
                void *value = bucket->value;
                bucket->key = next->key;
                bucket->value = next->value;
                free(bucket->next);
                bucket->next = next;
                _ChainedMap_add(map, key, value, true);
                map->indexA = i;
                return;
            }
        }
    }
    
    // check to see if table B has reached 75% cap.
    uint high_load = pow2(map->log2capB + 2) + pow2(map->log2capB + 1);
    if (map->sizeB < high_load)
        return;
    
    uint log2newtablesize = map->log2capB + 1; // grow by 2x
    if (map->tableA != NULL)
        free(map->tableA);
    map->tableA = map->tableB;
    map->log2capA = map->log2capB;
    map->sizeA = map->sizeB;
    map->indexA = 0;
    map->tableB = calloc(pow2(log2newtablesize), sizeof(ChainedMapBucket));
    map->log2capB = log2newtablesize;
    map->sizeB = 0;
}

void *ChainedMap_get(ChainedMap *map, void *key)
{
    assert(map != NULL);
    assert(key != NULL);
    uint hash = map->hash(key);
    uint indexB = hash & mask(map->log2capB);
    ChainedMapBucket *bucket = _ChainedMap_get(map, map->tableB, indexB, key);
    if (bucket != NULL)
        return bucket->value;
    if (map->tableA == NULL)
        return NULL;
    uint indexA = hash & mask(map->log2capA);
    bucket = _ChainedMap_get(map, map->tableA, indexA, key);
    if (bucket == NULL)
        return NULL;
    return bucket->value;
}

bool ChainedMap_has(ChainedMap *map, void *key)
{
    assert(key != NULL);
    uint hash = map->hash(key);
    uint indexB = hash & mask(map->log2capB);
    ChainedMapBucket *bucket = _ChainedMap_get(map, map->tableB, indexB, key);
    if (bucket != NULL)
        return true;
    uint indexA = hash & mask(map->log2capA);
    bucket = _ChainedMap_get(map, map->tableA, indexA, key);
    return bucket != NULL;
}

void *ChainedMap_remove(ChainedMap *map, void *key)
{
    uint hash = map->hash(key);
    uint indexB = hash & mask(map->log2capB);
    ChainedMapBucket *bucket = &map->tableB[indexB];
    void *value = NULL;
    if (map->comp(bucket->key, key))
    {
        value = bucket->value;
        ChainedMapBucket *next = bucket->next;
        if (bucket->next != NULL)
        {
            bucket->key = next->key;
            bucket->value = next->value;
            bucket->next = next->next;
            free(next);
        }
        else
            bucket->key = NULL;
        map->sizeB--;
        return value;
    }
    ChainedMapBucket *prev = bucket;
    for (bucket = bucket->next; bucket != NULL; bucket = bucket->next)
    {
        if (map->comp(bucket->key, key))
        {
            prev->next = bucket->next;
            value = bucket->value;
            free(bucket);
            map->sizeB--;
            return value;
        }
        prev = bucket;
    }
    uint indexA = hash & mask(map->log2capA);
    bucket = &map->tableA[indexA];
    if (map->comp(bucket->key, key))
    {
        value = bucket->value;
        ChainedMapBucket *next = bucket->next;
        if (bucket->next != NULL)
        {
            bucket->key = next->key;
            bucket->value = next->value;
            bucket->next = next->next;
            free(next);
        }
        else
            bucket->key = NULL;
        map->sizeA--;
        return value;
    }
    prev = bucket;
    for (bucket = bucket->next; bucket != NULL; bucket = bucket->next)
    {
        if (map->comp(bucket->key, key))
        {
            prev->next = bucket->next;
            value = bucket->value;
            free(bucket);
            map->sizeA--;
            return value;
        }
        prev = bucket;
    }
    return NULL;
}

// Add to table B, requires that the key is not yet in map
static void _ChainedMap_add(ChainedMap *map, void *key, void *value, bool recurrant)
{
    uint index = map->hash(key) & mask(map->log2capB);
    ChainedMapBucket *bucket = &map->tableB[index];
    ChainedMapBucket *new_bucket = malloc(sizeof(ChainedMapBucket));
    new_bucket->key = bucket->key;
    new_bucket->value = bucket->value;
    new_bucket->next = bucket->next;
    bucket->key = key;
    bucket->value = value;
    bucket->next = new_bucket;
    map->sizeB++;
    if (!recurrant)
        _ChainedMap_transfer(map); // also move an item from tableA to tableB
}

void *ChainedMap_set(ChainedMap *map, void *key, void *value)
{
    assert(map != NULL);
    assert(key != NULL);
    uint hash = map->hash(key);
    uint indexB = hash & mask(map->log2capB);
    ChainedMapBucket *bucketB = _ChainedMap_get(map, map->tableB, indexB, key);
    void *old_value;
    if (bucketB != NULL)
    {
        old_value = bucketB->value;
        bucketB->value = value;
        return old_value;
    }
    if (map->tableA != NULL)
    {
        uint indexA = hash & mask(map->log2capA);
        ChainedMapBucket *bucketA =
            _ChainedMap_get(map, map->tableA, indexA, key);
        if (bucketA != NULL)
        {
            old_value = bucketB->value;
            bucketA->value = value;
            return old_value;
        }
    }
    // We didn't find an existing such key, so add it
    _ChainedMap_add(map, key, value, false);
    return NULL;
}

void ChainedMap_del(ChainedMap *map)
{
    uint i;
    ChainedMapBucket *bucket, *next;
    if (map->tableA != NULL)
    {
        for (i = 0; i < pow2(map->log2capA); i++)
        {
            bucket = &map->tableA[i];
            next = bucket->next;
            while (next != NULL)
            {
                free(next);
                next = next->next;
                bucket->next = next;
            }
        }
        free(map->tableA);
    }
    for (i = 0; i < pow2(map->log2capB); i++)
    {
        bucket = &map->tableB[i];
        next = bucket->next;
        while (next != NULL)
        {
            free(next);
            next = next->next;
            bucket->next = next;
        }
    }
    free(map->tableB);
    free(map);
}

void ChainedMap_test()
{
    ChainedMap *map = ChainedMap_new(stringhash, stringcomp);
    ChainedMap_set(map, "test0", "a");
    ChainedMap_set(map, "test1", "b");
    ChainedMap_set(map, "test2", "c");
    ChainedMap_set(map, "test3", "d");
    ChainedMap_set(map, "test0", "e");
    CU_ASSERT(strcmp(ChainedMap_get(map, "test0"), "e") == 0);
    CU_ASSERT(strcmp(ChainedMap_get(map, "test1"), "b") == 0);
    CU_ASSERT(strcmp(ChainedMap_get(map, "test2"), "c") == 0);
    CU_ASSERT(strcmp(ChainedMap_get(map, "test3"), "d") == 0);
    ChainedMap_remove(map, "test2");
    CU_ASSERT(strcmp(ChainedMap_get(map, "test0"), "e") == 0);
    CU_ASSERT(strcmp(ChainedMap_get(map, "test1"), "b") == 0);
    CU_ASSERT(ChainedMap_get(map, "test2") == NULL);
    CU_ASSERT(strcmp(ChainedMap_get(map, "test3"), "d") == 0);
    ChainedMap_del(map);
}

////////////////////////////////////////////////////////////////////////////////
// MultiMap
// Like Map, but each key can have 1 or more values (instead of just 1).
// An open addressing scheme is used.
////////////////////////////////////////////////////////////////////////////////

MultiMap *MultiMap_new(uint (*hash)(void*), bool (*comp)(void*,void*))
{
    return Map_new(hash, comp);
}

MultiMap *MultiMap_new_sized(uint log2size, uint (*hash)(void*), bool (*comp)(void*,void*))
{
    return Map_new_sized(log2size, hash, comp);
}

bool MultiMap_has(MultiMap *map, void *key)
{
    return Map_has(map, key);
}

bool MultiMap_has_pair(MultiMap *map, void *key, void *value)
{
    LinkedList *list = Map_get(map, key);
    LinkedListElement *first = list->first;
    LinkedListElement *elem = first;
    do
    {
        if (elem->value == value)
            return true;
        elem = elem->next;
    } while (elem != first);
    return false;
}

LinkedList *MultiMap_get(MultiMap *map, void *key)
{
    return Map_get(map, key);
}

bool MultiMap_remove_key(MultiMap *map, void *key)
{
    LinkedList *list = Map_remove(map, key);
    if (list == NULL)
        return false;
    LinkedList_del(list);
    return true;
}

LinkedList *MultiMap_remove(MultiMap *map, void *key, void *value)
{
    LinkedList *list = Map_get(map, key);
    if (list == NULL)
        return NULL;
    LinkedList_remove_all(list, value);
    return list;
}

LinkedList *MultiMap_add(MultiMap *map, void *key, void *value)
{
    LinkedList *list = Map_get(map, key);
    if (list == NULL)
    {
        list = LinkedList_new();
        Map_set(map, key, list);
    }
    LinkedList_add(list, value);
    return list;
}

void MultiMap_del(MultiMap *map)
{
    uint i;
    for (i = 0; i < pow2(map->log2capA); i++)
    {
        LinkedList *list = map->tableA[i].value;
        if (list != NULL)
            LinkedList_del(list);
    }
    for (i = 0; i < pow2(map->log2capB); i++)
    {
        LinkedList *list = map->tableB[i].value;
        if (list != NULL)
            LinkedList_del(list);
    }
    Map_del(map);
}

////////////////////////////////////////////////////////////////////////////////
// Set
// A set type implemented by hash table
////////////////////////////////////////////////////////////////////////////////

static void _Set_add(Set *set, void *value, bool recurrant);

Set *Set_new_sized(uint log2tablesize,
                   uint (*hash)(void*),
                   bool (*comp)(void*,void*))
{
    Set *set = malloc(sizeof(Set));
    set->hash = hash;
    set->comp = comp;
    set->indexA = 0;
    set->sizeA = 0;
    set->sizeB = 0;
    set->log2capA = 0;
    set->log2capB = log2tablesize;
    set->tableA = NULL;
    set->tableB = calloc(pow2(log2tablesize), sizeof(SetBucket));
    return set;
}

Set *Set_new(uint (*hash)(void*), bool (*comp)(void*,void*))
{
    return Set_new_sized(4, hash, comp);
}

SetIterator Set_iter(Set *set)
{
    SetIterator iter;
    iter.set = set;
    iter.tableA = false;
    iter.index = 0;
    return iter;
}

static SetBucket *_Set_iter_current(SetIterator *iter)
{
    SetBucket *table;
    if (iter->tableA)
        table = tableA;
    else
        table = tableB;
    SetBucket *bucket;
    bucket = &table[iter->bucket_index];
    assert(bucket->value != NULL);
    uint i = iter->chain_index;
    while (i --> 0)
    {
        bucket = bucket->next;
        assert(bucket != NULL);
    }
    return bucket;
}

void *Set_iter_current(SetIterator *iter)
{
    return _Set_iter_current(iter)->value;
}

// The iterator should be describing the position of the bucket returned once
// this function returns
static SetBucket *_Set_iter_next(SetIterator *iter)
{
    SetBucket *table;
    uint tablesize;
    if (iter->tableA)
    {
        table = tableA;
        tablesize = pow2(set->log2capA);
    }
    else
    {
        table = tableB;
        tablesize = pow2(set->log2capB);
    }
    iter->chain_index++;
    SetBucket *bucket;
    while (true)
    {
        bucket = &table[iter->bucket_index];
        if (bucket->value != NULL)
            break;
        iter->chain_index = 0;
        iter->bucket_index++;
        // If we reach the end of this table...
        if (iter->bucket_index >= tablesize)
        {
            iter->bucket_index = 0;
            if (iter->tableA)
            {
                // we're done iterating. Reset the iterator and return NULL
                iter->tableA = false;
                return NULL;
            }
            else
            {
                // go on to table A
                iter->tableA = true;
            }
        }
    }
    uint i = iter->chain_index;
    while (i --> 0)
    {
        bucket = bucket->next;
        if (bucket == NULL)
            return NULL;
    }
    return bucket;
}

void *Set_iter_next(SetIterator *iter)
{
    return _Set_iter_next(iter);
}

static SetBucket *_Set_get_chain(Set *set, SetBucket *settable,
                                 uint index, void *value)
{
    SetBucket *bucket = &settable[index];
    if (bucket->value != NULL)
    {
        do
        {
            if (set->comp(bucket->value, value))
                return bucket;
            bucket = bucket->next;
        } while (bucket != NULL);
    }
    return NULL;
}

// Move one entry from tableA to tableB
static void _Set_transfer(Set *set)
{
    assert(set != NULL);
    if (set->tableA != NULL)
    {
        uint i = set->indexA;
        uint cap = pow2(set->log2capA);
        SetBucket *bucket, *next;
        for (i = 0; i < cap; i++)
        {
            bucket = &set->tableA[i];
            if (bucket->value != NULL)
            {
                next = bucket->next->next;
                void *value = bucket->value;
                bucket->value = next->value;
                free(bucket->next);
                bucket->next = next;
                _Set_add(set, value, true);
                set->indexA = i;
                return;
            }
        }
    }
    
    // check to see if table B has reached 75% cap.
    uint high_load = pow2(set->log2capB + 2) + pow2(set->log2capB + 1);
    if (set->sizeB < high_load)
        return;
    
    uint log2newtablesize = set->log2capB + 1; // grow by 2x
    if (set->tableA != NULL)
        free(set->tableA);
    set->tableA = set->tableB;
    set->log2capA = set->log2capB;
    set->sizeA = set->sizeB;
    set->indexA = 0;
    set->tableB = calloc(pow2(log2newtablesize), sizeof(SetBucket));
    set->log2capB = log2newtablesize;
    set->sizeB = 0;
}

bool Set_has(Set *set, void *value)
{
    assert(set != NULL);
    assert(value != NULL);
    uint hash = set->hash(value);
    uint indexB = hash & mask(set->log2capB);
    SetBucket *bucket = _Set_get_chain(set, set->tableB, indexB, value);
    while (bucket != NULL)
    {
        if (set->comp(bucket->value, value))
            return true;
        bucket = bucket->next;
    }
    uint indexA = hash & mask(set->log2capA);
    bucket = _Set_get_chain(set, set->tableA, indexA, value);
    while (bucket != NULL)
    {
        if (set->comp(bucket->value, value))
            return true;
        bucket = bucket->next;
    }
    return false;
}

static bool _Set_remove(Set *set, SetBucket *table, void *value, uint index)
{
    SetBucket *bucket = &table[index];
    if (set->comp(bucket->value, value))
    {
        SetBucket *next = bucket->next;
        if (bucket->next != NULL)
        {
            bucket->value = next->value;
            bucket->next = next->next;
            free(next);
        }
        else
            bucket->value = NULL;
        
        return true;
    }
    SetBucket *prev = bucket;
    for (bucket = bucket->next; bucket != NULL; bucket = bucket->next)
    {
        if (set->comp(bucket->value, value))
        {
            prev->next = bucket->next;
            value = bucket->value;
            free(bucket);
            return true;
        }
        prev = bucket;
    }
    return false;
}

void Set_remove(Set *set, void *value)
{
    assert(set != NULL);
    assert(value != NULL);
    uint hash = set->hash(value);
    uint indexB = hash & mask(set->log2capB);
    bool removed = _Set_remove(set, set->tableB, value, indexB);
    if (removed)
    {
        set->sizeB--;
        return;
    }
    uint indexA = hash & mask(set->log2capA);
    bool removed = _Set_remove(set, set->tableA, value, indexA);
    if (removed)
    {
        set->sizeA--;
        return;
    }
}

// Add to table B, requires that the value is not yet in table B
static void _Set_add(Set *set, void *value, bool recurrant)
{
    uint index = set->hash(value) & mask(set->log2capB);
    SetBucket *bucket = &set->tableB[index];
    SetBucket *new_bucket = malloc(sizeof(SetBucket));
    new_bucket->value = bucket->value;
    new_bucket->next = bucket->next;
    bucket->value = value;
    bucket->next = new_bucket;
    set->sizeB++;
    if (!recurrant)
        _Set_transfer(set); // also move an item from tableA to tableB
}

void Set_del(Set *set)
{
    uint i;
    SetBucket *bucket, *next;
    if (set->tableA != NULL)
    {
        for (i = 0; i < pow2(set->log2capA); i++)
        {
            bucket = &set->tableA[i];
            next = bucket->next;
            while (next != NULL)
            {
                free(next);
                next = next->next;
                bucket->next = next;
            }
        }
        free(set->tableA);
    }
    for (i = 0; i < pow2(set->log2capB); i++)
    {
        bucket = &set->tableB[i];
        next = bucket->next;
        while (next != NULL)
        {
            free(next);
            next = next->next;
            bucket->next = next;
        }
    }
    free(set->tableB);
    free(set);
}

/// This could actually be pretty bad, iterating through the set while removing
/// items... Maybe have an iterator method to remove the current item gracefully?
void Set_intersect_inplace(Set *set1, Set *set2)
{
    SetIterator iter = Set_iter(set1);
    SetBucket *bucket;
    while ((bucket = _Set_iter_next(&iter)) != NULL)
    {
        if (!Set_has(set2, bucket->value))
        {
            SetBucket *table = (iter.tableA)? set1->tableA : set1->tableB;
            _Set_remove(set1, table, bucket->value, iter.bucket_index);
        }
    }
}

void Set_union_inplace(Set *set1, Set *set2)
{
    SetIterator iter = Set_iter(set2);
    SetBucket *bucket = _Set_iter_next(&iter);
    for ( ; bucket != NULL; bucket = _Set_iter_next(&iter))
        _Set_add(set1, bucket->value, false);
}

void Set_difference_inplace(Set *set1, Set *set2)
{
    SetIterator iter = Set_iter(set1);
    SetBucket *bucket = _Set_iter_next(&iter);
    for ( ; bucket != NULL; bucket = _Set_iter_next(&iter))
    {
        if (Set_has(set2, bucket->value))
        {
            SetBucket *table = (iter.tableA)? set1->tableA : set1->tableB;
            _Set_remove(set1, table, bucket->value, iter.bucket_index);
        }
    }
}

void Set_symdifference_inplace(Set *set1, Set *set2)
{
    Set_difference_inplace(set1, set2);
    Set_union_inplace(set1, Set_difference(set2, set1));
}

Set *Set_intersection(Set *set1, Set *set2)
{
    Set *set = Set_copy(set1);
    Set_intersect_inplace(set, set2);
    return set;
}

Set *Set_union(Set *set1, Set *set2)
{
    Set *set = Set_copy(set1);
    Set_union_inplace(set, set2);
    return set;
}

Set *Set_difference(Set *set1, Set *set2)
{
    Set *set = Set_copy(set1);
    Set_difference_inplace(set, set2);
    return set;
}

Set *Set_symdifference(Set *set1, Set *set2)
{
    Set *set = Set_copy(set1);
    Set_symdifference_inplace(set, set2);
    return set;
}

Set *Set_copy(Set *set)
{
    Set *new = malloc(sizeof(Set));
    new->hash = set->hash;
    new->comp = set->comp;
    new->indexA = set->indexA;
    new->sizeA = set->sizeA;
    new->sizeB = set->sizeB;
    new->log2capA = set->log2capA;
    new->log2capB = set->log2capB;
    uint tableAsize = sizeof(SetBucket) * pow2(new->log2capA);
    uint tableBsize = sizeof(SetBucket) * pow2(new->log2capB)
    new->tableA = malloc(tableAsize);
    new->tableB = malloc(tableBsize);
    memcpy(new->tableA, set->tableA, tableAsize);
    memcpy(new->tableB, set->tableB, tableBsize);
    /// now we need to iterate through the tables and copy the chains
    todo
}

void Set_test()
{
    Set *set = Set_new();
    uint i;
    // Add all integers from 1 to 50
    for (i = 1; i <= 50; i++)
        Set_add(set, (void*)i);
    // Add all integers from 25 to 75
    for (i = 25; i <= 75; i++)
        Set_add(set, (void*)i);
    // Make sure there are exactly 75 integers in the set
    CU_ASSERT(set->sizeA + set->sizeB == 75);
}

////////////////////////////////////////////////////////////////////////////////
// StrBuilder
// Useful for building lengths of string
////////////////////////////////////////////////////////////////////////////////

uint _StrBuilder_nextsize(uint size)
{
    return size + (size >> 1);
}

StrBuilder *StrBuilder_new(char *initial)
{
    StrBuilder *sb = malloc(sizeof(StrBuilder));
    if (initial == NULL)
    {
        sb->size = 0;
        sb->cap = 8;
    }
    else
    {
        sb->size = strlen(initial);
        sb->cap = _StrBuilder_nextsize(sb->size);
    }
    sb->s = malloc(sizeof(char) * sb->cap);
    if (initial != NULL)
        memcpy(sb->s, initial, sb->size);
    return sb;
}

void StrBuilder_del(StrBuilder *sb)
{
    free(sb->s);
    free(sb);
}

void StrBuilder_appendN(StrBuilder *sb, char *s, uint len)
{
    uint size = sb->size;
    sb->size += len;
    if (sb->size > sb->cap)
    {
        sb->cap = _StrBuilder_nextsize(sb->size);
        sb->s = realloc(sb->s, sb->cap);
    }
    memcpy(sb->s + size, s, len);
}

void StrBuilder_append(StrBuilder *sb, char *s)
{
    uint len = strlen(s);
    StrBuilder_appendN(sb, s, len);
}

void StrBuilder_appendC(StrBuilder *sb, char c)
{
    StrBuilder_appendN(sb, &c, 1);
}

void StrBuilder_join(StrBuilder *sb1, StrBuilder *sb2)
{
    StrBuilder_appendN(sb1, sb2->s, sb2->size);
}

char *StrBuilder_tostring(StrBuilder *sb)
{
    StrBuilder_appendN(sb, "\0", 1);
    char *s = sb->s;
    free(sb);
    return s;
}

void StrBuilder_print(StrBuilder *sb)
{
    uint i;
    for (i = 0; i < sb->size; i++)
        printf("%c", sb->s[i]);
}

void StrBuilder_test()
{
    StrBuilder *sb = StrBuilder_new(NULL);
    StrBuilder_append(sb, "test1");
    StrBuilder_append(sb, "test2\0");
    StrBuilder_append(sb, "test3");
    CU_ASSERT(strcmp(StrBuilder_tostring(sb), "test1test2test3") == 0);
}

////////////////////////////////////////////////////////////////////////////////
// BitArray2D
// A static 2D array of bits with an additional position parameter for use in
// pixel-perfect collision detection. The representation is such that the most
// significant bit is on the left.
////////////////////////////////////////////////////////////////////////////////

// Determines the smallest number of bytes to contain the number of bits given.
#define bits2bytes(_nbits) (((_nbits) >> 3)+(((_nbits) & 0b111)?1:0))

typedef enum
{
    COPY,
    MOVE,
    AND,
    OR,
    XOR
} Bitop;

// Copy or otherwise perform a bitwise operation on a contiguous run of bits.
// The destination should have at least nbits_to_nbytes(nbits) bytes allocated.
static inline void bitop(uchar *dest, uint dest_bit_offset,
                         uchar *src, uint src_bit_offset,
                         uint nbits, Bitop operation)
{
    assert(dest != NULL);
    assert(src != NULL);
    if (dest_bit_offset >= 8)
    {
        dest += (dest_bit_offset >> 3) * sizeof(uchar);
        dest_bit_offset &= 0b111;
    }
    if (src_bit_offset >= 8)
    {
        src += (src_bit_offset >> 3) * sizeof(uchar);
        src_bit_offset &= 0b111;
    }
    uint nbytes = bits2bytes(nbits);
    uchar src_byte;
    uint i;
    
    void edge_case()
    {
        // Note, we've only moved a multiple of 8 number of bits. There may
        // very well be some remainder. So we want to do one more iteration but
        // incomplete. This gets tricky because we might want to read/write to
        // one or two bytes.
        
        uint nbits_extra = nbits & 0b111;
        uint i = nbytes - 1;
        
        // Read 8 bits, some of which won't be needed
        src_byte = (src[i] << src_bit_offset) |
                   (src[i + 1] >> (8 - src_bit_offset));
        // Now select only the extra bits needed
        src_byte &= ~mask(8 - nbits_extra);
        
        uint nbits_extra_src_left = min(nbits_extra, 8 - src_bit_offset);
        uint nbits_extra_src_right = 0;
        if (nbits_extra > nbits_extra_src_left)
            nbits_extra_src_right = nbits_extra - nbits_extra_src_left;
        uint nbits_extra_dest_left = min(nbits_extra, 8 - dest_bit_offset);
        uint nbits_extra_dest_right = 0;
        if (nbits_extra > nbits_extra_dest_left)
            nbits_extra_dest_right = nbits_extra - nbits_extra_dest_left;
        
        // We now also need new masks with the correct number of zeros
        uchar src_mask_left = ~(mask(nbits_extra_src_left) <<
                                (8 - nbits_extra_src_left - src_bit_offset));
        uchar src_mask_right = ~(mask(nbits_extra_src_right) <<
                                 (8 - nbits_extra_src_right));
        uchar dest_mask_left = ~(mask(nbits_extra_dest_left) <<
                                 (8 - nbits_extra_dest_left - dest_bit_offset));
        uchar dest_mask_right = ~(mask(nbits_extra_dest_right) <<
                                  (8 - nbits_extra_dest_right));
        
        switch (operation)
        {
            case MOVE:
            // Erase the source extra bits
            src[i] &= src_mask_left;
            src[i + 1] &= src_mask_right;
            // no break
            case COPY:
            // Erase the target extra bits
            dest[i] &= dest_mask_left;
            dest[i + 1] &= dest_mask_right;
            // no break
            case OR:
            dest[i] |= src_byte >> dest_bit_offset;
            dest[i + 1] |= src_byte << (8 - dest_bit_offset);
            break;
            case AND:
            dest[i] &= (src_byte >> dest_bit_offset) | dest_mask_left;
            dest[i + 1] &= (src_byte << (8 - dest_bit_offset)) | dest_mask_right;
            break;
            case XOR:
            dest[i] ^= src_byte >> dest_bit_offset;
            dest[i + 1] ^= src_byte << (8 - dest_bit_offset);
            break;
            default: assert(false); break;
        }
    }
    
    
    // Masks to clear the exact 8 bits over a byte boundary
    uchar dest_mask_right = mask(8 - dest_bit_offset);
    uchar dest_mask_left = ~dest_mask_right;
    uchar src_mask_right = mask(8 - src_bit_offset);
    uchar src_mask_left = ~src_mask_right;
    
    // We need to make sure if (dest < src) to move in descending rather than
    // ascending order to prevent overwrite in the case of overlap when copying.
    if (dest < src)
    {
        i = 0;
    }
    else
    {
        edge_case();
        i = nbytes - 2;
    }
    
    while (true)
    {
        src_byte = (src[i] << src_bit_offset) |
                   (src[i + 1] >> (8 - src_bit_offset));

        switch (operation)
        {
            case MOVE:
            // Erase the source 8 bits
            src[i] &= src_mask_left;
            src[i + 1] &= src_mask_right;
            // no break
            case COPY:
            // Erase the target 8 bits
            dest[i] &= dest_mask_left;
            dest[i + 1] &= dest_mask_right;
            // no break
            case OR:
            dest[i] |= src_byte >> dest_bit_offset;
            dest[i + 1] |= src_byte << (8 - dest_bit_offset);
            break;
            case AND:
            dest[i] &= (src_byte >> dest_bit_offset) | dest_mask_left;
            dest[i + 1] &= (src_byte << (8 - dest_bit_offset)) | dest_mask_right;
            break;
            case XOR:
            dest[i] ^= src_byte >> dest_bit_offset;
            dest[i + 1] ^= src_byte << (8 - dest_bit_offset);
            break;
            default: assert(false); break;
        }
        if (dest < src)
        {
            i++;
            if (i > nbytes - 2)
            {
                edge_case();
                break;
            }
        }
        else
        {
            if (i == 0)
                break;
            i--;
        }
    }
}

// Allocates the BitArray struct but not the array representation itself.
// Note: x and y are the shifted "positions" of the array relative to the
// origin. They affect how bitwise operations work.
static BitArray *_BitArray_new(uint x, uint y, uint width, uint height)
{
    BitArray *array = malloc(sizeof(BitArray));
    array->width = width;
    array->height = height;
    array->x = x;
    array->y = y;
    // note, array->bits not yet allocated in this function.
    return array;
}

BitArray *BitArray_newempty(uint x, uint y, uint width, uint height)
{
    BitArray *array = _BitArray_new(x, y, width, height);
    array->bits = calloc(bits2bytes(width) * height, sizeof(uchar));
    return array;
}

BitArray *BitArray_newcircle(uint xpos, uint ypos, uint radius)
{
    uint width = (radius << 1) + 1;
    BitArray *array = BitArray_newempty(xpos, ypos, width, width);
    
    // Using the Midpoint Circle Algorithm, creates a bit array with a circle
    // outline of 'on' bits.
    
    int x = (int)radius;
    int y = 0;
    int error = 1 - x;
    while (x >= y)
    {
        BitArray_setbit(array, radius + x, radius + y, true);
        BitArray_setbit(array, radius + y, radius + x, true);
        BitArray_setbit(array, radius - x, radius + y, true);
        BitArray_setbit(array, radius - y, radius + x, true);
        BitArray_setbit(array, radius - x, radius - y, true);
        BitArray_setbit(array, radius - y, radius - x, true);
        BitArray_setbit(array, radius + x, radius - y, true);
        BitArray_setbit(array, radius + y, radius - x, true);
        y++;
        if (error < 0)
        {
            error += (y << 1) + 1;
            continue;
        }
        x--;
        error += (y - x + 1) << 1;
    }
    return array;
}

BitArray *BitArray_newsquare(uint x, uint y, uint width, uint height)
{
    assert(width > 0);
    assert(height > 0);
    BitArray *array = _BitArray_new(x, y, width, height);
    uint bytewidth = bits2bytes(array->width);
    uchar *bits = malloc(bytewidth * array->height * sizeof(uchar));
    array->bits = bits;
    uint i, j;
    for (j = 0; j < array->height; j++)
        for (i = 0; i < bytewidth; i++)
            bits[i + j * bytewidth] |= 0b11111111;
    return array;
}

bool BitArray_getbit(BitArray *array, uint i, uint j)
{
    assert(array != NULL);
    assert(i < array->width);
    assert(j < array->height);
    uint bytewidth = bits2bytes(array->width);
    uint byte = j * bytewidth + (i >> 3);
    uint bit = 8 - (i & 0b111);
    return array->bits[byte] & pow2(bit);
}

void BitArray_setbit(BitArray *array, uint i, uint j, bool on)
{
    assert(array != NULL);
    assert(i < array->width);
    assert(j < array->height);
    uint byte = j * bits2bytes(array->width) + (i >> 3);
    uint bit = pow2(7 - (i & 0b111));
    if (on)
        array->bits[byte] |= bit;
    else
        array->bits[byte] &= (0b11111111 - bit);
}

void BitArray_setcolumn(BitArray *array, uint x, bool on)
{
    assert(array != NULL);
    assert(x < array->width);
    uint j;
    uint xbyte = x >> 3;
    uint xbit = pow2(x & 0b111);
    uint bytewidth = bits2bytes(array->width);
    if (on)
    {
        for (j = 0; j < array->height; j++)
            array->bits[j * bytewidth + xbyte] &= 0b11111111 - xbit;
    }
    else
    {
        for (j = 0; j < array->height; j++)
            array->bits[j * bytewidth + xbyte] |= xbit;
    }
}

void BitArray_setrow(BitArray *array, uint y, bool on)
{
    assert(array != NULL);
    assert(y < array->height);
    uint value = (on)?0b11111111:0;
    uint bytewidth = bits2bytes(array->width);
    memset(array->bits + y * bytewidth, value, bytewidth);
}

// In the -y direction, add n rows of zeros. This will also change the y
// position of the bitarray.
void BitArray_growup(BitArray *array, uint n)
{
    assert(array != NULL);
    uint bytewidth = bits2bytes(array->width);
    uint old_size = bytewidth * array->height;
    uint new_size = bytewidth * (array->height + n);
    uint additional_size = new_size - old_size;
    array->bits = realloc(array->bits, new_size);
    memmove(array->bits + additional_size, array->bits, old_size);
    memset(array->bits, 0, additional_size);
    array->height += n;
    array->y -= n;
}

// In the +y direction, add n rows of zeros.
void BitArray_growdown(BitArray *array, uint n)
{
    assert(array != NULL);
    uint bytewidth = bits2bytes(array->width);
    uint old_size = bytewidth * array->height;
    uint new_size = bytewidth * (array->height + n);
    array->bits = realloc(array->bits, new_size);
    memset(array->bits + old_size, 0, new_size - old_size);
    array->height += n;
}

// In the -x direction, add n columns of zeros. This will also change the x
// position of the bitarray.
void BitArray_growleft(BitArray *array, uint n)
{
    BitArray_growright(array, n);
    BitArray_shiftright(array, n);
    array->x -= n;
}

// In the +x direction, add n columns of zeros
void BitArray_growright(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(array->bits != NULL);
    uint bytewidth = bits2bytes(array->width);
    uint new_bytewidth = bits2bytes(array->width + n);
    uint new_size = new_bytewidth * array->height;
    uchar *newbits = calloc(new_size, sizeof(uchar));
    uchar *bits = array->bits;
    uint j;
    for (j = 0; j < array->height; j++)
        memcpy(newbits + j * new_bytewidth, bits + j * bytewidth, bytewidth);
    free(array->bits);
    array->bits = newbits;
    array->width += n;
}

// Shift in the -y direction without wrapping
void BitArray_shiftup(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(n <= array->height);
    uint bytewidth = bits2bytes(array->width);
    uint i, j;
    uchar *bits = array->bits;
    for (j = 0; j < array->height - n; j++)
        for (i = 0; i < bytewidth; i++)
            bits[j * bytewidth + i] = bits[(j + n) * bytewidth + i];
    for (; j < array->height; j++)
        for (i = 0; i < bytewidth; i++)
            bits[j * bytewidth + i] = 0;
}

// Shift in the +y direction without wrapping
void BitArray_shiftdown(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(n <= array->height);
    uint bytewidth = bits2bytes(array->width);
    uchar *bits = array->bits;
    uint i, j;
    for (j = 0; j < n; j++)
        for (i = 0; i < bytewidth; i++)
            bits[j * bytewidth + i] = 0;
    for (; j < array->height; j++)
        for (i = 0; i < bytewidth; i++)
            bits[j * bytewidth + i] = bits[(j - n) * bytewidth + i];
}

// Shift in the -x direction without wrapping
void BitArray_shiftleft(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(n < array->width);
    uint bytewidth = bits2bytes(array->width);
    uint j;
    for (j = 0; j < array->height; j++)
    {
        uchar *row = array->bits + j * bytewidth;
        bitop(row, 0, row, n, array->width - n, MOVE);
    }
}

// Shift in the +x direction without wrapping
void BitArray_shiftright(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(n < array->width);
    uint bytewidth = bits2bytes(array->width);
    uint j;
    for (j = 0; j < array->height; j++)
    {
        uchar *row = array->bits + j * bytewidth;
        bitop(row, n, row, 0, array->width - n, MOVE);
    }
}

// Remove first n rows. This will also change the y position of the array.
void BitArray_shrinkup(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(n < array->height);
    uint bytewidth = bits2bytes(array->width);
    uint old_size = bytewidth * array->height;
    uint new_size = bytewidth * (array->height - n);
    uint size_removed = old_size - new_size;
    uchar *old_bits = array->bits;
    uchar *new_bits = malloc(new_size * sizeof(uchar));
    memcpy(new_bits, old_bits + size_removed, new_size * sizeof(uchar));
    free(old_bits);
    array->bits = new_bits;
    array->height -= n;
    array->y += n;
}

// Remove last n rows
void BitArray_shrinkdown(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(n < array->height);
    uint bytewidth = bits2bytes(array->width);
    uint new_size = bytewidth * (array->height - n);
    array->bits = realloc(array->bits, new_size * sizeof(uchar));
    array->height -= n;
}

// Remove first n columns. This will also change the x position of the array.
void BitArray_shrinkleft(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(n < array->width);
    uint bytewidth = bits2bytes(array->width);
    uint new_bytewidth = bits2bytes(array->width - n);
    uint new_size = new_bytewidth * array->height;
    uchar *old_bits = array->bits;
    uchar *new_bits = malloc(new_size * sizeof(uchar));
    uint j;
    for (j = 0; j < array->height; j++)
    {
        uchar *old_row = old_bits + j * bytewidth;
        uchar *new_row = new_bits + j * new_bytewidth;
        bitop(new_row, 0, old_row, n, array->width - n, COPY);
    }
    free(old_bits);
    array->bits = new_bits;
    array->width -= n;
    array->x += n;
}

// Remove last n columns
void BitArray_shrinkright(BitArray *array, uint n)
{
    assert(array != NULL);
    assert(n < array->width);
    uint bytewidth = bits2bytes(array->width);
    uint new_bytewidth = bits2bytes(array->width - n);
    uint new_size = new_bytewidth * array->height;
    uchar *old_bits = array->bits;
    uchar *new_bits = malloc(new_size * sizeof(uchar));
    uint j;
    for (j = 0; j < array->height; j++)
    {
        uchar *old_row = old_bits + j * bytewidth;
        uchar *new_row = new_bits + j * new_bytewidth;
        bitop(new_row, 0, old_row, 0, array->width - n, COPY);
    }
    free(old_bits);
    array->bits = new_bits;
    array->width -= n;
}

bool BitArray_compare(BitArray *array1, BitArray *array2)
{
    if (array1->width != array2->width || array1->height != array2->height)
        return false;
    uchar *bits1 = array1->bits;
    uchar *bits2 = array2->bits;
    uint remainder = array1->width % 8;
    uint bytewidth = bits2bytes(array1->width);
    uchar remainder_mask = ~mask(8 - remainder);
    uint i, j;
    for (j = 0; j < array1->height; j++)
    {
        for (i = 0; i < bytewidth - 1; i++)
        {
            if (bits1[i + j * bytewidth] !=
                bits2[i + j * bytewidth])
            {
                return false;
            }
        }
        if ((bits1[i + j * bytewidth] & remainder_mask) !=
            (bits2[i + j * bytewidth] & remainder_mask))
        {
            return false;
        }
    }
    return true;
}

void BitArray_print(BitArray *array)
{
    void print_byte(uchar byte, uchar first_n_bits)
    {
        byte >>= (8 - first_n_bits);
        uchar buffer[9];
        uchar *b = buffer;
        int i = first_n_bits - 1;
        while (true)
        {
            if (byte & pow2(i))
                *b++ = '1';
            else
                *b++ = '.';
            if (!i--) break;
        }
        *b = '\0';
        printf("%s", buffer);
    }
    
    uint bytewidth = bits2bytes(array->width);
    uint remainder = array->width % 8;
    if (!remainder) remainder = 8;
    uint i, j;
    
    printf("\n--Start BitArray---- width: %lu height: %lu\n",
           array->width, array->height);
    for (j = 0; j < array->height; j++)
    {
        for (i = 0; i < (bytewidth - 1); i++)
            print_byte(array->bits[i + j * bytewidth], 8);
        print_byte(array->bits[i + j * bytewidth], remainder);
        printf("\n");
    }
    printf("--End BitArray------\n");
}

// Make a (deep) copy of the BitArray
BitArray *BitArray_copy(BitArray *array)
{
    uint byte_size = array->width * array->height;
    BitArray *new = _BitArray_new(array->x, array->y, array->width, array->height);
    new->bits = malloc(sizeof(uchar) * byte_size);
    memcpy(new->bits, array->bits, sizeof(uchar) * byte_size);
    return new;
}

// Returns a new array from the intersection of the given ones. When used for
// pixel-perfect collision detection, if any bit in the resulting array is 'on'
// then a collision has been detected.
BitArray *BitArray_and(BitArray *array1, BitArray *array2)
{
    // Find the overlapping region of the two arrays. 
    uint x1 = max(array1->x, array2->x);
    uint y1 = max(array1->y, array2->y);
    uint x2 = min(array1->x + array1->width, array2->x + array2->width);
    uint y2 = min(array1->y + array1->height, array2->y + array2->height);
    // If the following tests are true, the two arrays don't overlap at all.
    if (x2 < x1 || y2 < y1)
        return NULL;
    uint width = x2 - x1;
    uint height = y2 - y1;
    BitArray *new_array = BitArray_newempty(x1, y2, width, height);
    uint bytewidth1 = bits2bytes(array1->width);
    uint bytewidth2 = bits2bytes(array2->width);
    uint j;
    for (j = 0; j < new_array->height; j++)
    {
        bitop(new_array->bits, 0,
              array1->bits + bytewidth1 * (y1 - array1->y), x1 - array1->x,
              new_array->width,
              COPY);
        bitop(new_array->bits, 0,
              array2->bits + bytewidth2 * (y2 - array2->y), x2 - array2->x,
              new_array->width, AND);
    }
    return new_array;
}

void BitArray_del(BitArray *array)
{
    free(array->bits);
    free(array);
}

void BitArray_test()
{
    BitArray *barray = BitArray_newcircle(200, 200, 17);
    BitArray *copy = BitArray_copy(barray);
    CU_ASSERT(BitArray_compare(barray, copy));
    BitArray_growleft(barray, 11);
    BitArray_growup(barray, 12);
    BitArray_shiftleft(barray, 11);
    BitArray_shiftup(barray, 12);
    BitArray_shrinkright(barray, 11);
    BitArray_shrinkdown(barray, 12);
    CU_ASSERT(BitArray_compare(barray, copy));
    BitArray_del(barray);
    BitArray_del(copy);
}


/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
   return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
   return 0;
}

int main()
{
    CU_pSuite pSuite = NULL;
    
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test of LinkedList", LinkedList_test)) ||
        (NULL == CU_add_test(pSuite, "test of ArrayList", ArrayList_test)) ||
        (NULL == CU_add_test(pSuite, "test of Map", Map_test)) ||
        (NULL == CU_add_test(pSuite, "test of ChainedMap", ChainedMap_test)) ||
        (NULL == CU_add_test(pSuite, "test of StrBuilder", StrBuilder_test)) ||
        (NULL == CU_add_test(pSuite, "test of BitArray", BitArray_test)) ||
        (NULL == CU_add_test(pSuite, "test of MultiMap", MultiMap_test)) ||
        (NULL == CU_add_test(pSuite, "test of Set", Set_test)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
