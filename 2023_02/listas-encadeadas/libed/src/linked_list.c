#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

//**********************structs**********************//
//****BEGIN-ESTATIC_LIST
typedef struct _estatic_singly_list{
    int size;
    int capacity;
    int *data;
}ESList;
//****END-ESTATIC_LIST

//****BEGIN-SINGLY_LIST
typedef struct _singly_node {
    int val;
    struct _singly_node *next;
}SinglyNode, SNode;

typedef struct _singly_linked_list {
    SNode *begin;
    SNode *end;
    int size;
}SinglyLinkedList, SList;
//****END-SINGLY_LIST

//****BEGIN-DOUBLY_LIST
typedef struct _doubly_node {
    int val;
    struct _doubly_node *prev;
    struct _doubly_node *next;
}DoublyNode, DNode;

typedef struct _doubly_linked_list {
    DNode *begin;
    DNode *end;
    int size;
}DoublyLinkedList, DList;

//****END-DOUBLY_LIST

//****BEGIN-CIRC_LIST
typedef struct _circ_node {
    int val;
    struct _circ_node *prev;
    struct _circ_node *next;
}CircNode, CNode;

typedef struct _circ_linked_list{
    CNode *begin;
    CNode *end;
    int size;
}CircLinkedList, CList;

//****END-CIRC_LIST

//*******************creating nodes******************//
SNode *create_SNode(int val){
    SNode *s_node   = (SNode *)calloc(1, sizeof(SNode));
    s_node->val = val;
    s_node->next = NULL;

    return s_node;
}
DNode *create_DNode(int val){
    DNode *d_node   = (DNode *)calloc(1, sizeof(DNode));
    d_node->val = val;
    d_node->prev    = d_node->next = NULL;

    return d_node;
}
CNode *create_CNode(int val){
    CNode *c_node = (CNode *)calloc(1, sizeof(CNode));
    c_node->val = val;
    c_node->prev = c_node->next = c_node;

    return c_node;
}

//*******************checking sizes******************//
bool ESList_is_full(const ESList *estatic_slist){return estatic_slist->size == estatic_slist->capacity;}
bool ESList_is_empty(const ESList *estatic_slist){return estatic_slist->size == 0;}
bool SList_is_empty(const SList *singly_list){return singly_list->size == 0;}
bool DList_is_empty(const DList *doubly_list){return doubly_list->size == 0;}
bool CList_is_empty(const CList *circ_list){return circ_list->size == 0;}
//*******************creating lists******************//
ESList *create_ESList(int capacity){
    ESList *list = (ESList *)calloc(1, sizeof(ESList));
    list->size = 0;
    list->capacity = capacity;
    list->data = (int *)calloc(capacity, sizeof(int));

    return list;
}
SList *create_SList(){
    SList *singly_list = (SList *)calloc(1, sizeof(SList));
    singly_list->begin = singly_list->end = NULL;
    singly_list->size  = 0;

    return singly_list;
}
DList *create_DList(){
    DList *doubly_list = (DList *)calloc(1, sizeof(DList));
    doubly_list->begin = doubly_list->end = NULL;
    doubly_list->size  = 0;

    return doubly_list;
}
CList *create_CList(){
    CList *circ_list = (CList *)calloc(1, sizeof(CList));
    circ_list->begin = circ_list->end = NULL;
    circ_list->size  = 0;

    return circ_list;
}

//******************destroying nodes*****************//
void destroy_CNode(CNode **ref_circ_node){
    free(*ref_circ_node);
    *ref_circ_node = NULL;
}
//******************destroying lists*****************//
void destroy_ESList(ESList **ref_estatic_slist){
    ESList *p = *ref_estatic_slist;

    free(p->data);
    free(p);
    *ref_estatic_slist = NULL;

}
void destroy_SList(SList **ref_singly_list){
    SList *singly_list = *ref_singly_list;
    SNode *node = singly_list->begin;

    while (node != NULL) {
        SNode *aux = node;
        node = node->next;
        free(aux);
    }

    free(singly_list);
    *ref_singly_list = NULL;
}

void destroy_DList(DList **ref_doubly_list){
    DList *doubly_list = *ref_doubly_list;
    DNode *node = doubly_list->begin;

    while (node != NULL){
        DNode *aux = node;
        node = node->next;
        free(aux);
    }
    
    free(doubly_list);
    *ref_doubly_list = NULL;
}
void destroy_CList(CList **ref_circ_list){
    CList *L = *ref_circ_list;
    CNode *p = L->begin;
    CNode *aux = NULL;

    while (p != L->end){
        aux = p;
        p = p->next;
        destroy_CNode(&aux);
    }

    destroy_CNode(&p);
    free(L);
    *ref_circ_list = NULL;
    
}
void destroy_CList_v2(CList **ref_circ_list){
    CList *L = *ref_circ_list;
    CNode *p = L->begin;
    CNode *aux = NULL;

    for (int i = 0; i < L->size; i++){
        aux = p;
        p = p->next;
        destroy_CNode(&aux);
    }

    free(L);
    *ref_circ_list = NULL;

}

//******************accessing values*****************//

//*******************adding to lists*****************//
//****BEGIN-ESTATIC_LIST
void append_in_ESList(ESList *estatic_slist, int val){
    if (ESList_is_full(estatic_slist)){
        fprintf(stderr, "ERROR in 'append'\n");
        fprintf(stderr , "List is full'\n");
        exit(EXIT_FAILURE);
    }
    else{
        estatic_slist->data[estatic_slist->size] = val;
        estatic_slist->size++;
    }
    
}
//****END-ESTATIC_LIST

//****BEGIN-SINGLY_LIST
void add_begin_SList(SList *singly_list, int val){
    SNode *p = create_SNode(val);

    SList_is_empty(singly_list) ? (singly_list->end = p) : (p->next = singly_list->begin);
    
    singly_list->begin = p;
    singly_list->size++;
}

void add_end_SList(SList *singly_list , int val){
    SNode *p = create_SNode(val);
    SList_is_empty(singly_list) ? (singly_list->begin = singly_list->end = p) : (singly_list->end->next = p, singly_list->end = p);
    singly_list->size++;
}
//****END-SINGLY_LIST

//****BEGIN-DOUBLY_LIST
void add_begin_DList(DList *doubly_list, int val){
    DNode *p = create_DNode(val);
    p->next = doubly_list->begin;

    DList_is_empty(doubly_list) ? (doubly_list->end = p) : (doubly_list->begin->prev = p);

    doubly_list->begin = p;
    doubly_list->size++;
}

void add_end_DList(DList *doubly_list, int val){
    DNode *p = create_DNode(val);
    p->prev = doubly_list->end;

    DList_is_empty(doubly_list) ? (doubly_list->begin = p) : (doubly_list->end->next = p);

    doubly_list->end = p;
    doubly_list->size++;
}
//****END-DOUBLY_LIST

//****BEGIN-CIRC_LIST
void add_begin_CList(CList *circ_list, int val){
    CNode *p = create_CNode(val);
    // printf("adding value %d in the header...\n", val);
    if (CList_is_empty(circ_list)){
        circ_list->end = p;
    }
    else{
        p->next = circ_list->begin;
        circ_list->begin->prev = p;
        p->prev = circ_list->end;
        circ_list->end->next = p;
    }
    circ_list->begin = p;
    circ_list->size++;
}
void add_end_CList(CList *circ_list, int val){
    // printf("adding value %d in the tail...\n", val);
    CNode *p = create_CNode(val);

    if (CList_is_empty(circ_list)){
        circ_list->begin = p;
    }
    else{
        circ_list->end->next = p;
        p->prev = circ_list->end;
        circ_list->begin->prev = p;
        p->next = circ_list->begin;
    }
    circ_list->end = p;
    circ_list->size++;
}

//****END-CIRC_LIST

//********************getting values*****************//
int lenDList(const DList *doubly_list){
    return doubly_list->size;
}
int getValDList(const DList *doubly_list, int pos){
    if (DList_is_empty(doubly_list) || pos > doubly_list->size || pos < 0){
        return INT_MIN;
    }
    else{
        DNode *p = doubly_list->begin;

        for (int i = 0; i < pos; i++){
            p = p->next;
        }

        return p->val;
    }
}
//********************counting numbers*****************//
bool isPrime(int num){
    if (num <= 1) {
        return false;
    }

    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return false;
        }
    }

    return true;
}

bool findValueDlist(const DList *doubly_list, int val){
    DNode *p = doubly_list->begin;

    while (p){
        if (p->val == val){
            return true;
        }
        
        p = p->next;
    }

    return false;
}

bool findUniqueValueDlist(const DList *doubly_list, int val){
    DNode *p = doubly_list->begin;
    int count = 0;

    while (p){
        if (p->val == val){
            count++;
        }
        if (count >= 2){
            break;
        }
        
        p = p->next;
    }
    
    return count <= 1;
}

int countEvenNumbersFromDList(const DList *doubly_list){
    int count = 0;

    if (DList_is_empty(doubly_list)){
        return count;
    }
    DNode *p = doubly_list->begin;

    while (p){
        if (p->val % 2 == 0){
            count++;
        }
        
        p = p->next;
    }
    
    return count;
}

float meanDList(const DList *doubly_list){
    float mean = 0;

    if (DList_is_empty(doubly_list)){
        return mean;
    }
    DNode *p = doubly_list->begin;

    while (p){
        mean += p->val;
        p = p->next;
    }
    mean /= doubly_list->size;

    return mean;
}
int maxValueFromDList(const DList *doubly_list){
    int value = INT_MIN;
    DNode *p = doubly_list->begin;

    if (DList_is_empty(doubly_list)){
        return value;
    }

    while (p){
        if (p->val > value){
            value = p->val;
        }
        
        p = p->next;
    }

    return value;
}

int minValueFromDList(const DList *doubly_list){
    int value = INT_MAX;
    DNode *p = doubly_list->begin;

    if (DList_is_empty(doubly_list)){
        return value;
    }

    while (p){
        if (p->val < value){
            value = p->val;
        }
        
        p = p->next;
    }

    return value;
}

int findIndexOfMaxValueInDList(const DList *doubly_list){
    int max_index = -1;

    if (DList_is_empty(doubly_list)){
        return max_index;
    }

    int current_index = 0;
    int max_value = INT_MIN;
    
    DNode *p = doubly_list->begin;

    while (p){
        if (p->val > max_value){
            max_value = p->val;
            max_index = current_index;
        }
        
        p = p->next;
        current_index++;
    }

    return max_index;
}

int findIndexOfMinValueInDList(const DList *doubly_list){
    int min_index = -1;

    if (DList_is_empty(doubly_list)){
        return min_index;
    }

    int min_value = INT_MAX;
    int current_index = 0;
    
    DNode *p = doubly_list->begin;

    while (p){
        if (p->val < min_value){
            min_value = p->val;
            min_index = current_index;
        }
        
        p = p->next;
        current_index++;
    }

    return min_index;
}

int countNodesAboveXInDList(const DList *doubly_list, int x){
    int count = 0;
    
    if (DList_is_empty(doubly_list)){
        return count;
    }

    DNode *p = doubly_list->begin;

    while (p) {
        if (p->val > x) {
            count++;
        }

        p = p->next;
    }

    return count;
}

int sumOfDList(const DList *doubly_list){
    int sum = 0;

    if (DList_is_empty(doubly_list)){
        return sum;
    }

    DNode *p = doubly_list->begin;

    while (p) {
        sum += p->val;
        p = p->next;
    }

    return sum;
}

int countPrimesInDList(const DList *doubly_list){
    int count = 0;
    DNode *p = doubly_list->begin;

    while (p) {
        if (isPrime(p->val)) {
            count++;
        }

        p = p->next;
    }

    return count;
}
//******************copying lists****************//

DList* copyDList(const DList *doubly_list){
    DList *copyDList = create_DList();
    DNode *p = doubly_list->begin;

    while (p){
        add_end_DList(copyDList, p->val);
        p = p->next;
    }
    
    return copyDList;
}

DList* copyInvertedDList(const DList *doubly_list){
    DList *copyDList = create_DList();
    DNode *p = doubly_list->begin;

    while (p){
        add_begin_DList(copyDList, p->val);
        p = p->next;
    }
    
    return copyDList;
}

/*
DList* copyUniquesDList(const DList *doubly_list){
    DList *copylist = create_DList();
    DNode *p = doubly_list->begin;

    while (p){
        if (findUniqueValueDlist(copylist, p->val)){
            add_end_DList(copylist, p->val);
        }        
        p = p->next;
    }
    
    return copylist;
}
*/

DList* copyUniquesDList(const DList *doubly_list){
    DList *copylist = create_DList();
    DNode *p = doubly_list->begin;

    while (p){
        if (findUniqueValueDlist(doubly_list, p->val)){
            add_end_DList(copylist, p->val);
        }        
        p = p->next;
    }
    
    return copylist;
}


//*******************checking sort******************//
bool sortedDList(const DList *doubly_list){
    DNode *p = doubly_list->begin->next;
    int ascending;
    int descending;
    ascending = descending = doubly_list->size;


    while (p){
        if (p->val < p->prev->val){
            ascending --;
        }
        if (p->val > p->prev->val){
            descending --;
        }

        p = p->next;
    }
    
    return (ascending == doubly_list->size || descending == doubly_list->size);
}

//******************combined operations****************//
DList *unionDList(const DList *doubly_list1, const DList *doubly_list2){
    DList *list = copyDList(doubly_list1);
    DNode *p = doubly_list2->begin;

    while (p){
        if (!findValueDlist(list, p->val)){
            add_end_DList(list, p->val);
        }
        p = p->next;
    }
    return list;
}

DList *intersectionDList(const DList *doubly_list1, const DList *doubly_list2){
    DList *list = create_DList();
    DNode *p = doubly_list1->begin;

    while (p){
        if (findValueDlist(doubly_list2, p->val)){
            add_end_DList(list, p->val);
        }
        p = p->next;
    } 
    return list;
}

//******************concatenating lists****************//
ESList *concat_ESList(const ESList *estatic_slist01, const ESList *estatic_slist02){
    int capacity = estatic_slist01->size + estatic_slist02->size;
    ESList *newlist = create_ESList(capacity);

    for (int i = 0; i < estatic_slist01->size; i++){
        append_in_ESList(newlist, estatic_slist01->data[i]);
        // newlist->size++;
    }

    for (int i = 0; i < estatic_slist02->size; i++){
        append_in_ESList(newlist, estatic_slist02->data[i]);
        // newlist->size++;
    }

    return newlist;
}

SList *concat_SList(const SList *singly_list_01, const SList *singly_list_02){
    SList *L = (SList *)calloc(1, sizeof(SList));

    if(!SList_is_empty(singly_list_01)){
        SNode *p = singly_list_01->begin;

        while (p !=NULL){
            add_end_SList(L, p->val);
            p = p->next;
        }       
    }
    if(!SList_is_empty(singly_list_02)){
        SNode *p = singly_list_02->begin;

        while (p !=NULL){
            add_end_SList(L, p->val);
            p = p->next;
        }       
    }

    return L;
}

DList *concat_DList(const DList *doubly_list_01, const DList *doubly_list_02){
    DList *L = create_DList();

    if (!DList_is_empty(doubly_list_01)){
        DNode *p = doubly_list_01->begin;

        while (p !=NULL){
            add_end_DList(L, p->val);
            p = p->next;
        } 
    }
    if (!DList_is_empty(doubly_list_02)){
        DNode *p = doubly_list_02->begin;
        
        while (p !=NULL){
            add_end_DList(L, p->val);
            p = p->next;
        } 
    }
    
    return L;
}


CList *concat_CList(const CList *circ_list_01, const CList *circ_list_02){
    CList *L = create_CList();

    if (!CList_is_empty(circ_list_01)){
        CNode *p = circ_list_01->begin;

        for (int i = 0; i < circ_list_01->size; i++){
            add_end_CList(L, p->val);
            p = p->next;
        }

    }
    if (!CList_is_empty(circ_list_02)){
        CNode *p = circ_list_02->begin;
        
        for (int i = 0; i < circ_list_02->size; i++){
            add_end_CList(L, p->val);
            p = p->next;
        }
    }

    return L;
}
//******************removing to lists****************//
//****BEGIN-ESTATIC_LIST
void removen_begin_ESList(ESList *estatic_slist, int n_items){
    if (ESList_is_empty(estatic_slist)){
        fprintf(stderr, "ERROR in 'removen_begin_ESList'\n");
        fprintf(stderr , "List is empty\n");
        //exit(EXIT_FAILURE);
    }
    else if (n_items >= estatic_slist->capacity){
        estatic_slist->size = 0;
    }
    else{
        for (int i = 0; i < estatic_slist->size - n_items; i++){
            estatic_slist->data[i] = estatic_slist->data[i + n_items];
        }
        estatic_slist->size-= n_items;
    }   
    
}
void removen_end_ESList(ESList *estatic_slist, int n_items){
    if (ESList_is_empty(estatic_slist)){
        fprintf(stderr, "ERROR in 'removen_end_ESList'\n");
        fprintf(stderr , "List is empty\n");
    }
    else if (n_items >= estatic_slist->capacity){
        estatic_slist->size = 0;
    }
    else{
        estatic_slist->size -= n_items;
    }
    
    
}
//****END-ESTATIC_LIST

//****BEGIN-SINGLY_LIST
void removel_SList(SList *singly_list, int val) {
    if (!SList_is_empty(singly_list)) {
        // caso 1 - o elemento está na cabeça da lista
        if (singly_list->begin->val == val) {
            SNode *pos = singly_list->begin;

            // a lista tem só um nó
            if (singly_list->begin == singly_list->end) {
                singly_list->end = NULL;
            }

            singly_list->begin = singly_list->begin->next;
            free(pos);

            singly_list->size--;
        }
        // caso 2 - o elemento está no meio da lista
        // caso 3 - o elemento está no final da lista
        else {
            SNode *prev = singly_list->begin;
            SNode *pos = singly_list->begin->next;

            while (pos != NULL && pos->val != val) {
                prev = prev->next;
                pos = pos->next;
            }

            if (pos != NULL) {
                prev->next = pos->next;

                // caso 3 - removendo o último nó da lista
                if (pos->next == NULL) {
                    singly_list->end = prev;
                }
                
                free(pos);
                singly_list->size--;
            }
        }
    }
}
void removeall_SList(SList *singly_list) {
    while (!SList_is_empty(singly_list)) {
        removel_SList(singly_list, singly_list->begin->val);
    }
}
void removen_begin_SList(SList *singly_list, int n_items) {
    while (n_items > 0 && !SList_is_empty(singly_list)) {
        removel_SList(singly_list, singly_list->begin->val);
        n_items--;
    }
}
void removen_end_SList(SList *singly_list, int n_items) {
    if (SList_is_empty(singly_list)){
        fprintf(stderr, "ERROR in 'removen_end_SList'\n");
        fprintf(stderr , "List is empty\n");
    }
    else if (n_items == singly_list->size){
        removeall_SList(singly_list);
    }
    else{
        for (int i = 0; i < n_items; i++){
            if (singly_list->size == 1) {
                free(singly_list->begin);
                singly_list->begin = singly_list->end = NULL;
                singly_list->size = 0;
            } else {
                SNode *p = singly_list->begin;
                SNode *prev = NULL;

                while (p->next != NULL) {
                    prev = p;
                    p = p->next;
                }

                free(p);
                if (prev != NULL) {
                    prev->next = NULL;
                    singly_list->end = prev;
                }
                singly_list->size--;
            }
            
        }
        
    }
    
}
//****END-SINGLY_LIST

//****BEGIN-DOUBLY_LIST
void removel_DList(DList *doubly_list, int val){
    if (!DList_is_empty(doubly_list)){
        DNode *p = doubly_list->begin;
        // val = l->begin->val

        if (doubly_list->begin->val == val){
            doubly_list->begin = p->next;
            doubly_list->size == 1 ? (doubly_list->end = NULL) : (doubly_list->begin->prev = NULL);
            
            free(p);
            p = NULL;
            doubly_list->size--;
            
        }
        else{
            p = p->next;
            while (p != NULL){
                if(p->val == val){
                    p->prev->next = p->next;
                    // val = l->end->val
                    // other_posicion
                    doubly_list->end == p ? (doubly_list->end = p->prev) : (p->next->prev = p->prev);

                    free(p);
                    p = NULL;
                    doubly_list->size--;    

                }else{p = p->next;}
        
            }
        }
        
    
    }
    else{puts("Lista vazia");}    
}
void removeall_DList(DList *doubly_list) {
    while (!DList_is_empty(doubly_list)) {
        removel_DList(doubly_list, doubly_list->begin->val);
    }
}

void removen_begin_DList(DList *doubly_list, int n_items){
    while (n_items > 0 && !DList_is_empty(doubly_list)) {
        removel_DList(doubly_list, doubly_list->begin->val);
        n_items--;
    }
}
void removen_end_DList(DList *doubly_list, int n_items){
    if (DList_is_empty(doubly_list)){
        fprintf(stderr, "ERROR in 'removen_end_DList'\n");
        fprintf(stderr , "List is empty\n");
    }
    else if (n_items >= doubly_list->size){
        removeall_DList(doubly_list);
    }
    else{
        for (int i = 0; i < n_items; i++){
            DNode *p = doubly_list->end;

            doubly_list->end = p->prev;
            doubly_list->end->next = NULL;
            free(p);
            doubly_list->size--;
        }
            
    }
        
}
//****END-DOUBLY_LIST

//****BEGIN-CIRC_LIST
void removel_CList(CList *circ_list, int val){
    // printf("removing val %d ... ", val);
    CNode *p = circ_list->begin;

    if (!CList_is_empty(circ_list)){
        int curent_size = circ_list->size;

        // case 1: element in the header
        if (val == circ_list->begin->val){
            // unique element in the list
            if (circ_list->size == 1){
                circ_list->begin = circ_list->end = NULL;
                destroy_CNode(&p);
            }
            else{
                circ_list->begin = p->next;
                circ_list->begin->prev = circ_list->end;
                circ_list->end->next = circ_list->begin;
                destroy_CNode(&p);
            }
            circ_list->size--;
            // puts("SUCCESS: Process finished\n");
        }
        else{
            p = p->next;
            
            while (p != circ_list->begin){
                if(p->val == val){
                    // case 2: element in the meedle and case 3: element in the tail
                    if(p == circ_list->end){
                        circ_list->end = p->prev;
                    }

                    p->prev->next = p->next;
                    p->next->prev = p->prev;

                    destroy_CNode(&p);
                    circ_list->size--;
                    // puts("SUCCESS: Process finished\n");
                    break;
                }
                else{
                    p = p->next;
                }
            }         
        }
        if (curent_size == circ_list->size){
            printf("FAILURE: val %d was not in funded in the list\n\n", val);
        }
    }
    else{
        puts("FAILURE: List is empty\n");
    }
}
void removeall_CList(CList *circ_list){
    while (!CList_is_empty(circ_list)){
        removel_CList(circ_list, circ_list->begin->val);
    }
}
void removen_begin_CList(CList *circ_list, int n_items){
    while (n_items > 0 && !CList_is_empty(circ_list)) {
        removel_CList(circ_list, circ_list->begin->val);
        n_items--;
    }
}
void removen_end_CList(CList *circ_list, int n_items){
    if (CList_is_empty(circ_list)){
        fprintf(stderr, "ERROR in 'removen_end_CList'\n");
        fprintf(stderr , "List is empty\n");
    }
    else if (n_items >= circ_list->size){
        removeall_CList(circ_list);
    }
    else{
        for (int i = 0; i < n_items; i++){
            CNode *p = circ_list->end;

            circ_list->end = p->prev;
            circ_list->end->next = circ_list->begin;
            circ_list->begin->prev = circ_list->end;
            free(p);
            circ_list->size--;
        }
            
    }
}
//****END-CIRC_LIST

//*********************print lists*******************//

void print_ESList(const ESList *estatic_slist){
    printf("    ES->size:     %d\n", estatic_slist->size);
    printf("    ES->capacity: %d\n", estatic_slist->capacity);
    puts("---");

    for (int i = 0; i < estatic_slist->size; i++) {
        printf("[%d] = %d\n", i, estatic_slist->data[i]);

    }

    puts("");
}

void print_SList(const SList *singly_list){
    SNode *current = singly_list->begin;

    printf("SinglyList-> ");
    while (current != NULL)
    {
        printf("%d -> ", current->val);
        current = current->next;
    }
    
    printf("NULL\n    L->begin = ");
    (singly_list->begin == NULL) ? (printf("NULL\n")) : (printf("%d\n", singly_list->begin->val));
    printf("    L->end   = ");
    (singly_list->end == NULL) ? (printf("NULL\n")) : (printf("%d\n", singly_list->end->val));
    printf("    L->size  = %d\n", singly_list->size);
    puts("");
}
void print_DList(const DList *doubly_list){
    DNode *current = doubly_list->begin;

    printf("DoublyList-> ");
    while (current != NULL)
    {
        printf("%d -> ", current->val);
        current = current->next;
    }
    printf("NULL\n    L->begin = ");
    (doubly_list->begin == NULL) ? (printf("NULL\n")) : (printf("%d\n", doubly_list->begin->val));
    printf("    L->end   = ");
    (doubly_list->end == NULL) ? (printf("NULL\n")) : (printf("%d\n", doubly_list->end->val));
    printf("    L->size  = %d\n", doubly_list->size);
    puts("");
}

void print_inverted_DList(const DList *doubly_list){
    DNode *current = doubly_list->end;

    printf("DoublyList-> end -> ");
    while (current != NULL)
    {
        printf("%d -> ", current->val);
        current = current->prev;
    }
    printf("NULL\nL->begin = ");
    (doubly_list->end == NULL) ? (printf("NULL\n")) : (printf("%d\n", doubly_list->begin->val));
    printf("Size = %d\n", doubly_list->size);
    puts("");
}
void print_CList(const CList *circ_list){

    printf("L-> =");
    if (CList_is_empty(circ_list)){
        puts(" NULL");
        printf("    L->begin = NULL\n");
        printf("    L->end   = NULL\n");
    }
    else{
        CNode *p = circ_list->begin;

        do{
            printf(" %d ->", p->val);
            p = p->next; 
        } while (p != circ_list->begin);
        puts("");
        printf("    L->begin = %d\n", circ_list->begin->val);
        printf("    L->end   = %d\n", circ_list->end->val);
    }    
    printf("    L->size  = %d\n\n", circ_list->size);
    puts("");

}
void print_CList_v2(const CList *circ_list){
    //puts("print_CList_v2");
    printf("CircList-> =");
    if (CList_is_empty(circ_list)){
        puts(" NULL");
        printf("    L->begin = NULL\n");
        printf("    L->end   = NULL\n");
    }
    else{
        CNode *p = circ_list->begin;

        for (int i = 0; i < circ_list->size; i++){
            printf(" %d ->", p->val);
            p = p->next;
        }

        puts("");
        printf("    L->begin = %d\n", circ_list->begin->val);
        printf("    L->end   = %d\n", circ_list->end->val);
    }    
    printf("    L->size  = %d\n\n", circ_list->size);
    puts("");
}
void print_inverted_CList(const CList *circ_list){
    //puts("print_inverted_CList");
    printf("CircListinverted->end =");
    if (CList_is_empty(circ_list)){
        puts(" NULL");
        printf("    L->begin = NULL\n");
    }
    else{
        CNode *p = circ_list->end;

        do{
            printf(" %d ->", p->val);
            p = p->prev; 
        } while (p != circ_list->end);
        puts("");
        printf("    L->begin = %d\n", circ_list->begin->val);
        printf("    L->end   = %d\n", circ_list->end->val);
    }    
    printf("    L->size  = %d\n\n", circ_list->size);
    puts("");
}
void print_inverted_CList_v2(const CList *circ_list){
    //puts("print_inverted_CList_v2");
    printf("CircListinverted->end =");
    if (CList_is_empty(circ_list)){
        puts(" NULL");
        printf("    L->begin = NULL\n");
    }
    else{
        CNode *p = circ_list->end;

        for (int i = 0; i < circ_list->size; i++){
            printf(" %d ->", p->val);
            p = p->prev;
        }

        puts("");
        printf("    L->begin = %d\n", circ_list->begin->val);
        printf("    L->end   = %d\n", circ_list->end->val);
    }    
    printf("    L->size  = %d\n\n", circ_list->size);
    puts("");
}

