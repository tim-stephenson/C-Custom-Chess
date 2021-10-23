// Timothy Stephenson May 25 2021

// Interface file for a chess board 

// By definition, the top left of the board board->array[0][0] exists and is a "open_space" upon initilization of new_board

//  The peices will be represented by a upper or lower case letter, with one player being uppercase and another being lowercase


extern const char open_space; // current the symbol '#' ascii code 35

extern const char danger_space; // current the symbol '*' ascii code 42 


struct board;

// apprivaties:    struct board   to    board
typedef struct board board;



//asserts a given board is valid
void Board_Valid(board*);

// creates an empty board of size height * width 
board* Board_New(unsigned short int, unsigned short int);

// returns the Height of the board
unsigned short int Board_Height(board*);

// returns the Width of the board
unsigned short int Board_Width(board*);

// return the peice at the given row, column
char Board_GetPiece(board*, unsigned short int, unsigned short int);

// assign the peice
void Board_AssignPiece(board*, char, unsigned short int, unsigned short int);

// copies the input board over, with no overlapping memory between the two boards
board* Board_Copy(board*) ;

//returns a char list (string) visually representing the board
char* Board_Visual(board*) ;

//free's the board and all underlying structures
void Board_Free(board*) ;



























// prints the visual representation from the visual_board function (TEMPORARY)
// void print_board(board*);




// a linked list of a "point" consisting of a row and a column
// used interially only (for now)
// struct point_node;

// apprivaties:    struct point_node   to    point_node
// typedef struct point_node point_node;





// returns a copy on the inputed board while placing the inputed piece at row, column
// returns a NULL pointer if cannot place the peice
// // does not free the inputed board
// board* place_piece(board*, char, int, int);




// // for a given (peice, sizeofBoard, row, column, &returnsize), returns a list of pairs
// // corresponding to cordinates (in (row , column) notation ) the given peice could attack
// point_node* points_of_attack(char, int, int, int);










