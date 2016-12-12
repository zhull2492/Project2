// author: Charles Smith <cas275@pitt.edu>
//
// a simple indexer

#include "index.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// our delim string
#define DELIM " .,-\n?!;:\"&()$%#~_=+"

int add_to_list(struct word_list** list, char * word);

struct word_list* index_string(char input[]){
    
    struct word_list* head = NULL;

    int i;
    char * next_word = strtok(input, DELIM);
    while(next_word != NULL){
        for(i=0; next_word[i]; i++){
            next_word[i] = tolower(next_word[i]);
        }
        printf("%s\n", next_word);
        
        add_to_list(&head, next_word);

        next_word = strtok(NULL, DELIM);
    }
    return head;

}

int add_to_list(struct word_list** list, char * word){
    if(*list == NULL){
        struct word_list* new_list = (struct word_list*) malloc(sizeof(struct word_list));
        strncpy(new_list->word, word, MAX_WORD_LENGTH);
        new_list->word[MAX_WORD_LENGTH] = '\0';
        new_list->count = 1;
        *list = new_list;
        return 0;
    }
    struct word_list* current = *list;
    struct word_list* prev = NULL;
    while(current != NULL){
        if(strcmp(word, current->word) == 0){
            current->count = current->count + 1;
            return 0;
        }
        // move to the next list entry
        prev = current;
        current = current->next;      
    }
    current = (struct word_list*) malloc(sizeof(struct word_list));
    strncpy(current->word, word, MAX_WORD_LENGTH);
    current->word[MAX_WORD_LENGTH] = '\0';
    current->count = 1;
    prev->next = current;

    return 0;
}
