// Timothy Stephenson June 14th 2021

// Implementation for binary Tree

// all keys must be reprsented by a void pointer, with inputed compare function upon initialization of tree
// all values must be void pointers 

// max number of elements in a given TREE is the max value of a unsigned int (4,294,967,295)


//Will use AVL Rebalancing to keep the Binary Search Tree balanced
// the term height refers to the the longest path from a given node to a empty node
// ie a "NULL" node has a hieght of 0, a leaf node has a height of 1, and the height of any given node is
// the max height of its two children + 1 

#include <stdlib.h>

#include <assert.h>

#include <stdbool.h>

#include "Tree.h"


typedef struct node node;
struct node{
    unsigned int size;
    unsigned short int height;
    void *key;
    void *val;
    node *l;
    node *r;
};
 
void node_Free(node *n , void (*free_key) (void*) , void (*free_val) (void*) ) {
    if(n!=NULL){
        node_Free(n->l , free_key , free_val);
        node_Free(n->r , free_key , free_val);
        (*free_key)(n->key);
        (*free_val)(n->val);
        free(n);
    }
    return;
}

node *node_Leaf(void *k , void *v ){
    node *n = (node*) malloc( sizeof(node) );
    n->size = 1;
    n->height = 1;
    n->key = k;
    n->val = v;
    n->l = NULL;
    n->r = NULL;
    return n;
}

unsigned int node_Size(node *n){
    if (n==NULL) return 0;
    return n->size;
}

unsigned short int node_Height(node *n){
    if(n==NULL) return 0;
    return n->height;
}

// assuming childrens nodes sizes are correct, fix the size of node *n
void node_FixSize(node *n){
    n->size =  1 + node_Size(n->l) + node_Size(n->r);
    return;
}

//assuming childrens heights are correct, fix the height of node *n 
void node_FixHeight(node *n){
    unsigned short int left_height = node_Height(n->l) , right_height = node_Height(n->r) ;

    // left child's node has a greater height:
    if(left_height > right_height) n->height = left_height + 1;

    // right child's node has a greater or equal height:
    else n->height = right_height + 1;

    return;
}

//postive for left-heavy , negative for right-heavy
short int node_HeightDiffernce(node *n){
    assert(n!=NULL);
    return node_Height(n->l) - node_Height(n->r);
}


void node_FillKeys(node *n, void **key_list){
    if(n==NULL) return;
    // element itself
    unsigned items_leftward = node_Size(n->l);

    key_list[items_leftward] = n->key;

    // fill left
    node_FillKeys(n->l,key_list);

    // fill right
    node_FillKeys(n->r, key_list + (items_leftward+1) );

    return;
}


void *node_Search(node *n, void *k, int (*cmp) (void*,void*) ){
    if(n==NULL) return NULL;
    int i = (*cmp)(k,n->key);
    //Equal
    if(i==0) return n->val;

    // Less
    if(i<0) return node_Search(n->l , k ,cmp);

    // Greater (i>0)
    return node_Search(n->r , k, cmp);
}

// to call function, call node_AverageDepth(n , n->size, 1)
double node_AverageDepth(node *n, unsigned int TotalSize, unsigned short int depth){
    if(n==NULL) return 0;
    return ( (double) depth / TotalSize ) + node_AverageDepth(n->l,TotalSize,depth+1) + node_AverageDepth(n->r,TotalSize,depth+1) ;
}


void node_Assign(node *n , void *k , void *v, int (*cmp) (void*,void*), void (*free_key) (void*) , void (*free_val) (void*) ){
    assert(n!=NULL);
    int i = (*cmp)(k,n->key);

    // found the key value pair
    if(i==0){
        (*free_key)(n->key);
        (*free_val)(n->val);
        n->key = k;
        n->val = v;
    } 
    // Less tthan
    if(i<0) node_Assign(n->l , k , v, cmp, free_key, free_val);
    // greater than
    if(i>0) node_Assign(n->r , k , v, cmp, free_key, free_val);

    return;
}

//precondition n->r != NULL
node* node_LeftRotate(node *n){
    assert(n->r != NULL);
    node *n_right = n->r;

    n->r = n_right->l ;
    node_FixSize(n);
    node_FixHeight(n);

    n_right->l = n;
    node_FixSize(n_right);
    node_FixHeight(n_right);
    return n_right;
}

//precondition n->l != NULL
node* node_RightRotate(node *n){
    assert(n->l != NULL);
    node *n_left = n->l;

    n->l = n_left->r ;
    node_FixSize(n);
    node_FixHeight(n);

    n_left->r = n;
    node_FixSize(n_left);
    node_FixHeight(n_left);
    return n_left;
}

node* node_ChildDirection(node *n, bool go_right){ assert(n!=NULL); if(go_right) return(n->r); return(n->l); }

//allocate space for min_max_child to store one node pointer, and free it afterwards
//precondition:
// the mix or max will not be the head of the intial function call
node *node_RemoveMinMax(node *n, bool go_right, void **to_fix, node **min_max_child){
    assert(n!=NULL);
    if(go_right && n->r == NULL){
        *(min_max_child) = n->l;
        n->l = NULL;
        node_FixSize(n);
        node_FixHeight(n);
        return n;
    }
    if(!go_right && n->l == NULL){
        *(min_max_child) = n->r;
        n->r = NULL;
        node_FixSize(n);
        node_FixHeight(n);
        return n;
    }


    node *min_max = node_RemoveMinMax(node_ChildDirection(n,go_right) , go_right , to_fix , min_max_child);

    //the child of n is the min/max
    if(min_max == node_ChildDirection(n,go_right) ){

        if(go_right) n->r = *(min_max_child);
        else n->l = *(min_max_child);

        *(to_fix) = n->key;
    }
    node_FixSize(n);
    node_FixHeight(n);
    return min_max;
}

//will merge two nodes who's heights differ by no more than +=1 (ie two previous children of a now deleted parent)
// the outputed node's height out will be <= 1 + max( node_Height(left) , node_Height(right) )
//Precondition:
// ( left !=NULL || right != NULL ) aka do not call merge on two empty nodes, as there is nothing for to_fix to be pointed at
node* node_Merge(node *left , node *right, void **to_fix){
    assert( left !=NULL || right != NULL );
    if (left==NULL){
        *(to_fix) = right->key;
        return right;
    } 
    if (right==NULL){
        *(to_fix) = left->key;
        return left; 
    }

    if (left->r ==NULL){
        left->r = right;
        node_FixSize(left);
        node_FixHeight(left);
        *(to_fix) = left->key;
        return left;
    }
    if (right->l==NULL){
        right->l = left;
        node_FixSize(right);
        node_FixHeight(right);
        *(to_fix) = right->key;
        return right;
    }

    node *new_root;
    node **temp = (node**) malloc(1 * sizeof(node*) );
    bool random_bool = (bool) rand()%2 ;
    if(left->height > right->height || (left->height==right->height && random_bool) ){
        new_root = node_RemoveMinMax(left , true , to_fix , temp ); 
    }
    else{
        new_root = node_RemoveMinMax(right , false , to_fix , temp ); 
    }
    free(temp);

    new_root->l = left;
    new_root->r = right;
    node_FixSize(new_root);
    node_FixHeight(new_root);
    return new_root;
}

//prconditions:
// a node with void *k exists in the tree with n at the root

// the node that the inputted void *k corresponds too and all of its parents, may be unbalanced by at most one 
// (either doubley left/right heavy, never tripley left/right heavy or more)
// all other nodes are balanced
node* node_RebalanceDirectionAVL(node *n, void *k, int (*cmp) (void*,void*) ){
    if(n==NULL) return NULL; //maybe assert(n!=NULL)
    int i = (*cmp)(k,n->key);
    // k is to the left
    if(i<0) n->l = node_RebalanceDirectionAVL(n->l , k, cmp);
    // k is to the right
    if(i>0) n->r = node_RebalanceDirectionAVL(n->r , k, cmp);
    node_FixHeight(n);

    if( node_HeightDiffernce(n) < -1 || 1 < node_HeightDiffernce(n) ){
        assert( node_HeightDiffernce(n) == -2 || node_HeightDiffernce(n) == 2 ); //for testing, should only be possible to get messed up by one height differnce

        //doubly left heavy:
        if(node_HeightDiffernce(n)==2){

            //n->l is balanced or left heavy:
            if( 0 <= node_HeightDiffernce(n->l) ) return node_RightRotate(n);
            //n->l is right heavy:
            else{
                n->l = node_LeftRotate(n->l);
                node_FixHeight(n);
                return node_RightRotate(n);
            }
        }

        //doubly right heavy:
        if(node_HeightDiffernce(n)==-2){
            //n->r is balanced or right heavy:
            if( node_HeightDiffernce(n->r) <= 0 ) return node_LeftRotate(n);
            //n->r is left heavy:
            else{
                n->r = node_RightRotate(n->r);
                node_FixHeight(n);
                return node_LeftRotate(n);
            }
        }


    }

    return n;
}


// inserts the key val pair to the node tree
// precondition: n is a valid node, void *k does not exist in the tree
void node_Insert(node *n, void *k, void *v, int (*cmp) (void*,void*) ){
    assert(n!=NULL);
    int i = (*cmp)(k,n->key);

    // Less
    if(i<0){
        if(n->l==NULL) n->l = node_Leaf(k,v);
        else node_Insert(n->l , k , v, cmp);
    }
    // greater
    if(i>0){
        if(n->r==NULL) n->r = node_Leaf(k,v);
        else node_Insert(n->r , k , v, cmp);
    }
    // precondition assures i==0 is not a possiblity 
    node_FixSize(n);
    node_FixHeight(n);
    return;
}

//remove the node with the void *k from the tree
//precondition: 
// node_Search(n , k) != NULL
// on the intial call on the root of the tree, n->size >= 2  (there must be a node left once the specified node is deleted)
//points the string pointer to_fix to the node to call node_RebalanceDirectionAVL on in order to maintain AVL property
// the only nodes that have potentially changed heights are "to_fix" and its parents
node *node_Remove(node *n, void *k, void **to_fix, int (*cmp) (void*,void*)  , void (*free_key) (void*) , void (*free_val) (void*)  ){
    assert(n!=NULL);
    int i = (*cmp)(k,n->key);
    node *to_return = n;

    //at the node to remove
    if(i==0){
        if(n->l==NULL && n->r==NULL) to_return = NULL;
        else to_return = node_Merge(n->l,n->r , to_fix);
        (*free_key)(n->key);
        (*free_val)(n->val);
        free(n);
    }
    else{
        // in case n->l or n->r is a leaf of only the node we want to delete, we save its parents key as the node to starting fixing from
        *(to_fix) = n->key;
        if(i<0) n->l = node_Remove(n->l , k , to_fix, cmp , free_key , free_val );
        if(i>0) n->r = node_Remove(n->r , k , to_fix, cmp , free_key , free_val );
        node_FixSize(n);
        node_FixHeight(n);
    }

    return to_return;
}

// precondition 0<= i < n->size
//to call from head node, give 0 as nodes_leftward : node_nth( head_node , i , 0) where i is the index of desired node
void *node_nth(node *n, unsigned i, unsigned nodes_leftward){
    assert(n!=NULL);
    unsigned true_nodes_leftward = nodes_leftward + node_Size(n->l);

    //are at nodes where exactly i nodes are "left" of it, thus 0-indexed, it is in the i'th spot
    if(i==true_nodes_leftward) return n->key;

    // too many nodes leftward (past index i)
    if(i<true_nodes_leftward) return node_nth(n->l , i , nodes_leftward);

    //too few nodes leftward (before index i) (i>true_nodes_leftward)
    return node_nth(n->r , i, true_nodes_leftward+1 );

}

unsigned node_Position( node *n , void *k , int (*cmp) (void*,void*) ){
    if(n==NULL) return 0;
    int i = (*cmp)(k , n->key);
    // are at the given node
    if (i==0) return node_Size(n->l);

    //go left
    if(i<0) return node_Position(n->l , k, cmp);

    //go right (i>0)
    return 1 + node_Size(n->l) + node_Position(n->r , k, cmp);
} 



void node_ValidHelper(node*n){
    if(n==NULL) return;
    assert(n->size == 1 + node_Size(n->l) + node_Size(n->r) );
    assert(n->key!=NULL);
    assert(n->val!=NULL);
    assert( node_Height(n) == node_Height(n->r) + 1 || node_Height(n) == node_Height(n->l) + 1 );

    //AVL Property:
    assert( -1 <= node_HeightDiffernce(n) && node_HeightDiffernce(n) <= 1 );

    node_ValidHelper(n->l);
    node_ValidHelper(n->r);
    // for(int i = 0; i<10; i+= 1) i -= 1;
    return;
}


void node_Valid(node *n, int (*cmp) (void*,void*) ){
    if(n==NULL) return;
    node_ValidHelper(n);

    //make sure keys are in correct sorted order, and tests out some functions
    void **key_list = (void**) malloc(n->size *  sizeof(void*)  );
    node_FillKeys(n , key_list);
    int i = 0;
    for(i=0; i< n->size ; i+=1){
        if (i!= n->size -1) assert( (*cmp)(key_list[i] , key_list[i+1]) < 0 );

        assert(node_Position(n,key_list[i], cmp) == i  );
        assert(node_nth(n,i,0)==key_list[i]);
    }
    free(key_list);

    return;
}











struct TREE{
    node *head;
    int (*cmp) (void* , void*);
    void (*free_key) (void*);
    void (*free_val) (void*);
};

void TREE_Valid(TREE *d){
    // Goes through a O(n) validation checker, would make O(log n) function like Add and delete O(n) if they validated the tree every time
    // only comment in for testing with smaller datasets
    node_Valid(d->head, d->cmp); 
    return;
}


// returns a empty Tree
TREE* TREE_Empty( int (*cmp) (void*, void*) , void (*free_key) (void*) , void (*free_val) (void*) ){
    TREE *d = (TREE*) malloc( sizeof(TREE) );
    d->head = NULL;
    d->cmp = cmp;
    d->free_key = free_key;
    d->free_val = free_val;
    TREE_Valid(d);
    return d;
}


// Free's a given Tree
void TREE_Free(TREE *d){ 
    node_Free(d->head , d->free_key , d->free_val);  
    free(d);  
    return; 
}

// adds a given:  void *key, void *val   pair,
// if the key already exists in the tree, the inputed val replaces one previously stored (frees the previously stored val)
// uses the callers key in the TREE (DO NOT FREE IT AFTER CALLING)
void TREE_Add(TREE *d , void *k , void *v){
    assert(v!=NULL);
    assert(k!=NULL);
    // currently empty
    if(d->head==NULL){
        d->head = node_Leaf(k,v);
    }
    else{
        // key does not exist already
        if( node_Search(d->head ,k, d->cmp)==NULL ){
            node_Insert(d->head , k , v, d->cmp);
            d->head = node_RebalanceDirectionAVL(d->head , k, d->cmp);
        }
        //key does already exist
        else{
            node_Assign(d->head , k , v , d->cmp , d->free_key, d->free_val);
        }
    }

    TREE_Valid(d);
    return;
}

// returns the void pointer associated with the given void* key in the TREE if it exists, returns NULL if it doesn't
// does not free in inputted void pointer key
void* TREE_Search(TREE *d , void *k){ return node_Search(d->head , k, d->cmp);  }


// removes (and frees) the key val pair associated with the inputted   void *key   
// does not free the callers  void *key
void TREE_Remove(TREE *d , void *k  ){
    if(node_Search(d->head , k , d->cmp)==NULL) return;
    
    if(node_Size(d->head)==1){
        node_Free(d->head , d->free_key , d->free_val);
        d->head = NULL;
    }
    else{
        void **to_fix = (void**) malloc( 1 * sizeof(void*) );
        d->head = node_Remove(d->head, k, to_fix , d->cmp , d->free_key , d->free_val );
        d->head = node_RebalanceDirectionAVL(d->head , *(to_fix) , d->cmp);
        free(to_fix);
    }

    TREE_Valid(d);
    return;
}

// returns the number of elements in the given TREE
unsigned TREE_Size(TREE *d){ return node_Size(d->head); }


// returns a sorted list of the keys in the given TREE
void** TREE_KeyList(TREE *d){
    if(d->head==NULL) return NULL;

    void **key_list = (void**) malloc(node_Size(d->head) *  sizeof(void*)  );
    node_FillKeys(d->head , key_list);

    return key_list;
}



// gives the average of the depths of all the nodes
// the root of a tree has a depth of 1, the roots child a depth of 2, the roots grandchild a depth of 3, etc etc
double TREE_AverageDepth(TREE *d){ return node_AverageDepth(d->head, node_Size(d->head), 1); }

// returns the hieght of the 'root/head' of the tree
unsigned TREE_Height(TREE *d){ return node_Height(d->head); }

// returns the key of the nth member in sorted order of the TREE, returns NULL if invalid n
// returns the actual key currently being used by the TREE
void *TREE_nth(TREE *d , unsigned n){
    if( n >= node_Size(d->head) ) return NULL;
    return node_nth(d->head , n, 0);
}

// the given key does not have to exist in the TREE and is not freed or mutated during runtime
// ie: if the inputted key is in the TREE, it is returning the index
//     if the inputted key is NOT in the TREE, returns numbers of elements that come before 
unsigned TREE_Position( TREE *d , void *k ){ return node_Position(d->head, k, d->cmp); }
