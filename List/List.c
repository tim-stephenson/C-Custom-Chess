/*

    Timothy Stephenson


    Simple python-like list object class
    only supports lists of the same type though
*/




#include <stdlib.h>

#include <assert.h>

#include "List.h"



void array_copy(void **dest , void **src, unsigned len){  for(unsigned i = 0; i< len; i +=1) dest[i] = src[i] ; }

struct LIST
{  
   unsigned size;
   unsigned array_size;
   void **array;
   void (*free_func) (void*);
};

LIST *LIST_Empty( void (*free_func) (void*) ){
    LIST *l = (LIST*) malloc( sizeof(LIST) );
    l->size = 0;
    l->array_size = 1;
    l->array = (void**) malloc( 1 * sizeof(void*) );
    l->free_func = free_func;
    return l;
}

void list_changeArraySize(LIST *l, unsigned new_array_size){
    assert(new_array_size >= l->size);
    void **new_array = (void**) realloc(l->array , new_array_size * sizeof(void*) );
    if(new_array == NULL) {
        new_array = (void**) malloc( new_array_size * sizeof(void*) );
        array_copy(new_array , l->array , l->size);
        free(l->array);
    }
    l->array = new_array;
    l->array_size = new_array_size;
    return;
}

void LIST_Free(LIST *l){
    for(unsigned i = 0 ; i< l->size ; i += 1 ) (*l->free_func)(l->array[i]);
    free(l->array);
    free(l);
    return;
}

void LIST_Append(LIST *l, void *x){
    if(l->size >= l->array_size) list_changeArraySize(l , l->array_size * 2);
    l->array[l->size] = x;
    l->size += 1;
    return;
}   

//Delete's an element from the end of the list
void LIST_Delete(LIST *l){
    if(l->size==0) return;
    // ultization percentage < 25 %
    if( l->size < l->array_size/4 ) list_changeArraySize(l , l->array_size/2); 
    l->size += -1;
    return;
}

void* LIST_nth(LIST *l , unsigned i ){
    if(i < l->size) return l->array[i];
    return NULL;
}

unsigned LIST_Len(LIST *l){return l->size;}

LIST * LIST_Copy(LIST *l){
    LIST *new_l = (LIST*) malloc( sizeof(LIST) );
    new_l->size = l->size;
    new_l->array_size = l->array_size;
    new_l->array =  (void**) malloc( l->array_size * sizeof(void*) );
    array_copy(new_l->array , l->array , l->size);
    new_l->free_func = l->free_func;
    return new_l;
}


void LIST_Concat(LIST *dest, LIST *src){
    if(src->size ==0) return;
    //finding number of elements in new array, and prefered array size
    unsigned new_size, new_array_size;
    new_size = dest->size + src->size;

    if(dest->array_size >= src->array_size) new_array_size = dest->array_size;
    else new_array_size = src->array_size;
    if(new_size > new_array_size) new_array_size *=2;

    if(dest->array_size != new_array_size) list_changeArraySize(dest , new_array_size);
    array_copy(dest->array + dest->size , src->array , src->size);
    dest->size = new_size;
    return;
}

