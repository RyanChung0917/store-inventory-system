#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"

static Product *products = NULL; //linked list of products
static int next_batch_id = 1;

Product *find_product(const char *pid){
    Product *ptr = products;
    while(ptr != NULL){
        if(strcmp(pid, ptr->pid) == 0){
            break;
        }else{
            ptr = ptr->next;
        }
    }
    return ptr;
}

Product *create_product(const char *pid, const char *name){
    //create a new product
    Product *p = malloc(sizeof(Product));
    if(p == NULL){
        return NULL;
    }

    p->pid = malloc(strlen(pid) + 1);
    if(p->pid == NULL){
        free(p);
        return NULL;
    }
    strcpy(p->pid, pid);

    p->name = malloc(strlen(name) + 1);
    if(p->name == NULL){
        free(p->pid);
        free(p);
        return NULL;
    }
    strcpy(p->name, name);

    p->price = -1;
    p->batches = NULL;
    p->next = NULL;

    //adding new product to products
    if(products == NULL){
        products = p;
    }else{
        Product *ptr = products;
        while(ptr->next != NULL){
            ptr = ptr->next;
        }
        ptr->next = p;
    }
    return p;
}

Batch *create_batch(Product *product, int quantity, int unit_cost){
    //create a new batch
    Batch *b = malloc(sizeof(Batch));
    if(b == NULL){
        return NULL;
    }

    b->batch_id = next_batch_id;
    b->quantity = quantity;
    b->unit_cost = unit_cost;
    b->next = NULL;
    next_batch_id++;

    //adding new batch to product
    if(product->batches == NULL){
        product->batches = b;
    }else{
        Batch *ptr = product->batches;
        while(ptr->next != NULL){
            ptr = ptr->next;
        }
        ptr->next = b;
    }
    return b;
}

void IMPORT(const char *filename){
    FILE *fp = fopen(filename, "r");

    if(fp == NULL){
        printf("ERR FILE_OPEN\n");
        return;
    }

    char line[1024];
    int batch_cnt = 0;
    while(fgets(line, sizeof(line), fp) != NULL){
        char *head_ptr = line;
        char *tail_ptr;
        char pid[18], name[35];
        int qty = 0, unit_cost = 0, len;
        
        tail_ptr = strstr(line, "###");
        len = tail_ptr-head_ptr;
        strncpy(pid, line, len);
        pid[len] = '\0';
        Product *product = find_product(pid);

        head_ptr = tail_ptr + 3;
        tail_ptr = strstr(line, "@@@");
        len = tail_ptr-head_ptr;
        strncpy(name, line, len);
        name[len] = '\0';

        head_ptr = tail_ptr + 3;
        tail_ptr = strstr(line, "&&&");
        while(head_ptr < tail_ptr){
            if(*head_ptr >= '0' && *head_ptr <= '9'){
                qty = qty * 10 + (*head_ptr - '0');
            }
            head_ptr++;
        }

        head_ptr = tail_ptr + 3;
        while(*head_ptr != '\0' && *head_ptr != '\n'){
            if(*head_ptr >= '0' && *head_ptr <= '9'){
                unit_cost = unit_cost * 10 + (*head_ptr - '0');
            }
            head_ptr++;
        }

        if(product == NULL){
            product = create_product(pid, name);
        } 
        Batch *batch = create_batch(product, qty, unit_cost);
        if(batch != NULL){
            batch_cnt++;
        }
    }

    fclose(fp);
    printf("OK IMPORT %d\n", batch_cnt);
    return;
}

void run_program(void){
    printf("Inventory system started.\n");
}