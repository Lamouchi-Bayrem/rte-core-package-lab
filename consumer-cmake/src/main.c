#include <rte/counter.h>
#include <inttypes.h>
#include <stdio.h>
int main(void) {
    rte_counter_t c;
    rte_counter_init(&c);
    for (unsigned i=0; i<3U; ++i) rte_counter_increment(&c);
    printf("CMake consumer counter=%" PRIu32 "\n", rte_counter_get(&c));
#ifdef RTE_ENABLE_TRACE
    rte_counter_trace(&c);
#endif
    return 0;
}
