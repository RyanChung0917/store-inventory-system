#ifndef STORE_H
#define STORE_H

typedef struct Batch{
    int batch_id;
    int unit_cost;
    int quantity;
    struct Batch *next;
}Batch;

typedef struct Product{
    char *pid;
    char *name;
    int price;
    Batch *batches;
    struct Product *next;
}Product;

typedef struct CartItem{
    char *pid;
    int quantity;
    struct CartItem *next;
}CartItem;

typedef struct Cart{
    char *cid;
    CartItem *items;
    struct Cart *next;
}Cart;

void IMPORT(char *filename);

void run_program(void);

#endif