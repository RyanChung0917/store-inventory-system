#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"

int main(void){
    char line[256];
    char command[20];
    char arg[100];
    int price = -1;
    while(fgets(line, sizeof(line), stdin) != NULL){
        if(sscanf(line, "%10s", command) != 1){
            continue;
        }

        if(strcmp(command, "IMPORT") == 0){
            sscanf(line, "%*s%99s", arg);
            IMPORT(arg);
        }else if(strcmp(command, "PRICE") == 0){
            sscanf(line, "%*s%16s%d", arg, &price);
            PRICE(arg, price);
        }else if(strcmp(command, "QUERY") == 0){
            sscanf(line, "%*s%16s", arg);
            QUERY(arg);
        }else if(strcmp(command, "REPORT") == 0){
            REPORT();
        }else if(strcmp(command, "QUIT") == 0){
            QUIT();
        }else{
            printf("ERR\n");
            return 0;
        }
    }
    return 0;
}