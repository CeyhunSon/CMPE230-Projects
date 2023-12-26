#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


struct Node{
    char data[256+1];
    struct Node* next;
    struct Node* prev;
    bool func ;
    bool var;
};

struct Variable{
    char data[24];
    struct Variable* next;
    struct Variable* prev;
    long long int val;
};


void operations(struct  Node** head);
void paren(struct Node** head);
void inside_func(struct Node** head,bool flag);


long long int xor(long long int a, long long int b){
    return a^b;
}
long long int ls(long long int a, long long int b){
    return a<<b;
}
long long int rs(long long int a, long long int b){
    return a>>b;
}
long long int lr(long long int n, unsigned int i){
    return (n << i)|(n >> (64 - i));
}
long long int rr(long long int n, unsigned int i){
    return (n >> i)|(n << (64 - i));

}

long long int addLastVar(struct Variable** head, char data[256+1]){
    struct Variable *temp1 = *head;
    struct Variable *temp2 = *head;

    while(temp1 != NULL){
        if (strcmp(data, temp1->data) == 0) {
            return temp1->val;
        }
        temp1 = temp1->next;
    }
    struct Variable *newVar = (struct Variable *) malloc(sizeof(struct Variable));
    strcpy(newVar->data, data);
    newVar->next = NULL;
    newVar->val = 0;

    if(*head == NULL){
        newVar->prev = NULL;
        *head = newVar;
        return 0;
    }

    while(temp2->next != NULL){
        temp2 = temp2->next;
    }
    temp2->next = newVar;
    newVar->prev = temp2;
    return 0;
}

void printList(struct Node* node){
    while (node != NULL){
        printf(" %s",node->data);
        node = node->next;
    }
    printf("\n");

}
void addLast(struct Node** head, char data[256+1], struct Variable** varHead, int beforeEquals) {
    struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
    struct Node *temp = *head;
    strcpy(newNode->data, data);
    newNode->next = NULL;
    long long int value = 0;

    if(strcmp(newNode->data,"xor") == 0 || strcmp(newNode->data,"ls") == 0 || strcmp(newNode->data,"rs") == 0 || strcmp(newNode->data,"lr") == 0 || strcmp(newNode->data,"rr") == 0 || strcmp(newNode->data,"not") == 0 || strcmp(newNode->data,"lr") == 0 || strcmp(newNode->data,"rr") == 0) {
        newNode->func = true;
        newNode->var = false;
    }
    else {
        newNode->func = false;
        if (isalpha((newNode->data)[0])) {
            newNode->var = true;
            value = addLastVar(varHead, data);
        } else {
            newNode->var = false;
        }
    }
    if (newNode->var && !beforeEquals) {
        if (value < 0) {
            value *= -1;
            char tok[256+1];
            sprintf(tok,"%lld", value);
            addLast(head, "(", varHead, 0);
            addLast(head, "0", varHead, 0);
            addLast(head, "-", varHead, 0);
            addLast(head, tok, varHead, 0);
            addLast(head, ")", varHead, 0);
        } else {
            char tok[256+1];
            sprintf(tok,"%lld", value);
            addLast(head, tok, varHead, 0);
        }
    } else {
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
}

long long int terminate(struct  Node** head){ // the function that traverse all linkedlist and do all requirements with using other functions
    struct Node *temp = *head;
    struct Node *func_point = *head;

    while (temp != NULL){
        if(temp->func){ // if temp is functions so we need to use inside func twice because we need to check left side and right side of func
            func_point = temp;
            temp = temp->next->next;
            inside_func(&temp,true);


            temp = temp->next->next;
            inside_func(&temp,false);
            long long int number1;
            long long int number2;
            sscanf(temp->prev->prev->data, "%lld", &number1);
            sscanf(temp->data, "%lld", &number2);

            long long int x;
            if(strcmp(func_point->data,"xor") == 0)
                x = xor(number1, number2);
            if(strcmp(func_point->data,"ls") == 0)
                x = ls(number1, number2);
            if(strcmp(func_point->data,"rs") == 0)
                x = rs(number1, number2);
            if(strcmp(func_point->data,"not") == 0)
                x = ~number1;
            if(strcmp(func_point->data,"lr") == 0)
                x = lr(number1, number2);
            if(strcmp(func_point->data,"rr") == 0)
                x = rr(number1, number2);
            char a[256+1];
            sprintf(a,"%lld",x);
            temp = temp->prev->prev->prev->prev;
            strcpy(temp->data, a);
            temp->func = false;
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
    long long int number2;
    sscanf(temp->data, "%lld", &number2); // there is only one node which is left and its data is the result

    return number2;
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
                long long int number1;
                long long int number2;
                sscanf(temp->prev->prev->data, "%lld", &number1);
                sscanf(temp->data, "%lld", &number2);

                //long long int number1 = atoi(temp->prev->prev);
                //long long int number2 = atoi(temp);

                long long int x;
                if(strcmp(func_point->data,"xor") == 0)
                    x = xor(number1, number2);
                if(strcmp(func_point->data,"ls") == 0)
                    x = ls(number1, number2);
                if(strcmp(func_point->data,"rs") == 0)
                    x = rs(number1, number2);
                if(strcmp(func_point->data,"not") == 0)
                    x = ~number1;
                if(strcmp(func_point->data,"lr") == 0)
                    x = lr(number1, number2);
                if(strcmp(func_point->data,"rr") == 0)
                    x = rr(number1, number2);
                char a[256+1];
                sprintf(a,"%lld",x);
                temp = temp->prev->prev->prev->prev;  //we change the function node's data with result of the function
                strcpy(temp->data, a);
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

            long long int x;
            long long int number1;
            long long int number2;

            sscanf(temp->prev->data, "%lld", &number1);
            sscanf(temp->next->data, "%lld", &number2);

            x = number1 * number2;
            char a[256+1];
            sprintf(a,"%lld",x);
            strcpy(temp->prev->data, a);
            //The reason for changing the data of the node before the operation node is because that
            //I want to delete the other node (* and the next node) and only the result of the operation remains.
            temp->prev->next = temp->next->next;
            //
            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;
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
        if (strcmp(temp->data, "+") == 0 || strcmp(temp->data, "-") == 0) {
            long long int x;
            long long int number1;
            long long int number2;

            sscanf(temp->prev->data, "%lld", &number1);
            sscanf(temp->next->data, "%lld", &number2);
            if (strcmp(temp->data, "+") == 0) {
                x = number1 + number2;
            }
            else {
                x = number1 - number2;
            }
            char a[256+1];
            sprintf(a,"%lld",x);

            strcpy(temp->prev->data, a);
            //The reason for changing the data of the node before the operation is because
            //I want to delete the other node (+ or - and the next node) and only the result of the operation remains.
            temp->prev->next = temp->next->next;
            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;

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
            long long int x;
            long long int number1;
            long long int number2;

            sscanf(temp->prev->data, "%lld", &number1);
            sscanf(temp->next->data, "%lld", &number2);
            x = number1 & number2;

            char a[256+1];
            sprintf(a,"%lld",x);

            strcpy(temp->prev->data, a);
            temp->prev->next = temp->next->next;
            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;

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
            long long int x;
            long long int number1;
            long long int number2;

            sscanf(temp->prev->data, "%lld", &number1);
            sscanf(temp->next->data, "%lld", &number2);
            x = number1 | number2;
            char a[256+1];
            sprintf(a,"%lld",x);
            //The reason for changing the data of the node before the operation is because
            //I want to delete the other node (| and the next node) and only the result of the operation remains.

            strcpy(temp->prev->data, a);
            temp->prev->next = temp->next->next;
            if(temp->next->next != NULL)
                temp->next->next->prev = temp->prev;

            temp = temp->prev;
            temp = temp->next;

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

int main() {

    struct Node *head = NULL;
    struct Variable *varHead = NULL;

    char input[256+1];
    int i;
    int commaNo, parenthesisNo, parenthesisAfterFunc, funcNo, handledFuncNo, notFuncNo;
    int innerFuncParenthesis[64], parenthesisAfterNotFunc[64];
    bool inFunc, isThereEqualSign, isThereError;

    printf(">");
    printf(" ");
    while (fgets(input, 1000, stdin) != NULL) {
        int containsEquals = (strchr(input, '=') != NULL);
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
                        addLast(&head, token, &varHead, containsEquals);
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
                        addLast(&head, token, &varHead, containsEquals);
                        break;
                    }
                }
                continue;
            } else if (input[i] == '+') {
                char token[] = "+";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == '-') {
                char token[] = "-";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == '*') {
                char token[] = "*";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == '=') {
                isThereEqualSign = true;
                containsEquals = 0;
                char token[] = "=";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == '|') {
                char token[] = "|";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == '(') {
                parenthesisNo++;
                if (notFuncNo > 0) {
                    parenthesisAfterNotFunc[notFuncNo-1]++;
                }
                char token[] = "(";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == ')') {
                parenthesisNo--;
                if (notFuncNo > 0) {
                    parenthesisAfterNotFunc[notFuncNo-1]--;
                    if (parenthesisAfterNotFunc[notFuncNo-1] == 0) {
                        notFuncNo--;
                        addLast(&head, ",", &varHead, containsEquals);
                        addLast(&head, "0", &varHead, containsEquals);
                    }
                }
                char token[] = ")";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == ',') {
                char token[] = ",";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == '&') {
                char token[] = "&";
                addLast(&head, token, &varHead, containsEquals);
                continue;
            } else if (input[i] == '\0' || input[i] == '%') {
                break;
            }
        }

        if (parenthesisNo != 0) {
            printf("Error!\n");
            isThereError = true;
        }
        struct Node *tkn = head;
        while (tkn != NULL) {
            if (isThereError) {
                break;
            }
            if (tkn->prev != NULL) {
                if (isalpha((tkn->prev->data)[0]) && isalpha((tkn->data)[0])) {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                } if (isalpha((tkn->prev->data)[0]) && isdigit((tkn->data)[0])) {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                } if (isdigit((tkn->prev->data)[0]) && isdigit((tkn->data)[0])) {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                } if (isdigit((tkn->prev->data)[0]) && isalpha((tkn->data)[0])) {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                } if ((tkn->data)[0] == '+' || (tkn->data)[0] == '-' || (tkn->data)[0] == '*' || (tkn->data)[0] == '&' || (tkn->data)[0] == '|' || (tkn->data)[0] == '=' || (tkn->data)[0] == ')') {
                    if ((tkn->prev->data)[0] == '+' || (tkn->prev->data)[0] == '-' || (tkn->prev->data)[0] == '*' || (tkn->prev->data)[0] == '&' || (tkn->prev->data)[0] == '|' || (tkn->prev->data)[0] == '=' || (tkn->prev->data)[0] == '(' || (tkn->prev->data)[0] == ',') {
                        printf("Error!\n");
                        isThereError = true;
                        break;
                    }
                } if ((tkn->data)[0] == '=' && tkn->prev->prev != NULL && !isalpha((tkn->prev->data)[0])) {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                } if (strcmp(tkn->data, "xor") == 0 || strcmp(tkn->data, "ls") == 0 || strcmp(tkn->data, "rs") == 0 || strcmp(tkn->data, "lr") == 0 || strcmp(tkn->data, "rr") == 0 || strcmp(tkn->data, "not") == 0) {
                    inFunc = true;
                    innerFuncParenthesis[funcNo - handledFuncNo] = parenthesisAfterFunc;
                    funcNo++;
                    if (tkn->next == NULL || (tkn->next->data)[0] != '(') {
                        printf("Error!\n");
                        isThereError = true;
                        break;
                    }
                    tkn = tkn->next;
                    continue;
                } if ((isalpha((tkn->prev->data)[0]) || isdigit((tkn->prev->data)[0])) && !(tkn->prev->func) && (tkn->data)[0] == '(') {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                } if ((tkn->data)[0] == ')') {
                    if ((tkn->prev->data)[0] == '+' || (tkn->prev->data)[0] == '-' || (tkn->prev->data)[0] == '*' || (tkn->prev->data)[0] == '&' || (tkn->prev->data)[0] == '|' || (tkn->prev->data)[0] == '(' || (tkn->prev->data)[0] == '=') {
                        printf("Error!\n");
                        isThereError = true;
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
                        printf("Error!\n");
                        isThereError = true;
                        break;
                    }
                    if (inFunc) {
                        parenthesisAfterFunc++;
                    }
                } if ((isalpha((tkn->data)[0]) || isdigit((tkn->data)[0])) && (tkn->prev->data)[0] == ')') {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                } if ((tkn->data)[0] == ',') {
                    if (!inFunc || parenthesisAfterFunc < (funcNo - handledFuncNo)) {
                        printf("Error!\n");
                        isThereError = true;
                        break;
                    } if ((tkn->prev->data)[0] == '+' || (tkn->prev->data)[0] == '-' || (tkn->prev->data)[0] == '*' || (tkn->prev->data)[0] == '&' || (tkn->prev->data)[0] == '|' || (tkn->prev->data)[0] == '=' || (tkn->prev->data)[0] == '(' || (tkn->prev->data)[0] == ',') {
                        printf("Error!\n");
                        isThereError = true;
                        break;
                    }
                    commaNo++;
                    if ((funcNo - commaNo) < 0) {
                        printf("Error!\n");
                        isThereError = true;
                        break;
                    }
                }
            } else if (isThereEqualSign) {
                if (!isalpha((tkn->data)[0]) && !isdigit((tkn->data)[0])) {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                } if (strcmp(tkn->data, "xor") == 0 || strcmp(tkn->data, "ls") == 0 || strcmp(tkn->data, "rs") == 0 || strcmp(tkn->data, "lr") == 0 || strcmp(tkn->data, "rr") == 0 || strcmp(tkn->data, "not") == 0) {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                }
            } else {
                if (strcmp(tkn->data, "xor") == 0 || strcmp(tkn->data, "ls") == 0 || strcmp(tkn->data, "rs") == 0 || strcmp(tkn->data, "lr") == 0 || strcmp(tkn->data, "rr") == 0 || strcmp(tkn->data, "not") == 0) {
                    inFunc = true;
                    innerFuncParenthesis[funcNo - handledFuncNo] = parenthesisAfterFunc;
                    funcNo++;
                    if (tkn->next == NULL || (tkn->next->data)[0] != '(') {
                        printf("Error!\n");
                        isThereError = true;
                        break;
                    }
                } if (!isalpha((tkn->data)[0]) && !isdigit((tkn->data)[0]) && (tkn->data)[0] != '(') {
                    printf("Error!\n");
                    isThereError = true;
                    break;
                }
            }
            tkn = tkn->next;
        }
        if (!isThereError && head != NULL) {
            if(!isThereEqualSign) {
                long long int x = terminate(&head);
                printf("%lld\n", x);
            } else {
                struct Node *tmp = head;
                struct Node *assignment = NULL;
                struct Variable *varble = varHead;
                while ((tmp->data)[0] != '=') {
                    if (tmp->var) {
                        assignment = tmp;
                    }
                    tmp = tmp->next;
                }
                while(varble != NULL) {
                    if (strcmp(assignment->data, varble->data) == 0) {
                        break;
                    }
                    varble = varble->next;
                }
                tmp->next->prev = NULL;
                head = tmp->next;
                long long int x = terminate(&head);
                varble->val = x;
            }
        }
        head = NULL;
        printf(">");
        printf(" ");
    }
    return 0;
}
