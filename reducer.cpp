#include "reducer.h"

using namespace std;

void swap(word_list *node1, word_list *node2);

void sortList(word_list * head) {

    word_list *current, *nextnode;

    current = head;

    cout << "SORT" << endl;

    while (current != NULL) {
	cout << "HERE1" << endl;
	nextnode = current->next;
	while (nextnode != NULL) {
	    cout << "HERE2" << endl;
	    if (strcmp(current->word, nextnode->word) > 0) {
		swap(nextnode, current);
	    }
	    nextnode = nextnode -> next;
	}
	current = current -> next;
    }

    current = head;

    while (current != NULL) {
	cout << current->word << "\t" << current->count << endl;
	current = current->next;
    }

    return;
}

word_list* mergeIndex(word_list *masterlist, word_list *newlist) {

    word_list *mptr, *nptr, *newnode;

    mptr = masterlist;
    nptr = newlist;

    cout << "merging" << endl;

    while (nptr != NULL) {
	mptr = masterlist;
	while(mptr != NULL) {
	    if (!strcmp(mptr -> word, nptr->word)) { // In master
		mptr -> count += nptr->count;
		newlist = nptr -> next;
		//DELETE
		nptr = newlist;
		break;
	    }
	    mptr = mptr -> next;
	}
	if (mptr == NULL){ // Not in master
	    newlist = nptr -> next;
	    nptr -> next = masterlist;
	    masterlist = nptr;
	    nptr = newlist;
	}
    }

    return masterlist;
}

void sortListN(word_list *head, int N) {

    word_list *current, *nextnode;

    current = head;

    cout << "SORT" << endl;

    int i;

    while (current != NULL) {
	nextnode = current->next;
	while (nextnode != NULL) {
	    if (((nextnode->word[0]-'a') % N) < ((current->word[0] - 'a') % N)) {//  strcmp(current->word, nextnode->word) > 0) {
		swap(nextnode, current);
	    }
	    nextnode = nextnode -> next;
	}
	current = current -> next;
    }

    return;

}

void swap(word_list *node1, word_list *node2) {

    char tempword[MAX_WORD_LENGTH+1];
    int tempcount;

    strcpy(tempword, node1->word);
    tempword[strlen(node1->word)] = '\0';
    tempcount = node1->count;

    strcpy(node1->word, node2->word);
    node1->word[strlen(node2->word)] = '\0';
    node1->count = node2->count;
 
    strcpy(node2->word, tempword);
    node2->word[strlen(tempword)] = '\0';
    node2->count = tempcount;

    return;
}
