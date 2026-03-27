#ifndef __APP_RPM_CAPTURE_H__
#define __APP_RPM_CAPTURE_H__

#include "NuMicro.h"

#define SLOT_COUNT                  72UL
#define MULTIPLY_FACTOR             15UL

#define RPM_MIN_TARGET              100UL
#define RPM_MAX_TARGET              750UL

#define FREQ_MIN_HZ                 ((SLOT_COUNT * RPM_MIN_TARGET * MULTIPLY_FACTOR) / 60UL)
#define FREQ_MAX_HZ                 ((SLOT_COUNT * RPM_MAX_TARGET * MULTIPLY_FACTOR) / 60UL)

#define TMR3_TICK_HZ                1000000UL
#define TMR3_PRESCALE               (48UL - 1UL)      /* 48 MHz HIRC -> 1 MHz timer tick */
#define TMR3_COUNTER_MODULO         0x1000000UL       /* 24-bit counter wrap value */
#define CAPTURE_AVG_SAMPLES         32UL
#define RPM_TIMEOUT_MS              200UL

void RPM_Capture_Init(void);
void RPM_Capture_Process(uint32_t u32NowMs);
void RPM_Capture_Log(void);
uint32_t RPM_Capture_GetFreqHz(void);
uint32_t RPM_Capture_GetRpm(void);

#endif /* __APP_RPM_CAPTURE_H__ */
