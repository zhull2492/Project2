// link list node
// and header
#ifndef llNode_H
#define llNode_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_WORD_LENGTH
#define MAX_WORD_LENGTH 50
#endif

#define MAX_DOC_LENGTH 256
#define MAX_DEPTH 50
#define MAX_BUFFER 256

struct listNode{
    char word[MAX_WORD_LENGTH + 1];
    int count;
    char document[MAX_DOC_LENGTH + 1];
    struct listNode * nextDoc;
    struct listNode * nextWord;
};

struct rootNode{
    char word[MAX_WORD_LENGTH + 1]; // +1 for '\0'
    struct rootNode * next;
    struct listNode * list;
};

struct indexNode {
    char word[MAX_BUFFER + 1];
    int count;
    struct indexNode *next;
    struct indexNode *list;
};

struct indexNode * findWord(char * sword, struct indexNode * root);

int insertWord(char* inword, char* docName, int count, struct indexNode * root);

#if 0
struct listNode * findWord(char * sword, struct rootNode * root);

int insertWord(char* inword, char* docName, int count, struct rootNode ** root);
#endif


#ifdef __cplusplus
}
#endif

#endif
