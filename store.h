#ifndef STORE_H
#define STORE_H

typedef struct Batch{
    int batch_id;
    int unit_cost;
    int quantity;
    struct Batch *next;
}Batch;

typedef struct Product{
    int pid;
    char *name;
    int price;
    Batch *batches;
    struct Product *next;
}Product;

typedef struct CartItem{
    int pid;
    int quantity;
    struct CartItem *next;
}CartItem;

typedef struct Cart{
    int cid;
    CartItem *items;
    struct Cart *next;
}Cart;

void run_program(void);

#endif