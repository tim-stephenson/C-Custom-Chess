/*

    Timothy Stephenson


    Simple python-like list object class
    only supports lists of the same type though
*/


struct LIST;

typedef struct LIST LIST;



LIST *LIST_Empty( void (*free_func) (void*) );

void LIST_Free(LIST *);

void LIST_Append(LIST *, void *);

//Delete's an element from the end of the list
void LIST_Delete(LIST *);

void* LIST_nth(LIST * , unsigned );

unsigned LIST_Len(LIST *);

LIST * LIST_Copy(LIST *);

// add's the items in the second list onto the end of the first list, does not mutate the second list
void LIST_Concat(LIST *, LIST *);