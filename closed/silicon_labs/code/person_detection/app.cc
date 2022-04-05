#include <cstdio>
#include "api/internally_implemented.h"
#include "app.h"
#include "em_cmu.h"

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  // Unlock DPLL
  CMU_DPLLUnlock();
  ee_benchmark_initialize();
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  int8_t c;
  c = getchar();
  if (c > 0){
    ee_serial_callback(c);
  }
}
