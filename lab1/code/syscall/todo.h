//
//

#ifndef USTCSSE_TODO_H
#define USTCSSE_TODO_H

#define TODO() \
do{            \
    fprintf(stderr, "Unimplemented: %s, %d\n", __FILE__, __LINE__)               \
}while(0)

#endif

