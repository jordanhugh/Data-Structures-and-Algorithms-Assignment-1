#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 30

typedef struct Surname {
    char surname[MAX_BUFFER];
    struct Person *head;
}Surname;

typedef struct Person{
    int personID;
    char depositionID[MAX_BUFFER];
    char forename[MAX_BUFFER];
    int age;
    char personType[MAX_BUFFER];
    char gender[MAX_BUFFER];
    char nationality[MAX_BUFFER];
    char religion[MAX_BUFFER];
    char occupation[MAX_BUFFER];
    struct Person *next;
}Person;

int hash1(char* s, int capacity);
int hash2(char* s, int capacity);
int hash3(char* s, int i, int capacity); //hash 3 = hash 1 + hash 2

int nextField(FILE *csv, char *buffer, int max_len);
void fillHashTable(FILE *csv, Surname *table, int capacity, int *numterms, int *collisions);

void printHashTable(Surname *table, int capacity);
void printLinkedList(Surname* table, int hash);
void printData(int capacity, int numterms, int collisions);
void search(Surname* table, int capacity);
void deleteLinkedList(Surname* table, int capacity);

int main(){
    Surname *hash_table;
    int capacity = 99991; int numterms = 0; int collisions = 0;
    hash_table = (Surname*)malloc(capacity * sizeof(Surname));

    //Try to open input file. If there is a problem, report failure and quit
    FILE *pFile;
    pFile = fopen("people.csv", "r");
    if (pFile == NULL){
        perror ("Error opening file");
    }
    else{
        printf("File opened successfully!\n");
    }

    fillHashTable(pFile, hash_table, capacity, &numterms, &collisions);
    printData(capacity, numterms, collisions);
    search(hash_table, capacity);
    
    deleteLinkedList(hash_table, capacity);    
    free(hash_table);
    
    return 0;
}



int hash1(char *s, int capacity){
    int hash = 0;

    while(*s){
        hash = (37 * hash + *s)%capacity;
        s++;
    }
    return hash;
}

int hash2(char *s, int capacity){
    int hash = (89989 - hash1(s, capacity)%89989)%capacity;

    return hash;
}

int hash3(char* s, int i, int capacity){
    int hash = (hash1(s, capacity) + i * hash2(s, capacity))%capacity;
    
    return hash;
}



int nextField(FILE *csv, char *buffer, int max_len){
    char c;
    bool quoted = false;

    for(int i = 0; i < max_len; i++){
        //Read in next character from line
        c = getc(csv);

        if (c == '"'){ //If we encounter quotation makrs then flip our state and read in the next character
            quoted = !quoted;
            i--;
        }

        else if (c == ',' && !quoted){ //End Record if we reach comma
            buffer[i] = '\0';
            return 0;
        }

        else if (c == '\n' || c == EOF){ //End Record if we reach new line or truncates fields that would overflow the buffer
            buffer[i] = '\0';
            return 1;
        }

        else{
            buffer[i] = c;
        }
    }
}

void fillHashTable(FILE *csv, Surname *table, int capacity, int *numterms, int *collisions){
    char buf[MAX_BUFFER];
    int hash;
    bool collision;

    //Remove Row 1
    char c;
    do{
        c = getc(csv);
    }while(c != '\n');

    //Reads in Records
    while(!feof(csv)){

        //Declare Node and Allocate Memory
        Person* person = (Person*)malloc(sizeof(Person));

        //Begin Initalising Node
        nextField(csv, buf, MAX_BUFFER); //Load PersonID into buf as string
        person->personID = atoi(buf); //Parse to Integer
        nextField(csv, buf, MAX_BUFFER); //Repeat...
        strcpy(person->depositionID, buf);
	    nextField(csv, buf, MAX_BUFFER);
        
        int i = 0;
        do{ //Calculate Hash Value
            hash = hash3(buf, i, capacity);

            if ((table + hash)->surname[0] == '\0' ){ //Checks if Index is empty
                strcpy((table + hash)->surname, buf);
                *numterms = *numterms + 1;
                collision = false;
            }
            else if(strcmp((table + hash)->surname, buf) == 0){ //Checks if Surname is the same
                collision = false;
            }
            else { //If Index is not empty or Surname is different, find new index
                *collisions = *collisions + 1;
                collision = true;
                i++;
            }
        }while(collision);
        
        //Finish Initialising Node
        nextField(csv, buf, MAX_BUFFER);
        strcpy(person->forename, buf);
        nextField(csv, buf, MAX_BUFFER);
        person->age = atoi(buf);
        nextField(csv, buf, MAX_BUFFER);
        strcpy(person->personType, buf);
        nextField(csv, buf, MAX_BUFFER);
        strcpy(person->gender, buf);
        nextField(csv, buf, MAX_BUFFER);
        strcpy(person->nationality, buf);
        nextField(csv, buf, MAX_BUFFER);
        strcpy(person->religion, buf);
        nextField(csv, buf, MAX_BUFFER);
        strcpy(person->occupation, buf);
        person->next = NULL;

        //Add Node to head of Linked List
        if((table + hash)->head != NULL){
            person->next = (table + hash)->head;  
        }
        (table + hash)->head = person;
    }
}



void printHashTable(Surname *table, int capacity){

    for (int i = 0; i < capacity; i++){
        if((table + i)->surname[0] != '\0'){ //Checks if Index is full before printing
            printf("%d", i);
            printf(" %s\n", (table + i)->surname);  
        }
    }
    
    printf("\n\n");
}

void printLinkedList(Surname* table, int hash){
    Person* curr = (table + hash)->head; //Initiate curr as the head of the Linked List

    printf("Person ID     ");
    printf("Deposition ID     ");
    printf("Surname     ");
    printf("Forename     ");
    printf("Age     ");
    printf("Person Type\n");

	while (curr != NULL){ //Interates through Linked List until it reaches the tail
		printf("%9d", curr->personID);
        printf("%18s", curr->depositionID);
        printf("%12s", (table + hash)->surname);
        printf("%13s", curr->forename);
        printf("%8d", curr->age);
        printf("%16s\n", curr->personType);

		curr = curr->next; 
	}
}

void printData(int capacity, int numterms, int collisions){
    double load = (double)numterms/capacity * 100;

    printf("Capacity: %d\n", capacity);
    printf("Num Terms: %d\n", numterms);
    printf("Collisions: %d\n", collisions);
    printf("Load: %.3f", load);
    printf("%%\n");
}

void search(Surname* table, int capacity){
    char name[MAX_BUFFER];
    int hash;

    printf("Enter term to get frequency or type ""quit"" to escape.\n");
    printf(">>> ");
    scanf("%s", name);
    while(strcmp(name, "quit") != 0){ //Terminates Loop if the user enters "quit"

        int i = 0;
        //Calculates hash value and recalculates until it reaches the index or an empty cell
        //If the index is empty that there is no index with that surname 
        do{
            hash = hash3(name, i, capacity);
            if(strcmp((table + hash)->surname, name) == 0){
                printLinkedList(table, hash);
                break;
            }
            i++;
        }while((table + hash)->surname[0] != '\0');
        
        //If the index is empty tell the user
        if((table + hash)->surname[0] == '\0'){
            printf("%s not in table.\n", name);
        }
        printf(">>> ");
        scanf("%s", name);
    }
}

void deleteLinkedList(Surname* table, int capacity){
    Person *next, *curr;

    for (int i = 0; i < capacity; i++){
        next = curr = (table + 1)->head;

        while (curr != NULL) {
            next = curr->next;
            free(curr);
            curr = next;
        }

        (table + 1)->head = NULL;
    }
}