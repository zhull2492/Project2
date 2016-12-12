// author: Charles Smith <cas275@pitt.edu>
//
// a simple test

#include <stdio.h>
#include <stdlib.h>
#include "index.h"

int main(void){
    char input[500];
    
    sprintf(input, "This is a test.\n Line 1, line 2. \nWhat does it all mean!?\nshe-sells-sea-shells-down-by-the;sea;shore\nCat cAt caT CAT");
    struct word_list * list = index_string(input);

    while(list != NULL){
        printf("%s, %d\n", list->word, list->count);
        list = list->next;
    }

}
