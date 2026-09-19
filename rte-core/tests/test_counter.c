#include "rte/counter.h"
#include <assert.h>
int main(void) {
    rte_counter_t c;
    rte_counter_init(&c);
    assert(rte_counter_get(&c) == 0U);
    rte_counter_increment(&c);
    assert(rte_counter_get(&c) == 1U);
    return 0;
}
