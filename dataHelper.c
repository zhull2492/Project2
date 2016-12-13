// helps with the data nodes
//
#include<stdlib.h>
#include<stdio.h>
#include "llNode.h"

// RETURNS NULL IF NOT FOUND
struct listNode * findWord(char * sword, struct rootNode * root){
    
    while(root != NULL){
        //if the word is less than the next 
        if(strcmp(sword, root->next->word) < 0){
            // the word might be under the current root, let's go find it
            struct listNode* listN = root->list;
            while(listN != NULL){
                if(strcmp(sword, listN->word) == 0){
                    // we found it!
                    return listN;
                }
                else{
                    //keep looking
                    listN = listN->nextWord;
                }
            }
        }
        else{
            // the word might be under a later root, let's go find it
            root = root->next;
        }
    }

    return NULL;
}

int insertWord(char* inword, char* docName, int count, struct rootNode **rootN){
    
    
    struct listNode* newNode = calloc(0, sizeof(struct listNode));
    strcpy(newNode->word, inword);
    strcpy(newNode->document, docName);
    newNode->count = count;

    
    struct listNode* list;
    struct listNode* prev = NULL;
    struct rootNode* root = *rootN;
    
    
    while(root!=NULL){
        if(root -> next == NULL || strcmp(inword, root->next->word) <0){
            // we're going down from here
            list = root->list;
            int depth = 0;
            while(list!=NULL){
                if(strcmp(inword,list->word) == 0){
                    // add to this list
                    while(count < list->count){
                        if(list->nextWord != NULL){
                            list = list->nextDoc;
                        }
                        else{
                            // new node is list's next
                            list->nextDoc = newNode;
                            return 0;
                        }
                    }
                    // before 1st node
                    if(prev == NULL){
                        //first node under root
                        newNode ->nextWord = root->list;
                        newNode ->nextDoc = list;
                        return 0;
                    }
                    else{
                        // prev is the word before
                        newNode->nextWord = prev->nextWord;
                        prev->nextWord = newNode;
                        newNode->nextDoc = list;
                        return 0;
                    }
                }
                else if(list->nextWord == NULL){
                    // add to end of list
                    if(depth< MAX_DEPTH){
                        list->nextWord = newNode;
                        return 0;
                    }
                    // add a new rootnode
                    struct rootNode* newRoot = calloc(0, sizeof(struct rootNode));
                    strcpy(newRoot->word, inword);
                    newRoot->next = root->next;
                    root->next = newRoot;
                    newRoot->list = newNode;

                }
                else if(strcmp(inword, list->nextWord->word)<0){
                    // add between
                    if(prev == NULL){
                        newNode->nextWord == root->list;
                        root->list = newNode;
                        return 0;
                    }
                    else{
                        newNode->nextWord = prev->nextWord;
                        prev->nextWord = newNode;
                        return 0;
                    }
                    
                }
                prev = list;
                list = list->nextWord;
                depth = depth +1;
            }
        }
        else{
            struct rootNode* newRoot = calloc(0,sizeof(struct rootNode));
            strcpy(newRoot->word, inword);
            newRoot->next = root;
            rootN = &newRoot;
            newRoot->list = newNode;
            return 0;
            // add before current root
            
        }
        
    }
    // root is null, Let's start fresh
    struct rootNode * newRoot = calloc(0, sizeof(struct rootNode));
    strcpy(newRoot->word, inword);
    rootN = &newRoot;

    
    newRoot->list = newNode;
    
    return 0;
}

