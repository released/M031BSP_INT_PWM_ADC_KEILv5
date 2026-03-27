#ifndef __APP_EMULATE_SIGNAL_H__
#define __APP_EMULATE_SIGNAL_H__

#include "NuMicro.h"

#define APP_EMU_EINT_FREQ_100HZ                 (100U)
#define APP_EMU_EINT_FREQ_120HZ                 (120U)
#define APP_EMU_EINT_DUTY_PERCENT               (15U)

#define APP_EMU_RPM_FREQ_100RPM_HZ              (1800U)
#define APP_EMU_RPM_FREQ_750RPM_HZ              (13500U)
#define APP_EMU_RPM_DUTY_PERCENT                (50U)
#define APP_EMU_RPM_MIN                         (100U)
#define APP_EMU_RPM_MAX                         (750U)

#define APP_EMU_RPM_STEP                        (50U)

void EMULATE_SIGNAL_SetRpm(uint16_t rpm);
void EMULATE_SIGNAL_AdjustRpmStep(int32_t step);
uint16_t EMULATE_SIGNAL_GetCurrentRpm(void);
void EMULATE_SIGNAL_INPUT_HIGH_SPEED(uint16_t rpm);

void EMULATE_SIGNAL_INPUT_LOW_SPEED_120HZ(void);
void EMULATE_SIGNAL_INPUT_LOW_SPEED_100HZ(void);

#endif /* __APP_EMULATE_SIGNAL_H__ */
