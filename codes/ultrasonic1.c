#include<stdio.h>
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
#include "sensor_pwm.h"


xQueueHandle g_pULTRA1Queue;
#define ULTRA1_ITEM_SIZE           sizeof(uint8_t)
#define ULTRA1_QUEUE_SIZE          5
#define ULTRA1TASKSTACKSIZE        128
#define pdTRUE          ( ( BaseType_t ) 1 )

void timer1A_Init(void);
void timer2A_Init_ultra1(void);
void timer_Enable_ultra1(void);
void Timer1A_Handler_ultra1(void);
void Timer2A_Handler_ultra1(void);

void timer4_Init(void);

void DisableInterrupts_ultra1(void);
void EnableInterrupts_ultra1(void);
void WaitForInterrupt_ultra1(void);

void GPIOPortD_Init_ultra1(void);
void GPIOPortF_Init_ultra1(void);
void GPIOPortF_Handler_ultra1(void);

int pw_para1;
uint32_t endTick_ultra1 ;
uint32_t startTick_ultra1,deltaTick_ultra1;
//int time1;
unsigned int count_ultra1=0;

void ultrasonic1(void)
{

    EnableInterrupts_ultra1();
    GPIOPortF_Init_ultra1();
    GPIOPortD_Init_ultra1();

    timer1A_Init();
    timer2A_Init_ultra1();
    timer_Enable_ultra1();
    timer4_Init();

    while(1)
    {

        WaitForInterrupt_ultra1();
        if(count_ultra1==10)
            GPIO_PORTD_DATA_R=0x40;
        else
            GPIO_PORTD_DATA_R=0x00;

    }
}




void GPIOPortD_Init_ultra1(void)
{

    SYSCTL_RCGC2_R |= 0x00000008;   /* 1) activate clock for PortB */

    GPIO_PORTD_AMSEL_R = 0x00;      /* 3) disable analog on PF */

    GPIO_PORTD_PCTL_R = 0x00000000; /* 4) PCTL GPIO on PF4-0 */

    GPIO_PORTD_DIR_R = 0xFF;        /* 5) PB0-7 as in pins */

    GPIO_PORTD_AFSEL_R = 0x00;      /* 6) disable alt funct on PF7-0 */

    GPIO_PORTD_DEN_R = 0xFF;        /* 7) enable digital I/O on PF4-0 */
}


/* multiple of millisecond delay using periodic mode */

void GPIOPortF_Init_ultra1(void)
{

    // F4 (input, interrupt driven)
      SYSCTL_RCGC2_R |= 0x00000020;   /* enable clock to GPIOF at clock gating control register */
      GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)

      GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4

      GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4

      GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO

      GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF

      GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4

      GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive

      //GPIO_PORTF_IBE_R |= 0x10;     //     PF4 is on both edges

      GPIO_PORTF_IEV_R &= 0x00;    //     PF4 edge event is irelevant

      GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4

      GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***

      NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5

      NVIC_EN0_R |= 0x40000000;     // (h) enable interrupt 30 in NVIC

      //EnableInterrupts_ultra1();           // (i) Clears the I bit


}

void timer1A_Init()
{

    SYSCTL_RCGCTIMER_R |= 0x02;     /* enable clock to Timer Block 1 */

    TIMER1_CTL_R = 0;            /* disable Timer before initialization */

    TIMER1_CFG_R = 0x04;         /* 16-bit option */

    TIMER1_TAMR_R = 0x02;        /* periodic mode and down-counter */

    TIMER1_TAILR_R = 1000;  /* Timer A interval load value register */

    TIMER1_TAPR_R = 0x01;        /* Timer A Prescale value 244(16-1) F5*/

    TIMER1_ICR_R = 0x1;          /* clear the TimerA timeout flag*/

    TIMER1_IMR_R = 0x01;         /* Enable Interrupt on Timer A timeout */

    NVIC_PRI5_R = (NVIC_PRI5_R & 0xFFFF1FFF) | 0x00007000; /*  priority 3(0111) */

    NVIC_EN0_R = 0x00200000;        /*  Enable interrupt 21 in NVIC */

}

void timer2A_Init_ultra1()
{

    SYSCTL_RCGCTIMER_R |= 0x04;     /* enable clock to Timer Block 2 */

    TIMER2_CTL_R = 0;            /* disable Timer before initialization */

    TIMER2_CFG_R = 0x04;         /* 16-bit option */

    TIMER2_TAMR_R = 0x02;        /* periodic mode and down-counter */

    TIMER2_TAILR_R = 65536 - 1;  /* Timer A interval load value register */

    TIMER2_TAPR_R = 0x18;        /* Timer A Prescale value 24(16-1) */

    TIMER2_ICR_R = 0x1;          /* clear the TimerA timeout flag*/

    TIMER2_IMR_R = 0x01;         /* Enable Interrupt onS Timer A timeout */

    NVIC_PRI5_R = (NVIC_PRI5_R & 0x1FFFFFFF) | 0xD0000000; /*  priority 5(1011) */

    NVIC_EN0_R = 0x00800000;        /*  Enable interrupt 23 in NVIC */

}

void timer_Enable_ultra1(void)
{
    /*GPIO_PORTF_DATA_R = 4;
    GPIO_PORTF_DATA_R = 4;
    GPIO_PORTF_DATA_R = 4;
    GPIO_PORTF_DATA_R = 4;*/

    //TIMER1_CTL_R |= 0x01;        /* enable Timer A after initialization */

    TIMER2_CTL_R |= 0x01;        /* enable Timer A after initialization */

    //GPIO_PORTF_DATA_R = 0;

}

void Timer1A_Handler_ultra1(void)  //faster
{
   // GPIO_PORTF_DATA_R ^= 2;      /* turn on red LED */
    //GPIO_PORTB_DATA_R = 0x00;
    //count=count
    //GPIO_PORTB_DATA_R=0x00;

    TIMER1_ICR_R = 0x1;          /* clear the TimerA timeout flag*/

    count_ultra1=count_ultra1+1;





    //GPIO_PORTF_DATA_R = 0;      /* turn on red LED */

}

void Timer2A_Handler_ultra1(void)  //slower
{
    //GPIO_PORTF_DATA_R &= 0x08;

    TIMER1_CTL_R |= 0x01;        /* enable Timer A after initialization */

    //GPIO_PORTF_DATA_R ^= 8;      /* turn on GREEN LED */

    TIMER2_ICR_R = 0x1;          /* clear the TimerA timeout flag*/


    count_ultra1=0;

    TIMER4_TAILR_R = 4999999;       // prepare new 100 ms interval
    startTick_ultra1=4999999;
  TIMER4_CTL_R |= 0x00000001;     // enable

    //GPIO_PORTF_DATA_R = 0;      /* turn on red LED */
}


void timer4_Init(void)
{
      SYSCTL_RCGCTIMER_R |= 0x0010;   // 0) activate timer0

      //delay = SYSCTL_RCGCTIMER_R;

      TIMER4_CTL_R = 0x00000000;    // 1) disable timer2A during setup

      TIMER4_CFG_R = 0x00000000;    // 2) configure for 32-bit mode

      TIMER4_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings

      TIMER4_TAILR_R = 4999999;//4999999;     // 4) reload value

      TIMER4_TAPR_R = 0;            // 5) bus clock resolution

      TIMER4_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag

      TIMER4_IMR_R = 0x00000001;    // 7) arm timeout interrupt

      NVIC_PRI17_R = (NVIC_PRI17_R&0xFF1FFFFF)|0x00800000; // 8) priority 4

      // interrupts enabled in the main program after all devices initialized

      // vector number 39, interrupt number 23

      NVIC_EN2_R = 0x00000040;           // 9) enable IRQ 23 in NVIC

      //TIMER0_CTL_R = 0x00000001;    // 10) enable timer0A
}


void GPIOPortF_Handler_ultra1(void)
{



        endTick_ultra1 = TIMER4_TAR_R;         // read current timer value (end)

          TIMER4_CTL_R &= ~0x00000001;    // stop timer
          GPIO_PORTF_ICR_R = 0x10;          // acknowledge flag for PF4
          deltaTick_ultra1 = startTick_ultra1- endTick_ultra1 -30000 ;

         // printf("\n pulse width %ld\n",deltaTick_ultra1);

      if(deltaTick_ultra1<40000)
        pw_para1=1;
      else
        pw_para1=0;


}

/*********** DisableInterrupts_ultra1 ***************
* disable interrupts
* inputs:  none
* outputs: none
*/

void DisableInterrupts_ultra1(void)
{
    __asm ("    CPSID  I\n");
}



/*********** EnableInterrupts_ultra1 ***************
* emable interrupts
* inputs:  none
* outputs: none
*/

void EnableInterrupts_ultra1(void)
{
    __asm  ("    CPSIE  I\n");
}



/*********** WaitForInterrupt_ultra1 *************************
* go to low power mode while waiting for the next interrupt
* inputs:  none
* outputs: none
*/

void WaitForInterrupt_ultra1(void)
{
    __asm  ("    WFI\n");
}



uint32_t ULTRASONIC1Init(void)
{

    //
    // Print the current loggling LED and frequency.
    //
    UARTprintf("initialising sensor pwm\n");

    //
    // Create a queue for sending messages to the LED task.
    //
    g_pULTRA1Queue = xQueueCreate(ULTRA1_QUEUE_SIZE,ULTRA1_ITEM_SIZE);

    //
    // Create the LED task.
    //
    if( xTaskCreate(ultrasonic1, (const portCHAR *)"ULTRA1", ULTRA1TASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_ULTRA1_TASK, NULL) != pdTRUE) {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
