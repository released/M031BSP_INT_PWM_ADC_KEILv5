#include "app_eint_capture.h"

volatile PWM_DUTY_MANAGER_T g_PWMDutyManager = 
{
    .previous_duty = 0U,
    .current_duty = 0U,
    .test_counter = 0U,
};


static uint32_t PWM_ClampDuty(uint32_t d)
{
    if (d > PWM_RESOLUTION)
    {
        return PWM_RESOLUTION;
    }
    return d;
}

void PWM_Adjust_Duty(uint32_t d)    // 0 ~ 1000 , CEHCK:PWM_RESOLUTION
{
    g_PWMDutyManager.current_duty = PWM_ClampDuty(d);
}

void PWM_Adjust_DutyStep(int32_t step)
{
    int32_t duty = (int32_t)g_PWMDutyManager.current_duty + step;

    if (duty < 0) duty = 0;
    if (duty > (int32_t)PWM_RESOLUTION) duty = (int32_t)PWM_RESOLUTION;

    PWM_Adjust_Duty((uint32_t)duty);
}

uint32_t PWM_GetCurrentDuty(void)
{
    return g_PWMDutyManager.current_duty;
}

// 1 ~ 1000 , 0.1 % to 100%
uint32_t CalNewDutyCMR(PWM_T *bpwm, uint32_t u32ChannelNum, uint32_t u32DutyCycle, uint32_t u32CycleResolution)
{
    if (u32DutyCycle >= u32CycleResolution)
        return PWM_GET_CNR(bpwm, u32ChannelNum);

    return (u32DutyCycle * (PWM_GET_CNR(bpwm, u32ChannelNum) + 1) / u32CycleResolution);
}

void PWM_SetOutputDutyCycle(PWM_T *pwm,uint32_t u32ChannelNum,uint32_t u32DutyCycle)		
{
    uint32_t u32NewCMR = 0;
    u32DutyCycle = PWM_ClampDuty(u32DutyCycle);
    u32NewCMR = CalNewDutyCMR(pwm, u32ChannelNum, u32DutyCycle, PWM_RESOLUTION);    
    PWM_SET_CMR(pwm, u32ChannelNum, u32NewCMR);
}

/*

    7  6        5       4       3       2       1       0 
    Reserved  MSKEN5  MSKEN4  MSKEN3  MSKEN2  MSKEN1  MSKEN0 

    [MSKENn] Reset Value : 0x0000_0000 
    PWM Mask Enable Bits 
    The PWM output signal will be masked when this bit is enabled. The corresponding PWM 
    channel n will output MSKDATn (PWM_MSK[5:0]) data.  
    0 = PWM output signal is non-masked. 
    1 = PWM output signal is masked and output MSKDATn data. 

    [MSKDATn] Reset Value : 0x0000_0000 
    PWM Mask Data Bit 
    This data bit control the state of PWMn output pin, if corresponding mask function is 
    enabled. Each bit n controls the corresponding PWM channel n. 
    0 = Output logic low to PWM channel n. 
    1 = Output logic high to PWM channel n. 
*/

static uint32_t EnterCritical(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void ExitCritical(uint32_t primask)
{
    __set_PRIMASK(primask);
}

static void PWM_CH_Mask(PWM_T *pwm, uint32_t ch, uint32_t level)
{
    uint32_t bit = (1UL << ch);
    uint32_t msk = pwm->MSK;
    uint32_t msken = pwm->MSKEN;
    uint32_t primask;

    if (level) { msk |= bit; }   // MSKDATn = 1
    else       { msk &= ~bit; }  // MSKDATn = 0

    msken |= bit;                // MSKENn = 1 (enable mask)

    primask = EnterCritical();
    pwm->MSK = msk;              // check level output
    pwm->MSKEN = msken;          // and open mask
    ExitCritical(primask);
}

static void PWM_CH_Unmask(PWM_T *pwm, uint32_t ch)
{
    uint32_t bit = (1UL << ch);
    uint32_t primask = EnterCritical();

    pwm->MSKEN &= ~bit;          // MSKENn = 0 (return to normal PWM)
    ExitCritical(primask);
}

//PC5:PWM1_CH0
void PWM_CH_Init(void)
{
    PWM_ConfigOutputChannel(PWM_SEL, PWM_CH_NUMBER, PWM_FREQ, PWM_DUTY);
    PWM_EnableOutput(PWM_SEL, (1UL << PWM_CH_NUMBER));
    PWM_Start(PWM_SEL, (1UL << PWM_CH_NUMBER));

    PWM_Adjust_Duty(PWM_DUTY);
    PWM_CH_Mask(PWM_SEL, PWM_CH_NUMBER, 0);
}

void EINT024_IRQHandler(void)
{    
    uint32_t duty = 0;

    /* To check if PB.5 external interrupt occurred */
    if(GPIO_GET_INT_FLAG(PB, BIT5))
    {
        GPIO_CLR_INT_FLAG(PB, BIT5);
        g_PWMDutyManager.test_counter++;

        if (PB5 == FALSE)   // during LOW period
        {            
            duty = g_PWMDutyManager.current_duty;

            PWM_CH_Unmask(PWM_SEL, PWM_CH_NUMBER);

            if (duty != g_PWMDutyManager.previous_duty)
            {
                PWM_SetOutputDutyCycle(PWM_SEL, PWM_CH_NUMBER, duty);
                g_PWMDutyManager.previous_duty = duty;
            }
        }
        else    // during HIGH , PWM turn OFF
        {
            PWM_CH_Mask(PWM_SEL, PWM_CH_NUMBER, 0);
        }
    }
}

//PB5:EINT0
void EINT_Init(void)
{
    GPIO_SetMode(PB, BIT5, GPIO_MODE_INPUT);
    GPIO_EnableInt(PB, 5, GPIO_INT_BOTH_EDGE);
    NVIC_EnableIRQ(EINT024_IRQn);

    // GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024);
    // GPIO_ENABLE_DEBOUNCE(PB, BIT5);
}
