#include <stdio.h>
#include "app_rpm_capture.h"

volatile uint32_t g_u32CapturePrev = 0UL;
volatile uint32_t g_u32CaptureDiffAvg = 0UL;
volatile uint32_t g_u32CaptureReady = 0UL;
volatile uint32_t g_u32CaptureValid = 0UL;

volatile uint32_t g_u32DiffAcc = 0UL;
volatile uint32_t g_u32DiffCount = 0UL;
volatile uint32_t g_u32LastCaptureMs = 0UL;

static uint32_t s_u32FreqHz = 0UL;
static uint32_t s_u32Rpm = 0UL;

extern volatile uint32_t g_u32SysTickMs;

static uint32_t CaptureDiffToFreqHz(uint32_t u32Diff)
{
    if (u32Diff == 0UL)
    {
        return 0UL;
    }

    return (TMR3_TICK_HZ / u32Diff);
}

static uint32_t FreqToRpm(uint32_t u32FreqHz)
{
    uint32_t u32Denominator;

    u32Denominator = SLOT_COUNT * MULTIPLY_FACTOR;
    if (u32Denominator == 0UL)
    {
        return 0UL;
    }

    return ((u32FreqHz * 60UL) / u32Denominator);
}

void RPM_Capture_Process(uint32_t u32NowMs)
{
    uint32_t u32DiffLocal;

    if (g_u32CaptureReady != 0UL)
    {
        __disable_irq();
        u32DiffLocal = g_u32CaptureDiffAvg;
        g_u32CaptureReady = 0UL;
        __enable_irq();

        s_u32FreqHz = CaptureDiffToFreqHz(u32DiffLocal);
        s_u32Rpm = FreqToRpm(s_u32FreqHz);
    }

    if ((u32NowMs - g_u32LastCaptureMs) > RPM_TIMEOUT_MS)
    {
        __disable_irq();
        g_u32CaptureValid = 0UL;
        g_u32DiffAcc = 0UL;
        g_u32DiffCount = 0UL;
        g_u32CaptureReady = 0UL;
        __enable_irq();

        s_u32FreqHz = 0UL;
        s_u32Rpm = 0UL;
    }
}

void RPM_Capture_Log(void)
{
    printf("RPM(capture)diff=%lu count, freq=%lu Hz, rpm=%lu\n",
            (unsigned long)g_u32CaptureDiffAvg,
            (unsigned long)RPM_Capture_GetFreqHz(),
            (unsigned long)RPM_Capture_GetRpm());
}

uint32_t RPM_Capture_GetFreqHz(void)
{
    return s_u32FreqHz;
}

uint32_t RPM_Capture_GetRpm(void)
{
    return s_u32Rpm;
}

void TMR3_IRQHandler(void)
{
    uint32_t u32CapNow;
    uint32_t u32Diff;

    if (TIMER_GetCaptureIntFlag(TIMER3) == 1U)
    {
        TIMER_ClearCaptureIntFlag(TIMER3);

        u32CapNow = TIMER_GetCaptureData(TIMER3);
        g_u32LastCaptureMs = g_u32SysTickMs;

        if (g_u32CaptureValid != 0UL)
        {
            if (u32CapNow >= g_u32CapturePrev)
            {
                u32Diff = u32CapNow - g_u32CapturePrev;
            }
            else
            {
                u32Diff = (TMR3_COUNTER_MODULO - g_u32CapturePrev) + u32CapNow;
            }

            if (u32Diff != 0UL)
            {
                g_u32DiffAcc += u32Diff;
                g_u32DiffCount++;

                if (g_u32DiffCount >= CAPTURE_AVG_SAMPLES)
                {
                    g_u32CaptureDiffAvg = g_u32DiffAcc / g_u32DiffCount;
                    g_u32CaptureReady = 1UL;
                    g_u32DiffAcc = 0UL;
                    g_u32DiffCount = 0UL;
                }
            }
        }

        g_u32CapturePrev = u32CapNow;
        g_u32CaptureValid = 1UL;
    }
}

//PA8 : TIM3_EXT
void RPM_Capture_Init(void)
{
    TIMER_Open(TIMER3, TIMER_CONTINUOUS_MODE, 1U);

    TIMER_SET_PRESCALE_VALUE(TIMER3, TMR3_PRESCALE);
    TIMER_SET_CMP_VALUE(TIMER3, TIMER_CMP_MAX_VALUE);

    TIMER_EnableCapture(TIMER3, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_RISING_EDGE);
    TIMER_EnableCaptureInt(TIMER3);

    NVIC_EnableIRQ(TMR3_IRQn);

    g_u32CapturePrev = 0UL;
    g_u32CaptureDiffAvg = 0UL;
    g_u32CaptureReady = 0UL;
    g_u32CaptureValid = 0UL;
    g_u32DiffAcc = 0UL;
    g_u32DiffCount = 0UL;
    g_u32LastCaptureMs = 0UL;

    s_u32FreqHz = 0UL;
    s_u32Rpm = 0UL;

    TIMER_Start(TIMER3);
}
