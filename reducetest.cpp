#include <stdlib.h>
#include <iostream>
#include <string>
#include "reducer.h"
#include "index.h"
#include <fstream>

using namespace std;

int main () {

    ifstream infile;
    string line;
    string word;
    int num;
    string delimiter = " ";
    word_list *head, *tail, *newnode, *current;

    infile.open("listtest.txt");

    head = NULL;
    tail = head;

    while(getline(infile, line)) {
	newnode = new word_list;
	size_t pos = line.find(delimiter);
	word = line.substr(0, pos);
	line.erase(0, pos+delimiter.length());
	num = atoi(line.c_str());
	cout << word << "\t" << num << endl;
	word.copy(newnode->word, word.length(), 0);
	newnode ->count = num;
	newnode ->next = NULL;
	if (head == NULL) {
	    head = newnode;
	    tail = head;
	}
	else {
	    tail -> next = newnode;
	    tail = newnode;
	}
    }

    cout << endl << endl;

    current = head;
    while (current != NULL) {
	cout << current ->word << "\t" << current -> count << endl;
	current = current -> next;
    }

    sortList(head);

    return 0;
}
