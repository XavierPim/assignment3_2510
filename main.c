#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Node {
    char* identifier;
    int base;
    int limit;
    struct Node* next;
} Node;

void printPrompts() {
    printf("1. load an input file\n");
    printf("2. merge holes\n");
    printf("3. compact memory\n");
    printf("4. print memory view\n");
    printf("5. Exit the program\n");
}

void printMemoryView(Node** head) {
    Node* current = *head;

    while (current != NULL) {
        printf("Node: ");
        printf("%s %d %d\n", current->identifier, current->base, current->limit);
        current = current->next;
    }

}

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

void mergeHoles(Node** head) {
    Node* current = *head;
    Node* hole = NULL;

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
                }
            }
            if (current->next != NULL)
            {
                hole->next = current->next;
                hole->limit = limitCount;
                current = current->next;
            } else
            {
                return;
            }
        } else {
            current = current->next;
        }
    }

}

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

void mergeSort(Node** headRef) {
    Node* head = *headRef;
    Node* a;
    Node* b;

    if ((head == NULL) || (head->next == NULL)) {
        return;
    }

    frontBackSplit(head, &a, &b);

    mergeSort(&a);
    mergeSort(&b);

    *headRef = sortedMerge(a, b);

}

int main() {
    char input[50];
    Node* head = NULL;

    while (true) {
        printPrompts();

        scanf("%s", input);
        int choice = atoi(input);

        switch (choice) {
            case 1:
                printf("1. Type the file name: ");
                scanf("%s", input);
                FILE *fp = fopen(input, "r");
                loadFile(&head, fp);
                mergeSort(&head);
                printf("operation successful\n");
                break;
            case 2:
                mergeHoles(&head);
                printf("operation successful\n");
                break;
            case 3:
                break;
            case 4:
                printMemoryView(&head);
                printf("operation successful\n");
                break;
            case 5:
                return 0;
            default:
                printf("Invalid choice\n");
                continue;
        }
    }
}