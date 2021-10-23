#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>  

#include <assert.h>

#include <string.h> 

// #include <ctype.h>

// #include <dirent.h> //  https://pubs.opengroup.org/onlinepubs/7990989775/xsh/dirent.h.html

#include "../Game/game.h"

#include "../Pieces/piece.h"

#include "../Board/board.h"

#include "../Tree/Tree.h"

#include "../List/List.h"

const unsigned max_input_len = 50;

const unsigned view_length = 1000;


char *game_view(void *g){
    char *view = (char*) malloc(view_length * sizeof(char) );
    unsigned int i = 0; //follows the index of view string


    strcpy(view + i, "\n\n");
    i += strlen("\n\n");

    
    strcpy(view + i, "Previous Turn Result: ");
    i += strlen("Previous Turn Result: ");
    strcpy(view + i, Game_PreviousTurnResult(g) );
    i += strlen( Game_PreviousTurnResult(g) );


    strcpy(view + i, "\n");
    i += strlen("\n");
    
    char *temp = Board_Visual( Game_CurrentBoard(g)  );
    strcpy(view + i , temp);
    i += strlen(temp);
    free(temp);

    if ( Game_CurrentTurn(g) ){
        strcpy(view + i, "It is: WHITE's turn\n");
        i += strlen("It is: WHITE's turn\n");
    }
    else{
        strcpy(view + i, "It is: BLACK's turn\n");
        i += strlen("It is: BLACK's turn\n");
    }

    // strcpy(view + i, "List of Previous Moves: ");
    // i += strlen("List of Previous Moves: ");

    // LIST *temp_list = Game_AllMoves(g);
    // for(unsigned j = 0; j < LIST_Len(temp_list); j+=1){
    //     strcpy(view + i, (char*) LIST_nth(temp_list , j) );
    //     i += strlen( (char*)  LIST_nth(temp_list , j) );
    //     strcpy(view + i , " ");
    //     i += 1;
    // }
    // strcpy(view + i, "\n");
    // i += 1;
    return view;
}

void *game_respond(void *g , char *action){
    Game_MakeMoveChar(g , action);
    return g;
}



// where view(model) returns a string reprsenting a visual view of the model type
// respond(model , action) applies the action to model and returns the new model state    
// in respond, it can also modify the current model and simple return the same model but with the action applied 
void controller( char* (*view)(void*) ,  void* (*respond)(void* , char*) , void *model){

    while(true){
        //displaying view:
        char *temp = (*view)(model);
        printf("%s", temp);
        free(temp);

        //taking input
        temp = (char*) malloc( max_input_len * sizeof(char) );
        scanf("%s" , temp);
        //some sort of quit condition
        if(strcmp(temp,"quit")==00 || strcmp(temp,"Quit")==00){
            break;
        }

        //applying input
        model = (*respond)(model,temp);
        free(temp);
    }

    return;
}

int main(void){

    // FILE *fp = fopen("Game/Setting/Horde.txt", "r");
    FILE *fp = fopen("Game/Setting/Defualt.txt", "r");

    game *g = Game_New(fp);
    fclose(fp);
    printf("Now starting chess game, at any point enter \"quit\" to end the program\n");
    controller( &game_view , &game_respond , (void*) g );

    return 0;
}