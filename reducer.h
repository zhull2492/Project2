#ifndef _REDUCER_
#define _REDUCER_

#include "index.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

void sortList(word_list * head);
word_list * mergeIndex(word_list * masterlist, word_list *newlist);
void sortListN(word_list *head, int N);
#endif
