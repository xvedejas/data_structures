#define swap(_a, _b) {__typeof__(_a) _c = _a; _a = _b; _b = _c;}

typedef unsigned long uint;

typedef enum
{
    false, true
} bool;

//
// LinkedList
// A circular doubly-linked list
//

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

//
// ArrayList
// A dynamically growing/shrinking array list
//

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

//
// Dict
// An incrementally resizing dictionary map with open addressing
//

typedef struct bucket
{
    void *key, *value;
} Bucket;

typedef struct
{
    uint (*hash)(void*); // algorithm used to hash keys
    bool (*comp)(void*, void*); // algorithm used to compare keys
    uint indexA;       // keeps track of how far we are in moving items from tableA
    uint sizeA, sizeB; // number of buckets occupied
    uint capA, capB;   // capacity: actual size of tables
    Bucket *tableA, *tableB; // "old" table and "new" table
} Dict;

uint stringhash(void *stringptr);
uint ptrhash(void *ptr);
bool stringcomp(void *str1, void*str2);
bool ptrcomp(void *ptr1, void *ptr2);

Dict *Dict_new(uint (*hash)(void*), bool (*comp)(void*,void*));
bool Dict_has(Dict *dict, void *key);
void *Dict_get(Dict *dict, void *key);
void *Dict_remove(Dict *dict, void *key);
void *Dict_set(Dict *dict, void *key, void *value);
void Dict_del(Dict *dict);

//
// StrBuilder
// Useful for building lengths of string
//

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
