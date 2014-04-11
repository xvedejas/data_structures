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
    LinkedList *list = LinkedList_new();
    uint i;
    for (i = 0; i < size; i++)
        LinkedList_add(list, array[i]);
    return list;
}

void LinkedList_reverse(LinkedList *list)
{
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
    LinkedListElement *elem = _LinkedList_index(list, index);
    if (elem == NULL)
        return NULL;
    return elem->value;
}

bool LinkedList_insert(LinkedList *list, int index, void *value)
{
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
// Dict
// An incrementally resizing dictionary map with open addressing
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

bool stringcomp(void *str1, void*str2)
{
    if (strcmp(str1, str2) == 0)
        return true;
    return false;
}

bool ptrcomp(void *ptr1, void *ptr2)
{
    return ptr1 == ptr2;
}

void _Dict_add(Dict *dict, void *key, void *value, bool recurrant);
void _Dict_transfer(Dict *dict);
Dict *_Dict_new(int tablesize, uint (*hash)(void*), bool (*comp)(void*,void*));
Bucket *_Dict_get(Dict *dict, void *key);

// add an item to tableB only
void _Dict_add(Dict *dict, void *key, void *value, bool recurrant)
{
    assert(key != NULL);
    uint hash = dict->hash(key) % dict->capB;
    Bucket *bucket = &dict->tableB[hash];
    while (bucket->key != NULL)
    {
        hash++;
        hash %= dict->capB;
        bucket = &dict->tableB[hash];
    }
    bucket->key = key;
    bucket->value = value;
    dict->sizeB++;
    if (!recurrant)
        _Dict_transfer(dict); // also move an item from tableA to tableB
}

// transfer any item from tableA to tableB
void _Dict_transfer(Dict *dict)
{
    if (dict->tableA != NULL)
    {
        while (dict->indexA < dict->capA)
        {
            Bucket *bucket = &dict->tableA[dict->indexA];
            if (bucket->key != NULL)
            {
                _Dict_add(dict, bucket->key, bucket->value, true);
                bucket->key = NULL;
                dict->sizeA--;
                return;
            }
            dict->indexA++;
        }
        // We have emptied tableA
        assert(dict->sizeA == 0);
        free(dict->tableA);
        dict->tableA = NULL;
    }
    
    // check to see if table B has reached 75% cap.
    uint high_load = (dict->capB >> 2) + (dict->capB >> 1);
    if (dict->sizeB < high_load)
        return;
    
    uint newtablesize = (dict->capB << 1); // grow by 2x
    dict->tableA = dict->tableB;
    dict->capA = dict->capB;
    dict->sizeA = dict->sizeB;
    dict->indexA = 0;
    dict->tableB = calloc(newtablesize, sizeof(Bucket));
    dict->capB = newtablesize;
    dict->sizeB = 0;
}

Dict *_Dict_new(int tablesize, uint (*hash)(void*), bool (*comp)(void*,void*))
{
    Dict *dict = malloc(sizeof(Dict));
    dict->hash = hash;
    dict->comp = comp;
    dict->indexA = 0;
    dict->sizeA = 0;
    dict->sizeB = 0;
    dict->capA = 0;
    dict->capB = tablesize;
    dict->tableA = NULL;
    dict->tableB = calloc(tablesize, sizeof(Bucket));
    return dict;
}

Dict *Dict_new(uint (*hash)(void*), bool (*comp)(void*,void*))
{
    return _Dict_new(8, hash, comp);
}

Bucket *_Dict_get(Dict *dict, void *key)
{
    assert(key != NULL);
    uint hash = dict->hash(key);
    uint hashB = hash % dict->capB;
    uint i = hashB;
    Bucket *bucket = &dict->tableB[i];
    do
    {
        if (bucket->key != NULL)
        {
            if (dict->comp(bucket->key, key))
                return bucket;
        }
        i++;
        i %= dict->capB;
        bucket = &dict->tableB[i];
    } while (i != hashB);
    if (dict->tableA == NULL)
        return NULL;
    uint hashA = hash % dict->capA;
    i = hashA;
    bucket = &dict->tableA[i];
    do
    {
        if (bucket->key != NULL)
        {
            if (dict->comp(bucket->key, key))
                return bucket;
        }
        i++;
        i %= dict->capA;
        bucket = &dict->tableA[i];
    } while (i != hashA);
    return NULL;
}

bool Dict_has(Dict *dict, void *key)
{
    Bucket *bucket = _Dict_get(dict, key);
    if (bucket == NULL)
        return false;
    return true;
}

void *Dict_get(Dict *dict, void *key)
{
    Bucket *bucket = _Dict_get(dict, key);
    if (bucket == NULL)
        return NULL;
    return bucket->value;
}

void *Dict_remove(Dict *dict, void *key)
{
    Bucket *bucket = _Dict_get(dict, key);
    if (bucket == NULL)
        return NULL;
    void *value = value;
    bucket->key = NULL;
    // check which table the bucket was in:
    if (bucket >= dict->tableA && bucket < dict->tableA + dict->capA)
        dict->sizeA--;
    else
        dict->sizeB--;
    return value;
}

void *Dict_set(Dict *dict, void *key, void *value)
{
    Bucket *bucket = _Dict_get(dict, key);
    void *oldvalue = NULL;
    if (bucket == NULL)
    {
        _Dict_add(dict, key, value, false);
    }
    else
    {
        oldvalue = bucket->value;
        bucket->value = value;
    }
    return oldvalue;
}

void Dict_del(Dict *dict)
{
    if (dict->tableA != NULL)
        free(dict->tableA);
    free(dict->tableB);
    free(dict);
}

void Dict_test()
{
    Dict *dict = Dict_new(stringhash, stringcomp);
    Dict_set(dict, "test0", "a");
    Dict_set(dict, "test1", "b");
    Dict_set(dict, "test2", "c");
    Dict_set(dict, "test3", "d");
    Dict_set(dict, "test0", "e");
    CU_ASSERT(strcmp(Dict_get(dict, "test0"), "e") == 0);
    CU_ASSERT(strcmp(Dict_get(dict, "test1"), "b") == 0);
    CU_ASSERT(strcmp(Dict_get(dict, "test2"), "c") == 0);
    CU_ASSERT(strcmp(Dict_get(dict, "test3"), "d") == 0);
    Dict_remove(dict, "test2");
    CU_ASSERT(strcmp(Dict_get(dict, "test0"), "e") == 0);
    CU_ASSERT(strcmp(Dict_get(dict, "test1"), "b") == 0);
    CU_ASSERT(Dict_get(dict, "test2") == NULL);
    CU_ASSERT(strcmp(Dict_get(dict, "test3"), "d") == 0);
    Dict_del(dict);
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
        (NULL == CU_add_test(pSuite, "test of Dict", Dict_test)) ||
        (NULL == CU_add_test(pSuite, "test of StrBuilder", StrBuilder_test)))
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
