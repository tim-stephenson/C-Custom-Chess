//Timothy Stephenson July 14th 2021

//interface file for the backend of a game

// will use Long Algebraic Notation for keep track of moves, though  will also store conventional Algebraic Notation for a human reader

// Long Algebraic Notation having the format:

// the piece letter upper case   (+)  the piece current location   (+) a '-'' for a move a 'x'' for a capture  (+) location of move / capture

// note the 'x' symbol is used to denote a attack/capture (ascii code 120), whereas some may used the Multiplication sign, this is because the Multiplication sign is not part of the standerd 7-bit ASCII charecters
//ie: 
// Rd3xd7 (a rook went from d3 to capture a piece at d7)
// Qa1-c3 (a queen went from a1 to c3)
// Km17-m18 (a king went from m17 to m18)   (note the notation used for a non-standerd board size)

//all notation was taken from:
// https://en.wikipedia.org/wiki/Algebraic_notation_(chess)


// struct game
// {
//     board *board;
//     board *first_moves; // Y for yes open_space for no or non applicable
//     TREE *pieces ;
//     LIST *moves;
//     bool current_move; //true for white, false for black
//     char white_win;
//     char black_win;
//     unsigned short int turns_taken;

//     char *previous_turn_result;

//     TREE *pieces_alive_dead;
//     //we wamt counters for how many of each type of piece is alive and dead
    
// };


// struct turn{
//     bool is_transformation;
//     char transform_into;

//     bool is_attack;

//     char piece; 

//     unsigned curr_row;
//     unsigned curr_column;
//     unsigned target_row;
//     unsigned target_column;
// }



#include "../Pieces/piece.h"

#include "../Board/Board.h"

#include "../List/List.h"


extern const unsigned int MAX_PATH;

struct game;
typedef struct game game;

struct turn;
typedef struct turn turn;

//checks to see if a certian game is valid
void Game_Valid(game *);

//frees a game and all underlying structures
void Game_Free(game *);

// creates a new game based on a game setting file exampled in Settings/game_settings_example.txt
game *Game_New(FILE *);

// saves a ongoing game to the inputed writeable and empty file pointer
// void Game_Save(game *, FILE *);

// from a saved game, relaunch the game
// game *Game_Continue(FILE *);

// exucutes a move based on a Long Algebraic Notation char *move
// sends a keyword/code about how the move went: "invalid" "valid" "check" "checkmate" "draw" to Game_PreviousTurnResult
void Game_MakeMoveChar(game *, char *);

// same as Game_MakeMoveChar, but inputing a struct turn type
void Game_MakeMove(game *, turn *);

// whose turn is it currently (true) for white (false) for black
bool Game_CurrentTurn(game *);

//return a list of all previous moves in conventinaol Algebraic Notation
// LIST *Game_AllMoves(game *);

unsigned Game_TurnsTaken(game *);

board *Game_CurrentBoard(game *);

char *Game_PreviousTurnResult(game *);

// returns a piece object for the given char identifier
piece *Game_piece(game * , char );

// I think we are going to offload this into the driver having direct acsess to the piece library and piece objects
// for the piece on the square denoted by char *piece
// piece and move locations will given by coordinates via Algerbraic Notation
// char** Game_ValidMoves(game *, char *piece);


//    string_input , index_of_string , board_height, board_width, return_row, return_column
//   will start reading string_input at string_input[ *index_of_string ] and will increment *index_of_string while reading the string
void algebraic_notation_to_cartesian_coordinates(char *, unsigned *, unsigned , unsigned , unsigned *, unsigned *);

	 // row , column , board_height , board_width
char *cartesian_coordinates_to_algebraic_notation(unsigned row, unsigned column, unsigned board_height, unsigned board_width);