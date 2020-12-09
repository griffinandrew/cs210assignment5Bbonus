#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#ifdef IMP
#include "reference_stackADT.h"
#else
#include "stackADT.h"
#endif

#include "memtrace.h"

// maximum size of a single command 
#define MAX_INPUT_SIZE 4096
// maximum size of a operation
#define MAX_OP_SIZE 64


void print_command_help();
int process(char *input, Stack dataStack, Stack opStack);
bool is_int(char *);
int runOperation(char *op, Stack dataStack);
int runCloseParen(Stack dataStack, Stack opStack);
bool higherPriority(char *op1, char *op2);

void error_msg_extraData(char *cmd) {
  printf("ERROR: %s: found data left over!\n", cmd);
}

void error_msg_badCommand(char *cmd) {
  printf("ERROR: bad command!\n");
}

void error_msg_opStackNotEmpty(char *cmd) {
  printf("ERROR: %s: not able to process all operations\n", cmd);
}

void error_msg_missingResult(char *cmd) {
  printf("ERROR: %s: no result!\n", cmd);
}

void error_msg_opMissingArgs(char *op) {
  printf("ERROR: op %s: empty stack need two arguments: found none.\n", op);
}

void error_msg_divByZero(void) {
  printf("Error: Division by zero!\n");
}

void error_msg_badOp(char *op) {
  printf("Error: Unrecognized operator: %s!\n", op);
}


//The main function will set up your stacks, take input from the command line, and call functions to calculate the result of
//the input
int main(int argc, char *argv[]) 
{
  Stack dataStack;
  Stack opStack;
  char *command = NULL;
  int max_input = MAX_INPUT_SIZE;
  int *res;
  dataStack = Stack_create();
  opStack = Stack_create();
  int run_op= 0;
  int j =0;


  command = (char*) malloc(sizeof(char));

  while(j++ < 1){ 
    printf("%s", fgets(command, max_input, stdin)); //get the input
    if(command == NULL){
      break;
    }
    if (process(command, dataStack, opStack) == 0){
      while(!Stack_is_empty(opStack)){
        char *val_top = Stack_pop(opStack); 
        run_op = runOperation(val_top, dataStack);
        free(val_top);
        val_top = NULL;
        if (run_op == -1){
          break;
        }
      }
      if(run_op == 0){ //if run op did not report an error
          if(!Stack_is_empty(opStack)){
            error_msg_opStackNotEmpty(command);
          }
          else if (Stack_is_empty(dataStack)){
            error_msg_missingResult(command);
          }
          else{
            res = Stack_pop(dataStack); //get result form top of stack
            if (!Stack_is_empty(dataStack)){
              error_msg_extraData(command);
            }
            else{
              printf("= %d\n", *res); //print the result
            }
            free(res); 
            res = NULL; //free the result
          }
        } 
      }
    Stack_make_empty(dataStack); //empoty stacks fotr next op
    Stack_make_empty(opStack);
  }
  Stack_destroy(dataStack); //destroy the stacks and free command
  Stack_destroy(opStack);
  free(command);
  command = NULL;
  return 0;
}

/***********************************************************************
 This is the main skeleton for processing a command string:
   You main task is to get it working by adding the necressary memory
   allocations and deallocations.  The rest of the logic is taken care
   of for you.  See writeup for and explanation of what this function 
   does.
***********************************************************************/

// The process function should read through the input piece by piece and put the data terms on the data stack and the
//operators on the operator stack. 
//There are two cases that make this function slightly more complex. First, if a close parenthesis is encountered, the
//function should attempt to call runOperation until the open parenthesis is found on the operator stack. Second, if the
//operator to be added to the stack has lower priority in the order of operations than the operator on the top of the stack,
//the function should call runOperation until this is no longer the case.
int
process(char *command, Stack dataStack, Stack opStack){
  char delim[] = " ";
  int *data = NULL;
  char *operation = NULL;
  int rc = 0;

  char* token = strtok(command, delim);

  while(token != NULL)
  { 
    assert(data == NULL && operation == NULL); //ensure they are both null
    data = (int*)malloc(sizeof(int)); //allocate space for data
    if( sscanf(token, "%d", data) == 1)
    {
      Stack_push(dataStack, data);
      data = NULL;
    }
    else //is operation
    {
      operation = (char*)malloc(sizeof(char) * MAX_OP_SIZE); //allocate space for operation
      if(sscanf(token, "%s", operation) == 1) 
      {
        if(strcmp(operation,")") == 0)
        {
          rc= runCloseParen(dataStack, opStack); //if op is ) run runclose
          if(rc < 0) //if rc fails break loop
          { 
            break;
          }
        }
        else if(strcmp(operation,"(") == 0)
        {
          Stack_push(opStack, operation); //if op is ( push that to stack
          operation = NULL;
        }
        else 
        {
          while(1) 
          {
            if(Stack_is_empty(opStack)) //if stack empty break
            {
              break;
            }
            char* val_top = Stack_pop(opStack);
            if(higherPriority(val_top, operation)) //if val top has higher priority 
            {
              rc = runOperation(val_top, dataStack);
              free(val_top);
              val_top = NULL;
              if(rc < 0) //if run op fails exit
              {
                goto done;
              }
            }
            else
            {
              Stack_push(opStack, val_top); //if doesn't push back to top
              break;
            }
          }
          Stack_push(opStack, operation); //push op to top of stack
          operation = NULL;
        }
      }
    }
    if (data){ //free data if allocated
      free(data);
      data = NULL;
    }
    if(operation){ ///free operation if allocated 
      free(operation); 
      operation = NULL;
    }
    token = strtok(NULL, delim);
  }
  done: //ensure they are freed
  if(data != NULL){
    free(data);
    data = NULL;
  }
  if(operation != NULL){
    free(operation);
    operation = NULL;
  }
  return rc;
}



//This function will be called when we encounter a close parenthesis. It should call runOperation with the top of the opStack
//until we find the corresponding open parenthesis, or we run out of operators on the stack.
int
runCloseParen(Stack dataStack, Stack opStack) {
  int rc = 0;
  char *op = NULL;
  int empty =0;
  int runOp_fail =0;
  char* val_top = NULL;

  while(1){
    if(Stack_is_empty(opStack)){ //if stack is epty break
      empty =1;
      break;
    }
    char* val_top = Stack_pop(opStack);
    if(strcmp(val_top,"(") == 0){ //if ( break
      break;
    }
    else{
      rc = runOperation(val_top, dataStack); //run the operation
      if (rc != 0){
        runOp_fail = 1;
        break;
      }
    }
  }

/*
  if (empty == 1){
    printf("ERROR stack empty");
  }
  if(runOp_fail == 1){
    printf("ERROR runOP fail");
  }
  */

  free(val_top);
  val_top = NULL;
  return rc;
}

int
getPriority(char* op)
{
  if(!strcmp(op,"*") || !strcmp(op, "/")) return 2;
  if(!strcmp(op,"+") || !strcmp(op, "-")) return 1;
  return 0;
}

_Bool
higherPriority(char *oldOp, char *newOp)
{
  return getPriority(oldOp) >= getPriority(newOp);
}

// This function executes the specified operation 
//  It's arguments are the first two values on the data stack
//  You must carefully analyize it and add the necessary code
//  to allocate and deallocte the necessary memory items 

//This function should attempt to get two terms from the data stack and perform the operator given as an argument with
//those two data terms. The function should fail if there are not enough terms to perform the operation.
int
runOperation(char *op, Stack dataStack)
{
  int* data1;
  int* data2;
  int* result = (int*)malloc(sizeof(int) * sizeof(dataStack));

  if(Stack_is_empty(dataStack)){ //if empty no data to pop
    error_msg_opMissingArgs(op);
    return -1;
  }
  data1 = Stack_pop(dataStack); //pop off top 
  if(Stack_is_empty(dataStack)){ //check if data to pop
    error_msg_opMissingArgs(op);
    return -1;
  }
  data2 = Stack_pop(dataStack); //pop off data
  if(strcmp(op,"+") ==0){ //if the operation is add add
    *result = *data2 + *data1; 
    Stack_push(dataStack, result); 
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    return 0;
  }
  else if(strcmp(op,"*") ==0){ //if the operation is multiply multiply
    *result = *data2 * *data1;
    Stack_push(dataStack, result);
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    return 0;
  }
  else if(strcmp(op,"/") ==0){
    if(*data1 == 0){ //if dividing by 0 free data and return error message
      free(data1);
      data1 = NULL;
      free(data2);
      data2 = NULL;
      free(result);
      result = NULL;
      error_msg_divByZero();
      return -1;
    }
    *result = *data2 / *data1; //if not divide by 0 do the division
    Stack_push(dataStack, result);
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    return 0;
  }
  else if(strcmp(op,"-") == 0){ //if the operation is subtraction subtract
    *result = *data2 - *data1;
    Stack_push(dataStack, result); 
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    return 0;
  }
  else{
    error_msg_badOp(op); //otherwise bad op
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    return -1;
  }
}
