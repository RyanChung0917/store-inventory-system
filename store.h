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
    int stock, value;
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

void IMPORT(const char *filename);

void PRICE(const char *pid, int price, int output);

void QUERY(const char *pid);

void REPORT();

void QUIT();

void CART(const char *cid);

void ADD(const char *cid, const char *pid, int qty);

void CHECKOUT(char *cid);

void PROMO(const char *pid_a, int n, const char *pid_b, int P);

void SAVE(const char *filename);

void LOAD(const char *filename);

#endif