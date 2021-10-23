// Timothy Stephenson June 14th 2021

// Interface / Specifications for binary Tree

// all keys must be reprsented by a void pointer, with inputed compare function upon initialization of tree
// all values must be void pointers 

// max number of elements in a given TREE is the max value of a unsigned int (4,294,967,295)


struct TREE;

typedef struct TREE TREE;




// returns a empty Tree
TREE* TREE_Empty( int (*cmp) (void*, void*) , void (*free_key) (void*) , void (*free_val) (void*) );

// Free's a given Tree
void TREE_Free(TREE*);

// adds a given:  void *key, void *val   pair,
// if the key already exists in the tree, the inputed val replaces one previously stored (frees the previously stored val)
// uses the callers key in the TREE (DO NOT FREE IT AFTER CALLING)
void TREE_Add(TREE* , void * , void *);

// returns the void pointer associated with the given void* key in the TREE if it exists, returns NULL if it doesn't
// does not free in inputted void pointer key
void* TREE_Search(TREE* , void*);

// removes (and frees) the key val pair associated with the inputted   void *key   
// does not free the callers  void *key
void TREE_Remove(TREE* , void* );

// returns the number of elements in the given TREE
unsigned TREE_Size(TREE*);

// returns a sorted list of the keys in the given TREE
void** TREE_KeyList(TREE*);

// gives the average of the depths of all the nodes
// the root of a tree has a depth of 1, the roots child a depth of 2, the roots grandchild a depth of 3, etc etc
double TREE_AverageDepth(TREE*);

// returns the hieght of the 'root/head' of the tree
unsigned TREE_Height(TREE*);

// returns the key of the nth member in sorted order of the TREE, returns NULL if invalid n
//returns the actual key currently being used by the TREE
void *TREE_nth(TREE * , unsigned);

//returns the number of elements that come before (BUT NOT AT) the inputed key
 // the given key does not have to exist in the TREE and is not freed or mutated during runtime
// ie: if the inputted key is in the TREE, it is returning the index
//     if the inputted key is NOT in the TREE, returns numbers of elements that come before 
unsigned TREE_Position( TREE * , void * );


