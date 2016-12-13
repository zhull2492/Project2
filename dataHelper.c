// helps with the data nodes
//
#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include "llNode.h"

#if 0
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
#endif

int insertWord(char* inword, char* docName, int count, struct indexNode *rootN){

    struct indexNode * newFileNode;// = calloc(1, sizeof(struct indexNode));
    struct indexNode * newRangeNode;
    struct indexNode * newWordNode;
    struct indexNode * current;

    if (!strlen(rootN->word)) {
	// NOTHING IN LIST
	newFileNode = calloc(1, sizeof(struct indexNode));
	newWordNode = calloc(1, sizeof(struct indexNode));
	strcpy(newFileNode -> word, docName);
	newFileNode->word[strlen(docName)] = '\0';
	newFileNode -> count = count;
	strcpy(newWordNode -> word, inword);
	newWordNode -> word[strlen(inword)] = '\0';
	newWordNode -> count = 1;
	newFileNode -> next = NULL;
	newFileNode -> list = NULL;
	newWordNode -> next = NULL;
	newWordNode -> list = newFileNode;

	strcpy(rootN -> word, inword);
	rootN -> word[strlen(inword)] = '\0';
	rootN -> count = 1;
	rootN -> list = newWordNode;
	rootN -> next = NULL;
    }
    else {
	//FIND RANGE TAG
	while (rootN -> next != NULL) {
	    rootN = rootN -> next;
	}
	if (strcmp(rootN->list->word, inword) < 0) {
	    //ADD FIRST
	    newWordNode = calloc(1, sizeof(struct indexNode));
	    newFileNode = calloc(1, sizeof(struct indexNode));
	    strcpy(newFileNode -> word, docName);
	    newFileNode->word[strlen(docName)] = '\0';
	    newFileNode -> count = count;
   	    strcpy(newWordNode -> word, inword);
	    newWordNode -> word[strlen(inword)] = '\0';
	    newWordNode -> count = 1;
  	    newFileNode -> next = NULL;
	    newFileNode -> list = NULL;
	    newWordNode -> next = rootN->list;
 	    newWordNode -> list = newFileNode;
	    rootN->list = newWordNode;
	    strcpy(rootN->word, inword);
	    rootN->word[strlen(inword)] = '\0';
	    rootN->count++;
	}
	else {
	    current = rootN->list;
	    while (current != NULL) {
		if (!strcmp(current->word, inword)){
		    //EQUAL
		    // NEED TO CHECK FILE
		    struct indexNode *filecrnt = current->list;
		    if (!strcmp(filecrnt->word, docName)){
			filecrnt->count += count;
			break;
		    }
		    while (filecrnt != NULL) {
		         if (strcmp(docName, filecrnt->word) < 0) {
			    //ADD FIRST
			    newFileNode = calloc(1, sizeof(struct indexNode));
			    strcpy(newFileNode -> word, docName);
			    newFileNode->word[strlen(docName)] = '\0';
			    newFileNode -> count = count;
			    newFileNode -> next = filecrnt;
			    newFileNode -> list = NULL;
			    current->list = newFileNode;
			    break;
		        }
		        else if (filecrnt -> next == NULL) {
			    //ADD TO END
			    newFileNode = calloc(1, sizeof(struct indexNode));
			    strcpy(newFileNode -> word, docName);
			    newFileNode->word[strlen(docName)] = '\0';
			    newFileNode -> count = count;
			    newFileNode -> next = NULL;
			    newFileNode -> list = NULL;
			    filecrnt -> next = newFileNode;
			    break;			
		        }
		        else if (strcmp(docName, filecrnt -> next -> word) < 0) {
			     //ADD MIDDLE
			    newFileNode = calloc(1, sizeof(struct indexNode));
			    strcpy(newFileNode -> word, docName);
			    newFileNode->word[strlen(docName)] = '\0';
			    newFileNode -> count = count;
			    newFileNode -> next = filecrnt -> next;
			    newFileNode -> list = NULL;
			    filecrnt -> next = newFileNode;
			    break;
		        }
		        else {
			    filecrnt = filecrnt -> next;
		        }
		    }
		    break;
		}
		else if (current->next == NULL) {
		    //ADD TO END
		    newFileNode = calloc(1, sizeof(struct indexNode));
		    newWordNode = calloc(1, sizeof(struct indexNode));
	    	    strcpy(newFileNode -> word, docName);
		    newFileNode->word[strlen(docName)] = '\0';
		    newFileNode -> count = count;
	   	    strcpy(newWordNode -> word, inword);
		    newWordNode -> word[strlen(inword)] = '\0';
		    newWordNode -> count = 1;
	  	    newFileNode -> next = NULL;
		    newFileNode -> list = NULL;
		    newWordNode -> next = NULL;
 		    newWordNode -> list = newFileNode;
		    current -> next = newWordNode;
		    rootN->count++;
		    break;
		}
		else if (strcmp(inword, current->next->word) < 0) {
		    // ADD TO MIDDLE
		    newFileNode = calloc(1, sizeof(struct indexNode));
		    newWordNode = calloc(1, sizeof(struct indexNode));
	    	    strcpy(newFileNode -> word, docName);
		    newFileNode->word[strlen(docName)] = '\0';
		    newFileNode -> count = count;
	   	    strcpy(newWordNode -> word, inword);
		    newWordNode -> word[strlen(inword)] = '\0';
		    newWordNode -> count = 1;
	  	    newFileNode -> next = NULL;
		    newFileNode -> list = NULL;
		    newWordNode -> next = current->next;
 		    newWordNode -> list = newFileNode;
		    current -> next = newWordNode;
		    rootN->count++;
		    break;
		}
		else {
		    current = current -> next;
		}
	    }
	}
    }

    if (rootN->count > MAX_DEPTH){
	//SPLIT
    }
#if 0 
    
    struct listNode* newNode = calloc(0, sizeof(struct listNode));
    strcpy(newNode->word, inword);
    strcpy(newNode->document, docName);
    newNode->count = count;

#if 0
    if (rootN == NULL) {
	// root is null, Let's start fresh	
	struct rootNode * newRoot = calloc(0, sizeof(struct rootNode));
	strcpy(newRoot->word, inword);
	*rootN = newRoot;
    
	newRoot->list = newNode;
	return 0;
    }
#endif    

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
	*rootN = newRoot;
    
	newRoot->list = newNode;
    
#endif
    return 0;
}

