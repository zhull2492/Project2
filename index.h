// author: Charles Smith <cas275@pitt.edu>
//
// A simple indexer
#ifndef index_h
#define index_h

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_WORD_LENGTH 50

struct word_list{
    char word[MAX_WORD_LENGTH + 1];
    int count;
    struct word_list * next;
};

struct word_list* index_string(char input[]);

#ifdef __cplusplus
}
#endif
#endif
