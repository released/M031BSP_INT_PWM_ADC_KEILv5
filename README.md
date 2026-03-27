# M031BSP_INT_PWM_ADC_KEILv5
M031BSP_INT_PWM_ADC_KEILv5

# UNDER \Library\CMSIS\Core\Include , REPLACE WITH KEIL V5 version


update @ 2026/03/26

1. init UART0 (PB12 : UART0_RX , PB13 : UART0_TX) for printf

2. init PB14 , PB15 for EVB LED toggle

3. init peripheral

- ADC : PB7 (ADC0_CH7) , PB6 (ADC0_CH6)

- PB5:EINT0 , to detect input pulse (100Hz , 120Hz)
	
- PC5:PWM1_CH0 , output freq : 18K , duty changeable

- PA5:PWM0_CH0 , for emulate singal OUTPUT ONLY , PWM output with 100Hz , 120Hz@15% duty , connect to PB5:EINT0

```
        Scenario :
        1. use INT pin , to detect input pulse (use PWM @ 100Hz , 120Hz , 15%duty)
        2. during INT pin low period : outout PWM 18K , duty changeable (duty prefer 1000 STEPs)
            - LOW : output PWM
            - HIGH : PWM off   
```


- PA8 : TIM3_EXT , for TIMER EXTERNAL CAPTURE INPUT , to calculate rpm 

- PA7:TM2 , for emulate singal OUTPUT ONLY , PWM output with 1.8K@50% duty(as 100rpm),13.5K@50% duty(as 750rpm) , connect to PA8:TIM3_EXT

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/M031LC2AE(LQFP48).jpg) 


4. use UART terminal 

- USE letter a/A , d/D , to control PC5:PWM1_CH0 , output freq : 18K , duty change step 10 (RESOLUTION : 1000 , duty will be 0.0% ~ 100.0%)

```c

    // adjust 18K PWM duty
    if (FLAG_PROJ_PWM_DUTY_DEC)
    {
        FLAG_PROJ_PWM_DUTY_DEC = 0;
        PWM_Adjust_DutyStep(-(int32_t)PWM_DUTY_STEP);

        printf("PWM(decrease):%lu\r\n",(unsigned long)PWM_GetCurrentDuty());        
    }
    if (FLAG_PROJ_PWM_DUTY_INC)
    {
        FLAG_PROJ_PWM_DUTY_INC = 0;
        PWM_Adjust_DutyStep((int32_t)PWM_DUTY_STEP);

        printf("PWM(increase):%lu\r\n",(unsigned long)PWM_GetCurrentDuty());
    }    

```

- USE letter 1 , 2 , to control emulate singal-PA7:TM2 , change freq from 1.8K (rpm100) , to 13.5K (rpm 750)

```c

    // adjust emulate RPM signal
    if (FLAG_PROJ_RPM_DEC)
    {
        FLAG_PROJ_RPM_DEC = 0;
        EMULATE_SIGNAL_AdjustRpmStep(-(int32_t)APP_EMU_RPM_STEP); // -50

        printf("RPM(decrease):%u\r\n",(unsigned short)EMULATE_SIGNAL_GetCurrentRpm());   
    }

    if (FLAG_PROJ_RPM_INC)
    {
        FLAG_PROJ_RPM_INC = 0;
        EMULATE_SIGNAL_AdjustRpmStep((int32_t)APP_EMU_RPM_STEP);  // +50

        printf("RPM(increase):%u\r\n",(unsigned short)EMULATE_SIGNAL_GetCurrentRpm());  
    }


```


5. Below is ADC log

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/log_adc.jpg) 


Below is log mesage and scope capture , about emulate singal-PA7:TM2 , freq change

from RPM 100

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/log_rpm_input_1.jpg) 

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_rpm_100.jpg) 

to RPM 750

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/log_rpm_input_2.jpg) 

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_rpm_750.jpg) 

Below is log mesage , about PA8 : TIM3_EXT , RPM calcuate result

from RPM 100

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/log_rpm_capture_1.jpg) 

to RPM 750

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/log_rpm_capture_2.jpg) 


Below is log mesage , about PC5:PWM1_CH0 , output freq : 18K , duty change

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/log_pwm_output_1.jpg) 

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/log_pwm_output_2.jpg) 


below is scope capture , about PB5:EINT0 , to detect input pulse

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_input_100Hz.jpg) 


below is scope capture , about PB5:EINT0 , to detect input pulse and PC5:PWM1_CH0 , output freq : 18K , duty change

PC5:PWM1_CH0 , output freq : 18K , duty : 10%

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_input_100Hz_output_18K_10.jpg) 

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_input_100Hz_output_18K_10_2.jpg) 


PC5:PWM1_CH0 , output freq : 18K , duty : 20%

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_input_100Hz_output_18K_20.jpg) 


PC5:PWM1_CH0 , output freq : 18K , duty : 40%

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_input_100Hz_output_18K_40.jpg) 


PC5:PWM1_CH0 , output freq : 18K , duty : 60%

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_input_100Hz_output_18K_60.jpg) 


PC5:PWM1_CH0 , output freq : 18K , duty : 99%

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_input_100Hz_output_18K_99.jpg) 

![image](https://github.com/released/M031BSP_INT_PWM_ADC_KEILv5/blob/main/scope_input_100Hz_output_18K_99_2.jpg) 

