#ifndef RTE_COUNTER_H
#define RTE_COUNTER_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { uint32_t value; } rte_counter_t;
void rte_counter_init(rte_counter_t *counter);
void rte_counter_increment(rte_counter_t *counter);
uint32_t rte_counter_get(const rte_counter_t *counter);
#ifdef RTE_ENABLE_TRACE
void rte_counter_trace(const rte_counter_t *counter);
#endif
#ifdef __cplusplus
}
#endif
#endif
