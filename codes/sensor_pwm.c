#include <stdio.h>

#include "inc/tm4c123gh6pm.h"

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"

#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

xQueueHandle g_pSENSORQueue;
#define SENSOR_ITEM_SIZE           sizeof(uint8_t)
#define SENSOR_QUEUE_SIZE          5
#define SENSORTASKSTACKSIZE        128
#define pdTRUE          ( ( BaseType_t ) 1 )


#include <stdint.h>
#include "inc/tm4c123gh6pm.h"


int count=0;
void timer0A_delayMs();
void GPIO_PORTF_Init(void);
void DisableInterrupts(void);
void EnableInterrupts(void);
void WaitForInterrupt(void);
void Timer0A_Handler(void);
void GPIOPortB_Init(void);
void GPIOPortA_Init(void);
void GPIOPortA_Handler(void);

void sensorpwm()
{

    //count = 0;

    GPIOPortA_Init();

    GPIOPortB_Init();
    timer0A_delayMs();
    EnableInterrupts();
    portTickType ui32WakeTime;
          uint32_t ui32LEDToggleDelay;
          ui32WakeTime = xTaskGetTickCount();
    while(1)
    {
        vTaskDelayUntil(&ui32WakeTime, ui32LEDToggleDelay / portTICK_RATE_MS);

        WaitForInterrupt();
    }

}

void GPIOPortA_Init(void)
{
    SYSCTL_RCGC2_R |= 0x00000001;   /* 1) activate clock for PortA */

    GPIO_PORTA_AMSEL_R = 0x00;      /* 3) disable analog on PF */

    GPIO_PORTA_PCTL_R = 0x00000000; /* 4) PCTL GPIO on PF4-0 */

    GPIO_PORTA_DIR_R = 0x00;        /* 5) PB0-7 as in pins */

    GPIO_PORTA_AFSEL_R = 0x00;      /* 6) disable alt funct on PF7-0 */

    GPIO_PORTA_DEN_R = 0xFF;        /* 7) enable digital I/O on PF4-0 */

    GPIO_PORTA_IS_R &= ~0xFF;       /*  PB7-0 is edge-sensitive */

        //  GPIO_PORTB_IBE_R &= ~0xFF;      /*  PB7-0 is both edges */

          GPIO_PORTA_IEV_R &= ~0xFF;      /*  PF4 falling edge event */

           GPIO_PORTA_ICR_R = 0xFF;        /*  Clear all flags initially*/

           GPIO_PORTA_IM_R |= 0xFF;        /* enable interrupt arm interrupt on PB0 */

           //count = 0;

    NVIC_PRI0_R = (NVIC_PRI0_R & 0xFFFFFF1F) | 0x000000A0; /*  priority 6 */

    NVIC_EN0_R |= 0x00000001;        /*  Enable interrupt 2 in NVIC */


}

void GPIOPortB_Init(void)
{

    SYSCTL_RCGC2_R |= 0x00000002;   /* 1) activate clock for PortB */

    GPIO_PORTB_AMSEL_R = 0x00;      /* 3) disable analog on PF */

    GPIO_PORTB_PCTL_R = 0x77000000; /* 4) PCTL GPIO on PF4-0 */

    GPIO_PORTB_DIR_R = 0xEF;        /* 5) PB0-7 as in pins */

    GPIO_PORTB_AFSEL_R = 0xC0;      /* 6) disable alt funct on PF7-0 */

    GPIO_PORTB_DEN_R = 0xFF;        /* 7) enable digital I/O on PF4-0 */

    GPIO_PORTB_PUR_R |= 0x10;     //     enable weak pull-up on PF4

//         GPIO_PORTB_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
//
//         //GPIO_PORTF_IBE_R |= 0x10;     //     PF4 is on both edges
//
//         GPIO_PORTB_IEV_R &= 0x00;    //     PF4 edge event is irelevant
//
//         GPIO_PORTB_ICR_R = 0x10;      // (e) clear flag4
//
//         GPIO_PORTB_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
//
//         NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF1FFF)|0x00006000; // (g) priority 5
//
//         NVIC_EN0_R |= 0x00000002;     // (h) enable interrupt 30 in NVIC
//
//         //EnableInterrupts();           // (i) Clears the I bit

}

void GPIOPortA_Handler(void)

{

    count=count+1;
//printf("\nCount %d",count);
    //printf("cnt value %u\n",cnt);
if(count >= 45000)
{
count = 0;
    //GPIO_PORTB_DATA_R = 0x00;
    //NVIC_EN0_R &= 0xFFF7FFFF;
    //TIMER0_TAILR_R = 0x0000;
   // TIMER0_IMR_R &= 0x00;
    TIMER0_TAMATCHR_R = 0xC340;
    //TIMER0_CTL_R &= 0x00;
    TIMER0_TBMATCHR_R = 0xC340;
    while(1){}

}

    GPIO_PORTA_ICR_R = 0xFF;

}



void timer0A_delayMs()
{
    //printf("timer0");

    SYSCTL_RCGCTIMER_R |= 1;     /* enable clock to Timer Block 0 */

    TIMER0_CTL_R = 0;            /* disable Timer before initialization */

    TIMER0_CFG_R = 0x04;         /* 16-bit option */

    //Timer 0a
    TIMER0_TAMR_R = 0x0000000A;

    TIMER0_TAILR_R = 0xC350 ;  /* Timer A interval load value register */

    TIMER0_TAMATCHR_R = 0x5F64;  /*2710 for 80% 4E20 for 60% and 7530 for 40%  for AFCD 10%*/

    //TIMER0_TAPR_R = 0x64;        /* Timer A Prescale value 15(16-1) */


     //Timer 0B

    TIMER0_TBMR_R = 0x0000000A;

       TIMER0_TBILR_R = 0xC350 ;  /* Timer B interval load value register */

       TIMER0_TBMATCHR_R = 0x6B6C;  /*2710 for 80% 4E20 for 60% and 7530 for 40%  for AFCD 10%*/

    TIMER0_ICR_R = 0x0101;          /* clear the TimerA timeout flag*/

   TIMER0_IMR_R = 0x0101;                     /* Enable Interrupt on Timer A timeout */

   //NVIC control
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x1FFFFFFF) | 0xC0000000; /*  priority 3(0110) */
    NVIC_PRI5_R = (NVIC_PRI5_R & 0xFFFFFF1F) | 0x000000C0;

    NVIC_EN0_R |= 0x00180000;        /*  Enable interrupt 30 in NVIC */


    TIMER0_CTL_R |= 0x0101;        /* enable Timer A after initialization */
    //GPIO_PORTF_DATA_R ^= 2;      /* turn on red LED */

   // EnableInterrupts();
   // count = 0;

}


/*********** DisableInterrupts ***************
* disable interrupts
* inputs:  none
* outputs: none
*/

void DisableInterrupts(void)
{
    __asm ("    CPSID  I\n");
}



/*********** EnableInterrupts ***************
* enable interrupts
* inputs:  none
* outputs: none
*/

void EnableInterrupts(void)
{
    __asm  ("    CPSIE  I\n");
}



/*********** WaitForInterrupt *************************
* go to low power mode while waiting for the next interrupt
* inputs:  none
* outputs: none
*/

void WaitForInterrupt(void)
{
    __asm  ("    WFI\n");
}



uint32_t SENSORInit(void)
{

    //
    // Print the current loggling LED and frequency.
    //
    //UARTprintf("initialising sensor pwm\n");

    //
    // Create a queue for sending messages to the LED task.
    //
    g_pSENSORQueue = xQueueCreate(SENSOR_QUEUE_SIZE,SENSOR_ITEM_SIZE);

    //
    // Create the LED task.
    //
    if( xTaskCreate(sensorpwm, (const portCHAR *)"SENSOR", SENSORTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_SENSOR_TASK, NULL) != pdTRUE) {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
