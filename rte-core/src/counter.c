#include "rte/counter.h"
#include <stddef.h>
#ifdef RTE_ENABLE_TRACE
#include <inttypes.h>
#include <stdio.h>
#endif
void rte_counter_init(rte_counter_t *counter) { if (counter != NULL) counter->value = 0U; }
void rte_counter_increment(rte_counter_t *counter) { if (counter != NULL) ++counter->value; }
uint32_t rte_counter_get(const rte_counter_t *counter) { return counter != NULL ? counter->value : 0U; }
#ifdef RTE_ENABLE_TRACE
void rte_counter_trace(const rte_counter_t *counter) {
    printf("[RTE TRACE] counter=%" PRIu32 "\n", rte_counter_get(counter));
}
#endif
