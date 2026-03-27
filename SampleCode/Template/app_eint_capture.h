#ifndef __APP_EINT_CAPTURE_H__
#define __APP_EINT_CAPTURE_H__

#include "NuMicro.h"

typedef struct _pwm_duty_manager_t
{
    uint32_t previous_duty;
    uint32_t current_duty;
    uint32_t test_counter;
}PWM_DUTY_MANAGER_T;


#define PWM_SEL                                     (PWM1)
#define PWM_FREQ 								    (18000U)	
#define PWM_DUTY                              	    (100U)
#define PWM_RESOLUTION 						        (1000U)	
#define PWM_CH_NUMBER                               (0U) // 0 ~ 5
#define PWM_DUTY_STEP                               (10U)

void PWM_Adjust_DutyStep(int32_t step);
uint32_t PWM_GetCurrentDuty(void);
void PWM_Adjust_Duty(uint32_t d);
void PWM_CH_Init(void);
void EINT_Init(void);

#endif /* __APP_EINT_CAPTURE_H__ */
