## Code Samples

```c
// ********************* 1. area of circle *********************
// circle.h
double area(double r);

// circle.c
double area(double r){
    return 3.14 * r * r;
}

// main.c
#include <stdio.h>
#include "circle.h"

int main(){
    printf("area = %lf\n", area(1.0));
    
    return 0;
}
```

```c
// ********************* 2. create new datatype *********************
// complex.h
// operation
struct complex_t *create(double x, double y);
struct complex_t *add(struct complex_t *c1, struct complex_t *c2);
void print(struct complex_t *c);


// complex.c
#include <stdio.h>
#include <stdlib.h>
#include "complex.h"

// part 1 : data structure
struct complex_t{
    // x + yi
    double x;
    double y;
};

// part 2 : operation
struct complex_t *create(double x, double y){
    struct complex_t *temp = malloc(sizeof(*temp));
    temp->x = x;
    temp->y = y;
    
    return temp;
}

struct complex_t *add(struct complex_t *c1, struct complex_t *c2){
    struct complex_t *temp = malloc(sizeof(*temp));
    temp->x = c1->x + c2->x;
    temp->y = c1->y + c2->y;
    
    return temp;
}

struct complex_t *multi(struct complex_t *c1, struct complex_t *c2){
    struct complex_t *temp = malloc(sizeof(*temp));
    temp->x = c1->x*c2->x - c1->y*c2->y;
    temp->y = c1->y*c2->x + c1->x*c2->y;
    
    return temp;
}

void print(struct complex_t *c){
    printf("%.2lf + %.2lfi", c->x, c->y);
}

// list.h
// operation
struct list_t *list_create();
struct list_t *list_insert(struct list_t *l, int data);
void list_print(struct list_t *l);

// list.c
#include "list.h"
#include <stdio.h>

// date type
struct list_t{
    int data;
    struct list_t *next;
}

// operation
struct list_t *list_create(){
    struct list_t *temp = malloc(sizeof(*temp));
    temp->data = 0;
    temp->next = 0;
    
    return temp;
}

struct list_t *list_insert(struct list_t *l, int data){
    // head insert method
    struct list_t *temp = malloc(sizeof(*temp));
    temp->data = data;
    temp->next = l->next;
    l->next = temp;
    
    return l;
}

void list_print(struct list_t *l){
    struct list_t *temp = l->next;
    printf("[");
    
    while(temp){
        printf("%d, ", temp->data);
        temp = temp->next;
    }
    printf("]\n");
    
    return ;
}

// main.c
#include <stdio.h>
#include "complex.h"

int main(){
    // init 2 complex numbers by using create func.
    struct complex_t *c1 = create(1.0, 1.0);
    struct complex_t *c2 = create(2.0, 2.0);
    struct complex_t *c3 = add(c1, c2);
    struct complex_t *c4 = multi(c1, c2);
    // print
    print(c3);
    printf("\n");
    print(c4);
    
    // struct complex2_t
    struct list_t *l = list_create();
    l = list_insert(l, 22);
    list_print(l);
    
    return 0;
}
```

