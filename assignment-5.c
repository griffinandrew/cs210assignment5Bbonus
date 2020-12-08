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
  int *res;
  dataStack = Stack_create();
  opStack = Stack_create();
  int run_op= 0;
  int j =0;


  command = (char*) malloc(sizeof(char));

  while(1){ //need for control i think
      //now i get a seg fault here wtf
    printf("%s", fgets(command, max_input, stdin)); //what should this be this causes seg fault 
    //exits right here
    if(command == NULL){
      break;
    }
    if (process(command, dataStack, opStack) == 0){ //happening in process
      while(!Stack_is_empty(opStack)){
        char *val_top = Stack_pop(opStack);
        run_op = runOperation(val_top, dataStack);
        free(val_top);
        val_top = NULL;
        if (run_op == -1){
          break;
        }
      }
      if(run_op == 0){
          if(!Stack_is_empty(opStack)){
            error_msg_opStackNotEmpty(command);
          }
          else if (Stack_is_empty(dataStack)){
            error_msg_missingResult(command);
          }
          else{
            res = Stack_pop(dataStack); //res is  memory address
            //result = *res;
            if (!Stack_is_empty(dataStack)){
              error_msg_extraData(command);
            }
            else{
              printf("= %d\n", *res);
              //printf("= %d\n", &res);
              //printf("= %d\n", res);
            }
            //free(res); //excpetion occurs here
            //res = NULL;
          }
        } //end of if op ==0
      }
    //Stack_make_empty(dataStack); //not sure about this location not 100 SURE ABOUT THIS
    //Stack_make_empty(opStack);
    }
    //Stack_make_empty(dataStack); //not sure about this location
    //Stack_make_empty(opStack);
    //free(command);
    //command = NULL;
 //}
 // Stack_make_empty(dataStack);
  Stack_destroy(dataStack);
  Stack_destroy(opStack);
  //Stack_make_empty(opStack);
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
int
process(char *command, Stack dataStack, Stack opStack){
  char delim[] = " ";
  int *data = NULL;
  char *operation = NULL;
  int rc = 0;

  char* token = strtok(command, delim);
 // while (token != ((void *)0)) {
  //  token = strtok(((void *)0), delim);
  //}
  //printf("data val at beg %d\n", data);
  //printf("op val at beg %s\n", operation);
  //printf("command at beg %s\n", command);
  while(token != NULL)
  { //this wont exectue bc of above 
    assert(data == NULL && operation == NULL);
    data = (int*)malloc(sizeof(int));
   // data = NULL;
    //operation = (char*)malloc(sizeof(char) * MAX_OP_SIZE);
    if( sscanf(token, "%d", data) == 1)
    {
      //printf("data val, %d\n", *data);
      Stack_push(dataStack, data);
     //free(data); //do i need that
      data = NULL;
     //break; //added this
    }
    else
    {
      operation = (char*)malloc(sizeof(char) * MAX_OP_SIZE);
      //operation = NULL;
      //printf("op val %s", operation);
      if(sscanf(token, "%s", operation) == 1)
      {
        //printf("op val %s", operation);
       // if (operation == NULL){
       //   printf("op is null");
       // }
        if(!strcmp(operation,")"))
        {
          rc= runCloseParen(dataStack, opStack);
          if(rc < 0)
          { //QUESTION IS REALLY ABOUT GETTING NEXT
            break;
          }
        }
        //else //wont ever enter this loop why?
        //{ 
          //if(sscanf(token, "%s", operation) == 1)
          //{
            else if(!strcmp(operation,"("))
            {
              Stack_push(opStack, operation);
              operation = NULL;
            // break; //not sure about this
            }
            else //doesn't go into this loop
            {
              while(1) //wont go into this loop
              {
                if(Stack_is_empty(opStack))
                {
                  break;
                }
                char* val_top = Stack_pop(opStack);
                //printf("val at top %s", val_top);
                if(higherPriority(val_top, operation))
                {
                  //is previous operator val top i think so
                  rc = runOperation(val_top, dataStack);
                  free(val_top);
                  val_top = NULL;
                  if(rc < 0)
                  {
                    //free(val_top);
                   // val_top = NULL;
                    goto done;
                  }
                  //free(val_top);
                  //val_top = NULL;
                }
                else
                {
                  Stack_push(opStack, val_top);
                 // free(val_top);
                 // val_top = NULL;
                  break;
                }
              }
              //puah the operation is still cauing the fault gooooddddddddd
              //printf("op val %s", operation);
              Stack_push(opStack, operation); //this push is causing seg fault bc push is reallocing memory for op again
              //free(operation);
              operation = NULL;
            }
            //Stack_push(opStack, operation); //this push is causing seg fault bc push is reallocing memory for op again
              //free(operation);
             // operation = NULL;
          //}
        //}
     // free(operation);
      }
      //operation = NULL; //.newly aDDED 
    }
    //printf("op val %s", operation);
    //printf("data val, %d\n", *data);
    if (data){
      //printf("data val at end %d\n", *data);
      free(data);
      data = NULL;
    }
    if(operation){
      //printf("op val %s at end", operation);
      free(operation); //this causes error must be double freeing or something here
      operation = NULL;
    }
    token = strtok(NULL, delim);
   // free(data);
   // data = NULL;
   // free(operation); //this causes error must be double freeing or something here
  //  operation = NULL;
  }
  //free(data);
 // data = NULL;
 // free(operation); //this causes error must be double freeing or something here
 // operation = NULL;
  //token =strtok(NULL, delim); //now error occurs here why? 
  done:
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
  
  //token =strtok(NULL, delim);
  //return rc;
//}


int
runCloseParen(Stack dataStack, Stack opStack) {
  int rc = 0;
  char *op = NULL;
  int empty =0;
  int runOp_fail =0;
  char* val_top = NULL;

  while(1){
    if(Stack_is_empty(opStack)){
      empty =1;
     // return -1;
      //return error(EXIT_FAILURE);
      break;
    }
    char* val_top = Stack_pop(opStack);
    if(strcmp(val_top,"(") == 1){
      break;
    }
    else{
      if (runOperation(val_top, dataStack) != 0){
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
int
runOperation(char *op, Stack dataStack)
{
  int* data1;
  int* data2;
 // int* result;
 // int* data_1;
 // int* data_2;
  //int* resultp; // = &result; //changed this
 // int oper;

  //oper = (int)*op;

  int* result = (int*)malloc(sizeof(int) * sizeof(dataStack));
  //printf("result at beginning of run op %d", result);

  if(Stack_is_empty(dataStack)){
    error_msg_opMissingArgs(op);
    return -1;
  }
  data1 = Stack_pop(dataStack);
 // printf("data1 %d", *data1);
  //data1 = *data_1;
  //free(data1);
 // data1 = NULL;
  if(Stack_is_empty(dataStack)){
    error_msg_opMissingArgs(op);
    return -1;
  }
  data2 = Stack_pop(dataStack);
 // printf("data2 %d", *data2);
  //data2 = *data_2;
  //free(data2);
  //data2 = NULL;
  if(strcmp(op,"+") ==0){
    *result = *data2 + *data1; //not pushing result onto the stack
    //resultp = &result;
    //*resultp = result; //something is up here sefg fault why tho its becuase im like dereferencing a null pointer maybe I need to alloc mem for it
   // printf("result in runOp %d", *result);
    //result is the correct value;
    Stack_push(dataStack, result); //how do i deal with these
   // free(result); //saying freeing invalid pointer here why invalid? 
    result = NULL;
   // free(resultp);
    //Stack_push(dataStack, resultp); //how do i deal with these
    return 0;
  }
  else if(strcmp(op,"*") ==0){
    *result = *data2 * *data1;
   // *resultp = result;
    Stack_push(dataStack, result);
    //free(result);
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    //Stack_push(dataStack, resultp);
    return 0;
  }
  else if(strcmp(op,"/") ==0){
    if(*data1 == 0){
      error_msg_divByZero();
      return -1;
    }
    *result = *data2 / *data2;
    //*resultp = result;
    Stack_push(dataStack, result);
    //free(result);
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    return 0;
  }
  else if(strcmp(op,"-") ==0){
    *result = *data2 - *data1;
    //*resultp = result;
    Stack_push(dataStack, result);  //NOTE THIS IS NOT A VOID * 
    //free(result);
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    return 0;
  }
  else{
    error_msg_badOp(op);
    //free(result);
    result = NULL;
    free(data1);
    data1 = NULL;
    free(data2);
    data2 = NULL;
    return -1;
  }
}
