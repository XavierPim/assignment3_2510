#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
/**
 * File: main.c
 * Author1:Patrick Cammayo A01278747
 * Author2:Ian Guano A01166459
 * Author3:Usman Arshad A00987350
 * Author4:Xavier Pimentel A00697839
 * Date: November 30
 * Course COMP 2510
 *
 * Summary of File:
 * This file takes in a input file then sorts, compacts, and prints a modified version as a sorted linked list.
 *
 *  Precondition:
 *  Takes in user input from 5 choices
 *  user has to type input file name postfix of type .txt
 *  each line in input file needs to be in syntax with a
 *  process identifier = 'P' + integer
 *  Base to represent index = integer
 *  Limit to represent how many "jumps" to the next node
 *
 *   Postcondition:
 *   sort a inputted file arranged in ascending order on "base" index
 *   merge "holes" in list
 *   push merged "holes" to the last node
 *
 *
 *   Description:
 *   This program takes in a input file and creates a linked list from each new line in the passed file.
 *   The user can choose to use
 *   2:merge sort the current linked list and order them based on ascending Base value while combining the holes
 *   3:combine the holes and push them to the last node
 *   4:print the current sorted linked list
 *
 */

typedef struct Node {
    char* identifier;
    int base;
    int limit;
    struct Node* next;
} Node;

void printPrompts();
void printMemoryView(Node** head);
void createNode(Node** head, FILE* fp);
void loadFile(Node** head , FILE* fp);
void mergeHoles(Node** head);
void compaction(Node** head);
void frontBackSplit(Node* source, Node** frontRef, Node** backRef);
void mergeFreeBlocks(Node** headRef);
int testMemoryView(Node** test);
int checkOverlap(Node* current, Node* nextNode);
/**
* int main()
* Summary of main function:
*
*         Drives the program. Takes in user input and prompts user between 5 options.
*
*          Parameters:Nothing
*
*          Return value:
*          A sorted linked list depending on user input
*
*   Description:
*
*        A user can pass in an input file then sorted based on ascending bases.
 *       A user can sort and merge holes in the linked list
 *       A user can sort and merge holes then push the combined holes to the end of the list.
*/
int main() {
    char input[50];
    Node* head = NULL;
    Node* test = NULL;
    FILE *fp;
    int inputBoolean = 0;

    while (true) {
        printPrompts();

        scanf("%s", input);
        int option = atoi(input);

        switch (option) {
            case 1:
                if(inputBoolean == 1){
                  printf("Input file already loaded. Please exit to load new file.");
                  break;
                }
                printf("1. Type the file name: ");
                scanf("%s", input);

                if (fopen(input, "r") == NULL) {
                    printf("Error: Entered invalid file name\n");
                    continue;
                }

                inputBoolean = 1;
                fp = fopen(input, "r");

                loadFile(&test, fp);
                mergeFreeBlocks(&test);

                if (testMemoryView(&test) == -1) {
                    test = NULL;
                    head = NULL;
                    continue;
                }

                rewind(fp);

                loadFile(&head, fp);
                mergeFreeBlocks(&head);
                printf("\noperation successful\n");
                break;
            case 2:
                mergeHoles(&head);
                printf("\noperation successful\n");
                break;
            case 3:
                compaction(&head);
                printf("\noperation successful\n");
                break;
            case 4:
//                printMemoryView(&test);
                printf("\noperation successful\n");
                printMemoryView(&head);
                printf("operation successful\n");
                break;
            case 5:
                return 0;
            default:
                printf("Invalid option\n");
                continue;
        }
    }
}
/**
 * void printPrompts()
 * Summary of printPrompts function:
 *
 *          This method simply prints the user prompts for required intputs.
 *
 *          Parameters: nothing
 *          return value: Nothing
 *    Description:
 *
 *          This function prints the choices for the user prompts.
 */
void printPrompts() {
    printf("\n1. load an input file\n");
    printf("2. merge holes\n");
    printf("3. compact memory\n");
    printf("4. print memory view\n");
    printf("5. Exit the program\n");
}

/**
 * void printMemoryView(Node** head)
 * Summary of printMemoryView function:
 *
 *          This method prints the most current structure of the data.
 *
 *          Parameters: typedef Node double pointer
 *          return value: Nothing
 *    Description:
 *
 *          This method prints the most current structure of the data after which ever sorting function they chose to implement from the initial prompts.
 */
void printMemoryView(Node** head) {
    Node* current = *head;
    printf("\nCurrent List:\n");
    int i = 1;
    while (current != NULL) {
        printf("Node %d: ", i);
        i++;
        if(current->identifier[0] == 'H') {
          printf("%s (Hole), base = %d, limit = %d\n", current->identifier, current->base, current->limit);
        } else {
          printf("%s, base = %d, limit = %d\n", current->identifier, current->base, current->limit);
        }
        current = current->next;
    }
}

/**
 * testMemoryView(Node** head)
 * Summary of testMemoryView function:
 *
 *          This method prints the prints a secondary ordered linked list and checks if there are overlaps in bases
 *
 *          Parameters: typedef Node double pointer
 *          return value: if there is an overlap between nodes, then it will return the line number of the error on file.
 *          returns 1 if no overlap
 *          returns -1 if overlap is present
 *
 *    Description:
 *
 *          This method prints the a sorted linked list checks if any of the nodes either overlap or create empty blocks.
 *
 */
int testMemoryView(Node** head) {
    Node* current = *head;

    if (current->next == NULL) {
        return 1;
    }

    Node* nextNode = current->next;
    if(current->limit >= 0) {
        int i = 1;

        while (current != NULL) {
            if (checkOverlap(current, nextNode) == 1) {
//            printf("Node %d: ", i);
//            printf("%s %d %d\n", current->identifier, current->base, current->limit);
                current = current->next;
                if (current->next == NULL) {
                    return 1;
                }
                nextNode = current->next;
                i++;
            } else {
                printf("Error found after line %d\n", i + 1);
                return -1;
            }
        }
    } else {
        printf("Error head node limit is negative\n");
        return -1;
    }
}


/**
 * void checkOverlap(Node* current, Node* nextNode)
 * Summary of checkOverlap function:
 *
 *          This method checks if 2 nodes overlap or create empty blocks in between them.
 *
 *          Parameters: Node* current, Node* nextNode
 *          return value: if there is an overlap between nodes, then it will return the line number of the error on file.
 *          returns 1 if no overlap
 *          returns -1 if overlap is present
 *
 *    Description:
 *
 *          This method checks the sorted test linked list has either an overlap present or an empty space.
 */
int checkOverlap(Node* current, Node* nextNode){
    if(nextNode->limit < 0 && current->base + current->limit != nextNode->base) {
        printf("Error: limit is negative\n");
        return -1;
    } else if (nextNode->limit >= 0  && current->base + current->limit != nextNode->base ) {
            printf("Error: Overlapping Bases or invalid empty memory slot\n");
            return -1;
        }
    return 1;
}



/**
 * void createNode(Node** head, FILE* fp)
 * Summary of createNode function:
 *
 *          This method takes in the file pointer and perses the input text to store each line into a node linking them until the end of file.
 *
 *          Parameters: typedef Node head double pointer, File pointer
 *          return value: Linked list of typedef Node
 *    Description:
 *
 *        This method takes in an input text file.
 *        It then parses its data line per line and allocates memory for each line at a size of a Node
 *        and links each node to each other until EOF.
 */
void createNode(Node** head, FILE* fp) {
    Node* current = *head;

    Node* newNode = calloc(1, sizeof(struct Node));

    char line[20];
    fgets(line, 20, fp);

    char identifier[5];
    int base;
    int limit;

    sscanf(line, "%s %d %d", identifier, &base, &limit);

    newNode->identifier = malloc(sizeof(char) * strlen(identifier) + 1);
    strcpy(newNode->identifier, identifier);

    newNode->base = base;
    newNode->limit = limit;

    if (*head == NULL) {
        *head = newNode;
        return;
    }

    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;

}

/**
* void loadFile(Node** head , FILE* fp)
* Summary of loadFile function:
*
*          This method takes in the file pointer and checks if fp is not EOF.
*
*          Parameters: typedef Node head double pointer, File pointer
*          return value: Linked list of typedef Node
*    Description:
*
*        This method takes in an input text file then passes it into the createNode().
*/
void loadFile(Node** head , FILE* fp) {
    char c;

    int i = 0;

    while (fp) {
        createNode(head, fp);
        c = fgetc(fp);
        if (c == EOF) {
            return;
        }
        fseek(fp, -1, SEEK_CUR);
        i++;
    }

}

/**
* void mergeHoles(Node** head)
* Summary of mergeHoles function:
*
*          This method merges holes in the current linked list.
*
*          Parameters: Node head
*          return value: linked list with merged holes
*    Description:
*
*        This method takes in the head of the linked list and merge sorts the nodes then checks if each node is a hole or not.
 *        Once holes are detected, the method merges them by keeping their bases and combining their limits.
 *        The extra holes removed and the minimum amount of combined holes are left.
*/
void mergeHoles(Node** head) {
    Node* current = *head;
    Node* hole = NULL;
    Node* freeHole = NULL;

    int limitCount;

    while (current != NULL) {
        if (current->identifier[0] == 'H')
        {
            hole = current;
            limitCount = hole->limit;
            if (current->next != NULL)
            {
                while (current->next->identifier[0] == 'H')
                {
                    current = current->next;
                    limitCount += current->limit;

                    freeHole = current;

                    if (current->next == NULL)
                    {
                        hole->next = current->next;
                        hole->limit = limitCount;
                        return;
                    }

                }

                hole->next = current->next;
                hole->limit = limitCount;
                current = current->next;

                free(freeHole);
            } else {
                return;
            }
        } else {
            current = current->next;
        }
    }
}

/**
* void compaction(Node** head)
* Summary of compaction function:
*
*          This method merges holes in the current linked list and pushes the combined holes to the back of the linked list.
*
*          Parameters: Node head
*          return value: linked list with merged holes at the back
*    Description:
*
*        This method takes in the head of the linked list and merge sorts the nodes then checks if each node is a hole or not.
 *        Once holes are detected, the method merges them by keeping their bases and combining their limits.
 *        The combined holes are pushed to the back of the linked list.
*/
void compaction(Node** head) {

    Node *current = *head;
    Node *hole = NULL;

    int limitCount;

    while (current != NULL) {

        if (current->next != NULL) {
            while (current->next->identifier[0] == 'H') {
                hole = current->next;
                limitCount += hole->limit;

                if (hole->next != NULL) {
                    current->next = hole->next;
                    current->next->base = current->base + current->limit;
                    free(hole);
                } else {
                    hole->base = current->base + current->limit;
                    hole->limit = limitCount;
                    return;
                }
            }
        } else if (current->next == NULL && hole != NULL) {
            hole->identifier[0] = 'H';
            hole->base = current->base + current->limit;
            hole->limit = limitCount;
            current->next = hole;
            hole->next = NULL;
            return;
        } else {
          return;
        }
        current = current->next;
    }
}


/**
* int compareValues(Node* d1, Node* d2)
* Summary of compareValues function:
*
*          This method compares if one node has a greater or lesser base value to another node.
*
*          Parameters: Node pointer1, Node pointer2
*          return value: return 1 if d1 < d2 and return -1 if d1 > d2
*    Description:
*
*        This method takes in 2 Node pointers and compares which base value is larger.
*/
int compareValues(Node* d1, Node* d2) {
    int value1 = d1->base + d1->limit;
    int value2 = d2->base + d2->limit;

    if (value1 < value2) {
        return 1;
    } else if (value1 > value2) {
        return -1;
    }
    return 0;
}

/**
* Node* sortedMerge(Node* d1, Node* d2)
* Summary of sortedMerge function:
*
*          Using the returned values from compareValues(), this method reassigns new positions for each Node.
*
*          Parameters: Node pointer1, Node pointer2
*          return value:
*           result->next = sortedMerge(d1->next, d2);
*           OR
*           result->next = sortedMerge(d1, d2->next);
*
*   Description:
*
*        This method takes in 2 Node pointers and compares which base value is larger then reassigns new positions for each Node
*        from smaller to larger.
*/
Node* sortedMerge(Node* d1, Node* d2) {
    Node* result = NULL;

    if (d1 == NULL) {
        return d2;
    } else if (d2 == NULL) {
        return d1;
    }

    int value = compareValues(d1, d2);

    /*
     * Returns 1 if d1 is greater than d2
     * Returns 0 if d1 is equal to d2
     * Returns -1 if d1 is less than d2
     */

    if (value >= 0) {
        result = d1;
        result->next = sortedMerge(d1->next, d2);
    } else {
        result = d2;
        result->next = sortedMerge(d1, d2->next);
    }

    return result;
}


/**
* void frontBackSplit(Node* source, Node** frontRef, Node** backRef)
* Summary of frontBackSplit function:
*
*          Option:2 Merge sort function by dividing the linked list into halves by using a fast and slow pointer.
*
*          Parameters:
*          Node* source, Node** frontRef, Node** backRef
*
*          Return value:
*          a split linked list with new heads assigned to a front and back linked lists
*
*   Description:
*
*        This method takes a node head and splits the linked list into 2 sides with a front reference and a back reference that is
*        the middle point of the list.
*/
void frontBackSplit(Node* source, Node** frontRef, Node** backRef) {
    Node* fast;
    Node* slow;

    slow = source;
    fast = source->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

/**
* void mergeFreeBlocks(Node** headRef)
* Summary of mergeFreeBlocks function:
*
*         Merge sort function by combining the linked list from the halves.
*
*          Parameters:
*          Node** headRef
*
*          Return value:
*          a split linked list with new heads assigned to a front and back linked lists
*
*   Description:
*
*        This method takes a node head and combines the split halves into pairs.
*/
void mergeFreeBlocks(Node** headRef) {
    Node* head = *headRef;
    Node* a;
    Node* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    frontBackSplit(head, &a, &b);

    mergeFreeBlocks(&a);
    mergeFreeBlocks(&b);

    *headRef = sortedMerge(a, b);

}

