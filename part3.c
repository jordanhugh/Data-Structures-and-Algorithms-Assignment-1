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
int hash3(char* s, int i, int capacity); //hash 3 = hash1 + hash2
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

int hash1(char *s, int capacity){
    int hash = 0;

    while(*s){
        hash = (37 * hash + *s)%capacity;
        s++;
    }
    return hash;
}

int hash2(char *s, int capacity){
    int hash = (47 - hash1(s, capacity)%47)%capacity;

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

void fillHashTable(FILE *csv, Name *table, int capacity, int *numterms, int *collisions){
    char buf[MAX_BUFFER];
    int hash;
    bool collision;

    //Reads in Records
    while(!feof(csv)){
	    nextField(csv, buf, MAX_BUFFER);
        
        int i = 0;
        do{ //Calculate Hash Value
            hash = hash3(buf, i, capacity);

            if ((table + hash)->name[0] == '\0'){ //Checks if Index is empty
                strcpy((table + hash)->name, buf);
                (table + hash)->frequency++;
                *numterms = *numterms + 1;
                collision = false;
            }
            else if(strcmp((table + hash)->name, buf) == 0){ //Checks if Surname is the same
                (table + hash)->frequency++;
                collision = false;
            }
            else { //If Index is not empty or Surname is different, find new index
                *collisions = *collisions + 1;
                collision = true;
                i++;
            }
        }while(collision);
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

        int i = 0;
        //Calculates hash value and recalculates until it reaches the index or an empty cell
        //If the index is empty that there is no index with that surname 
        do{
            hash = hash3(name, i, capacity);
            if(strcmp((table + hash)->name, name) == 0){
                printf("%s", (table + hash)->name);
                printf(" %d\n", (table + hash)->frequency);
                break;
            }
            i++;

        }while((table + hash)->name[0] != '\0');
        
        //If the index is empty tell the user
        if((table + hash)->name[0] == '\0'){
            printf("%s not in table.\n", name);
        }
        printf(">>> ");
        scanf("%s", name);
    }
}