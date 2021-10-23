//Timothy Stephenson July 15th 2021

//implementation file for the backend of a game



#define _GNU_SOURCE

#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>  

#include <assert.h>

#include <string.h> 

#include <ctype.h>

#include <dirent.h> //  https://pubs.opengroup.org/onlinepubs/7990989775/xsh/dirent.h.html

#include "game.h"

// #include "../Pieces/pieces.h"

// #include "../Board/Board.h"

#include "../Tree/Tree.h"

// #include "../List/List.h"

const unsigned int MAX_PATH = 256;

const unsigned int turn_responce_message_max_length = 10; // e.g. : valid,  (maybe)  check, checkmate, draw, invalid1, invalid2, invalid3, invalid4, ...


struct game
{
    board *board;
    board *first_moves; // Y for yes open_space for no or non applicable
    TREE *pieces ;
    // LIST *moves;
    bool current_move; //true for white, false for black
    char white_win;
    char black_win;
    unsigned short int turns_taken;

    char *previous_turn_result;

    TREE *pieces_alive_dead;
    //we wamt counters for how many of each type of piece is alive and dead
    
};

//checks to see if a certian game is valid
void Game_Valid(game *g){
    Board_Valid(g->board);
    Board_Valid(g->first_moves);

    // moves 

    assert( !g->current_move == g->turns_taken%2 );
    if(g->white_win != '\0') assert(TREE_Search(g->pieces , &g->white_win) != NULL );
    if(g->black_win != '\0') assert(TREE_Search(g->pieces , &g->black_win) != NULL );

    // check validity of many more things, e.g. all pieces in board are in both peices trees, 
    // pieces_alive_dead is accurate (atleast for the alive counters)

}

//frees a game and all underlying structures
void Game_Free(game *g){
    if(g->board !=NULL) Board_Free(g->board);
    if(g->first_moves != NULL) Board_Free(g->first_moves);
    if(g->pieces !=NULL) TREE_Free(g->pieces);
    // if(g->moves != NULL) LIST_Free(g->moves);
    if(g->previous_turn_result != NULL) free(g->previous_turn_result);
    return;
}  



int charcmp(void *x, void *y){
    char *a = (char*) x, *b = (char*) y ;
    return *a - *b ;
}

game *Game_Empty(void){
    game *g = (game*) malloc( sizeof(game) );
    g->board = NULL;
    g-> first_moves = NULL;
    g->pieces = TREE_Empty(&charcmp , &free , &piece_Free);
    // g->moves = LIST_Empty( &free );
    g->current_move = true;
    g->white_win = '\0';
    g->black_win = '\0';
    g->turns_taken = 0;
    g->previous_turn_result = (char*) malloc( turn_responce_message_max_length * sizeof(char) );
    strcpy(g->previous_turn_result, "None");
    g->pieces_alive_dead = TREE_Empty(&charcmp , &free , &free);
    return g;
}

void add_piece(TREE *a_tree , piece *p){
    assert(p!=NULL);
    piece_Valid(p);
    char *temp_char = (char*) malloc( sizeof(char) );
    *temp_char = piece_Identifier(p);
    assert( TREE_Search(a_tree, temp_char ) ==NULL);
    TREE_Add(a_tree , temp_char , p) ;

}

int *new_tuple(int x1, int x2){
    int *to_return = (int*) malloc(2 * sizeof(int) );
    to_return[0] = x1;
    to_return[1] = x2;
    return to_return;
}


// REMOVE Littered print statements once you finish
// Maybe keep printing what peices are imported
game *Game_New(FILE *fp){
    game *g = Game_Empty();

    //Board Height and Width
    unsigned short int height, width;
    int i = fscanf(fp , "%hu %hu" , &height , &width);
    assert(i==2);
    g->board = Board_New(height , width);
    g->first_moves = Board_New(height , width);

    //Directory of Pieces
    char *dir_char = (char*) malloc( MAX_PATH * sizeof(char) ); 
    assert( fgets(dir_char , MAX_PATH , fp ) == dir_char );//flushing previous line when getting height and width

    assert( fgets(dir_char , MAX_PATH , fp ) == dir_char );

    unsigned int dir_len = strlen(dir_char);
    while( ( dir_char[dir_len-1] == '\n' || dir_char[dir_len-1] == '\r' ) && dir_len > 1 ) dir_len += -1;


    //Pieces
    // dir_char[dir_len] = '/' ;
    // dir_len += 1;
    i = fscanf(fp, "%s", dir_char + dir_len );
    assert(i==1);
    FILE *piece_fp;
    piece *p;
    
    if( strcmp(dir_char + dir_len,"WHOLEDIR")==0 ){
        dir_char[dir_len] = '\0';
        DIR *whole_directory = opendir(dir_char);
        assert(whole_directory != NULL);
        struct dirent *item_in_DIR = NULL;
        while(true){
            item_in_DIR = readdir(whole_directory);
            if(item_in_DIR==NULL) break;
            if(item_in_DIR->d_name[0]=='.') continue;
            
            strcpy(dir_char + dir_len , item_in_DIR->d_name);

             printf("GETTING PIECE: %s\n", dir_char);

            piece_fp = fopen(dir_char, "r");
            p = piece_FromFile(piece_fp);
            fclose(piece_fp);

            add_piece(g->pieces , p);

        }
        assert(closedir(whole_directory)==0);
    }   
    else{

        while( strcmp(dir_char + dir_len,"END")!=0 ){
            printf("GETTING PIECE: %s\n", dir_char);

            piece_fp = fopen(dir_char, "r");
            p = piece_FromFile(piece_fp);
            fclose(piece_fp);

            add_piece(g->pieces , p);

            i = fscanf(fp, "%s", dir_char + dir_len );
            assert(i==1);
        }
    }
    free(dir_char);

    //Filling g->pieces_alive_dead
    for(int index = 0; index < TREE_Size(g->pieces); index += 1 ){
        char *temp_char = (char*) malloc( 1 * sizeof(char) );
        *temp_char = *( (char*)  TREE_nth(g->pieces, index)   );
        TREE_Add(g->pieces_alive_dead , temp_char , (void*) new_tuple(0,0) );
    }

    //Win conditions:
    char *temp = (char*) malloc(5 * sizeof(char) ); //NONE /0 takes 5 chars
    i = fscanf(fp, "%s" , temp);
    assert(i==1);
    if( strcmp(temp,"NONE") != 0) g->white_win = temp[0];
    i = fscanf(fp, "%s" , temp);
    assert(i==1);
    if( strcmp(temp,"NONE") != 0) g->black_win = temp[0];
    free(temp);

    //Actual Grid:
    char at_square;
    unsigned short row=0, column=0;
    while(row < Board_Height(g->board) ){ //traversing vertically
        column = 0;
        while(column < Board_Width(g->board) ){  //traversing horazontially
            i = fscanf(fp , "%c" ,&at_square);
            if(1!=1) break;
            if(at_square != ' ' && at_square != '\n' && at_square != '\r'){
                if(at_square != open_space){
                    assert(islower(at_square) || isupper(at_square) );
                    assert(TREE_Search(g->pieces , &at_square) != NULL);

                    ( (int*) TREE_Search(g->pieces_alive_dead , &at_square) )[0] += 1; ///TEST THIS WORKS

                    Board_AssignPiece(g->board , at_square , row , column );
                    Board_AssignPiece(g->first_moves, 'Y' , row , column) ;
                     
                }
                column += 1;  
            }
        }
        row +=1;
    }

    return g;
}


// // saves a ongoing game to the inputed writeable and empty file pointer
// void Game_Save(game *g, FILE *fp){
//     return; //TO DO LATER
// }

// // from a saved game, relaunch the game
// game *Game_Continue(FILE *fp){
//     return NULL; //TO DO LATER
// }


struct turn{
    bool is_transformation;
    char transform_into;

    bool is_attack;

    char piece; 

    unsigned curr_row;
    unsigned curr_column;
    unsigned target_row;
    unsigned target_column;
};

unsigned IsValidMove(game *, turn *);
void ExecuteMove(game *, turn *);
turn *long_algebraic_notation_TO_a_turn(char *, unsigned, unsigned, bool);

void Game_MakeMove(game *g, turn *a_turn){
    unsigned validmove_Code = IsValidMove(g,a_turn);
    if (validmove_Code==0){
        strcpy(g->previous_turn_result,"valid");
        ExecuteMove(g,a_turn);
    }
    else{
        strcpy(g->previous_turn_result,"invalid");
        g->previous_turn_result[ strlen("invalid") ] = (char) validmove_Code + 48;
        g->previous_turn_result[ strlen("invalid") + 1] = '\0';
    }
    return;
}

void Game_MakeMoveChar(game *g, char *move){
    turn *a_turn = long_algebraic_notation_TO_a_turn(move ,  Board_Height(g->board), Board_Width(g->board), g->current_move );
    // print_turn(a_turn);
    Game_MakeMove(g, a_turn);
    return;
}


void ExecuteMove(game *g, turn *a_turn){
    if(a_turn->is_transformation){
        //IMPLEMENT THIS
        assert(false && "tramsformations not implemented");
    }

    char piece_at_target = Board_GetPiece(g->board , a_turn->target_row , a_turn->target_column);

    //mark a first move having been made from where both where the piece starts and ends its turn
    if( Board_GetPiece(g->first_moves, a_turn->curr_row , a_turn->curr_column) != open_space ) Board_AssignPiece(g->first_moves ,open_space, a_turn->curr_row , a_turn->curr_column );
    if( Board_GetPiece(g->first_moves, a_turn->target_row , a_turn->target_column) != open_space ) Board_AssignPiece(g->first_moves ,open_space, a_turn->target_row , a_turn->target_column );

    if(a_turn->is_attack){
        ( (int*) TREE_Search(g->pieces_alive_dead, &piece_at_target) )[0] += -1; //one less alive
        ( (int*) TREE_Search(g->pieces_alive_dead, &piece_at_target) )[1] += 1; //one more dead
    }

    Board_AssignPiece(g->board , a_turn->piece , a_turn->target_row , a_turn->target_column);
    Board_AssignPiece(g->board , open_space , a_turn->curr_row , a_turn->curr_column);

    g->current_move = !g->current_move ;
    g->turns_taken += 1;

    return;

}

// 0 for valid, other number for invalid code
unsigned IsValidMove(game *g, turn *a_turn){

    if(a_turn->is_transformation){
        //IMPLEMENT THIS
        assert(false && "tramsformations not implemented");
    }

    char piece_at_target = Board_GetPiece(g->board , a_turn->target_row , a_turn->target_column);

    //does the piece match the piece at curr_row curr_column
    if(a_turn->piece != Board_GetPiece(g->board, a_turn->curr_row, a_turn->curr_column) ) return 1;

    if(TREE_Search(g->pieces , &(a_turn->piece) ) ==NULL) return 2;

    // is atleast one of these peices still alive
    if( ( (int*) TREE_Search(g->pieces_alive_dead, &(a_turn->piece) ) )[0] < 1 ) return 3;

    // upper case peices = whites turn
    // lower case pieces = blacks turn

    if( g->current_move != (bool) isupper(a_turn->piece) ) return 4;

    if(a_turn->is_attack){
        if( piece_at_target == open_space ) return 5;
        if( g->current_move != (bool) islower(piece_at_target) ) return 6;
    }
    else{
        if( piece_at_target != open_space  ) return 7;
    }

    // is an attack
    if(a_turn->is_attack){
        if( ! piece_ValidAttack( TREE_Search(g->pieces, &(a_turn->piece) ) , a_turn->curr_row , a_turn->curr_column  , a_turn->target_row , a_turn->target_column, g->board ) ) return 8;
    }
    // is a first move
    else if (Board_GetPiece(g->first_moves, a_turn->curr_row, a_turn->curr_column) != open_space){
        if( ! piece_ValidFirstMove( TREE_Search(g->pieces, &(a_turn->piece) ) , a_turn->curr_row , a_turn->curr_column , a_turn->target_row , a_turn->target_column , g->board) ) return 9;
    }
    // is a regular move
    else{
        if( ! piece_ValidMove( TREE_Search(g->pieces, &(a_turn->piece) ) , a_turn->curr_row , a_turn->curr_column , a_turn->target_row , a_turn->target_column, g->board ) ) return 10;
    }

    return 0;
}


turn *long_algebraic_notation_TO_a_turn(char *long_algebraic_notation, unsigned board_height, unsigned board_width, bool whitesTurn){
    turn *a_turn = (turn*) malloc( sizeof(turn) );
   
    a_turn->is_transformation = false;
    a_turn->transform_into = '\0';
    //HOW TO DO TRANSFORMATIONS

    unsigned i = 0;
    a_turn->piece = long_algebraic_notation[i];
    i += 1;
    if(whitesTurn) a_turn->piece = toupper(a_turn->piece);
    if(! whitesTurn) a_turn->piece = tolower(a_turn->piece);


    algebraic_notation_to_cartesian_coordinates(long_algebraic_notation, &i, board_height,  board_width , &(a_turn->curr_row) , &(a_turn->curr_column) );

    a_turn->is_attack =  long_algebraic_notation[i] == 'x';
    i += 1;

    algebraic_notation_to_cartesian_coordinates(long_algebraic_notation , &i,  board_height , board_width,  &(a_turn->target_row) ,  &(a_turn->target_column) );

    return a_turn;
}




// whose turn is it currently (true) for white (false) for black
bool Game_CurrentTurn(game *g){ return g->current_move; }

//return a list of all previous moves in conventinaol Algebraic Notation
// LIST *Game_AllMoves(game *g){ return g->moves; }

unsigned Game_TurnsTaken(game *g){   return g->turns_taken;  }

board *Game_CurrentBoard(game *g){ return g->board; }

char *Game_PreviousTurnResult(game *g){ return g->previous_turn_result; }

// returns a piece object for the given char identifier
piece *Game_piece(game *g , char c ){    return TREE_Search(g->pieces , &c);   }





void algebraic_notation_to_cartesian_coordinates(char *chr, unsigned *chr_index, unsigned board_height, unsigned board_width, unsigned *return_row, unsigned *return_column){
    *return_column = ( (unsigned) chr[*chr_index] ) - 97 ;
    *chr_index += 1;
    assert(*return_column < board_width);

    unsigned chess_row_number;
    unsigned temp_num_length;
    sscanf(chr + *chr_index , "%d%n" , &chess_row_number , &temp_num_length );
    *chr_index += temp_num_length;
    *return_row = board_height - chess_row_number;
    assert(*return_row < board_height);

    return;
}

 char *cartesian_coordinates_to_algebraic_notation(unsigned row, unsigned column, unsigned board_height, unsigned board_width){
    assert(row < board_height && column < board_width);

    char *return_char = (char*) malloc (4 * sizeof(char) );
    int i = 0, temp;
    sprintf(return_char + i , "%c%n", (char) (column+97) , &temp );
    i += temp;
    sprintf(return_char + i , "%u%n", board_height-row , &temp );
    i += temp;
    return_char[i] = '\0';
    return return_char;
 }

