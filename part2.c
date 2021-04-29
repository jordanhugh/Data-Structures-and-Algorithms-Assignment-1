#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 15

typedef struct Name {
    char name[MAX_BUFFER];
    int frequency;
}Name;

int hash1(char* s, int capacity);
int hash2(char* s, int capacity);
int nextField(FILE *csv, char *buffer, int max_len);
void fillHashTable(FILE *csv, Name *table, int capacity, int *numterms, int *collisions);
void printHashTable(Name *table, int capacity);
void printData(int capacity, int numterms, int collisions);
void search(Name* table, int capacity);

int main(){
    Name *hash_table;
    int capacity = 50; int numterms = 0; int collisions = 0;
    hash_table = (Name*)malloc(capacity * sizeof(Name));

    //Try to open input file. If there is a problem, report failure and quit
    FILE *pFile;
    pFile = fopen("names.csv", "r");
    if (pFile == NULL){
        perror ("Error opening file");
    }
    else{
        printf("File opened successfully!\n");
    }

    fillHashTable(pFile, hash_table, capacity, &numterms, &collisions);
    printHashTable(hash_table, capacity);
    printData(capacity, numterms, collisions);
    search(hash_table, capacity);
    free(hash_table);

    return 0;
}

int hash1(char* s, int capacity){
    int hash = 0;
    while(*s){
        hash = (hash + *s)%capacity;
        s++;
    }
    return hash;
}

int hash2(char* s, int capacity){
    int hash = 0;

    while(*s){
        hash = (37 * hash + *s)%capacity;
        s++;
    }
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

void fillHashTable(FILE *csv, Name *table, int capacity, int *numterms, int *collisions){
    char buf[MAX_BUFFER];
    int hash;
    bool rehash;

    //Reads in Records
    while(!feof(csv)){
	    nextField(csv, buf, MAX_BUFFER);
        hash = hash2(buf, capacity);

        int i = hash;
        while (i%capacity != hash - 1){ //Searches until it has checked every index once
            if ((table + i)->name[0] == '\0'){ //Checks if Index is empty
                strcpy((table + i)->name, buf);
                (table + i)->frequency++;
                *numterms = *numterms + 1;
                break;
            }
            else if(strcmp((table + i)->name, buf) == 0){ //Checks if Surname is the same
                (table + i)->frequency++;
                break;
            }
            else{ //If Index is not empty or Surname is different, find new index
                *collisions = *collisions + 1;
                i++;
            }
        }
    }
}

void printHashTable(Name *table, int capacity){ 

    for (int i = 0; i < capacity; i++){ 
        if((table + i)->frequency > 0){ //Checks if Index is full before printing
            printf("%d", i);
            printf(" %s", (table + i)->name);
            printf(" %d\n", (table + i)->frequency);
            
        }
    }
    
    printf("\n\n");
}

void printData(int capacity, int numterms, int collisions){
    double load = (double)numterms/capacity * 100;

    printf("Capacity: %d\n", capacity);
    printf("Num Terms: %d\n", numterms);
    printf("Collisions: %d\n", collisions);
    printf("Load: %.3f", load);
    printf("%%\n");
}

void search(Name* table, int capacity){
    char name[MAX_BUFFER];
    int hash;

    printf("Enter term to get frequency or type ""quit"" to escape.\n");
    printf(">>> ");
    scanf("%s", name);
    while(strcmp(name, "quit") != 0){ //Terminates Loop if the user enters "quit"

        hash = hash2(name, capacity);

        int i = hash;
        //Searches until it has checked every index once
        while(i%capacity != hash - 1){
            if(strcmp((table + i)->name, name) == 0){
                printf("%s", (table + i)->name);
                printf(" %d\n", (table + i)->frequency);
                break;
            }

            i++;
        }

        //If the index is empty tell the user
        if(i%capacity == hash - 1){
            printf("%s not in table.\n", name);
        }

        printf(">>> ");
        scanf("%s", name);
    }
}