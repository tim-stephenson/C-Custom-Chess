// Timothy Stephenson May 25 2021

// Implementation file for a chess board 

// By definition, the top left of the board board->array[0][0] exists and is a "open_space" upon initilization of new_board

//  The peices will be represented by a upper or lower case letter, with one player being uppercase and another being lowercase



#include <stdbool.h>

#include <stdlib.h>

#include <assert.h>


#include "board.h"


const char open_space = '#';

const char danger_space = '*'; 


struct board{
    char **array;
    unsigned short int height;
    unsigned short int width;
};

void Board_Valid(board *b){
    assert(b != NULL);
    assert(b->array != NULL);
    for(int i=0; i< b->height; i+=1){
        assert(b->array[i] != NULL);
        for(int j=0; j<b->width; j+=1){
            char temp = b->array[i][j];
            assert(0<= temp && temp < 255);
            assert(temp == open_space || temp == danger_space || ('a' <= temp && temp <= 'z') || ('A' <= temp && temp <= 'Z') );
        }
    }
    return;
}


// creates an empty board of size width * height
board* Board_New(unsigned short int height , unsigned short int width){
    assert(height>=1);
    assert(width>=1);
    board *b = malloc(sizeof(board));
    b->height = height;
    b->width = width;
    b->array = (char**) malloc(height * sizeof(char*) );
    for(int i=0; i<height; i+=1) {
        b->array[i] = (char*) malloc(width * sizeof(char) );
        assert(b->array[i] != NULL);
        for(int j=0; j<width; j+=1) b->array[i][j] = open_space;
    }

    Board_Valid(b);

    return b;
}

// returns the Height of the board
unsigned short int Board_Height(board *b){return b->height;}

// returns the Width of the board
unsigned short int Board_Width(board *b){return b->width;}


// return the peice at the given row, column
char Board_GetPiece(board *b, unsigned short int row, unsigned short int column){
    assert(0 <= row && row < b->height );
    assert(0 <= column && column < b->width );
    return b->array[row][column];

}

// assign the peice
void Board_AssignPiece(board *b, char c, unsigned short int row, unsigned short int column){
    assert(0 <= row && row < b->height );
    assert(0 <= column && column < b->width );
    b->array[row][column] = c;

    Board_Valid(b);

    return;
}


// copies the input board over
board* Board_Copy(board *b){
    board *to_return = (board*) malloc( sizeof(board) );
    to_return->height = b->height;
    to_return->width = b->width;
    to_return->array = (char**) malloc(b->height * sizeof(char*) );
    for(int i=0; i<b->height; i+=1) {
        to_return->array[i] = (char*) malloc(b->width * sizeof(char) );
        for(int j=0; j<b->width; j+=1) to_return->array[i][j] = b->array[i][j];
    }

    Board_Valid(to_return);

    return to_return;
} 

//returns a char list (string) visually representing the board
char* Board_Visual(board *b){
    char *chr_lst = (char*) malloc( (2*b->height*b->width + 4*b->height + 3) * sizeof(char) );
    int a = 0;
    chr_lst[a] = '\n';
    a +=1;
    for(int i=0; i<b->height; i+=1) {
        chr_lst[a] = '{';
        chr_lst[a+1] = ' ';
        a += 2;
        for(int j=0; j<b->width; j+=1) {
            chr_lst[a] = b->array[i][j];
            chr_lst[a+1] = ' ';
            a += 2;
        }
        chr_lst[a] = '}';
        chr_lst[a+1] = '\n';
        a += 2;
    }
    chr_lst[a] = '\n';
    chr_lst[a+1] = '\0';
    a += 2;
    assert( a == (2*b->height*b->width + 4*b->height + 3) );
    return chr_lst;
}


//free's the board and underlying structures
void Board_Free(board *b){
    for(int i=0; i<b->height; i+=1) free(b->array[i]);
    free(b->array);
    free(b);
    return;
}





















// prints the visual representation from the visual_board function
// void print_board(board *b){
//     char *lst = visual_board(b);
//     printf("%s",lst);
//     free(lst);
// }








// linked list of point_nodes
// struct point_node{
//     int row;
//     int column;
//     point_node *next;
// };


// helper white box function to a point linked list
// void free_point_node(point_node *p_n){
//         if (p_n!=NULL){
//             free_point_node(p_n->next);
//             free(p_n);
//         }
//         return;
//     }

// for a given board size n, assert a p_n point_node is valid
// void valid_pointer_node(point_node *p_n, int n){
//     point_node *temp = p_n;
//     bool failed = false;
//     while(temp != NULL){
//         if (!(0 <= temp->row && temp->row < n ) ) failed = true;

//         if (!(0 <= temp->column && temp->column < n ) ) failed = true;
//         temp = temp->next;
//     }

//     if (failed){
//         temp = p_n;
//         while(temp != NULL){
//             printf("(%d,%d)\n",temp->row,temp->column);
//             temp = temp->next;
//         }
//         assert(false);
//     }

//     return;
// }



// returns a copy on the inputed board while placing the inputed piece at row, column
// returns a NULL pointer if cannot place the peice
// board* place_piece(board *b, char c, int row, int column){
//     assert(0 <= row && row < b->size );
//     assert(0 <= column && column < b->size );

//     if(b->array[row][column] != open_space ) return NULL;

//     board *b2 = copy_board(b);

//     // assign the peice to where it should be
//     b2->array[row][column] = c;


//     point_node *p = points_of_attack(c, b->size, row, column);
//     valid_pointer_node(p,b->size);
//     point_node *temp;


//     char temp_char;
//     while(p != NULL){
//         // ISSUE HERE
//         assert(p->row != -1);
//         assert(p->column != -1);
//         assert(0<=p->row && p->row < b->size);
//         assert(0<=p->column && p->column < b->size);


//         temp_char = b2->array[p->row][p->column] ;

//         // mark as a place that is able to be attacked
//         if( temp_char == open_space) b2->array[p->row][p->column] = danger_space ;

//         // case that given place on board is neither a open or a danger place, thus a piece is there
//         else if (temp_char != danger_space ){
//             free_board(b2);
//             free_point_node(p);
//             return NULL;
//         }
//         temp = p;
//         p = p->next;
//         free(temp);
//     }

//     valid_board(b2);

//     return b2;


// }


// //for a given point (row,column) insert after node p and return p->next
// point_node* add_point(point_node *p, int row, int column) {
//     assert(p->next == NULL);
//     p->next = (point_node*) malloc(sizeof(point_node));
//     assert(p->next != NULL);
//     p = p->next;
//     p->row = row;
//     p->column = column;
//     p->next = NULL;
//     return p;
// }




// // CASE FOR NO VALID POINTS OF ATTACK
// point_node * points_of_attack(char c, int n, int row, int column){
//     // a valid point for a size of n
//     assert(0 <= row && row < n );
//     assert(0 <= column && column < n );

//     // a valid peice
//     assert(c=='Q' || c=='K' || c=='R' || c=='B' || c=='N' || c=='p');




//     // (4n-4) should be the maxium attackable spaces for a queen, which should  always have the greatest possible moves
//     // set to 4n +1 for breathing room
//     // DO LATER: create efficiances of what size to make xs based on the inputed peice
//     point_node *head = malloc(sizeof(point_node) ) ;
//     head->row = -1;
//     head->column = -1; 
//     head->next = NULL;
//     point_node *p = head;

//     int i;
//     int j;


//     // not sure what to do with pawns yet
//     if(c=='p')assert(false);


//     if(c=='N' || c=='K'){
//         // representing the movements a knight can make, in  {row_shift, column_shift} notation
//         // which is all possible pairs of (1,2) and (2,1) with all possible sign combinations
//         // and represents the possible moves on a king, 1 in any direction
//         // int movements[8][2];


//         int *movements;
//         // a array of 8 pairs of 2 representing changes from the peice (either King or Knight)

//         if(c=='N'){
//             int temp[16] = {  2,1 ,   2,-1  ,  -2,1  ,  -2,-1  ,       1,2  ,  1,-2  ,  -1,2  ,  -1,-2  };
//             movements = temp;
//         }


//         if(c=='K'){
//             int temp[16] = { 1,-1  ,  1,0  ,  1,1  ,         -1,-1  ,  -1,0  ,  -1,1  ,          0,1  ,  0,-1  };
//             movements = temp;
//         }
        


//         for(int count=0; count<16; count+=2){
//             if( 0 <= (movements[count]+row) && (movements[count]+row) < n 
//                 && 0 <= (movements[count+1]+column) && (movements[count+1]+column) < n  ){
//                 p = add_point(p, movements[count]+row, movements[count+1]+column);
//             }
//         }

//     }



//     if(c=='Q' || c=='R'){
//         //     horizontially:
//         for(j = 0; j<n; j+= 1) {
//             if (j!=column) p = add_point(p, row, j);
//         }
            
//         //     vertically:
//         for(i = 0; i<n; i+= 1) {
//             if (i!=row) p = add_point(p, i, column);
//         }
//     }




//     if(c=='Q' || c=='B'){
            
//         //     up + left diagonioal
//         i = row-1;
//         j = column-1;
//         while(i>=0 && j>=0) {
//             p = add_point(p, i, j);
//             i += -1;
//             j += -1;
//         }
            
//         //     up + right diagonial
//         i = row-1;
//         j = column+1;
//         while(i>=0 && j<n) {
//             p = add_point(p, i, j);
//             i += -1;
//             j += 1;
//         }
            
//         //     down + left diagonioal
//         i = row+1;
//         j = column-1;
//         while(i<n && j>=0) {
//             p = add_point(p, i, j);
//             i += 1;
//             j += -1;
//         }

//         //     down + right diagonioal
//         i = row+1;
//         j = column+1;
//         while(i<n && j<n) {
//             p = add_point(p, i, j);
//             i += 1;
//             j += 1;
//         }
//     }

//     p = head->next;

//     free(head);


//     return p;
        
// }