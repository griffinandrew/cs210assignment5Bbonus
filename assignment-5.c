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

int main(int argc, char *argv[]) 
{
  Stack dataStack;
  Stack opStack;
  char *command = NULL;
  int max_input = MAX_INPUT_SIZE;
  int result;
  dataStack = Stack_create();
  opStack = Stack_create();
  int run_op= 0;


command = (char*) malloc(sizeof(char));

while(1){

  if (fgets(command, max_input, stdin) == NULL){ //what should this be this causes seg fault 
    break;
  }
  printf("command %p", command);

  if(command == NULL){
    break;
  }
 /* if (process(command, dataStack, opStack) != 0) {
    error_msg_badCommand(command);
    return -1;
  }
  */
  if (process(command, dataStack, opStack) == 0){
    while(!Stack_is_empty(opStack)){
      char* val_top = Stack_pop(opStack);

      run_op = runOperation(command, dataStack);
      free(val_top);
      if (run_op !=0){
        Stack_push(opStack, val_top);
        break;
      }
      else{
        if(!Stack_is_empty(opStack)){
          error_msg_opStackNotEmpty(command);
          //break;
        }
        else if (!Stack_is_empty(dataStack)){
          error_msg_missingResult(command);
        }
        else{
          //Stack_push(dataStack, val_top); //push or pop here
          int* res = Stack_pop(dataStack);
          result = *res;
          if (!Stack_is_empty(dataStack)){
            error_msg_extraData(command);
          }
          else{
            //print result
          
            printf("= %d\n", result);
          }
        }
      }
    }
    
  }
  Stack_make_empty(dataStack);
  Stack_make_empty(opStack);
  }


  // PART B: See writup for details.
  // Your job is to implment the calculator taking care of all the dynamic
  // memory management using the stack module you implmented in Part A
  return 0;
}

/***********************************************************************
 This is the main skeleton for processing a command string:
   You main task is to get it working by adding the necressary memory
   allocations and deallocations.  The rest of the logic is taken care
   of for you.  See writeup for and explanation of what this function 
   does.
***********************************************************************/
int
process(char *command, Stack dataStack, Stack opStack){
  char delim[] = " ";
  int *data = ((void *)0);
  char *operation = ((void *)0);
  int rc = 0;

  char* token = strtok(command, delim);
  while (token != ((void *)0)) {
    token = strtok(((void *)0), delim);
  }
  while(token != NULL){
    if( sscanf(token, "%d", data) == 1){
     Stack_push(dataStack, data);
    }
    else{
      if( sscanf(token, "%c", operation) == ')'){
        //runCloseParen();
        if(runCloseParen(dataStack, opStack) != 0){
            break;
        }
        else {
          if(sscanf(token, "%c", operation) == '('){
          //if(operation == 'C'){ 
            Stack_push(opStack,operation);
          }
          else{
            while(1){
              if(Stack_is_empty(opStack)){
                break;
              }
              char* val_top = Stack_pop(opStack);
              if(higherPriority(val_top, operation) == 1){
                if (runOperation(val_top, dataStack) != 0){
                  break; //diont know if this exits outer or both
                }
              }
              else{
                Stack_push(opStack, val_top);
                break;
              }
            }
            Stack_push(opStack, operation);
          }
        }
      }
    }
  }
  token =strtok(NULL, delim);
  return rc;
}


int
runCloseParen(Stack dataStack, Stack opStack) {
  int rc = 0;
  char *op = ((void *)0);

  while(1){
    if(Stack_is_empty(opStack)){
      return -1;
      //return error(EXIT_FAILURE);
      break;
    }
    char* val_top = Stack_pop(opStack);
    if(val_top == "("){
      break;
    }
    else{
      if (runOperation(val_top, dataStack) != 0){
        return -1;
        //return error(EXIT_FAILURE);
        break;
      }
    }
  }

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
int
runOperation(char *op, Stack dataStack)
{
  int data1;
  int data2;
  int result;
  int* data_1;
  int* data_2;
  int* resultp;

  if(Stack_is_empty(dataStack)){
    error_msg_opMissingArgs(op);
    return -1;
  }
  data_1 = Stack_pop(dataStack);
  data1 = *data_1;
  if(Stack_is_empty(dataStack)){
    error_msg_opMissingArgs(op);
    return -1;
  }
  data_2 = Stack_pop(dataStack);
  data2 = *data_2;
  if(op == "+"){
    result = data2 + data1;
    Stack_push(dataStack, (int *)result); //how do i deal with these
    return 0;
  }
  else if(op == "*"){
    result = data2 * data1;
    resultp = &result;
    Stack_push(dataStack, resultp);
    return 0;
  }
  else if(op == "/"){
    if(data1 == 0){
      error_msg_divByZero();
      return -1;
    }
    result = data2 / data2;
    resultp = &result;
    Stack_push(dataStack, resultp);
    return 0;
  }
  else if(op == "-"){
    result = data2 - data1;
    resultp = &result;
    Stack_push(dataStack, resultp);  //NOTE THIS IS NOT A VOID * 
    return 0;
  }
  else{
    error_msg_badOp(op);
    return -1;
  }
}
