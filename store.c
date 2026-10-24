#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"

static Product *products = NULL; //linked list of products
static Cart *carts = NULL;//linked list of carts
static int next_batch_id = 1, product_cnt = 0, batch_cnt = 0;
static int revenue = 0, cogs = 0, gift_cost = 0;
static char promo_pid[16], discount_pid[16];
static int promo_threshold = -1, discount = 100, have_discount = 0;

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

static Cart *find_cart(const char *cid){
    Cart *ptr = carts;
    while(ptr != NULL){
        if(strcmp(cid, ptr->cid) == 0){
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
    product_cnt++;
    return p;
}

static Batch *create_batch(Product *product, int quantity, int unit_cost, int batch_id){
    //create a new batch
    Batch *b = malloc(sizeof(Batch));
    if(b == NULL){
        return NULL;
    }

    b->batch_id = batch_id;
    b->quantity = quantity;
    b->unit_cost = unit_cost;
    b->next = NULL;

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
    batch_cnt++;
    return b;
}

static void free_product(Product *p){
    Batch *b = p->batches;
    while(b != NULL){
        Batch *next_b = b->next;
        free(b);
        b = next_b;
    }
    free(p->pid);
    free(p->name);
    free(p);
}

static void free_cart(Cart *c){
    CartItem *item = c->items;
    while(item != NULL){
        CartItem *next_item = item->next;
        free(item->pid);
        free(item);
        item = next_item;
    }
    free(c->cid);
    free(c);
}

static void close_cart(char *cid){
    Cart *prev = NULL, *cur = carts;
    while(strcmp(cid, cur->cid) != 0){
        prev = cur;
        cur = cur->next;
    }

    if(prev == NULL){
        carts = carts->next;
    }else{
        prev->next = cur->next;
    }
    free_cart(cur);
}

static void delete_batch(Batch *b, Product *p){
    Batch *prev = NULL, *cur = p->batches;
    while(cur != b){
        prev = cur;
        cur = cur->next;
    }

    p->stock -= b->quantity;
    p->value -= (b->quantity * b->unit_cost);
    batch_cnt--;
    if(prev == NULL){
        p->batches = cur->next;
    }else{
        prev->next = cur->next;
    }
    free(b);
}

void IMPORT(const char *filename){
    FILE *fp = fopen(filename, "r");

    if(fp == NULL){
        printf("ERR FILE_OPEN\n");
        return;
    }

    char line[256];
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
        Batch *batch = create_batch(product, qty, unit_cost, next_batch_id);
        next_batch_id++;
        if(batch != NULL){
            batch_cnt++;
        }
    }

    fclose(fp);
    printf("OK IMPORT %d\n", batch_cnt);
}

void PRICE(const char *pid, int price, int output){
    Product *product = find_product(pid);

    if(product == NULL){
        printf("ERR NO_PRODUCT\n");
        return;
    }

    product->price = price;
    if(output){
        printf("OK PRICE %s %d\n", pid, price);
    }
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
        if(ptr->stock != 0 && ptr->value != 0){
            printf("  %s stock=%d value=%d\n", ptr->pid, ptr->stock, ptr->value);
        }
        ptr = ptr->next;
    }
    printf("REVENUE %d\nCOGS %d\nGIFTCOST %d\nPROFIT %d\n", revenue, cogs, gift_cost, revenue-cogs-gift_cost);
}

void QUIT(){
    Product *p_ptr = products;
    while(p_ptr != NULL){
        Product *next_product = p_ptr->next;
        free_product(p_ptr);
        p_ptr = next_product;
    }

    Cart *c_ptr = carts;
    while(c_ptr != NULL){
        Cart *next_cart = c_ptr->next;
        free_cart(c_ptr);
        c_ptr = next_cart;
    }

    printf("BYE\n");
}

void CART(const char *cid){
    Cart *c = malloc(sizeof(Cart));
    if(c == NULL){
        return;
    }

    c->cid = malloc(strlen(cid)+1);
    if(c->cid == NULL){
        free(c);
        return;
    }
    strcpy(c->cid, cid);
    c->items = NULL;
    c->next = NULL;

    if(carts == NULL){
        carts = c;
    }else{
        Cart *ptr = carts;
        while(ptr->next != NULL){
            ptr = ptr->next;
        }
        ptr->next = c;
    }

    printf("OK CART %s\n", cid);
}

void ADD(const char *cid, const char *pid, int qty){
    Cart *cart = find_cart(cid);
    Product *product = find_product(pid);

    if(cart == NULL){
        printf("ERR NO_CART\n");
        return;
    }
    if(product == NULL){
        printf("ERR NO_PRODUCT\n");
        return;
    }

    //if product pid was in cart
    CartItem *ptr = cart->items;
    while(ptr != NULL){
        if(strcmp(ptr->pid, pid) == 0){
            ptr->quantity += qty;
            printf("OK ADD %s %s %d\n", cid, pid, ptr->quantity);
            return;
        }
        ptr = ptr->next;
    }

    CartItem *item = malloc(sizeof(CartItem));
    if(item == NULL){
        return;
    }

    item->pid = malloc(strlen(pid)+1);
    if(item->pid == NULL){
        free(item);
        return;
    }
    strcpy(item->pid, pid);
    item->quantity = qty;
    item->next = NULL;

    CartItem *prev = NULL, *cur = cart->items;
    while(cur != NULL && strcmp(pid, cur->pid) > 0){
        prev = cur;
        cur = cur->next;
    }
    if(prev == NULL){
        item->next = cur;
        cart->items = item;
    }else{
        prev->next = item;
        item->next = cur;
    }
    printf("OK ADD %s %s %d\n", cid, pid, qty);
}

void CHECKOUT(char *cid){
    Cart *cart = find_cart(cid);
    if(cart == NULL){
        printf("ERR NO_CART\n");
        return;
    }

    //checking price and stock
    CartItem *item = cart->items;
    while(item != NULL){
        Product *p = find_product(item->pid);
        if(p->price == -1){
            printf("ERR NO_PRICE\n");
            return;
        }
        if(p->stock < item->quantity){
            printf("ERR INSUFFICIENT %s\n", p->pid);
            return;
        }
        if(strcmp(p->pid, promo_pid) == 0 && item->quantity >= promo_threshold){
            have_discount = 1;
        }
        item = item->next;
    }

    printf("RECEIPT %s\n", cid);
    //calculating price and take stock
    int total = 0, cost = 0, P = 100;
    item = cart->items;
    while(item != NULL){
        if(have_discount && strcmp(item->pid, discount_pid) == 0){
            P = discount;
        }else{
            P = 100;
        }
        Product *cur_product = find_product(item->pid);
        Batch *cur_batch = cur_product->batches;
        int subtotal = (cur_product->price * item->quantity * P + 50) / 100;
        int need = item->quantity;

        while(need > 0){
            if(cur_batch->quantity <= need){
                cost += (cur_batch->quantity * cur_batch->unit_cost);
                need -= cur_batch->quantity;
                Batch *next_batch = cur_batch->next;
                delete_batch(cur_batch, cur_product);
                cur_batch = next_batch;
            }else{
                cost += (need * cur_batch->unit_cost);
                cur_product->stock -= need;
                cur_product->value -= (need * cur_batch->unit_cost);
                cur_batch->quantity -= need;
                need = 0;
            }
        }

        if(P == 100){
            printf("  %s x%d @%d = %d\n", item->pid, item->quantity, cur_product->price, subtotal);
        }else{
            printf("  %s x%d @%d *%d%% = %d\n", item->pid, item->quantity, cur_product->price, discount, subtotal);
        }
        
        total += subtotal;
        item = item->next;
    }

    printf("TOTAL %d\n", total);
    printf("COST %d\n", cost);

    //gifts
    int gift_num = total/1000, min_index, gift_index, g_cost = 0, i;
    int gift_short = 0;
    char gift_id[18];
    if(gift_num == 0){
        printf("GIFT none\n");
    }
    for(i = 0; i < gift_num; ++i){
        min_index = 10005;
        Product *best = NULL;
        Product *p = products;
        while(p != NULL){
            if(p->price != -1 && p->stock > 0 && p->batches != NULL){
                gift_index = p->price - p->batches->unit_cost;
                if(gift_index < min_index){
                    min_index = gift_index;
                    strcpy(gift_id, p->pid);
                    best = p;
                }else if(gift_index == min_index){
                    if(strcmp(p->pid, gift_id) < 0){
                        min_index = gift_index;
                        strcpy(gift_id, p->pid);
                        best = p;
                    }
                }
            }
            p = p->next;
        }

        if(best == NULL){
            gift_short = 1;
            break;
        }

        Batch *b = best->batches;
        g_cost += b->unit_cost;
        printf("GIFT %s @%d\n", best->pid, b->unit_cost);
        if(b->quantity == 1){
            delete_batch(b, best);
        }else{
            b->quantity -= 1;
            best->stock -= 1;
            best->value -= (b->unit_cost);
        }
    }

    if(gift_short){
        printf("GIFT_SHORT %d\n", gift_num-i);
    }

    //totals
    printf("PROFIT %d\n", total-cost-g_cost);
    revenue += total;
    cogs += cost;
    gift_cost += g_cost;

    close_cart(cid);
}

void PROMO(const char *pid_a, int n, const char *pid_b, int P){
    strcpy(promo_pid, pid_a);
    strcpy(discount_pid, pid_b);
    promo_threshold = n;
    discount = P;
    printf("OK PROMO\n");
}

void SAVE(const char *filename){
    FILE *fp = fopen(filename, "w");
    if(fp == NULL){
        printf("ERR FILE_OPEN\n");
        return;
    }

    fprintf(fp, "NEXT_BATCH %d\n", next_batch_id);
    fprintf(fp, "PRODUCTS %d\n", product_cnt);

    Product *ptr = products;
    while(ptr != NULL){
        fprintf(fp, "%s %d\n", ptr->pid, ptr->price);
        fprintf(fp, "%s\n", ptr->name);
        ptr = ptr->next;
    }

    if(promo_threshold != -1){
        fprintf(fp, "PROMO 1\n");
        fprintf(fp, "%s %d %s %d\n", promo_pid, promo_threshold, discount_pid, discount);
    }else{
        fprintf(fp, "PROMO 0\n");
    }

    fprintf(fp, "TOTALS %d %d %d\n", revenue, cogs, gift_cost);

    fprintf(fp, "BATCHES %d\n", batch_cnt);
    ptr = products;
    Batch *b = NULL;
    while(ptr != NULL){
        b = ptr->batches;
        while(b != NULL){
            fprintf(fp, "%s %d %d %d\n", ptr->pid, b->batch_id, b->quantity, b->unit_cost);
            b = b->next;
        }
        ptr = ptr->next;
    }
    fclose(fp);
    printf("OK SAVE\n");
}

void LOAD(const char *filename){
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        printf("ERR FILE_OPEN\n");
        return;
    }

    //close all carts
    Cart *c_ptr = carts;
    while(c_ptr != NULL){
        Cart *next_cart = c_ptr->next;
        free_cart(c_ptr);
        c_ptr = next_cart;
    }
    carts = NULL;

    char line[256], command[128];
    int num1 = -1, num2 = -1, num3 = -1, num4 = -1;
    char arg1[100], arg2[100];
    while(fgets(line, sizeof(line), fp) != NULL){
        if(sscanf(line, "%s", command) != 1){
            continue;
        }

        if(strcmp(command, "NEXT_BATCH") == 0){
            sscanf(line, "%*s%d", &num1);
            next_batch_id = num1;
        }else if(strcmp(command, "PRODUCTS") == 0){
            sscanf(line, "%*s%d", &num1);

            Product *p_ptr = products;
            while(p_ptr != NULL){
                Product *next_product = p_ptr->next;
                free_product(p_ptr);
                p_ptr = next_product;
            }
            products = NULL;
            product_cnt = 0;

            for(int i = 0; i < num1; ++i){
                fgets(command, sizeof(command), fp);
                sscanf(command, "%s%d", arg1, &num2);
                fgets(arg2, sizeof(arg2), fp);
                arg2[strcspn(arg2, "\r\n")] = '\0';

                create_product(arg1, arg2);
                PRICE(arg1, num2, 0);
            }
        }else if(strcmp(command, "PROMO") == 0){
            have_discount = 0;
            sscanf(line, "%*s%d", &num1);
            if(num1 == 0){
                promo_pid[0] = '\0';
                promo_threshold = -1;
                discount_pid[0] = '\0';
                discount = 100;
            }else{
                fgets(command, sizeof(command), fp);
                sscanf(command, "%s%d%s%d", arg1, &num2, arg2, &num3);
                strcpy(promo_pid, arg1);
                promo_threshold = num2;
                strcpy(discount_pid, arg2);
                discount = num3;
            }
        }else if(strcmp(command, "TOTALS") == 0){
            sscanf(line, "%*s%d%d%d", &num1, &num2, &num3);
            revenue = num1;
            cogs = num2;
            gift_cost = num3;
        }else if(strcmp(command, "BATCHES") == 0){
            sscanf(line, "%*s%d", &num1);
            batch_cnt = 0;
            for(int i = 0; i < num1; ++i){
                fgets(command, sizeof(command), fp);
                sscanf(command, "%s%d%d%d", arg1, &num2, &num3, &num4);
                Product *p = find_product(arg1);
                create_batch(p, num3, num4, num2);
            }
        }
    }

    fclose(fp);
    printf("OK LOAD\n");
}