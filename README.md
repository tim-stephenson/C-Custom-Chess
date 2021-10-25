# C-Custom-Chess
A Customizable Chess game in C

At its current state to play the game:

Game Settings:
choose your game setting via the Templates given in Game/Setting
change the chosen game settings by changing the global constant GameSettings in Driver/game_driver.c

Pieces:
Game Settings templates will ask you to name a directory with piece files
a Model can be found at Pieces/Model.txt and examples found at Pieces/DefualtPieces

Running the game:
run the make in the main directory, then run output.out
To input moves follow the description of Long algebraic notation (detailed here: https://en.wikipedia.org/wiki/Algebraic_notation_(chess) as well)
discussed in the header file Game/game.h:

// Long Algebraic Notation having the format:

// the piece letter upper case   (+)  the piece current location   (+) a '-'' for a move a 'x'' for a capture  (+) location of move / capture

// note the 'x' symbol is used to denote a attack/capture (ascii code 120), whereas some may used the Multiplication sign, this is because the Multiplication sign is not part of the standerd 7-bit ASCII charecters
//ie: 
// Rd3xd7 (a rook went from d3 to capture a piece at d7)
// Qa1-c3 (a queen went from a1 to c3)
// Km17-m18 (a king went from m17 to m18)   (note the notation used for a non-standard board size)

//all notation was taken from:
// https://en.wikipedia.org/wiki/Algebraic_notation_(chess)