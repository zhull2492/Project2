// link list node
// and header
#ifndef llNode_H
#define llNode_H

#ifndef MAX_WORD_LENGTH
#define MAX_WORD_LENGTH 50
#endif

#define MAX_DOC_LENGTH 256

struct listNode{
    char word[MAX_WORD_LENGTH + 1];
    int count;
    char document[MAX_DOC_LENGTH];
    struct listNode * nextDoc;
    struct listNode * nextWord;
};

struct rootNode{
    char word[MAX_WORD_LENGTH + 1]; // +1 for '\0'
    struct rootNode * next;
    struct listNode * list;
};

struct listNode * findWord(char * word);

#endif