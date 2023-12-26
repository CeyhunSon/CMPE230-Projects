#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

FILE *fpWrite;
FILE *test;
int counter = 1;

struct Node{
    char data[256+1];
    char variableName[256+1];
    int type;
    struct Node* next;
    struct Node* prev;
    bool func ;
    bool var;
};

void operations(struct  Node** head);
void paren(struct Node** head);
void inside_func(struct Node** head,bool flag);

// Adds the Node that has taen as a parameter to the end of the double linkedlist by iterating.
// The function also checks if the token is a function name or variable or neither of them.
void addLast(struct Node** head, char data[256+1]) {
    struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
    struct Node *temp = *head;
    strcpy(newNode->data, data);
    newNode->next = NULL;

    if(strcmp(newNode->data,"xor") == 0 || strcmp(newNode->data,"ls") == 0 || strcmp(newNode->data,"rs") == 0 || strcmp(newNode->data,"lr") == 0 || strcmp(newNode->data,"rr") == 0 || strcmp(newNode->data,"not") == 0 || strcmp(newNode->data,"lr") == 0 || strcmp(newNode->data,"rr") == 0) {
        newNode->func = true;
        newNode->var = false;
    }
    else {
        newNode->func = false;
        if (isalpha((newNode->data)[0])) {
            newNode->var = true;
        } else {
            newNode->var = false;
        }
    }
    if(*head == NULL){
        newNode->prev = NULL;
        *head = newNode;
        return;
    }

    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = newNode;
    newNode->prev = temp;
}

void terminate(struct  Node** head){ // the function that traverse all linkedlist and do all requirements with using other functions
    struct Node *temp = *head;
    struct Node *func_point = *head;

    while (temp != NULL){
        if(temp->func){ // if temp is functions so we need to use inside func twice because we need to check left side and right side of func
            func_point = temp;
            temp = temp->next->next;
            inside_func(&temp,true);
            temp = temp->next->next;
            inside_func(&temp,false);
            //with these inside functions,we get only the parameters of outermost function

            //print the operation name with data of function's parameters(since two nodes come from operations the node's type must be register or number
            if(strcmp(func_point->data,"xor") == 0){
//                printf("%%rg%d = xor i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                fprintf(fpWrite,"%%rg%d = xor i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
            }
            if(strcmp(func_point->data,"ls") == 0){
//                printf("%%rg%d = shl i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                fprintf(fpWrite, "%%rg%d = shl i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
            }
            if(strcmp(func_point->data,"rs") == 0){
//                printf("%%rg%d = ashr i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                fprintf(fpWrite, "%%rg%d = ashr i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
            }
            if(strcmp(func_point->data,"not") == 0){
//                printf("%%rg%d = xor i32 -1 ,%s \n",counter,temp->prev->prev->data);
                fprintf(fpWrite, "%%rg%d = xor i32 -1 ,%s \n",counter,temp->prev->prev->data);
            }
            if(strcmp(func_point->data,"lr") == 0){
//                printf("%%rg%d = shl i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                fprintf(fpWrite, "%%rg%d = shl i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                counter++;
//                printf("%%rg%d = sub i32 32,%s\n",counter,temp->data);
                fprintf(fpWrite, "%%rg%d = sub i32 32,%s\n",counter,temp->data);
                counter++;
//                printf("%%rg%d = lshr i32 %s,%%rg%d\n",counter,temp->prev->prev->data,(counter-1));
                fprintf(fpWrite, "%%rg%d = lshr i32 %s,%%rg%d\n",counter,temp->prev->prev->data,(counter-1));
                counter++;
//                printf("%%rg%d = or i32 %%rg%d,%%rg%d\n",counter,(counter-3),(counter-1));
                fprintf(fpWrite, "%%rg%d = or i32 %%rg%d,%%rg%d\n",counter,(counter-3),(counter-1));
            }
            if(strcmp(func_point->data,"rr") == 0){
//                printf("%%rg%d = lshr i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                fprintf(fpWrite, "%%rg%d = lshr i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                counter++;
//                printf("%%rg%d = sub i32 32,%s\n",counter,temp->data);
                fprintf(fpWrite, "%%rg%d = sub i32 32,%s\n",counter,temp->data);
                counter++;
//                printf("%%rg%d = shl i32 %s,%%rg%d\n",counter,temp->prev->prev->data,(counter-1));
                fprintf(fpWrite, "%%rg%d = shl i32 %s,%%rg%d\n",counter,temp->prev->prev->data,(counter-1));
                counter++;
//                printf("%%rg%d = or i32 %%rg%d,%%rg%d\n",counter,(counter-3),(counter-1));
                fprintf(fpWrite, "%%rg%d = or i32 %%rg%d,%%rg%d\n",counter,(counter-3),(counter-1));
            }
            char a[256+1];
            sprintf(a,"%%rg%d",counter);


            temp = temp->prev->prev->prev->prev;  //we change the function node's data with result of the function
            strcpy(temp->data,a);
            temp->type = 1;
            counter++;
            temp->func = false; //since the node's data is number,it can no longer be func so it is important to change it is not func.
            temp->next = temp->next->next->next->next->next->next;
            if(temp->next != NULL){
                temp->next->prev = temp;
            }
            temp = *head;
            continue;
        }
        else {
            temp = temp->next;
        }
    }
    temp = *head;
    // there is no longer functions in this linkedlist.
    paren(&temp); // do operations and check paranthesis.



}
void inside_func(struct Node** head,bool flag){ // in this func we do all operations and removes paranthesis before or after comma in function
    //there can be functions in functions so we need to get innermost function and iteratively to uppermost function
    struct Node *temp = *head;
    int count = 0;
    int number_of_functions = 0;


    struct Node *func_point = *head;

    count++;
    while(temp != NULL){

        if(temp->func){
            number_of_functions++;

            temp = temp->next;
            continue;
        }
        if(strcmp(temp->data,"(") == 0){
            count++;
            temp = temp->next;
            continue;
        }
        if(strcmp(temp->data,")") == 0){
            count--;
            if(count == 0 && !flag) // if flag is true and count = 0 means we get the outermost paranthesis after the comma
                break;
            temp = temp->next;
            continue;
        }
        if(strcmp(temp->data,",") == 0 && count == 1 && flag){     //if flag is true, we should traverse until we get comma and count = 1 means we get the comma of outermos function's comma
            //if count is not 1, the comma is the comma of the another func.
            break;
        }
        else {
            temp = temp->next;
        }
    }
    temp = *head;
    int find_last_func = 0;//we try to find the innermost function before or after comma
    while(number_of_functions != 0){
        if(temp->func) {
            find_last_func++;
            if (number_of_functions == find_last_func) {
                func_point = temp;

                temp = temp->next->next;

                paren(&temp); //inside the inner function,there can still be operations and paranthesis
                temp = temp->next->next; // temp->next->next means it is the node after comma
                paren(&temp);


                if(strcmp(func_point->data,"xor") == 0){
//                    printf("%%rg%d = xor i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                    fprintf(fpWrite, "%%rg%d = xor i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                }
                if(strcmp(func_point->data,"ls") == 0){
//                    printf("%%rg%d = shl i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                    fprintf(fpWrite, "%%rg%d = shl i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                }
                if(strcmp(func_point->data,"rs") == 0){
//                    printf("%%rg%d = ashr i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                    fprintf(fpWrite, "%%rg%d = ashr i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                }
                if(strcmp(func_point->data,"not") == 0){
//                    printf("%%rg%d = xor i32 -1 ,%s \n",counter,temp->prev->prev->data);
                    fprintf(fpWrite, "%%rg%d = xor i32 -1 ,%s \n",counter,temp->prev->prev->data);
                }
                if(strcmp(func_point->data,"lr") == 0){
//                    printf("%%rg%d = shl i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                    fprintf(fpWrite, "%%rg%d = shl i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                    counter++;
//                    printf("%%rg%d = sub i32 32,%s\n",counter,temp->data);
                    fprintf(fpWrite, "%%rg%d = sub i32 32,%s\n",counter,temp->data);
                    counter++;
//                    printf("%%rg%d = lshr i32 %s,%%rg%d\n",counter,temp->prev->prev->data,(counter-1));
                    fprintf(fpWrite, "%%rg%d = lshr i32 %s,%%rg%d\n",counter,temp->prev->prev->data,(counter-1));
                    counter++;
//                    printf("%%rg%d = or i32 %%rg%d,%%rg%d\n",counter,(counter-3),(counter-1));
                    fprintf(fpWrite, "%%rg%d = or i32 %%rg%d,%%rg%d\n",counter,(counter-3),(counter-1));
                }
                if(strcmp(func_point->data,"rr") == 0){
//                    printf("%%rg%d = lshr i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                    fprintf(fpWrite, "%%rg%d = lshr i32 %s,%s\n",counter,temp->prev->prev->data,temp->data);
                    counter++;
//                    printf("%%rg%d = sub i32 32,%s\n",counter,temp->data);
                    fprintf(fpWrite, "%%rg%d = sub i32 32,%s\n",counter,temp->data);
                    counter++;
//                    printf("%%rg%d = shl i32 %s,%%rg%d\n",counter,temp->prev->prev->data,(counter-1));
                    fprintf(fpWrite, "%%rg%d = shl i32 %s,%%rg%d\n",counter,temp->prev->prev->data,(counter-1));
                    counter++;
//                    printf("%%rg%d = or i32 %%rg%d,%%rg%d\n",counter,(counter-3),(counter-1));
                    fprintf(fpWrite, "%%rg%d = or i32 %%rg%d,%%rg%d\n",counter,(counter-3),(counter-1));
                }
                char a[256+1];
                sprintf(a,"%%rg%d",counter);


                temp = temp->prev->prev->prev->prev;  //we change the function node's data with result of the function
                strcpy(temp->data,a);
                temp->type = 1;
                counter++;
                temp->func = false; //since the node's data is number,it can no longer be func so it is important to change it is not func.
                temp->next = temp->next->next->next->next->next->next;
                if(temp->next != NULL){
                    temp->next->prev = temp;
                }
                find_last_func = 0;
                number_of_functions--;
                temp = *head;
            }
            else{
                temp = temp->next;
            }
        }
        else{
            temp = temp->next;
        }
    }
    temp = *head;
    paren(&temp); // there is no longer functions before or after comma.
    temp = *head;

}

void paren(struct Node** head){ //aim is that find the number of paranthesis and iteratively do operations in paranthesis starting from the innermost one
    struct Node *temp = *head;

    int number_of_parant = 0;
    int find_last_parant = 0;
    int closed_parant = 0;
    while(temp != NULL){
        if(strcmp(temp->data,"(") == 0){
            number_of_parant++;
            temp = temp->next;
            continue;
        }
        if(strcmp(temp->data,")") == 0){
            closed_parant++;

            if(closed_parant > number_of_parant) // it can be xor(a,b) and if we start from the beginning of b, if we counter with ")" we should end the loop
                break;

            temp = temp->next;
            continue;

        }
        if(strcmp(temp->data,",") == 0){// it can be xor(a,b) and if we start from the beginning of a, if we counter with "," we should end the loop
            break;
        }
        else{
            temp = temp->next;
        }
    }
    temp = *head;
    while(number_of_parant != 0) {
        if (strcmp(temp->data, "(") == 0) {
            find_last_parant++;
            if (number_of_parant == find_last_parant) {//we found the innermost paranthesis of the nested ones.
                operations(&temp); // function performs mathematical operations inside the innermost paranthesis ofthe nested parentheses
                //after the operations we should change the data of paranthesis with result
                strcpy(temp->data,temp->next->data);
                temp->type = 1;
                //arrange the temp->next with the adjacent of the )
                temp->next = temp->next->next->next;
                if(temp->next != NULL)
                    temp->next->prev = temp;

                temp = *head;
                number_of_parant--;
                find_last_parant = 0;
                continue;
            }
            temp = temp->next;
        }
        else
            temp = temp->next;
    }
    //if there is no paranthesis,we should perform operations from the beginning
    temp = *head;
    operations(&temp);
    temp = *head;
}

void operations(struct  Node** head){ //function performs mathematical operations inside the innermost paranthesis ofthe nested parentheses
    struct Node *temp = *head;
    //this function traverses the linked list from the beginning with a while loop to find operations by operator precedence
    while (temp != NULL) {
        if (strcmp(temp->data, "*") == 0 ) { //if temp->data is *, we should multiply temp->next with temp->prev

//            printf("%%rg%d = mul i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            fprintf(fpWrite, "%%rg%d = mul i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            char a[256+1];
            sprintf(a,"%%rg%d",counter);
            strcpy(temp->prev->data,a);
            temp->prev->type = 1;
            temp->prev->next = temp->next->next;

            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;
            counter++;
            continue;

        }
        if (strcmp(temp->data, ")") == 0 || strcmp(temp->data, ",") == 0 || temp == NULL) { // temp can be in function or end of the paranthesis,so it should break.
            break;
        }
        else {
            temp = temp->next;
        }
    }
    temp = *head;

    while (temp != NULL) {
        //this function traverses the linked list from the beginning with a while loop to find operations by operator precedence
        if (strcmp(temp->data, "/") == 0) {

            // printf("%%rg%d= sdiv i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            fprintf(fpWrite, "%%rg%d = sdiv i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            char a[256+1];
            sprintf(a,"%%rg%d",counter);
            strcpy(temp->prev->data,a);
            temp->prev->type = 1;
            temp->prev->next = temp->next->next;

            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;
            counter++;
            if(temp == NULL){
                break;
            }
            continue;
        }
        if (strcmp(temp->data, ")") == 0 || strcmp(temp->data, ",") == 0 || temp == NULL) {
            break;
        }
        else {
            temp = temp->next;
        }
    }

    temp = *head;

    while (temp != NULL) {
        //this function traverses the linked list from the beginning with a while loop to find operations by operator precedence
        if (strcmp(temp->data, "%") == 0 ) {

            //printf("%%rg%d = srem i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            fprintf(fpWrite, "%%rg%d = srem i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            char a[256+1];
            sprintf(a,"%%rg%d",counter);
            strcpy(temp->prev->data,a);
            temp->prev->type = 1;
            temp->prev->next = temp->next->next;

            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;
            counter++;
            if(temp == NULL){
                break;
            }
            continue;
        }
        if (strcmp(temp->data, ")") == 0 || strcmp(temp->data, ",") == 0 || temp == NULL) {
            break;
        }
        else {
            temp = temp->next;
        }
    }






    temp = *head;

    while (temp != NULL) {
        //this function traverses the linked list from the beginning with a while loop to find operations by operator precedence
        if (strcmp(temp->data, "+") == 0 || strcmp(temp->data, "-") == 0) {
            if(strcmp(temp->data, "+") == 0)
                //printf("%%rg%d = add i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
                fprintf(fpWrite, "%%rg%d = add i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            else
                //printf("%%rg%d = sub i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
                fprintf(fpWrite, "%%rg%d = sub i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            char a[256+1];
            sprintf(a,"%%rg%d",counter);
            strcpy(temp->prev->data,a);
            temp->prev->type = 1;
            temp->prev->next = temp->next->next;

            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;
            counter++;
            if(temp == NULL){
                break;
            }
            continue;
        }
        if (strcmp(temp->data, ")") == 0 || strcmp(temp->data, ",") == 0 || temp == NULL) {
            break;
        }
        else {
            temp = temp->next;
        }
    }
    temp = *head;

    while (temp != NULL) {
        if (strcmp(temp->data, "&") == 0 ) {
            //printf("%%rg%d = and i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            fprintf(fpWrite, "%%rg%d = and i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            char a[256+1];
            sprintf(a,"%%rg%d",counter);
            strcpy(temp->prev->data,a);
            temp->prev->type = 1;
            temp->prev->next = temp->next->next;

            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;
            counter++;

            if(temp == NULL){
                break;
            }
            continue;
        }
        if (strcmp(temp->data, ")") == 0 || strcmp(temp->data, ",") == 0 || temp == NULL) {
            break;
        }
        else {
            temp = temp->next;
        }
    }
    temp = *head;
    while (temp != NULL) {
        if (strcmp(temp->data, "|") == 0 ) {
            //printf("%%rg%d = or i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            fprintf(fpWrite, "%%rg%d = or i32 %s,%s\n",counter,temp->prev->data,temp->next->data);
            char a[256+1];
            sprintf(a,"%%rg%d",counter);
            strcpy(temp->prev->data,a);
            temp->prev->type = 1;
            temp->prev->next = temp->next->next;

            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;
            counter++;

            if(temp == NULL){
                break;
            }
            continue;
        }
        if (strcmp(temp->data, ")") == 0 || strcmp(temp->data, ",") == 0 || temp == NULL) {
            break;
        }
        else {
            temp = temp->next;
        }
    }
    //after all the operations, we get only one node which is head and its data is the result
    temp = *head;

}

int main(int argc, char* argv[]) {

    char input_filename[64];
    char output_filename[64];
    char *ext_pos;
// Instantiate input-output file
    strncpy(input_filename, argv[1], 64 - 1);
    ext_pos = strrchr(input_filename, '.');
    strncpy(output_filename, input_filename, ext_pos - input_filename);
    strcpy(output_filename + (ext_pos - input_filename), ".ll");
    fpWrite = fopen(output_filename, "w");
    test = fopen(input_filename, "r");
    fprintf(fpWrite, "; ModuleID = 'advcalc2ir' \n");
    fprintf(fpWrite, "declare i32 @printf(i8*, ...)\n \n");
    fprintf(fpWrite, "@print.str = constant [4 x i8] c\"%%d\\0A\\00\"\n \n \n");
    fprintf(fpWrite, "define i32 @main() { \n");

    struct Node *head = NULL;
    struct Variable *varHead = NULL;
     
    bool globError = false;
    int initVars = 0;
    char *initArray[300];
    char *(*ptr)[300] = &initArray;
    char input[256+1];
    int i, s;
    int linechecker = 0;
    int commaNo, parenthesisNo, parenthesisAfterFunc, funcNo, handledFuncNo, notFuncNo, equalNo;
    int innerFuncParenthesis[64], parenthesisAfterNotFunc[64];
    bool inFunc, isThereEqualSign, isThereError, isInit;
    // Reading input file line by line
    while (fgets(input, 1000, test) != NULL) {
    //while (fgets(input, 1000, stdin) != NULL) {
        // Increment number of line
        linechecker++;
        equalNo = 0;
        notFuncNo = 0;
        inFunc = false;
        isThereEqualSign = false;
        isThereError = false;
        commaNo = 0;
        parenthesisNo = 0;
        parenthesisAfterFunc = 0;
        funcNo = 0;
        handledFuncNo = 0;
        memset(parenthesisAfterNotFunc, 0, sizeof(parenthesisAfterNotFunc));
        memset(innerFuncParenthesis, 0, sizeof(innerFuncParenthesis));
        // Parse the every line into tokens
        for (i = 0; i < sizeof(input); i++) {
            if (isalpha(input[i])) {
                char token[256+1];
                for (int j = 0; j < sizeof(input) - i; j++) {
                    if (isalpha(input[i])) {
                        token[j] = input[i];
                        i++;
                    } else {
                        i--;
                        token[j]= '\0';
                        if (strcmp(token, "not") == 0) {
                            notFuncNo++;
                        }
                        addLast(&head, token);
                        break;
                    }
                }
                continue;
            } else if (isdigit(input[i])) {
                char token[256+1];
                for (int j = 0; j < sizeof(input) - i; j++) {
                    if (isdigit(input[i])) {
                        token[j] = input[i];
                        i++;
                    } else {
                        i--;
                        token[j]= '\0';
                        addLast(&head, token);
                        break;
                    }
                }
                continue;
            } else if (input[i] == '+') {
                char token[] = "+";
                addLast(&head, token);
                continue;
            } else if (input[i] == '-') {
                char token[] = "-";
                addLast(&head, token);
                continue;
            } else if (input[i] == '*') {
                char token[] = "*";
                addLast(&head, token);
                continue;
            } else if (input[i] == '/') {
                char token[] = "/";
                addLast(&head, token);
                continue;
            } else if (input[i] == '=') {
                equalNo++;
                isThereEqualSign = true;
                char token[] = "=";
                addLast(&head, token);
                continue;
            } else if (input[i] == '|') {
                char token[] = "|";
                addLast(&head, token);
                continue;
            } else if (input[i] == '(') {
                parenthesisNo++;
                if (notFuncNo > 0) {
                    parenthesisAfterNotFunc[notFuncNo-1]++;
                }
                char token[] = "(";
                addLast(&head, token);
                continue;
            } else if (input[i] == ')') {
                parenthesisNo--;
                if (notFuncNo > 0) {
                    parenthesisAfterNotFunc[notFuncNo-1]--;
                    if (parenthesisAfterNotFunc[notFuncNo-1] == 0) {
                        notFuncNo--;
                        addLast(&head, ",");
                        addLast(&head, "0");
                    }
                }
                char token[] = ")";
                addLast(&head, token);
                continue;
            } else if (input[i] == ',') {
                char token[] = ",";
                addLast(&head, token);
                continue;
            } else if (input[i] == '&') {
                char token[] = "&";
                addLast(&head, token);
                continue;
            } else if (input[i] == '%') {
                char token[] = "%";
                addLast(&head, token);
                continue;
            } else if (input[i] == '\0') {
                break;
            }
        }
        // Checks if the parenthesis are valid
        if (parenthesisNo != 0) {
            printf("Error on line %d!\n", linechecker);
            isThereError = true;
            globError = true;
        }
        struct Node *tkn = head;
        // Error detector part
        while (tkn != NULL) {
            if (isThereError) {
                break;
            } else if (equalNo > 1) {
                printf("Error on line %d!\n", linechecker);
                isThereError = true;
                globError = true;
                break;
            }
            if (tkn->prev != NULL) {
                if (isalpha((tkn->prev->data)[0]) && isalpha((tkn->data)[0])) {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                } if (isalpha((tkn->prev->data)[0]) && isdigit((tkn->data)[0])) {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                } if (isdigit((tkn->prev->data)[0]) && isdigit((tkn->data)[0])) {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                } if (isdigit((tkn->prev->data)[0]) && isalpha((tkn->data)[0])) {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                } if ((tkn->data)[0] == '+' || (tkn->data)[0] == '-' || (tkn->data)[0] == '*' || (tkn->data)[0] == '/' || (tkn->data)[0] == '%' || (tkn->data)[0] == '&' || (tkn->data)[0] == '|' || (tkn->data)[0] == '=' || (tkn->data)[0] == ')') {
                    if ((tkn->prev->data)[0] == '/' || (tkn->prev->data)[0] == '%' || (tkn->prev->data)[0] == '+' || (tkn->prev->data)[0] == '-' || (tkn->prev->data)[0] == '*' || (tkn->prev->data)[0] == '&' || (tkn->prev->data)[0] == '|' || (tkn->prev->data)[0] == '=' || (tkn->prev->data)[0] == '(' || (tkn->prev->data)[0] == ',') {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    }
                } if ((tkn->data)[0] == '=' && (tkn->prev->prev != NULL || !(tkn->prev->var))) {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                } if (strcmp(tkn->data, "xor") == 0 || strcmp(tkn->data, "ls") == 0 || strcmp(tkn->data, "rs") == 0 || strcmp(tkn->data, "lr") == 0 || strcmp(tkn->data, "rr") == 0 || strcmp(tkn->data, "not") == 0) {
                    inFunc = true;
                    innerFuncParenthesis[funcNo - handledFuncNo] = parenthesisAfterFunc;
                    funcNo++;
                    if (tkn->next == NULL || (tkn->next->data)[0] != '(') {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    }
                    tkn = tkn->next;
                    continue;
                } if ((isalpha((tkn->prev->data)[0]) || isdigit((tkn->prev->data)[0])) && !(tkn->prev->func) && (tkn->data)[0] == '(') {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                } if ((tkn->data)[0] == ')') {
                    if ((tkn->prev->data)[0] == '/' || (tkn->prev->data)[0] == '%' || (tkn->prev->data)[0] == '+' || (tkn->prev->data)[0] == '-' || (tkn->prev->data)[0] == '*' || (tkn->prev->data)[0] == '&' || (tkn->prev->data)[0] == '|' || (tkn->prev->data)[0] == '(' || (tkn->prev->data)[0] == '=') {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    }
                    if (inFunc) {
                        parenthesisAfterFunc--;
                        if (innerFuncParenthesis[funcNo - handledFuncNo - 1] == parenthesisAfterFunc) {
                            handledFuncNo++;
                        }
                        if (funcNo == handledFuncNo) {
                            inFunc = false;
                        }
                    }
                } if ((tkn->data)[0] == '(') {
                    if ((isalpha((tkn->prev->data)[0]) || isdigit((tkn->prev->data)[0]) || (tkn->prev->data)[0] == ')') && !inFunc) {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    }
                    if (inFunc) {
                        parenthesisAfterFunc++;
                    }
                } if ((isalpha((tkn->data)[0]) || isdigit((tkn->data)[0])) && (tkn->prev->data)[0] == ')') {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                } if ((tkn->data)[0] == ',') {
                    if (!inFunc || parenthesisAfterFunc < (funcNo - handledFuncNo)) {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    } if ((tkn->prev->data)[0] == '/' || (tkn->prev->data)[0] == '%' || (tkn->prev->data)[0] == '+' || (tkn->prev->data)[0] == '-' || (tkn->prev->data)[0] == '*' || (tkn->prev->data)[0] == '&' || (tkn->prev->data)[0] == '|' || (tkn->prev->data)[0] == '=' || (tkn->prev->data)[0] == '(' || (tkn->prev->data)[0] == ',') {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    }
                    commaNo++;
                    if ((funcNo - commaNo) < 0) {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    }
                } if ((tkn->data)[0] == '/' || (tkn->data)[0] == '+' || (tkn->data)[0] == '-' || (tkn->data)[0] == '*' || (tkn->data)[0] == '&' || (tkn->data)[0] == '|' || (tkn->data)[0] == '=' || (tkn->data)[0] == '(' || (tkn->data)[0] == '%') {
                    if (tkn->next == NULL) {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    }
                }
            } else if (isThereEqualSign) {
                if (!isalpha((tkn->data)[0]) && !isdigit((tkn->data)[0])) {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                } if (strcmp(tkn->data, "xor") == 0 || strcmp(tkn->data, "ls") == 0 || strcmp(tkn->data, "rs") == 0 || strcmp(tkn->data, "lr") == 0 || strcmp(tkn->data, "rr") == 0 || strcmp(tkn->data, "not") == 0) {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                }
            } else {
                if (strcmp(tkn->data, "xor") == 0 || strcmp(tkn->data, "ls") == 0 || strcmp(tkn->data, "rs") == 0 || strcmp(tkn->data, "lr") == 0 || strcmp(tkn->data, "rr") == 0 || strcmp(tkn->data, "not") == 0) {
                    inFunc = true;
                    innerFuncParenthesis[funcNo - handledFuncNo] = parenthesisAfterFunc;
                    funcNo++;
                    if (tkn->next == NULL || (tkn->next->data)[0] != '(') {
                        printf("Error on line %d!\n", linechecker);
                        isThereError = true;
                        globError = true;
                        break;
                    }
                } if (!isalpha((tkn->data)[0]) && !isdigit((tkn->data)[0]) && (tkn->data)[0] != '(') {
                    printf("Error on line %d!\n", linechecker);
                    isThereError = true;
                    globError = true;
                    break;
                }
            }
            tkn = tkn->next;
        }
        // Convert the code to LLVM form
        struct Node *tok = head;
        if (!isThereError && head != NULL) {
            if(!isThereEqualSign) {
                while (tok != NULL) {
                    isInit = false;
                    for (s = 0; s < initVars; s++) {
                        if (strcmp(tok->data, (*ptr)[s]) == 0) {
                            isInit = true;
                            break;
                        }
                    }
                    if (isdigit((tok->data)[0])) {
                        tok->type = 0;
                    } else if (tok->var) {
                        if (!isInit) {
                            printf("Error on line %d!\n", linechecker);
                            isThereError = true;
                            globError = true;
                            break;
                        }
                        strcpy(tok->variableName,tok->data);
                        char a[256+1];
                        sprintf(a,"%%rg%d", counter);
                        strcpy(tok->data,a);
                        //printf("%s = load i32, i32* %%%s\n", tok->data, tok->variableName);
                        fprintf(fpWrite, "%s = load i32, i32* %%%s\n", tok->data, tok->variableName);
                        counter++;
                        tok->type = 1;
                    }
                    tok = tok->next;
                }
                if (!isThereError) {
                    terminate(&head);
                    //printf("call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @print.str, i32 0, i32 0), i32 %s)\n",head->data);
                    fprintf(fpWrite, "call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @print.str, i32 0, i32 0), i32 %s)\n",head->data);
                }
            } else {
                isInit = false;
                for (s = 0; s < initVars; s++) {
                    if (strcmp(tok->data, (*ptr)[s]) == 0) {
                        isInit = true;
                        break;
                    }
                }
                char a[256+1];
                strcpy(a,tok->data);
                if (!isInit) {
                    (*ptr)[initVars] = tok->data;
                    initVars++;
                    //printf("%%%s = alloca i32\n", tok->data);
                    fprintf(fpWrite, "%%%s = alloca i32\n", tok->data);
                }
                bool beforeeqn = true;
                while (tok != NULL) {
                    isInit = false;
                    for (s = 0; s < initVars; s++) {
                        if (strcmp(tok->data, (*ptr)[s]) == 0) {
                            isInit = true;
                            break;
                        }
                    }
                    if (beforeeqn) {
                        if ((tok->data)[0] == '=') {
                            beforeeqn = false;
                        }
                        tok = tok->next;
                        head = tok;
                    } else {
                        if (isdigit((tok->data)[0])) {
                            tok->type = 0;
                        } else if (tok->var) {
                            if (!isInit) {
                                printf("Error on line %d!\n", linechecker);
                                isThereError = true;
                                globError = true;
                                break;
                            }
                            strcpy(tok->variableName, tok->data);
                            char b[256 + 1];
                            sprintf(b, "%%rg%d", counter);
                            strcpy(tok->data, b);
                            //printf("%s = load i32, i32* %%%s\n", tok->data, tok->variableName);
                            fprintf(fpWrite, "%s = load i32, i32* %%%s\n", tok->data, tok->variableName);
                            counter++;
                            tok->type = 1;
                        }
                        tok = tok->next;
                    }
                }
                if (!isThereError) {
                    terminate(&head);
                    //printf("store i32 %s, i32* %%%s\n", head->data, a);
                    fprintf(fpWrite, "store i32 %s, i32* %%%s\n", head->data, a);
                }
            }
        }
        head = NULL;
    }
    fprintf(fpWrite, "ret i32 0\n");
    fprintf(fpWrite, "}");
    // If there is an error remove the output file
    if (globError) {
    	remove(output_filename);
    }
    return 0;
}
