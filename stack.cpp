#include "stack.h"
#include "colors.h"

void recalcHash (stack_t* Stk) {
    Stk->h_t = murMur((const unsigned char*) (uintptr_t) &Stk->parrot1, 
                      (uintptr_t) &Stk->h_t - (uintptr_t)(&Stk->parrot1 + 1));

    Stk->h_a = murMur((const unsigned char*) Stk->data, 
                      Stk->capacity * sizeof(type));
}

void stackInit(stack_t* Stk) {
    unsigned int k =  8 - (uintptr_t)(Stk->data + Stk->capacity) % 8;

    if (k != 8) {
        Stk->data = (type*) calloc(BASE_SIZE * sizeof(type) + 2 * sizeof(type) + k, sizeof(char));
        *((size_t*)((uintptr_t) Stk->data + sizeof(type) + BASE_SIZE * sizeof(type))) = 0;
        *((size_t*)((uintptr_t) Stk->data + BASE_SIZE * sizeof(type) + sizeof(type) + k)) = CANARY2;
    } else {
        Stk->data = (type*) calloc(BASE_SIZE * sizeof(type) + 2 * sizeof(type), sizeof(char));
        *((size_t*)((uintptr_t) Stk->data + sizeof(type) + BASE_SIZE * sizeof(type))) = CANARY2;
    }
    

    if (!Stk->data) {
        PRINT_ERROR(stderr, "Unluck in allocating memory for Stk->data");   
    }

    Stk->size = 0;
    Stk->capacity = BASE_SIZE;

    Stk->parrot1  = CANARY1;
    Stk->parrot2  = CANARY2;
    *((size_t*)Stk->data)  = CANARY1;

    Stk->data = (type*) ((uintptr_t) Stk->data + sizeof(type));

    for (int i = 0; i < BASE_SIZE; i++) {
        Stk->data[i] = POISON;
    } 

    recalcHash (Stk);

    if (stackVerify(Stk)) {
        stackDump(Stk, __FILE__, __LINE__);
    }
}

stack_t stackPush(stack_t* Stk, type num) {
    if (stackVerify(Stk)) {
        stackDump(Stk, __FILE__, __LINE__);
    }

    if (Stk->size >= Stk->capacity) {
        Stk = stackRealloc (Stk, CAPACITY_MULTIPLIER * Stk->capacity);
    }

    Stk->data[Stk->size] = num;
    Stk->size++;

    for (unsigned int i = Stk->size; i < Stk->capacity; i++) {
        Stk->data[i] = POISON;
    }

    recalcHash (Stk);

    if (stackVerify(Stk)) {
        stackDump(Stk, __FILE__, __LINE__);
    }
    return *Stk;
}

stack_t* stackRealloc (stack_t* Stk, size_t change) {
    if (stackVerify(Stk)) {
        stackDump(Stk, __FILE__, __LINE__);
    }

    Stk->capacity = change;
    unsigned int i =  8 - (uintptr_t)(Stk->data + Stk->capacity) % 8;

    Stk->data = (type*) ((uintptr_t) realloc((type*) ((uintptr_t) Stk->data - sizeof(type)),
                      Stk->capacity * sizeof(type) + 2 * sizeof(type) + i) + sizeof(type));
    Stk->data[Stk->capacity] = CANARY2;

    if (i != 8) {
        Stk->data[Stk->capacity] = 0;
        *((size_t*) ((char*) (&Stk->data[Stk->capacity]) + i)) = CANARY2;
    }

    recalcHash (Stk);

    if (stackVerify(Stk)) {
        stackDump(Stk, __FILE__, __LINE__);
    }

    return Stk;
}

type stackPop (stack_t* Stk) {

    if (stackVerify(Stk)) {
        stackDump(Stk, __FILE__, __LINE__);
    }

    if (Stk->size * 4 <= Stk->capacity) {  
        Stk = stackRealloc (Stk, Stk->capacity / CAPACITY_MULTIPLIER);
    }

    type res = Stk->data[Stk->size - 1];
    Stk->data[Stk->size - 1] = POISON;
    Stk->size--;

    recalcHash (Stk);

    if (stackVerify(Stk)) {
        stackDump(Stk, __FILE__, __LINE__);
    }

    return res;
}

void stackDtor (stack_t* Stk) {

    if (stackVerify(Stk)) {
        stackDump(Stk, __FILE__, __LINE__);
    }

    for (unsigned int i = 0; i < Stk->size; i++) {
        Stk->data[i] = POISON;
    }

    free ((type*) ((uintptr_t) Stk->data - sizeof(type)));
    Stk->size = POISON;
    Stk->capacity = POISON;
}

void stackDump (stack_t* Stk, const char* file, const size_t line) {
    int res = stackVerify (Stk);

    printf ("File: %s\nLine: %d\n", file, line);
    if (res & BAD_STACK) {
        PRINT_ERROR (stderr, "NULL pointer of stack\n");                                        //PRINT_ERROR with stderr automatically
    } else {
        printf ("Stack pointer: %p\n", Stk);
        if (res & BAD_SIZE) {
            PRINT_ERROR (stderr, "Size: %u\n", Stk->size);
        } else {
            printf ("Size: %u\n", Stk->size);
        }
        if (res & BAD_CAPACITY) {
            PRINT_ERROR (stderr, "Capacity: %u\n", Stk->capacity);
        } else {
            printf ("Capacity: %u\n", Stk->capacity);
        }
        if (res & BAD_DATA) {
            PRINT_ERROR (stderr, "Data pointer: %p\n", Stk->data);
        } else {
            printf ("Data pointer: %p\n", Stk->data);
        }
        if (res & P1_FRIED_T) {
            PRINT_ERROR (stderr, "Left struct canary: %u\n", Stk->parrot1);
            PRINT_GREEN ("Expected value of left struct canary: %u\n", CANARY1);
        }
        if (res & P2_FRIED_T) {
            PRINT_ERROR (stderr, "Right struct canary: %u\n", Stk->parrot2);
            PRINT_GREEN ("Expected value of right struct canary: %u\n", CANARY2);
        }
        if (!(res & BAD_SIZE) && !(res & BAD_DATA)) {
            if (Stk->size <= MAX_NUM_ELEM) {
                printf("Stack elements: ");
                stackPrintAll (Stk);
            } else {
                stackPrintLast (Stk);
            }
        }
        if (res & BAD_HASH_T) {
            PRINT_ERROR (stderr, "Struct hash: %u\n", Stk->h_t);
            PRINT_GREEN ("Expected value of struct hash: %u\n", murMur((const unsigned char*) (uintptr_t) &Stk->parrot1, 
                      (uintptr_t) &Stk->h_t - (uintptr_t)(&Stk->parrot1 + 1)));
        }
        if (!(res & BAD_CAPACITY) && !(res & BAD_DATA)) {
            if (res & P1_FRIED_A) {
                PRINT_ERROR (stderr, "Left array canary: %u\n", *((size_t*)((uintptr_t) Stk->data - sizeof(type))));
                PRINT_GREEN ("Expected value of left array canary: %u\n", CANARY1);
            }

            if (res & P2_FRIED_A) {
                PRINT_ERROR (stderr, "Right array canary: %u\n", *((size_t*)(Stk->data + Stk->capacity)));
                PRINT_GREEN ("Expected value of right array canary: %u\n", CANARY2);
            }

            if (res & BAD_HASH_A) {
                PRINT_ERROR (stderr, "Array hash: %u\n", Stk->h_a);
                PRINT_GREEN ("Expected value of struct hash: %u\n", murMur((const unsigned char*) Stk->data, Stk->capacity * sizeof(type)));
            }
        }
    }
}

void stackPrintAll (stack_t* Stk) {
    for (unsigned int i = 0; i < Stk->size; i++) {
        printf(SPECIFICATOR"\t", Stk->data[i]);               
    }
    printf("\n");
}

int stackVerify (stack_t* Stk) {
    int res = 0;
    if (!Stk) {             
        res |= BAD_STACK;    // |=    
    } else { 
        if (Stk->size > 1 << 10) {
            res |= BAD_SIZE;
        } 
        
        if(Stk->capacity > 1 << 10) {
            res |= BAD_CAPACITY;
        } 
        
        if (Stk->parrot1 != CANARY1) { 
            res |= P1_FRIED_T;
        }
        
        if (Stk->parrot2 != CANARY2) {
            res |= P2_FRIED_T;
        } 
        
        if (Stk->h_t != murMur((const unsigned char*) (uintptr_t) &Stk->parrot1, 
                      (uintptr_t)&(Stk->h_t) - (uintptr_t)(&Stk->parrot1 + 1))) {
            res |= BAD_HASH_T;
        } 
        
        if (!Stk->data) {
            res |= BAD_DATA;
        } 
        
        if (!(res & BAD_DATA) && !(res & BAD_CAPACITY)) {
            
            if (*((size_t*)((uintptr_t) Stk->data - sizeof(type))) != CANARY1) {
                res |= P1_FRIED_A;
            }
            if (*((size_t*)((uintptr_t) Stk->data + Stk->capacity * sizeof(type) )) != CANARY2) {
                res |= P2_FRIED_A;
            }
            if (Stk->h_a != murMur((const unsigned char*) Stk->data, 
               Stk->capacity * sizeof(type))) {
                res |= BAD_HASH_A;
            }
        }
    } 
    return res;
}

inline unsigned int murMurScramble(unsigned int k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}

unsigned int murMur(const void* ptr, size_t len) { // 
    const unsigned char* key = (const unsigned char*) ptr;
	unsigned int h = 0;
    unsigned int k;

    for (size_t i = len >> 2; i; i--) {
        memcpy(&k, key, sizeof(int));
        key += sizeof(unsigned int);
        h ^= murMurScramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }

    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }

    h ^= murMurScramble(k);
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

void stackPrintLast (stack_t* Stk) {
    for (size_t i = Stk->size - MAX_NUM_ELEM; i < Stk->size; i++) {
        printf(SPECIFICATOR"\t", Stk->data[i]);
    }

    printf("\n");
}