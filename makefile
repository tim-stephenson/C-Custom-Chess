output.out:  game_driver.o game.o piece.o board.o Tree.o List.o
	gcc -Wall -o output.out game_driver.o game.o piece.o board.o Tree.o List.o

game_driver.o: Driver/game_driver.c
	gcc -Wall -g -c Driver/game_driver.c

game.o: Game/game.c
	gcc -Wall -g -c Game/game.c 

piece.o: Pieces/piece.c
	gcc -Wall -g -c Pieces/piece.c

board.o: Board/board.c
	gcc -Wall -g -c Board/board.c

Tree.o: Tree/Tree.c
	gcc -Wall -g -c Tree/Tree.c

List.o: List/List.c
	gcc -Wall -g -c List/List.c 

clean:
	rm *.o output.out

# *.o signafies to remove anything that ends with ".o" as the * means anything can come before



# General layout for make
# target: dependencies
# 	action

# Recursive make???
# board_lst:
# 	cd Board && $(MAKE)

# Possiblities:
# 	cd Possiblities && $(MAKE)















# output: Possibilities.o board.o board_lst.o
# 	gcc -Wall -o output Possibilities.o board.o board_lst.o


# Possibilities.o: Possibilities/possibilities.c
# 	gcc -Wall -c Possibilities/possibilities.c

# board.o: Board/board.c
# 	gcc -Wall -c Board/board.c

# board_lst.o: Board/board_lst.c
# 	gcc -Wall -c Board/board_lst.c

# clean:
# 	rm *.o output