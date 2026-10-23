#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"

static Product *products = NULL; //linked list of products
static int next_batch_id = 1;
static int revenue = 0, cogs = 0, gift_cost = 0;

static Product *find_product(const char *pid){
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

static Product *create_product(const char *pid, const char *name){
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
    p->stock = 0;
    p->value = 0;
    p->batches = NULL;
    p->next = NULL;

    //adding new product to product list
    if(products == NULL){
        products = p;
    }else if(strcmp(pid, products->pid) < 0){
        p->next = products;
        products = p;
    }else{
        Product *cur = products, *prev = NULL;
        while(cur != NULL){
            if(strcmp(pid, cur->pid) > 0){
                prev = cur;
                cur = cur->next;
            }else{
                break;
            }
        }
        p->next = cur;
        prev->next = p;
    }
    return p;
}

static Batch *create_batch(Product *product, int quantity, int unit_cost){
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
    product->stock += quantity;
    product->value += (quantity*unit_cost);
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
        strncpy(pid, head_ptr, len);
        pid[len] = '\0';
        Product *product = find_product(pid);

        head_ptr = tail_ptr + 3;
        tail_ptr = strstr(line, "@@@");
        len = tail_ptr-head_ptr;
        strncpy(name, head_ptr, len);
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
}

void PRICE(const char *pid, int price){
    Product *product = find_product(pid);

    if(product == NULL){
        printf("ERR NO_PRODUCT\n");
        return;
    }

    product->price = price;
    printf("OK PRICE %s %d\n", pid, price);
}

void QUERY(const char *pid){
    Product *product = find_product(pid);

    if(product == NULL){
        printf("ERR NO_PRODUCT\n");
        return;
    }

    if(product->price == -1){
        printf("QUERY %s %s price=none stock=%d\n", pid, product->name, product->stock);
    }else{
        printf("QUERY %s %s price=%d stock=%d\n", pid, product->name, product->price, product->stock);
    }
    
    if(product->stock != 0){
        Batch *ptr = product->batches;
        while(ptr != NULL){
            printf("  #%d qty=%d cost=%d\n", ptr->batch_id, ptr->quantity, ptr->unit_cost);
            ptr = ptr->next;
        }
    }
}

void REPORT(){
    printf("REPORT\n");
    Product *ptr = products;
    while(ptr != NULL){
        printf("  %s stock=%d value=%d\n", ptr->pid, ptr->stock, ptr->value);
        ptr = ptr->next;
    }
    printf("REVENUE %d\nCOGS %d\nGIFTCOST %d\nPROFIT %d\n", revenue, cogs, gift_cost, revenue-cogs-gift_cost);
}

void QUIT(){
    Product *ptr = products;
    while(ptr != NULL){
        Product *next_product = ptr->next;

        Batch *ptr_batch = ptr->batches;
        while(ptr_batch != NULL){
            Batch *next_batch = ptr_batch->next;
            free(ptr_batch);
            ptr_batch = next_batch;
        }

        free(ptr);
        ptr = next_product;
    }
    //cart and cartItem

    printf("BYE\n");
}