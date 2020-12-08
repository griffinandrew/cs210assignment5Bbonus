#include <stdio.h>
#include <stdlib.h>
#ifdef IMP
#include "reference_stackADT.h"
#else
#include "stackADT.h"
#endif
#include "memtrace.h"


struct node{
    void* data;
    struct node *next;
};

struct stack_type{
    struct node *top;
};

//this function is used to end the program if an error occurs
static void terminate(const char *message){
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}


//this allocates space for and intializes the top of the stack
Stack Stack_create(void){

    Stack s = malloc(sizeof(struct stack_type)); //cant use size of for incomplete types
    if (s == NULL){
        terminate("Error in create: stack could not be created.\n");
    }
    s->top = NULL; //initalize to null bc empty
    return s;
}


//this function is used to free all the memory used for the stack, first it empties the stack then deallocates the stack memory
void Stack_destroy(Stack s){
    Stack_make_empty(s); //take everything out of stack
    free(s); //free the space for it 
}


//this function tells if the stack is empty or not it checks if the top is null and returns the appropriate value
bool Stack_is_empty(Stack s){
    return s->top == NULL; //is the top of the stack is null there are no entries in the stack
}


//this fucntion returns the data from the element at the top of stack if it is empty it terminates otherwise
//the data is copied from the old top then the new top is set to next, the old top is freed and then set to null to avoid memory leaks
//the data from the old top is returned
void * Stack_pop(Stack s){
    struct node *old_top;
    void* i;
    if(Stack_is_empty(s)){
        terminate("error in pop, stack is empty\n"); //stack empty cannot pop
      }
    old_top = s->top; //set one to be poped to old top
    i = old_top->data; //copy old_tops data to be returned
    s->top = old_top->next; //set top to next to remove old-top from linked list
    free(old_top); //free the allocated space for it
    old_top = NULL; //set to null to avoid leaks
    return i; //return tops data
}


//this function loops is empty to determine if at the top or not, it then checks the return value of pop to sufficiently free the allocated memory and set to null to avoid leaks
//it does this until the end of the stack is detected
void Stack_make_empty(Stack s){
    void* i;
    while(!Stack_is_empty(s)){//while still items in the stack use pop to remove them all
        i = Stack_pop(s); //pop off stack but save pointer to check if not 
        if(i!=NULL){ //if not last elememt 
            free(i); //free the allocated element space
            i = NULL; //set to null to avoid leak
        }
    }
}


//this function checks if the stack is full
//it is never full because it is a linkd list and always returns false
bool Stack_is_full(Stack s){
    return false; //stack is never full bc we are using a linked list 
}


//this function allocates space for a new node to be added to the stack if it does have suffcient space 
//the given data is assigned to the node and the new node is added to the front of the list
void Stack_push(Stack s, void *i){ 
    //seg fault is happening when allocating memory for new node
   // printf("what is being pushed %d", i);
    struct node *new_node = malloc(sizeof(struct node)); //allocate space for the node to be pushed
    if(new_node == NULL){
        terminate("error new node is NULL\n"); //if malloc returns null error occured
    }
    new_node->data = i; //set the new nodes data to given data
    new_node->next = s->top; //chain newnode to front of the list
    s->top = new_node; //assign top to newly added node
}