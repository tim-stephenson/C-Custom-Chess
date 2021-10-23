// Timothy Stephenson June 2nd 2021
// Interface file for a "piece type"





#include "../Board/board.h"
// This also defines open_space 



// defines a chess peice (ie  white queen   black rock   white bishop)
struct piece;
typedef struct piece piece;



// defined by point_node->row  point_node->column  point_node->next  
struct point_node;
typedef struct point_node point_node;

// free's a given pointt_node and all underlying structures
void point_node_Free(point_node*);

//these three functions should be self explanitory
int point_node_Row(point_node*);

int point_node_Column(point_node*);

point_node* point_node_Next(point_node*);



// free's a given piece and all underlying structures
void piece_Free(piece*);

// asserts a piece is valid
void piece_Valid(piece*);

//  for  a ( piece, curr_row, curr_column, target_row, target_column, board) , return if this is a valid moves
bool piece_ValidMove(piece* , int , int ,  int , int , board* );

//  for  a ( piece, curr_row, curr_column, target_row, target_column, board) , return if this is a valid attack
bool piece_ValidAttack(piece* , int , int ,  int , int , board* );

//  for  a ( piece, curr_row, curr_column, target_row, target_column, board) , return if this is a valid first move
bool piece_ValidFirstMove(piece*, int , int , int , int , board* ); 


// for a given (peice, row, column, board_height, board_width), returns a list of pairs
// corresponding to cordinates (in (row , column) notation ) the given peice could move
// returns NULL if invalid piece or no possiblie moves
point_node *piece_PossibleMoves(piece* , int , int , int , int );

// for a given (peice, row, column, board_height, board_width), returns a list of pairs
// corresponding to cordinates (in (row , column) notation ) the given peice could attack
// returns NULL if invalid piece or no possiblie moves
point_node *piece_PossibleAttacks(piece* , int , int , int , int );


// for a given (peice, row, column, board_height, board_width), returns a list of pairs
// corresponding to cordinates (in (row , column) notation ) the given peice could move on its first movew
// returns NULL if invalid piece or no possiblie moves
point_node *piece_PossibleFirstMoves(piece* , int , int , int , int );



// retturns a char list representing other pieces a given piece can transform into at the end of the board 
// ex: a pawn transform into anything at the end of the board
// can be NULL if empty
char *piece_Transformations(piece*);

// returns a pieces proper name, cannot be NULL
char *piece_ProperName(piece*);

// returns the char identtifier for a piece
char piece_Identifier(piece*);

bool piece_JumpOver(piece*);

bool piece_AttackAgain(piece*);


//piece_FromFile, but will fill char ** with an error message if an error occured, used for troubleshooting
piece *piece_FromFileErrorMessage(FILE *, char **);

//from a file pointer via the specifacations designated in Chess/Pieces/model.txt and examples in Chess/Pieces/Defualt_Pieces
piece *piece_FromFile(FILE *);



















// How are we going to add
// castle-ing
// En Passant
// can a piece go over another piece

// we figure this out later



/*





// returns a char list representing a visual display of all the defualt pieces
// MAYBE we should make this accept some input of types of customizable pieces
char *pieces_VisualDisplay(void);








*/