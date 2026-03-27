#include <stdio.h>
#include "app_emulate_signal.h"

static uint16_t g_emulate_rpm = APP_EMU_RPM_MIN;

static uint16_t EMULATE_SIGNAL_ClampRpm(uint16_t rpm)
{
    if (rpm < APP_EMU_RPM_MIN)
    {
        return APP_EMU_RPM_MIN;
    }

    if (rpm > APP_EMU_RPM_MAX)
    {
        return APP_EMU_RPM_MAX;
    }

    return rpm;
}

static uint32_t EMULATE_SIGNAL_RpmToFreqHz(uint16_t rpm)
{
    uint16_t rpm_limited = EMULATE_SIGNAL_ClampRpm(rpm);

    /* 100 rpm -> 1800 Hz, 750 rpm -> 13500 Hz
       freq_hz = rpm * 18 */
    return ((uint32_t)rpm_limited * 18U);
}

//PA7:TM2,1.8K@50% duty(as 100rpm),13.5K@50% duty(as 750rpm)
void EMULATE_SIGNAL_INPUT_HIGH_SPEED(uint16_t rpm)
{
    uint32_t freq_hz = EMULATE_SIGNAL_RpmToFreqHz(rpm);

    printf("RPM(INPUT)freq=%lu Hz\r\n",(unsigned long)freq_hz);

    TIMER_Stop(TIMER2);
    TIMER_Open(TIMER2, TIMER_TOGGLE_MODE, freq_hz*2);
    TIMER_Start(TIMER2);
}

uint16_t EMULATE_SIGNAL_GetCurrentRpm(void)
{
    return g_emulate_rpm;
}

void EMULATE_SIGNAL_SetRpm(uint16_t rpm)
{
    uint16_t new_rpm = EMULATE_SIGNAL_ClampRpm(rpm);

    if (new_rpm == g_emulate_rpm)
        return;

    g_emulate_rpm = new_rpm;
    EMULATE_SIGNAL_INPUT_HIGH_SPEED(g_emulate_rpm);
}

void EMULATE_SIGNAL_AdjustRpmStep(int32_t step)
{
    int32_t rpm = (int32_t)g_emulate_rpm + step;

    if (rpm < (int32_t)APP_EMU_RPM_MIN) rpm = APP_EMU_RPM_MIN;
    if (rpm > (int32_t)APP_EMU_RPM_MAX) rpm = APP_EMU_RPM_MAX;

    EMULATE_SIGNAL_SetRpm((uint16_t)rpm);
}


//PA5:PWM0_CH0 , 100Hz , 120Hz , 15%duty
void EMULATE_SIGNAL_INPUT_LOW_SPEED_120HZ(void)
{
    uint32_t PWM_CH_NUMBER = 0;
    PWM_ConfigOutputChannel(PWM0, PWM_CH_NUMBER, APP_EMU_EINT_FREQ_120HZ, APP_EMU_EINT_DUTY_PERCENT);
    PWM_EnableOutput(PWM0, (1UL << PWM_CH_NUMBER));
    PWM_Start(PWM0, (1UL << PWM_CH_NUMBER));
}

void EMULATE_SIGNAL_INPUT_LOW_SPEED_100HZ(void)
{
    uint32_t PWM_CH_NUMBER = 0;
    PWM_ConfigOutputChannel(PWM0, PWM_CH_NUMBER, APP_EMU_EINT_FREQ_100HZ, APP_EMU_EINT_DUTY_PERCENT);
    PWM_EnableOutput(PWM0, (1UL << PWM_CH_NUMBER));
    PWM_Start(PWM0, (1UL << PWM_CH_NUMBER));
}
