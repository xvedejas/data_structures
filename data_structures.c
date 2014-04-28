#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "CUnit/Basic.h"
#include "data_structures.h"

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
        return NULL;
    void *value = elem->value;
    free(elem);
    return value;
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
    LinkedListElement *next = elem->next;
    do
    {
        free(elem);
        elem = next;
        next = elem->next;
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

ArrayList *_ArrayList_new(int cap)
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
// HashTable
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

void _HashTable_add(HashTable *hashtable, void *key, void *value, bool recurrant);
void _HashTable_transfer(HashTable *hashtable);
HashTable *_HashTable_new(int tablesize, uint (*hash)(void*), bool (*comp)(void*,void*));
HashTableBucket *_HashTable_get(HashTable *hashtable, void *key);

// add an item to tableB only
void _HashTable_add(HashTable *hashtable, void *key, void *value, bool recurrant)
{
    assert(key != NULL);
    uint hash = hashtable->hash(key) % hashtable->capB;
    HashTableBucket *bucket = &hashtable->tableB[hash];
    while (bucket->key != NULL)
    {
        hash++;
        hash %= hashtable->capB;
        bucket = &hashtable->tableB[hash];
    }
    bucket->key = key;
    bucket->value = value;
    hashtable->sizeB++;
    // also move an item from tableA to tableB
    if (!recurrant)
        _HashTable_transfer(hashtable);
}

// transfer any item from tableA to tableB
void _HashTable_transfer(HashTable *hashtable)
{
    if (hashtable->tableA != NULL)
    {
        while (hashtable->indexA < hashtable->capA)
        {
            HashTableBucket *bucket = &hashtable->tableA[hashtable->indexA];
            if (bucket->key != NULL)
            {
                _HashTable_add(hashtable, bucket->key, bucket->value, true);
                bucket->key = NULL;
                hashtable->sizeA--;
                return;
            }
            hashtable->indexA++;
        }
        // We have emptied tableA
        assert(hashtable->sizeA == 0);
        free(hashtable->tableA);
        hashtable->tableA = NULL;
    }
    
    // check to see if table B has reached 75% cap.
    uint high_load = (hashtable->capB >> 2) + (hashtable->capB >> 1);
    if (hashtable->sizeB < high_load)
        return;
    
    uint newtablesize = (hashtable->capB << 1); // grow by 2x
    hashtable->tableA = hashtable->tableB;
    hashtable->capA = hashtable->capB;
    hashtable->sizeA = hashtable->sizeB;
    hashtable->indexA = 0;
    hashtable->tableB = calloc(newtablesize, sizeof(HashTableBucket));
    hashtable->capB = newtablesize;
    hashtable->sizeB = 0;
}

HashTable *_HashTable_new(int tablesize, uint (*hash)(void*), bool (*comp)(void*,void*))
{
    HashTable *hashtable = malloc(sizeof(HashTable));
    hashtable->hash = hash;
    hashtable->comp = comp;
    hashtable->indexA = 0;
    hashtable->sizeA = 0;
    hashtable->sizeB = 0;
    hashtable->capA = 0;
    hashtable->capB = tablesize;
    hashtable->tableA = NULL;
    hashtable->tableB = calloc(tablesize, sizeof(HashTableBucket));
    return hashtable;
}

HashTable *HashTable_new(uint (*hash)(void*), bool (*comp)(void*,void*))
{
    return _HashTable_new(8, hash, comp);
}

HashTableBucket *_HashTable_get(HashTable *hashtable, void *key)
{
    assert(key != NULL);
    uint hash = hashtable->hash(key);
    uint hashB = hash % hashtable->capB;
    uint i = hashB;
    HashTableBucket *bucket = &hashtable->tableB[i];
    do
    {
        if (bucket->key != NULL)
        {
            if (hashtable->comp(bucket->key, key))
                return bucket;
        }
        i++;
        i %= hashtable->capB;
        bucket = &hashtable->tableB[i];
    } while (i != hashB);
    
    if (hashtable->tableA == NULL)
        return NULL;
    uint hashA = hash % hashtable->capA;
    i = hashA;
    bucket = &hashtable->tableA[i];
    do
    {
        if (bucket->key != NULL)
        {
            if (hashtable->comp(bucket->key, key))
                return bucket;
        }
        i++;
        i %= hashtable->capA;
        bucket = &hashtable->tableA[i];
    } while (i != hashA);
    return NULL;
}

bool HashTable_has(HashTable *hashtable, void *key)
{
    HashTableBucket *bucket = _HashTable_get(hashtable, key);
    if (bucket == NULL)
        return false;
    return true;
}

void *HashTable_get(HashTable *hashtable, void *key)
{
    HashTableBucket *bucket = _HashTable_get(hashtable, key);
    if (bucket == NULL)
        return NULL;
    return bucket->value;
}

void *HashTable_remove(HashTable *hashtable, void *key)
{
    HashTableBucket *bucket = _HashTable_get(hashtable, key);
    if (bucket == NULL)
        return NULL;
    void *value = bucket->value;
    bucket->key = NULL;
    // check which table the bucket was in:
    if (bucket >= hashtable->tableA &&
        bucket < hashtable->tableA + hashtable->capA)
        hashtable->sizeA--;
    else
        hashtable->sizeB--;
    return value;
}

void *HashTable_set(HashTable *hashtable, void *key, void *value)
{
    HashTableBucket *bucket = _HashTable_get(hashtable, key);
    void *oldvalue = NULL;
    
    
    if (bucket == NULL)
    {
        _HashTable_add(hashtable, key, value, false);
    }
    else
    {
        oldvalue = bucket->value;
        bucket->value = value;
    }
    return oldvalue;
}

void HashTable_del(HashTable *hashtable)
{
    if (hashtable->tableA != NULL)
        free(hashtable->tableA);
    free(hashtable->tableB);
    free(hashtable);
}

void HashTable_test()
{
    HashTable *hashtable = HashTable_new(stringhash, stringcomp);
    HashTable_set(hashtable, "test0", "a");
    HashTable_set(hashtable, "test1", "b");
    HashTable_set(hashtable, "test2", "c");
    HashTable_set(hashtable, "test3", "d");
    HashTable_set(hashtable, "test0", "e");
    CU_ASSERT(strcmp(HashTable_get(hashtable, "test0"), "e") == 0);
    CU_ASSERT(strcmp(HashTable_get(hashtable, "test1"), "b") == 0);
    CU_ASSERT(strcmp(HashTable_get(hashtable, "test2"), "c") == 0);
    CU_ASSERT(strcmp(HashTable_get(hashtable, "test3"), "d") == 0);
    HashTable_remove(hashtable, "test2");
    CU_ASSERT(strcmp(HashTable_get(hashtable, "test0"), "e") == 0);
    CU_ASSERT(strcmp(HashTable_get(hashtable, "test1"), "b") == 0);
    CU_ASSERT(HashTable_get(hashtable, "test2") == NULL);
    CU_ASSERT(strcmp(HashTable_get(hashtable, "test3"), "d") == 0);
    HashTable_del(hashtable);
}

////////////////////////////////////////////////////////////////////////////////
// Map
// An incrementally resizing hashtable map with chaining
////////////////////////////////////////////////////////////////////////////////

void _Map_add(Map *map, void *key, void *value, bool recurrant);

Map *_Map_new(int tablesize, uint (*hash)(void*), bool (*comp)(void*,void*))
{
    Map *map = malloc(sizeof(Map));
    map->hash = hash;
    map->comp = comp;
    map->indexA = 0;
    map->sizeA = 0;
    map->sizeB = 0;
    map->capA = 0;
    map->capB = tablesize;
    map->tableA = NULL;
    map->tableB = calloc(tablesize, sizeof(MapBucket));
    return map;
}

Map *Map_new(uint (*hash)(void*), bool (*comp)(void*,void*))
{
    return _Map_new(8, hash, comp);
}

MapBucket *_Map_get(Map *map, MapBucket *maptable, uint hash, void *key)
{
    MapBucket *bucket = &maptable[hash];
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
void _Map_transfer(Map *map)
{
    uint i = map->indexA;
    MapBucket *bucket, *next;
    for (i = 0; i < map->capA; i++)
    {
        bucket = &map->tableA[i];
        if (bucket->key != NULL)
        {
            next = bucket->next;
            void *key = bucket->key;
            void *value = bucket->value;
            bucket->key = next->key;
            bucket->value = next->value;
            bucket->next = next;
            _Map_add(map, key, value, true);
            map->indexA = i;
            return;
        }
    }
    
    // check to see if table B has reached 75% cap.
    uint high_load = (map->capB >> 2) + (map->capB >> 1);
    if (map->sizeB < high_load)
        return;
    
    uint newtablesize = (map->capB << 1); // grow by 2x
    map->tableA = map->tableB;
    map->capA = map->capB;
    map->sizeA = map->sizeB;
    map->indexA = 0;
    map->tableB = calloc(newtablesize, sizeof(MapBucket));
    map->capB = newtablesize;
    map->sizeB = 0;
}

void *Map_get(Map *map, void *key)
{
    assert(key != NULL);
    uint hash = map->hash(key);
    uint hashB = hash % map->capB;
    MapBucket *bucket = _Map_get(map, map->tableB, hashB, key);
    if (bucket != NULL)
        return bucket->value;
    if (map->tableA == NULL)
        return NULL;
    uint hashA = hash % map->capA;
    bucket = _Map_get(map, map->tableA, hashA, key);
    if (bucket == NULL)
        return NULL;
    return bucket->value;
}

bool Map_has(Map *map, void *key)
{
    assert(key != NULL);
    uint hash = map->hash(key);
    uint hashB = hash % map->capB;
    MapBucket *bucket = _Map_get(map, map->tableB, hashB, key);
    if (bucket != NULL)
        return true;
    uint hashA = hash % map->capA;
    bucket = _Map_get(map, map->tableA, hashA, key);
    return bucket != NULL;
}

void *Map_remove(Map *map, void *key)
{
    uint hash = map->hash(key);
    uint hashB = hash % map->capB;
    MapBucket *bucket = &map->tableB[hashB];
    void *value = NULL;
    if (map->comp(bucket->key, key))
    {
        value = bucket->value;
        MapBucket *next = bucket->next;
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
    MapBucket *prev = bucket;
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
    uint hashA = hash % map->capA;
    bucket = &map->tableA[hashA];
    if (map->comp(bucket->key, key))
    {
        value = bucket->value;
        MapBucket *next = bucket->next;
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
void _Map_add(Map *map, void *key, void *value, bool recurrant)
{
    uint hash = map->hash(key) % map->capB;
    MapBucket *bucket = &map->tableB[hash];
    MapBucket *new_bucket = malloc(sizeof(MapBucket));
    new_bucket->key = bucket->key;
    new_bucket->value = bucket->value;
    new_bucket->next = bucket->next;
    bucket->key = key;
    bucket->value = value;
    bucket->next = new_bucket;
    map->sizeB++;
    if (!recurrant)
        _Map_transfer(map); // also move an item from tableA to tableB
}

void *Map_set(Map *map, void *key, void *value)
{
    assert(map != NULL);
    assert(key != NULL);
    uint hash = map->hash(key);
    uint hashB = hash % map->capB;
    MapBucket *bucketB = _Map_get(map, map->tableB, hashB, key);
    void *old_value;
    if (bucketB != NULL)
    {
        old_value = bucketB->value;
        bucketB->value = value;
        return old_value;
    }
    if (map->tableA != NULL)
    {
        uint hashA = hash % map->capA;
        MapBucket *bucketA = _Map_get(map, map->tableA, hashA, key);
        if (bucketA != NULL)
        {
            old_value = bucketB->value;
            bucketA->value = value;
            return old_value;
        }
    }
    // We didn't find an existing such key, so add it
    _Map_add(map, key, value, false);
    return NULL;
}

void Map_del(Map *map)
{
    uint i;
    MapBucket *bucket, *next;
    for (i = 0; i < map->capA; i++)
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
    for (i = 0; i < map->capB; i++)
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
// MultiMap
// todo
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Set
// todo
////////////////////////////////////////////////////////////////////////////////

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
void bitop(uchar *dest, uint dest_bit_offset,
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
        src_byte &= 0b11111111 - ((1 << (8 - nbits_extra)) - 1);
        
        uint nbits_extra_src_left = min(nbits_extra, 8 - src_bit_offset);
        uint nbits_extra_src_right = 0;
        if (nbits_extra > nbits_extra_src_left)
            nbits_extra_src_right = nbits_extra - nbits_extra_src_left;
        uint nbits_extra_dest_left = min(nbits_extra, 8 - dest_bit_offset);
        uint nbits_extra_dest_right = 0;
        if (nbits_extra > nbits_extra_dest_left)
            nbits_extra_dest_right = nbits_extra - nbits_extra_dest_left;
        
        // We now also need new masks with the correct number of zeros
        uchar src_mask_left = 0b11111111 - (((1 << nbits_extra_src_left) - 1) <<
                                      (8 - nbits_extra_src_left - src_bit_offset));
        uchar src_mask_right = 0b11111111 - (((1 << nbits_extra_src_right) - 1) <<
                                       (8 - nbits_extra_src_right));
        uchar dest_mask_left = 0b11111111 - (((1 << nbits_extra_dest_left) - 1) <<
                                      (8 - nbits_extra_dest_left - dest_bit_offset));
        uchar dest_mask_right = 0b11111111 - (((1 << nbits_extra_dest_right) - 1) <<
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
    uchar dest_mask_right = (1 << (8 - dest_bit_offset)) - 1;
    uchar dest_mask_left = 0b11111111 - dest_mask_right;
    uchar src_mask_right = (1 << (8 - src_bit_offset)) - 1;
    uchar src_mask_left = 0b11111111 - src_mask_right;
    
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
BitArray *_BitArray_new(uint x, uint y, uint width, uint height)
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
    return array->bits[byte] & (1 << bit);
}

void BitArray_setbit(BitArray *array, uint i, uint j, bool on)
{
    assert(array != NULL);
    assert(i < array->width);
    assert(j < array->height);
    uint byte = j * bits2bytes(array->width) + (i >> 3);
    uint bit = 1 << (7 - (i & 0b111));
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
    uint xbit = 1 << (x & 0b111);
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
    {
        printf("A\n");
        return false;
    }
    uchar *bits1 = array1->bits;
    uchar *bits2 = array2->bits;
    uint remainder = array1->width % 8;
    uint bytewidth = bits2bytes(array1->width);
    uchar remainder_mask = 0b11111111 - ((1 << (8 - remainder)) - 1);
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
            if (byte & (1 << i))
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
        (NULL == CU_add_test(pSuite, "test of HashTable", HashTable_test)) ||
        (NULL == CU_add_test(pSuite, "test of Map", Map_test)) ||
        (NULL == CU_add_test(pSuite, "test of StrBuilder", StrBuilder_test)) ||
        (NULL == CU_add_test(pSuite, "test of BitArray", BitArray_test)))
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
