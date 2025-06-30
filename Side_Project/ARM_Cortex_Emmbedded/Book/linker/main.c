#include <stdint.h>

extern uint8_t _sheap, _eheap, _sstack, _estack;
extern uint8_t _sdata, _edata, _sbss, _ebss;

void Reset_Handler(void);
extern int main(void);

__attribute__((section(".isr_vector")))
const void *vector_table[] = {
    (void *)&_estack,
    Reset_Handler,
};

void Reset_Handler(void) {
    main();
    while (1);
}

int main(void) {
    volatile uintptr_t sheap  = (uintptr_t)&_sheap;
    volatile uintptr_t eheap  = (uintptr_t)&_eheap;
    volatile uintptr_t sstack = (uintptr_t)&_sstack;
    volatile uintptr_t estack = (uintptr_t)&_estack;

    volatile uintptr_t sdata = (uintptr_t)&_sdata;
    volatile uintptr_t edata = (uintptr_t)&_edata;
    volatile uintptr_t sbss  = (uintptr_t)&_sbss;
    volatile uintptr_t ebss  = (uintptr_t)&_ebss;

    volatile uintptr_t dummy = sheap + eheap + sstack + estack + sdata + edata + sbss + ebss;

    while (1);
}
