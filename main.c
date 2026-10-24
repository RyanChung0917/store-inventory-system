#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"

int main(void){
    char line[256];
    char command[20];
    char arg1[100], arg2[100];
    int price = -1, qty = -1, discount = -1;
    while(fgets(line, sizeof(line), stdin) != NULL){
        if(sscanf(line, "%10s", command) != 1){
            continue;
        }

        if(strcmp(command, "IMPORT") == 0){
            sscanf(line, "%*s%99s", arg1);
            IMPORT(arg1);
        }else if(strcmp(command, "PRICE") == 0){
            sscanf(line, "%*s%16s%d", arg1, &price);
            PRICE(arg1, price, 1);
        }else if(strcmp(command, "QUERY") == 0){
            sscanf(line, "%*s%16s", arg1);
            QUERY(arg1);
        }else if(strcmp(command, "REPORT") == 0){
            REPORT();
        }else if(strcmp(command, "QUIT") == 0){
            QUIT();
            return 0;
        }else if(strcmp(command, "CART") == 0){
            sscanf(line, "%*s%16s", arg1);
            CART(arg1);
        }else if(strcmp(command, "ADD") == 0){
            sscanf(line, "%*s%16s%16s%d", arg1, arg2, &qty);
            ADD(arg1, arg2, qty);
        }else if(strcmp(command, "CHECKOUT") == 0){
            sscanf(line, "%*s%16s", arg1);
            CHECKOUT(arg1);
        }else if(strcmp(command, "PROMO") == 0){
            sscanf(line, "%*s%16s%d%16s%d", arg1, &qty, arg2, &discount);
            PROMO(arg1, qty, arg2, discount);
        }else if(strcmp(command, "SAVE") == 0){
            sscanf(line, "%*s%99s", arg1);
            SAVE(arg1);
        }else if(strcmp(command, "LOAD") == 0){
            sscanf(line, "%*s%99s", arg1);
            LOAD(arg1);
        }
    }
    return 0;
}