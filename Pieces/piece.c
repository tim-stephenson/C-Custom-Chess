// Timothy Stephenson June 2nd 2021
// Implementation file for a "piece type"



// Current need to work issues:


// implement a function to with a given piece, save it to a file

#include <stdbool.h>


#include <stdio.h> //need this for file managment

#include <stdlib.h>

#include <assert.h>

#include <string.h>

#include <ctype.h>

#include "piece.h"


const unsigned int MAX_FILE_LINE_LENGTH = 256;


// details possible movements
typedef struct moves moves;

struct moves{
    // things a rook would have
    bool up;
    bool down;
    bool left;
    bool right;

    // things a bishop would have
    bool up_left;
    bool up_right;
    bool down_left;
    bool down_right;

    // a list of fixed movement away from piece in the notation of:
    //  (movements_lst[0],movements_lst[1])  (movements_lst[2],movements_lst[3])  are both shifts to the nature of (row_shift,column_shift)
    // with the possibities of these numbers being negative a intergral component of ie: a Knight
    int *movements_lst;
    int movements_length; //length of int *movements  (must be even)  (will also be the length of movements_lst_2 if it exists)


    // how to add things such as castling and En Passant ????
    // how to implement "jumping" in checkers
};


// defines a chess peice (ie  white queen   black rock   white bishop)
// has the possiblities that attacks and/or first_moves point to the same moves structure as movement
// however cannot be the case where attacks and first_moves point to the same moves strucutre, which movement does not point to
struct piece{
    moves *movement;
    moves *attacks;
    moves *first_moves;

    bool jump_over; //can a piece jump over other pieces when moving, ie a knight can but a rook cannott

    bool attack_again; //piece can attack again after sucesfull atttack

    char identifier; // single char idenfier, ie: Q or K or B, either a lowercase or uppercase letter

    char *transformations; // terminated with a '\0'

    char *proper_name; // terminated with a '\0'
};



// defined by point_node->row  point_node->column  point_node->next  
struct point_node{
    int row;
    int column;
    point_node *next;
};

void point_node_Free(point_node *p_n){
    if(p_n != NULL){
        free(p_n->next);
        free(p_n);
    }
    return;
}

void point_node_Valid(point_node *p_n, int board_height , int board_width){
    if(p_n != NULL){
        assert(0 <= p_n->row && p_n->row < board_height);
        assert(0 <= p_n->column && p_n->column < board_width);
        point_node_Valid(p_n->next, board_height , board_width);
    }
    return;
}


//these three functions should be self explanitory
int point_node_Row(point_node *p_n) { return p_n->row; }

int point_node_Column(point_node *p_n) { return p_n->column; }

point_node* point_node_Next(point_node *p_n) { return p_n->next; }




void moves_Free(moves *m){
    if (m->movements_lst != NULL ) free(m->movements_lst);
    free(m);
    return;
}

void piece_Free(piece *p){

    if (p->proper_name != NULL) free(p->proper_name);

    if (p->transformations != NULL) free(p->transformations);

    if (p->movement != NULL) moves_Free(p->movement);

    if (p->attacks != NULL ) moves_Free(p->attacks);

    if (p->first_moves != NULL) moves_Free(p->first_moves);

    free(p);
    return;
}



void moves_Valid(moves *m){
    assert(m != NULL);
    assert(m->movements_length%2 ==0);
    if(m->movements_length >0 ) assert(m->movements_lst != NULL);
    return;

}

void piece_Valid(piece *p){
    assert(p!=NULL);
    assert( isalpha(p->identifier) ) ;

    for(int i = 0; p->transformations[i] != '\0'; i+=1){
        assert( isalpha(p->transformations[i]) ) ;
    }
    

    assert(p->proper_name != NULL);
    assert( strlen(p->proper_name) >=1 );

    moves_Valid(p->movement);
    if (p->attacks != NULL) moves_Valid(p->attacks);

    if(p->first_moves != NULL) moves_Valid(p->first_moves);
    return;
}



bool moves_ValidMovement(moves *m, int curr_row, int curr_column  , int target_row , int target_column, board *b ){
    assert(0 <= curr_row && curr_row < Board_Height(b)          );
    assert(0 <= curr_column && curr_column < Board_Width(b)     );
    assert(0 <= target_row && target_row < Board_Height(b)      );
    assert(0 <= target_column && target_column < Board_Width(b) );

    // no move is never a valid move
    if(curr_row==target_row && curr_column==target_column) return false;


    // TO IMPLEMENT: SOMETIMES A "CUSTOM" MOVE SHOULD BE ABLE TO JUMP OTHER PIECES, SOMETIMES IT SHOULDN'T
    // FIND A WAY TO DIFFERICIATE THIS
    int hypo_row, hypo_column;
    for(int i=0; i<m->movements_length; i+=2){
        hypo_row = curr_row + m->movements_lst[i];
        hypo_column = curr_column + m->movements_lst[i+1];
        if (hypo_row==target_row && hypo_column == target_column) return true;
    }

    // all "up, down, left, right, up_right, down_left, up_left, down_right" are FOR NOW
    // assumed to not be able to jump other pieces

    // where  dy = row change, dx = column change
    int  dy = 0, dx = 0;
    // both still at zero imply there is not a possible move

    // on the same vertical row
    if(curr_column==target_column){
        if(target_row<curr_row && m->up) dy = -1;

        if (target_row>curr_row && m->down) dy = 1;
    }

    // on the same horizontial column
    if(curr_row==target_row){
        if(target_column<curr_column && m->left) dx = -1;

        if (target_column>curr_column && m->right) dx = 1;
    }

    // on the same  "down left" / "up right" diagonial
    if( curr_row+curr_column == target_row+target_column ){
        if(target_column>curr_column && m->up_right) {
            dy = -1;
            dx = 1;
        }

        if(target_column<curr_column && m->down_left) {
            dy = 1;
            dx = -1;
        }
    }

    // on the same  "up left" / "down right" diagonial
    if( curr_row-curr_column == target_row-target_column ){
        if(target_column<curr_column && m->up_left) {
            dy = -1;
            dx = -1;
        }

        if(target_column>curr_column && m->down_right) {
            dy = 1;
            dx = 1;
        }
    }

    if (dx==0 && dy==0) return false;


    int inbetweenRow = curr_row, inbetweenCol = curr_column;
    while(true){
        inbetweenRow += dy;
        inbetweenCol += dx;
        if(inbetweenRow==target_row && inbetweenCol==target_column) break;
        if( Board_GetPiece(b, inbetweenRow, inbetweenCol) != open_space ) return false;

        assert(0 <= inbetweenRow && inbetweenRow < Board_Height(b) );
        assert(0 <= inbetweenCol && inbetweenCol < Board_Height(b) );
    } 
    // there is a valid move and there are no pieces in the way of the piece making this move
    return true;
}



//  for  a ( piece, curr_row, curr_column, board_height, board_width, target_row, target_column) , return if this is a valid moves
bool piece_ValidMove(piece *p , int curr_row, int curr_column , int target_row , int target_column , board *b ){
    return moves_ValidMovement(p->movement, curr_row, curr_column, target_row, target_column, b );
}


//  for  a ( piece, curr_row, curr_column, board_height, board_width, target_row, target_column) , return if this is a valid attack
bool piece_ValidAttack(piece *p , int curr_row, int curr_column , int target_row , int target_column , board *b ){
    moves *m;
    if (p->attacks ==NULL) m = p->movement;
    else m = p->attacks;

    return moves_ValidMovement(m, curr_row, curr_column, target_row, target_column, b );
}

//  for  a ( piece, curr_row, curr_column, board_height, board_width, target_row, target_column) , return if this is a valid first move
bool piece_ValidFirstMove(piece *p , int curr_row, int curr_column  , int target_row , int target_column , board *b ){
    moves *m;
    if (p->first_moves ==NULL) m = p->movement;
    else m = p->first_moves;

    return moves_ValidMovement(m, curr_row, curr_column, target_row, target_column, b );
}


//for a given point (row,column) insert after node p and return p->next
point_node* add_point(point_node *p, int row, int column) {
    assert(p->next == NULL);
    p->next = (point_node*) malloc( sizeof(point_node) );
    assert(p->next != NULL);
    p = p->next;
    p->row = row;
    p->column = column;
    p->next = NULL;
    return p;
}


// returns a point_node linked list for 
point_node * moves_AllMovements(moves *m, int row, int column, int board_height , int board_width ){
    assert(0 <= row && row < board_height );
    assert(0 <= column && column < board_width );


    point_node *head = (point_node*) malloc( sizeof(point_node) ) ;
    // purposley invalid dummy head values
    head->row = -1;
    head->column = -1; 
    head->next = NULL;
    point_node *p = head;

    int i,j;

    for(i = 0; i < m->movements_length; i+=2){
        if( 0 <= (m->movements_lst[i]+row) && (m->movements_lst[i]+row) < board_height 
            && 0 <= (m->movements_lst[i+1]+column) && (m->movements_lst[i+1]+column) < board_width  ){
            p = add_point(p, m->movements_lst[i]+row, m->movements_lst[i+1]+column);
        }
    }

    if(m->right){
        // rightwards horizontial
        for(j = column+1; j<board_width; j+= 1)  p = add_point(p, row, j);
    }
    if(m->left){
        // leftwards horizontial
        for(j = 0; j<column; j+= 1)   p = add_point(p, row, j);
    }
            
    if(m->down){
        // downwards vertical
        for(i = row+1; i<board_height; i+= 1)  p = add_point(p, i, column);
    }
    if(m->up){
        // upwards horizontial
        for(i = 0; i<row; i+= 1) p = add_point(p, i, column);
    }



    if(m->up_left){   
        //     up + left diagonioal
        i = row-1;
        j = column-1;
        while(i>=0 && j>=0) {
            p = add_point(p, i, j);
            i += -1;
            j += -1;
        }
    }
    if(m->up_right){  
        //     up + right diagonial
        i = row-1;
        j = column+1;
        while(i>=0 && j<board_width) {
            p = add_point(p, i, j);
            i += -1;
            j += 1;
        }
    }
    if(m->down_left){        
        //     down + left diagonioal
        i = row+1;
        j = column-1;
        while(i<board_height && j>=0) {
            p = add_point(p, i, j);
            i += 1;
            j += -1;
        }
    }
    if(m->down_right){
        //     down + right diagonioal
        i = row+1;
        j = column+1;
        while(i<board_height && j<board_width) {
            p = add_point(p, i, j);
            i += 1;
            j += 1;
        }
    }

    p = head->next;
    free(head);
    point_node_Valid(p, board_height , board_width);
    return p;      
}




// for a given (peice, row, column, board_length), returns a list of pairs
// corresponding to cordinates (in (row , column) notation ) the given peice could move
// returns NULL if invalid piece or no possiblie moves
point_node *piece_PossibleMoves(piece *p , int row, int column , int board_height , int board_width ){
    return moves_AllMovements(p->movement, row, column, board_height , board_width);

}

// for a given (peice, row, column, board_length), returns a list of pairs
// corresponding to cordinates (in (row , column) notation ) the given peice could attack
// returns NULL if invalid piece or no possiblie moves
point_node *piece_PossibleAttacks(piece *p , int row, int column , int board_height , int board_width ){
    moves *m;
    if(p->attacks==NULL) m = p->movement;
    else m = p->attacks;

    return moves_AllMovements(m, row, column, board_height , board_width);
}


// for a given (peice, row, column, board_length), returns a list of pairs
// corresponding to cordinates (in (row , column) notation ) the given peice could move on its first movew
// returns NULL if invalid piece or no possiblie moves
point_node *piece_PossibleFirstMoves(piece *p , int row, int column , int board_height , int board_width  ){
    moves *m;
    if(p->first_moves==NULL) m = p->movement;
    else m = p->first_moves;
    
    return moves_AllMovements(m, row, column, board_height , board_width );
}





// retturns a char list representing other pieces a given piece can transform into at the end of the board 
// ex: a pawn transform into anything at the end of the board
// transfermations char list is terminated with '\0'
char *piece_Transformations(piece *p) {return p->transformations; }

// returns a pieces proper name, cannot be NULL
char *piece_ProperName(piece *p){ return p->proper_name; }

// returns the char identtifier for a piece
char piece_Identifier(piece *p) { return p->identifier; }


bool piece_JumpOver(piece *p) {return p->jump_over; }

bool piece_AttackAgain(piece *p) {return p->attack_again; }





piece* piece_Empty(void){
    piece *p = (piece*) malloc( sizeof(piece) );

    p->movement = NULL;
    p->attacks = NULL;
    p ->first_moves = NULL;

    p->transformations = NULL;

    p->proper_name = NULL;

    p->jump_over = false;
    p->attack_again = false;

    return p;
}

// returns a moves object that has no possible moves
moves* moves_Empty(void){
    moves *m = (moves*) malloc( sizeof(moves)  ) ;

    m->up = false;
    m->down = false;
    m->left = false;
    m->right = false;

    m->up_left = false;
    m->up_right = false;
    m->down_left = false;
    m->down_right = false;

    m->movements_lst = NULL;
    m->movements_length = 0;

    return m;
}


// get a line from fp (if possible) and remove any \n or \r from the char_lst
// and return the length of the "useable charecters" excluding the '\0'
// return 0 if no new possible lines to read in fp, or emptyLine
int getLine_Remove_endlines(char *char_lst, FILE *fp){
    if ( feof(fp) ){
        char_lst[0] = '\0';
        return 0;
    }
    fgets(char_lst , MAX_FILE_LINE_LENGTH ,fp);
    int i=0;
    while(char_lst[i] != '\r' && char_lst[i] != '\n'){
        i+=1;
    }
    char_lst[i] = '\0';
    return i;
}




// for the given moves object created by moves_Empty, and a file pointer which should be just after a:
// "Movement:" or "Attacks:" or "First Moves:"
// fill char list representing the most recent line taken from the file that does not have to do with a moves object
// as fill the moves object if sucessful
// return false and fill char_lst with a named error if a issue comes up.
bool moves_FromFile(FILE *fp, moves *m, char *char_lst ){
    int i = getLine_Remove_endlines(char_lst , fp);

    if ( strncmp(char_lst,"Movements list:",15) != 0 ){
        strcpy(char_lst,"No \"Movements list:\" header for a given moves object");
        return false;
    }
    int j, num_commas = 0;

    for(j=15; j<i; j+=1){ // from the end of "Movements list: to the end of the line"
        if(char_lst[j]==',') num_commas += 1;
    }

    if(num_commas==0){
        m->movements_length = 0;
        m->movements_lst = NULL;
    }
    else{
        m->movements_length = num_commas+1;
        m->movements_lst = (int*) malloc( m->movements_length * sizeof(int)  );
    }

    char *temp_char_lst = &char_lst[15] ;//point to the charecters after "Movementts list:"
    // can also be = char_lst+15

    int val,chars_read;
    j=0;
    while(temp_char_lst[0] != '\0'){
        if( ( 48 <= temp_char_lst[0] && temp_char_lst[0] <=57 ) || temp_char_lst[0]=='-' ){ //found a number or a negative sign
            if (  sscanf(temp_char_lst, "%d%n", &val, &chars_read) != 1  ){
                strcpy(char_lst,"issue getting Movements list for a given moves object");
                return false;
            }
            if( j>= m->movements_length ){
                strcpy(char_lst,"found too many value (based on number of commas) in Movements list for a given moves object");
                return false;
            }
            m->movements_lst[j] = val;
            j+=1;
            temp_char_lst = &temp_char_lst[chars_read]; // should also be able to be temp_char_lst = temp_char_lst + chars_read 
        }
        else{
            temp_char_lst = &temp_char_lst[1]; // should also be able to be temp_char_lst = temp_char_lst + 1
        }
    }
    if (j != m->movements_length){
        strcpy(char_lst,"inconsistent number values (based on number of commas) in Movements list for a given moves object");
        return false;
    }


    getLine_Remove_endlines(char_lst , fp);  // get next line

    if( strcmp(char_lst,"up") ==0){
        m->up = true;
        getLine_Remove_endlines(char_lst , fp);
    }

    if( strcmp(char_lst,"down") ==0){
        m->down = true;
        getLine_Remove_endlines(char_lst , fp);
    }

    if( strcmp(char_lst,"left") ==0){
        m->left = true;
        getLine_Remove_endlines(char_lst , fp);
    }

    if( strcmp(char_lst,"right") ==0){
        m->right = true;
        getLine_Remove_endlines(char_lst , fp);
    }



    if( strcmp(char_lst,"up_left") ==0){
        m->up_left = true;
        getLine_Remove_endlines(char_lst , fp);
    }

    if( strcmp(char_lst,"up_right") ==0){
        m->up_right = true;
        getLine_Remove_endlines(char_lst , fp);
    }

    if( strcmp(char_lst,"down_left") ==0){
        m->down_left = true;
        getLine_Remove_endlines(char_lst , fp);
    }

    if( strcmp(char_lst,"down_right") ==0){
        m->down_right = true;
        getLine_Remove_endlines(char_lst , fp);
    }
    return true;

}



// for a given file with the format of model.txt, create a piece object
// returns NULL if invalid file format, and mallocs a char_lst with a error message that error_message points at
// MAX_FILE_LINE_LENGTH is the max length a given line in piece_FromFile is
piece *piece_FromFileErrorMessage(FILE *fp, char **error_message){
    assert(fp != NULL);
    // if( *(error_message) != NULL ){
    //     free( *(error_message) );
    //     *(error_message) = NULL;
    // }

    char *char_lst = (char*) malloc( MAX_FILE_LINE_LENGTH * sizeof(char) );

    if (feof(fp)){
        strcpy(char_lst,"started at end of file");
        *(error_message) = char_lst;
        return NULL;
    }
    piece *p = piece_Empty();
    int i,j;


    // get the single char identifier
    i = getLine_Remove_endlines(char_lst , fp);
    if(i!=1){
        strcpy(char_lst,"No char identifier");
        piece_Free(p);
        *(error_message) = char_lst;
        return NULL;
    }

    p->identifier = char_lst[0];
    if ( !isalpha(p->identifier)  ){
        strcpy(char_lst,"Invalid char identifier (must be upper or lowercase letter)");
        piece_Free(p);
        *(error_message) = char_lst;
        return NULL;
    }
    


    //get the proper name
    i = getLine_Remove_endlines(char_lst , fp);
    if(i==0){
        strcpy(char_lst,"No proper_name");
        piece_Free(p);
        *(error_message) = char_lst;
        return NULL;
    }
    p->proper_name = (char*) malloc( (i+1) * sizeof(char) );
    strcpy(p->proper_name,char_lst);

    //Movement:
    i = getLine_Remove_endlines(char_lst , fp);
    if( strcmp(char_lst,"Movement:") != 0 ){
        strcpy(char_lst,"No \"Movement:\" header");
        piece_Free(p);
        *(error_message) = char_lst;
        return NULL;
    }

    p->movement = moves_Empty();
    if ( ! moves_FromFile(fp, p->movement, char_lst) ){
        //char_lst is already holding the error message
        piece_Free(p);
        *(error_message) = char_lst;
        return NULL;
    }

    //if Attacks is differnt than movement
    if( strcmp(char_lst,"Attacks:")==0 ){
        p->attacks = moves_Empty();
        if ( ! moves_FromFile(fp, p->attacks, char_lst) ){
            //char_lst is already holding the error message
            piece_Free(p);
            *(error_message) = char_lst;
            return NULL;
        }   
    }

    //if Firtst Moves is differnt than movement
    if( strcmp(char_lst,"First Moves:")==0 ){
        p->first_moves = moves_Empty();
        if ( ! moves_FromFile(fp, p->first_moves, char_lst) ){
            //char_lst is already holding the error message
            piece_Free(p);
            *(error_message) = char_lst;
            return NULL;
        }   
    }


    // get transformations list
    if( strncmp(char_lst,"Transformations list:",21) != 0 ){
        strcpy(char_lst,"No \"Transformations list:\" header");
        piece_Free(p);
        *(error_message) = char_lst;
        return NULL;
    }

    char *temp_char_lst = &char_lst[21]; 
    int count = 0;
    for(j=0; temp_char_lst[j] !='\0' ; j+=1){
        if( isalpha(temp_char_lst[j])  ){ //either a lower or uppercase letter
            char_lst[count] = temp_char_lst[j]; //move it and overwrite the begining of the char_lst
            count+=1;
        }
    }
    char_lst[count] = '\0';
    p->transformations = (char*) malloc( (count+1) * sizeof(char) );
    strcpy(p->transformations,char_lst);


    // jump_over and attack_again

    getLine_Remove_endlines(char_lst , fp);
    if ( strcmp(char_lst,"jump_over")==0  ){
        p->jump_over = true;
        getLine_Remove_endlines(char_lst , fp);
    }

    if ( strcmp(char_lst,"attack_again")==0  ){
        p->attack_again = true;
        getLine_Remove_endlines(char_lst , fp);
    }


    free(char_lst);

    piece_Valid(p);
    
    return p;
}


piece *piece_FromFile(FILE *fp){
    char **error_message_pointer = (char**) malloc( 1 * sizeof(char*) );
    *(error_message_pointer) = NULL;
    piece *p = piece_FromFileErrorMessage(fp , error_message_pointer);
    if(*(error_message_pointer) != NULL) free( *(error_message_pointer) );
    free(error_message_pointer);
    return p;
}